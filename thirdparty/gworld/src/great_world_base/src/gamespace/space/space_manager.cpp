#include "great_world_base/gamespace/space/space_manager.h"

#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/gamespace/space/cell.h"
#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/res_loader_interface.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_resource.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/common/reader/json_reader.h"
#include "great_world_base/common/network/network_mgr.h"

static const uint16_t kSpaceDestroyDelayTimeMS = 5000;  // space预销毁时间间隔
GW_GAME_BEGIN

GWCLASS_DECL_IMPLEMENT(SpaceManager)
GWCLASS_DECL_IMPLEMENT(GameSpaceNetworkSender)

RetCode SpaceManager::Init(GWString conf_path) {
  if (res_loader_scheduler_) {
    res_loader_scheduler_->Init();
  }
  
  auto reader = new JsonReader(conf_path);
  if(!reader) {
    return READER_NOT_FOUND;
  }
  if(!cell_conf_.Init(reader)) {
    return CONF_INIT_FAIL;
  }

  gw::TimerManager::Get()->DeleteTimer(check_migrate_timer_);
  gw::TimerManager::Get()->DeleteTimer(report_load_timer_);

  check_migrate_timer_ = gw::TimerManager::Get()->CreateTimer(std::bind(&SpaceManager::CheckSpacesMigration, this),
                                                              cell_conf_.migrate_check_interval(), true);
  report_load_timer_ = gw::TimerManager::Get()->CreateTimer(std::bind(&SpaceManager::ReportLoad, this), 1000, true);

  return SUCCESS;
}

bool SpaceManager::Clear() {
  UnInit(false);
  return true;
}

void SpaceManager::UnInit(bool retain_shm) {
  gw::TimerManager::Get()->DeleteTimer(report_load_timer_);
  gw::TimerManager::Get()->DeleteTimer(check_migrate_timer_);
  if (res_loader_scheduler_) {
    res_loader_scheduler_->UnInit();
  }

  if (retain_shm) {
    return;
  }
  for (const auto& it : this->spaces_) {
    it.second->Dispose();
  }
  this->spaces_.clear();
  this->Dispose();
}

GameAppEntry* SpaceManager::FindGameApp(uint64_t gapp_id, bool create_if_not_found) {
  auto it = this->gapps_.find(gapp_id);
  if (it == this->gapps_.end()) {
    if (create_if_not_found) {
      GameAppEntry* gapp = ShmNew<GameAppEntry>();
      gapp->set_id(gapp_id);
      this->gapps_.emplace(gapp_id, gapp);
      return gapp;
    } else {
      return nullptr;
    }
  } else {
    return it->second;
  }
}

bool SpaceManager::AddCell(uint64_t space_id, const GWString& tag, const StringSlice& data, bool is_static) {
  GW_LOG_INFO("space_id=%d,data_size=%d,gapp=%lu", space_id, data.size(), gapp_id_);
  MemoryIStream ins(data);
  auto it = this->spaces_.find(space_id);
  Space* space = nullptr;
  if (it != this->spaces_.end()) {
    GW_LOG_ERROR("failed to add cell to existing space, space_id=%d", space_id);
    return false;
  } else {
    space = ShmNew<Space>(space_id);
    space->set_tag(tag);
    this->spaces_.emplace(space_id, space);
  }

  CellNode* node = space->FindCellNode(gapp_id_);
  if (node) {
    GW_LOG_ERROR("cell already exist, space_id=%d", space_id);
    space->Dispose();
    space = nullptr;
    this->spaces_.erase(space_id);
    return false;
  }
  if (!space->UpdateLayout(ins)) {
    GW_LOG_ERROR("failed to update layout, space_id=%d", space_id);
    space->Dispose();
    space = nullptr;
    this->spaces_.erase(space_id);
    return false;
  }
  node = space->FindCellNode(gapp_id_);
  if (!node) {
    GW_LOG_ERROR("cell node dont exist, space_id=%lu, gapp_id=%lu", space_id, gapp_id_);
    space->Dispose();
    space = nullptr;
    this->spaces_.erase(space_id);
    return false;
  }
  RegionNode* root = space->root_region_node();
  if (root && root->region_node_type() == RegionNodeType::kStripGroup) {
    node->set_fixed(false);
  }
  Cell* cell = ShmNew<Cell>(space, node);
  space->set_cell(cell);
  space->ChangeCellState(CellState::kCreated);
  space->set_is_static(is_static);
  return true;
}

bool SpaceManager::RemoveCell(uint64_t space_id) {
  auto it = this->spaces_.find(space_id);
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), false, "space not found : %d", space_id);
  Space* space = it->second;
  // 发包通知要被迁移走了，然后等回调
  space->ChangeCellState(CellState::kRemoving);
  return true;
}

RetCode SpaceManager::UpdateLayout(uint64_t space_id, const GWString& tag, const StringSlice& data, bool is_static) {
  auto it = this->spaces_.find(space_id);
  if (it == this->spaces_.end()) {
    GW_RETURN_IF_ERROR(!AddCell(space_id, tag, data, is_static), FAIL, "create space fail:%d", space_id);
    return SPACE_ADD_CELL;
  }
  MemoryIStream ins(data);
  if (!it->second->UpdateLayout(ins)) {
    return SPACE_UPDATE_FAIL;
  }
  if (!it->second->curr_cell_node()->cell_keep(gapp_id_)) {
    RemoveCell(space_id);
  } else if (it->second->space_resource()) {
    it->second->space_resource()->CheckRegionChange();
  }
  return SUCCESS;
}

bool SpaceManager::DestroySpace(uint64_t space_id) {
  auto it = this->spaces_.find(space_id);
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), false, "space not found: %d", space_id);
  Space* space = it->second;
  space->PrepareDestroy();
  TimerManager::Get()->CreateTimer([this, space_id](uint64_t) {
    this->DoDestroySpace(space_id);
  },
                                   kSpaceDestroyDelayTimeMS, false);
  return true;
}

bool SpaceManager::DoDestroySpace(uint64_t space_id) {
  auto it = this->spaces_.find(space_id);
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), false, "space not found: %d", space_id);
  Space* space = it->second;
  adapter_->OnDestroySpace(space_id, space->tag());
  space->OnDestroy();
  space->Dispose();
  this->spaces_.erase(space_id);
  return true;
}

bool SpaceManager::SyncCell(uint64_t space_id, StringSlice& layout_data, bool is_static) {
  GW_LOG_INFO("space_id=%d", space_id);
  auto it = this->spaces_.find(space_id);
  if (it != this->spaces_.end()) {
    GW_LOG_ERROR("failed to sync cell, space id: %d", space_id);
    return false;
  }
  Space* space = ShmNew<Space>(space_id);
  this->spaces_.emplace(space_id, space);
  MemoryIStream ins(layout_data);
  if (!space->UpdateLayout(ins)) {
    GW_LOG_ERROR("fail to update layout, space id: %d", space_id);
    space->Dispose();
    space = nullptr;
    this->spaces_.erase(space_id);
    return false;
  }
  space->set_is_static(is_static);
  return true;
}

Space* SpaceManager::GetSpace(uint64_t space_id) {
  auto it = this->spaces_.find(space_id);
  if (it == this->spaces_.end()) {
    return nullptr;
  }
  return it->second;
}

void SpaceManager::ReportLoad() {
  ReportLoadMsg msg;
  msg.set_load(adapter_->GetLoad());
  msg.set_memsize(adapter_->GetMemsize());
  uint32_t all_num_avatars = 0;
  uint32_t all_num_entities = 0;
  uint32_t all_num_reals = 0;
  for (auto it : this->spaces_) {
    CellReportMsg cr;
    cr.set_space_id(it.first);
    Space* space = it.second;
    cr.set_num_entities(space->num_entities());
    all_num_entities += space->num_entities();
    cr.set_num_reals(space->num_reals());
    all_num_reals += space->num_reals();
    cr.set_num_avatars(space->num_entity_avatars());
    all_num_avatars += space->num_entity_avatars();
    Cell* cell = space->cell();
    if (cell && !cell->node()->fixed()) {
      cr.mutable_entity_rect() = cell->entity_rect();
      cr.mutable_tile_rect() = cell->tile_rect();
    }
    msg.add_cell_report(std::move(cr));
  }
  msg.set_num_avatars(all_num_avatars);
  msg.set_num_entities(all_num_entities);
  msg.set_num_reals(all_num_reals);
  NetworkOStream os(NetworkMessage::ReportLoad);
  msg.ToString(&os);
  SEND_MSG_TO_CELLMGR(os);
}

void SpaceManager::CheckSpacesMigration() {
  std::vector<uint64_t> wait_remove_spaces;
  for (auto it : this->spaces_) {
    Space* space = it.second;
    Cell* cell = space->cell();
    if (!cell) {
      continue;
    }
    auto real_count = cell->CheckMigration();
    // GW_LOG_INFO("Check Space Migration, space id: %d,%d,%d", space->id(), real_count, space->wait_remove());
    if (real_count == 0 && space->wait_remove()) {
      wait_remove_spaces.push_back(it.first);
    }
  }
  for (auto space_id : wait_remove_spaces) {
    auto it = this->spaces_.find(space_id);
    if (it != this->spaces_.end()) {
      Cell* cell = it->second->cell();
      if (!cell) {
        continue;
      }
      adapter_->OnDestroySpace(cell->space()->id(), cell->space()->tag());
      it->second->Dispose();
      this->spaces_.erase(it);
      GW_LOG_TRACE("CheckSpacesMigration del space:%d", it->first);
    }
  }
}

void SpaceManager::OnChangeCellStateSuccessFromGame(uint64_t space_id, uint32_t sid, uint64_t other_gapp_id, CellState other_state) {
  GW_LOG_TRACE("OnChangeCellStateSuccessFromGame:%d", space_id);
  auto it = this->spaces_.find(space_id);
  if (it == this->spaces_.end()) {
    return;
  }
  it->second->OnChangeCellStateSuccessFromGame(sid, other_gapp_id, other_state);
}

bool SpaceManager::InitSpaceResource(uint64_t space_id) {
  auto it = this->spaces_.find(space_id);
  if (it == this->spaces_.end()) {
    return false;
  }
  return it->second->InitSpaceResource();
}

uint64_t SpaceManager::choose_migration_cell(uint64_t eid, uint64_t node_id) {
  if (choose_migration_cell_)
    return choose_migration_cell_(eid, node_id);
  else
    return 0;
}

bool SpaceManager::QueryCell(uint64_t space_id, float x, float z, uint64_t& gapp_id) {
  auto iter = this->spaces_.find(space_id);
  if (iter == this->spaces_.end()) {
    return false;
  }
  Space* space = iter->second;
  if (!space->is_static()) {
    return false;
  }
  CellNode* cell_node = space->FindCellNode(x, z);
  if (!cell_node) {
    return false;
  }
  gapp_id = cell_node->main_gapp_id();
  return true;
}

GW_GAME_END
