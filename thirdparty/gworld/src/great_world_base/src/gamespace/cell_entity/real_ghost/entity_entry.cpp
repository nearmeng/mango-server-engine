#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/gamespace/space/game_app_entry.h"
#include "great_world_base/gamespace/cell_entity/cell_entity.h"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/common/memory_stream_private.h"
#include "great_world_base/gamespace/space/space_manager.h"

GW_GAME_BEGIN

void RealEntry::DeleteGhostsOnLeave() {
  for (auto ghost : ghosts().entries()) {
    if (ghost->is_created()) {
      DeleteGhost(*ghost);
    }
  }
}

EntityEntry::~EntityEntry() {
  if (real_entry_) {
    real_entry_->Dispose();
  }
}

void RealEntry::Migrate() {
  auto entity_entry = cell_entity_->entity_entry();
  uint64_t gapp_id = entity_entry->dst_gapp_id();
  GW_LOG_DEBUG("OnMigrate dst gapp id: %lu", gapp_id);
  if(!cell_entity_->adapter()->RequestMigrateOut(gapp_id)) {
    GW_LOG_WARN("%lu Migrate to %lu fail", cell_entity_->GetEid(), gapp_id);
    cell_entity_->space()->curr_cell_node()->OnMigrateDone(cell_entity_->GetEid());
  }
}

void RealEntry::DeleteGhost(const gw::game::GhostEntry& ghost_entry) {
  uint64_t gapp_id = ghost_entry.gapp()->id();
  GW_LOG_DEBUG("OnDeleteGhost gapp_id: %lu，%p", gapp_id, this);
  NetworkOStream os(NetworkMessage::DeleteGhost);
  os << cell_entity_->GetEid() << cell_entity_->space()->id();
  SEND_MSG_TO_CELL(gapp_id, os);
}

void RealEntry::CreateGhost(const gw::game::GhostEntry& ghost_entry) {
  uint64_t gapp_id = ghost_entry.gapp()->id();
  GW_LOG_DEBUG("OnCreateGhost gapp_id: %lu，%p,%lu", gapp_id, this, cell_entity_->GetEid());
  NetworkOStream os(NetworkMessage::CreateGhost);
  os << cell_entity_->GetEid(); 
  os << cell_entity_->space()->id();
  // 玩家数据
  cell_entity_->adapter()->WriteToMemoryStreamGhost(os);
  // 内部数据
  cell_entity_->InnerWriteToMemoryStreamGhost(os);
  SEND_MSG_TO_CELL(gapp_id, os);
}

void EntityEntry::PrepareDestroy() { }

void EntityEntry::OnCellDestroy() { 
  cell_entity_->space()->Leave(cell_entity_, LeaveSpaceReason::SpaceDestory);
}

void EntityEntry::AddReal() {
  if (!real_entry()) {
    auto real = gw::ShmNew<RealEntry>(cell_entity_);
    set_real_entry(real);
    cell_entity_->space()->IncreaseNumReals(cell_entity_);
  } else {
    GW_LOG_ERROR("repeat add real");
  }
}

void EntityEntry::RemoveReal() {
  if (real_entry()) {
    RealEntry* p = static_cast<RealEntry*>(real_entry());
    set_real_entry(nullptr);
    cell_entity_->space()->DecreaseNumReals(cell_entity_);
    gw::ShmDelete<RealEntry>(p);
  } else {
    GW_LOG_ERROR("fail to remove real part");
  }
}

bool EntityEntry::ConvertToGhost() {
  if(!real_entry_) {
    return false;
  }
  RemoveReal();
  return true;
}

bool EntityEntry::ConvertToReal() {
  if (real_entry_) {
    return false;
  }
  AddReal();
  return true;
}

bool EntityEntry::UpdateGhostEntries(const std::vector<uint64_t>& entries) {
  gw::game::GhostEntries& ghost_entries = real_entry()->ghosts();
  for (auto gapp_id : entries) {
    auto gapp = SPACE_MGR.FindGameApp(gapp_id, true);
    if (gapp) {
      ghost_entries.Add(gapp);
    } else {
      return false;
    }
  }
  return true;
}

GW_GAME_END