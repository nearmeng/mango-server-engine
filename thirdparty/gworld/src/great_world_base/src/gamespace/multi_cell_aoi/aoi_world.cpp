#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_manager.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/common/reader/json_reader.h"
#include "great_world_base/gamespace/space/space_manager.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(AoIWorld)

AoIWorld::~AoIWorld() {
  for (auto& it : aoi_manager_map_) {
    delete it.second;
  }
}

RetCode AoIWorld::Init(GWString conf_path) {
  auto reader = new JsonReader(conf_path);
  if(!reader) {
    return READER_NOT_FOUND;
  }
  if(!aoi_conf_.Init(reader)) {
    return CONF_INIT_FAIL;
  }
  return SUCCESS;
}

void AoIWorld::Uninit() {
  for (auto& it : aoi_manager_map_) {
    DestroySpace(it.first);
  }
}

void AoIWorld::OwnResume(void* tmp) {
  auto aoi_world = reinterpret_cast<AoIWorld*>(tmp);
  auto tmp_aoi_manager_map = aoi_world->aoi_manager_map_;
  aoi_world->aoi_manager_map_.clear();
  for (auto& it : tmp_aoi_manager_map) {
    aoi_world->aoi_manager_map_.emplace(it.first, new AoIManager());
    GW_LOG_DEBUG("AoIWorld Resumed! %lu", it.first);
  }
}

RetCode AoIWorld::CreateSpace(uint64_t space_id) {
  auto it = aoi_manager_map_.find(space_id);
  if (it != aoi_manager_map_.end()) {
    GW_LOG_ERROR("create space fail, space_id %lu", space_id);
    return AOI_SAPCE_REPEAT;
  }
  aoi_manager_map_.emplace(space_id, new AoIManager());
  return SUCCESS;
}

RetCode AoIWorld::DestroySpace(uint64_t space_id) {
  auto it = aoi_manager_map_.find(space_id);
  if (it == aoi_manager_map_.end()) {
    GW_LOG_ERROR("Destroy space fail, space not found, space_id %lu", space_id);
    return AOI_SPACE_NOT_FOUND;
  }
  delete it->second;
  aoi_manager_map_.erase(space_id);
  return SUCCESS;
}

AoIManager* AoIWorld::GetAoIManager(uint64_t space_id) {
  auto it = aoi_manager_map_.find(space_id);
  if (it == aoi_manager_map_.end()) {
    return nullptr;
  }
  return it->second;
}

AoIEntity* AoIWorld::Enter(uint64_t space_id, uint64_t eid, uint32_t radius, Vector3 pos) {
  auto aoi_mgr = GetAoIManager(space_id);
  if (!aoi_mgr) {
    GW_LOG_ERROR("enter aoi fail, aoi_mgr not found:space %lu, eid %lu", space_id, eid);
    return nullptr;
  }
  pos.z = pos.y;
  return aoi_mgr->Enter(eid, pos);
}

void AoIWorld::Leave(uint64_t space_id, uint64_t eid) {
  auto aoi_mgr = GetAoIManager(space_id);
  if (!aoi_mgr) {
    GW_LOG_ERROR("space_id not found,space_id=%lu, eid=%lu", space_id, eid);
    return;
  }
  aoi_mgr->Leave(eid);
}

void AoIWorld::OnBeginObserver(const GWVector<uint64_t>& self, uint64_t other) {
  SPACE_MGR.adapter()->OnBeginObserver(self, other);
}

void AoIWorld::OnEndObserver(uint64_t self, uint64_t other) { 
  SPACE_MGR.adapter()->OnEndObserver(self, other); 
}

GWVector<AoIDiffProp>&& AoIWorld::GetDiffProps(uint64_t eid, uint32_t labels) {
  return std::move(SPACE_MGR.adapter()->GetDiffProps(eid, labels));
}

IMemoryOStream* AoIWorld::NotifyOS() { 
  return SPACE_MGR.adapter()->NotifyOS(); 
}

GW_GAME_END
