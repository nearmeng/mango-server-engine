#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_manager.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/aoi_imp_list.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN

AoIManager::AoIManager() {
  imp_ = new AoiImpList();
}

AoIManager::~AoIManager() {
  delete imp_;
  for(auto& it:aoi_entities_) {
    delete it.second;
  }
}

AoIEntity* AoIManager::Enter(uint64_t eid, Vector3& pos) {
  auto new_entity = new AoIEntity(eid, this, pos);
  aoi_entities_.emplace(eid, new_entity);
  GW_LOG_TRACE("%lu enter pos, %f,%f,%f", eid, pos.x, pos.y, pos.z);
  if(imp_->Enter(eid, pos)) {
    return new_entity;
  }
  else {
    delete new_entity;
    return nullptr;
  }
}

void AoIManager::Leave(uint64_t eid) {
  imp_->Leave(eid);
  auto it = aoi_entities_.find(eid);
  if(it != aoi_entities_.end()) {
    aoi_entities_.erase(eid);
    delete it->second;
  }
}

void AoIManager::Move(uint64_t eid, Vector3& pos) {
  GW_LOG_TRACE("%lu move pos, %f,%f,%f", eid, pos.x, pos.y, pos.z);
  imp_->Move(eid, pos);
}

bool AoIManager::AddTrigger(uint64_t eid, uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave) {
  return imp_->AddTrigger(eid, radius, on_enter, on_leave);
}

void AoIManager::RemoveTrigger(uint64_t eid, uint32_t radius) {
  imp_->RemoveTrigger(eid, radius);
}

AoIEntity* AoIManager::GetAoIEntity(uint64_t eid) {
  auto it = aoi_entities_.find(eid);
  if(it == aoi_entities_.end()) {
    return nullptr;
  }
  else {
    return it->second;
  }
}

GW_GAME_END