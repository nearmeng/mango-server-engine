#pragma once
#include "great_world_base/gamespace/cell_entity/cell_entity_interface.h"
#include "great_world_base/common/common.h"
#include "great_world_base/gamespace/space/space_resource.h"

GW_GAME_BEGIN

class ISpace {
 public:
  // 只需要处理 real 的，ghost 的部分内部会自动处理掉
  virtual std::pair<ICellEntity*, RetCode> Enter(uint64_t eid, uint64_t base_eid, uint64_t base_gapp_id, Vector3 pos,
                                                 ICellEntityAdapter* apapter) = 0;

  // 主动离开space
  virtual void Leave(ICellEntity*) = 0;

  virtual void DebugOutput() = 0;

  // 迁移
  virtual bool set_ghost_range(float range) = 0;

  // resource 
  virtual void set_resource_mask(uint32_t resource_mask) = 0;
  virtual SpaceResource* space_resource() = 0;
};

GW_GAME_END