#include "great_world_base/cellmgr/cellmgr.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/region_node.h"
#include "great_world_base/cellmgr/basemgr.h"
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/cellmgr/proxy/mgr_service_proxy.h"
#include "great_world_base/cellmgr/space.h"
#include "great_world_base/cellmgr/spacelayout/space_layout_mgr.h"
#include "great_world_base/cellmgr/spacelayout/space_layout_xml_reader.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/cellmgr/balance/default_mem_weight_policy.h"
#include "great_world_base/common/reader/json_reader.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(CellMgr)
GWCLASS_DECL_IMPLEMENT(CellMgrNetworkSender)

CellMgr::CellMgr() {
}

static void DoBlance(uint64_t timer_id) {
  CellMgr::GetInstance().Balance();
  CellMgr::GetInstance().GappBalance();
}

bool CellMgr::Init(GWString config_path) {
  if (!SPACE_LAYOUT_MGR->reader()) {
    SPACE_LAYOUT_MGR->SetReader(std::make_shared<SpaceLayoutXMLReader>());
  }
  mem_weight_policy_ = ShmNew<DefaultMemWeightPolicy>();
  config_path_ = config_path;
  auto reader = new JsonReader(config_path);
  return config_.Init(reader);
}

void CellMgr::StartUp() {
  balance_timer_ = gw::TimerManager::Get()->CreateTimer(DoBlance, CELLMGR_CFG.balance_interval(), true);
}

void CellMgr::OwnResume(void* p) {
  CellMgr* cell_mgr = reinterpret_cast<CellMgr*>(p);
  auto reader = new JsonReader(cell_mgr->config_path_);
  reader->Init();
  cell_mgr->config_.SetReader(reader);
}

void CellMgr::UnInit(bool retain_shm) {
  if (retain_shm) {
    return;
  }
  gw::TimerManager::Get()->DeleteTimer(balance_timer_);
  balance_timer_ = gw::TimerManager::kInvalidTimerId;
  for (const auto& it : this->gapps_) {
    it.second->Dispose();
  }
  this->gapps_.clear();
  for (const auto& it : this->spaces_) {
    it.second->Dispose();
  }
  this->spaces_.clear();
  this->Dispose();
  mem_weight_policy_->Dispose();
}

bool CellMgr::Clear() {
  UnInit(false);
  return true;
}

CellMgr::~CellMgr() {
}

Space* CellMgr::CreateSpace(const GWString& name, const GWString& tag, uint16_t min_cell_num, uint16_t max_cell_num, 
                            const Rect& rect, RegionNode* root, bool is_static, const GWString& meta_data) {
  Space* space = ShmNew<Space>(++(this->last_space_id_), name, tag, min_cell_num, max_cell_num, rect, root);
  this->spaces_.emplace(space->id(), space);
  MemoryOStream os;
  space->WriteLayoutToStream(os);
  space->set_is_static(is_static);
  space->set_meta_data(meta_data);
  AddCellMsg msg;
  msg.set_space_id(space->id());
  msg.set_layout_data(os.str());
  msg.set_is_static(is_static);
  msg.set_tag(tag);
  GW_LOG_INFO("create space %d on %d game apps(all:%d)", space->id(), space->cells().size(), this->gapps_.size());
  for (const auto& iter : space->cells()) {
    mgr_service_.OnAddCell(iter.first, msg);
    GW_LOG_INFO("add cell on game app(%d) for space %d", iter.first, space->id());
  }
  if (is_static) {
    for (const auto& iter : BASEMGR.game_apps()) {
      mgr_service_.OnSyncCell(iter.first, msg);
      GW_LOG_INFO("sync cell, base app: %ld, space: %ld", iter.first, space->id());
    }
  }
  return space;
}

bool CellMgr::DestroySpace(uint64_t id) {
  GW_LOG_INFO("id=%d", id);
  auto it = this->spaces_.find(id);
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), false, "space not found:%d", id);
  DestroySpaceNotice msg;
  msg.set_space_id(id);
  for (const auto& gapp_it : it->second->cells()) {
    mgr_service_.OnDestroyCell(gapp_it.first, msg);
  }
  if (it->second->is_static()) {
    for (const auto& iter : BASEMGR.game_apps()) {
      mgr_service_.OnDestroyCell(iter.first, msg);
    }
  }
  adapter_->OnDestroySpace(id, it->second->tag());
  it->second->Dispose();  // delete space here
  this->spaces_.erase(id);
  return true;
}

bool CellMgr::UpdateLayout(uint64_t id, std::shared_ptr<std::vector<uint64_t>> extra_gapp_ids) {
  auto it = this->spaces_.find(id);
  MemoryOStream os;
  SpaceLayoutMsg msg;
  LayoutMsg layout;
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), false, "space not found:%d", id);
  it->second->WriteLayoutToStream(os);
  layout.set_space_id(id);
  layout.set_layout_data(os.str());
  layout.set_is_static(it->second->is_static());
  layout.set_tag(it->second->tag());
  msg.add_layouts(std::move(layout));
  for (const auto& gapp_it : it->second->cells()) {
    mgr_service_.OnUpdateSpaceLayout(gapp_it.first, msg);
  }
  if (extra_gapp_ids) {
    for (auto it : *extra_gapp_ids) {
      mgr_service_.OnUpdateSpaceLayout(it, msg);
    }
  }
  return true;
}

void CellMgr::UpdateLayoutToGame(uint64_t space_id, uint64_t gapp_id) {
  auto space_it = CELLMGR.FindSpace(space_id);
  if (!space_it) {
    GW_LOG_DEBUG("space not found:space_id %u, gapp_id %lu", space_id, gapp_id);
    return;
  }
  MemoryOStream os;
  SpaceLayoutMsg msg;
  LayoutMsg layout;
  space_it->WriteLayoutToStream(os);
  layout.set_space_id(space_id);
  layout.set_layout_data(os.str());
  layout.set_is_static(space_it->is_static());
  layout.set_tag(space_it->tag());
  msg.add_layouts(std::move(layout));
  mgr_service_.OnUpdateSpaceLayout(gapp_id, msg);
  GW_LOG_DEBUG("UpdateLayoutToGame:space_id %u, gapp_id %lu", space_id, gapp_id);
}

bool CellMgr::RegisterGameApp(uint64_t gapp_id) {
  /// 注册 GameApp 相关数据
  GW_LOG_INFO("gapp_id=%d", gapp_id);
  auto it = this->gapps_.find(gapp_id);
  GW_RETURN_IF_ERROR(it != this->gapps_.end(), false, "already registered : %d", gapp_id);
  GameAppEntry* gapp = ShmNew<GameAppEntry>(gapp_id);
  this->gapps_.emplace(gapp_id, gapp);

  return true;
}

bool CellMgr::UnregisterGameApp(uint64_t gapp_id) {
  GW_LOG_INFO("gapp_id=%d", gapp_id);
  auto it = this->gapps_.find(gapp_id);
  GW_RETURN_IF_ERROR(it == this->gapps_.end(), false, "not registered : %d", gapp_id);
  it->second->Dispose();
  this->gapps_.erase(it);
  // TODO 完善容灾
  return true;
}

void CellMgr::ConfirmAddCell(uint64_t gapp_id, uint64_t space_id) {
  GW_LOG_DEBUG("gapp_id=%d space_id=%d", gapp_id, space_id);
  auto it = this->spaces_.find(space_id);
  CellNode* node = nullptr;
  GW_JUMP_IF_ERROR(it == this->spaces_.end(), "space not found:%d", space_id);
  node = it->second->FindCell(gapp_id);
  GW_JUMP_IF_ERROR(node == nullptr, "cell not found:gapp_id=%d space_id=%d", gapp_id, space_id);
  GW_JUMP_IF_ERROR(node->state(gapp_id) != CellState::kInit, "invalid cell state:gapp_id=%d space_id=%d state=%d", gapp_id, space_id, node->state(gapp_id));
  node->set_state(CellState::kCreated, gapp_id);
  if (it->second->IsAllCellCreated() && !it->second->is_notified()) {
    it->second->set_is_notified(true);
    AllCellCreatedMsg msg;
    msg.set_space_id(it->second->id());
    msg.set_tag(it->second->tag());
    msg.set_meta_data(it->second->meta_data());
    for (const auto& iter : this->gapps_) {
      mgr_service_.OnAllCellCreated(iter.first, msg);
    }
    for (const auto& iter : BASEMGR.game_apps()) {
      mgr_service_.OnAllCellCreated(iter.first, msg);
    }
    adapter_->AllCellCreated(msg);
  }
FEXIT:
  return;
}

void CellMgr::ReportLoad(uint64_t gapp_id, ReportLoadMsg& msg) {
  GW_LOG_DEBUG("gapp_id=%d load=%f memsize=%d num_entities=%d num_real=%d num_avatar=%d",
               gapp_id, msg.load(), msg.memsize(), msg.num_entities(), msg.num_reals(), msg.num_avatars());
  auto it = this->gapps_.find(gapp_id);
  if (it == this->gapps_.end()) {
    GW_LOG_ERROR("gapp not exist : %d", gapp_id);
    return;
  }
  GameAppEntry* gapp = it->second;
  float load = msg.load();
  gapp->set_curr_load(load);
  gapp->set_conv_load(load * 0.1f + gapp->conv_load() * 0.9f);  // TODO 配置公式化
  gapp->set_revised_load(gapp->conv_load());                    // 重置修正的负载
  gapp->set_memsize(msg.memsize());
  gapp->set_num_entities(msg.num_entities());
  gapp->set_num_reals(msg.num_reals());
  gapp->set_num_avatars(msg.num_avatars());
  if (msg.cell_report_size()) {
    for (auto& report : msg.cell_report()) {
      auto space_it = this->spaces_.find(report.space_id());
      if (space_it == this->spaces_.end()) {
        GW_LOG_ERROR("space not found : %d", report.space_id());
        continue;
      }
      Space* space = space_it->second;
      CellNode* node = space->FindCell(gapp_id);
      if (!node) {
        GW_LOG_ERROR("cell %d not found for space %d", gapp_id, report.space_id());
        continue;
      }
      node->set_num_entities(report.num_entities(), gapp_id);
      node->set_num_reals(report.num_reals(), gapp_id);
      node->set_num_avatars(report.num_avatars(), gapp_id);
      node->mutable_entity_rect() = (report.entity_rect());
      node->mutable_tile_rect() = (report.tile_rect());
      GW_LOG_DEBUG("node_load:space %lu, node_id %lu, gapp_id %lu, avatar_num %u, real_num %u, parallel_visible %d",
                   space_it->first, node->id(), gapp_id, report.num_avatars(), report.num_reals(),
                   node->parallel_visible());
    }
  }
}

GameAppEntry* CellMgr::SearchGood(GameAppFilter& filter) const {
  uint32_t min_memsize = CELLMGR_CFG.upper_good_memsize();
  float upper_load = CELLMGR_CFG.upper_normal_load();
  GameAppEntry* best_gapp = nullptr;
  GameAppEntry* gapp = nullptr;
  // 先搜索符合负载要求并且内存最小的game app
  for (auto iter : this->gapps_) {
    gapp = iter.second;
    if (filter.Check(gapp) && gapp->available() && gapp->revised_load() < upper_load && gapp->memsize() < min_memsize) {
      min_memsize = gapp->memsize();
      best_gapp = gapp;
    }
  }
  if (best_gapp) {
    return best_gapp;
  }
  // 再搜索符合内存要求并且负载最低的game app
  float upper_memsize = CELLMGR_CFG.upper_normal_memsize();
  float min_load = kMaxLoad;
  for (auto iter : this->gapps_) {
    gapp = iter.second;
    if (filter.Check(gapp) && gapp->available() && gapp->memsize() < upper_memsize && gapp->revised_load() < min_load) {
      min_load = gapp->revised_load();
      best_gapp = gapp;
    }
  }
  if (best_gapp) {
    GW_LOG_INFO("SearchGood conv_load:%f revised_load:%f memsize:%d",
                best_gapp->conv_load(), best_gapp->revised_load(), best_gapp->memsize());
  }
  return best_gapp;
}

GameAppEntry* CellMgr::SearchLeastLoaded(GameAppFilter& filter) const {
  float min_load = kMaxLoad;
  GameAppEntry* min_gapp = nullptr;
  GameAppEntry* gapp = nullptr;
  for (auto iter : this->gapps_) {
    gapp = iter.second;
    if (filter.Check(gapp) && gapp->available() && gapp->revised_load() < min_load) {
      min_load = gapp->revised_load();
      min_gapp = gapp;
    }
  }
  return min_gapp;
}

CellNode* CellMgr::FindCell(uint64_t space_id, float x, float y) const {
  auto it = this->spaces_.find(space_id);
  GW_RETURN_IF_ERROR(it == this->spaces_.end(), nullptr, "space not found:%d", space_id);
  return it->second->FindCell(x, y);
}

Space* CellMgr::FindSpace(uint64_t space_id) const {
  auto it = this->spaces_.find(space_id);
  return it != this->spaces_.end() ? it->second : nullptr;
}

void CellMgr::Balance() {
  if (!this)
    return;
  uint8_t max_round_mask = CELLMGR_CFG.space_balance_round_mask();
  bool tracing = CELLMGR_CFG.tracing_balance();
  G6_ASSERT(MathUtility::IsPowerOfTwo(max_round_mask + 1));
  // 分帧处理space的负载均衡
  for (auto it : this->spaces_) {
    if ((it.second->id() & max_round_mask) == this->space_balance_round_) {
      if (it.second->Balance(tracing)) {
        UpdateLayout(it.first);
      }
    }
  }
  this->space_balance_round_ += 1;
  if (this->space_balance_round_ > max_round_mask) {
    this->space_balance_round_ = 0;
  }
}

void CellMgr::GappBalance() {
  if (!this)
    return;
  uint8_t max_round_mask = CELLMGR_CFG.gapp_balance_round_mask();
  bool tracing = CELLMGR_CFG.tracing_balance();
  G6_ASSERT(MathUtility::IsPowerOfTwo(max_round_mask + 1));
  // 分帧处理gapp的负载均衡
  for (auto it : this->spaces_) {
    if ((it.second->id() & max_round_mask) == this->gapp_balance_round_) {
      if (it.second->GAppBalance(tracing)) {
        UpdateLayout(it.first);
      }
    }
  }
  this->gapp_balance_round_ += 1;
  if (this->gapp_balance_round_ > max_round_mask) {
    this->gapp_balance_round_ = 0;
  }
}

GW_CELL_MGR_END