#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_imp.h"
#include "great_world_base/common/simple_simd_hash_map.hpp"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN

class AoIEntity;
class AoIManager {
 public:
  AoIManager();
  ~AoIManager();
  AoIEntity* Enter(uint64_t eid, Vector3& pos);
  void Leave(uint64_t eid);

  void Move(uint64_t eid, Vector3& pos);

  bool AddTrigger(uint64_t eid, uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave);
  void RemoveTrigger(uint64_t eid, uint32_t radius);

  AoIEntity* GetAoIEntity(uint64_t eid);

  inline std::size_t entity_size() const { return aoi_entities_.size(); }

  void DebugOutput() { return imp_->DebugOutput(); }

 private:
  AoiImp* imp_;
  QUICK_HASH_MAP<uint64_t, AoIEntity*> aoi_entities_;
};

GW_GAME_END