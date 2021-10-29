#include "great_world_base/cellmgr/basemgr.h"
#include "great_world_base/common/common_private.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(BaseMgr)

void BaseMgr::Init() {
}
void BaseMgr::Uninit(bool retain_shm) {
  if (retain_shm) {
    return;
  }
  for (const auto& it : this->gapps_) {
    it.second->Dispose();
  }
  this->gapps_.clear();
  this->Dispose();
}

void BaseMgr::ReportLoad(uint64_t gapp_id, ReportLoadMsg& msg) {
  GW_LOG_DEBUG("gapp_id=%d load=%f memsize=%d", gapp_id, msg.load(), msg.memsize());
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
}

uint64_t BaseMgr::ChooseFreeGame() const {
  float min_load = kMaxLoad;
  GameAppEntry* min_gapp = nullptr;
  GameAppEntry* gapp = nullptr;
  std::vector<uint64_t> gapp_ids;
  for (auto iter : this->gapps_) {
    if (iter.second->available()) {
      gapp_ids.push_back(iter.first);
    }
  }
  if(gapp_ids.empty()) {
    return 0;
  }
  else {
    return gapp_ids[rand()%gapp_ids.size()];
  }
}

bool BaseMgr::RegisterGameApp(uint64_t gapp_id) {
  GW_LOG_INFO("gapp_id=%d", gapp_id);
  auto it = this->gapps_.find(gapp_id);
  GW_RETURN_IF_ERROR(it != this->gapps_.end(), false, "already registered : %d", gapp_id);
  GameAppEntry* gapp = ShmNew<GameAppEntry>(gapp_id);
  this->gapps_.emplace(gapp_id, gapp);
  return true;
}

bool BaseMgr::UnregisterGameApp(uint64_t gapp_id) {
  GW_LOG_INFO("gapp_id=%d", gapp_id);
  auto it = this->gapps_.find(gapp_id);
  GW_RETURN_IF_ERROR(it == this->gapps_.end(), false, "not registered : %d", gapp_id);
  it->second->Dispose();
  this->gapps_.erase(it);
  return true;
}

GW_CELL_MGR_END
