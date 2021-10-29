#pragma once
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/shm/shm_mgr.h"

GW_GAME_BEGIN

struct Managers {
  ShmPoolMgr* poll_mgr_;
  SpaceManager* space_mgr_;
  AoIWorld* aoi_world_;
};

GW_GAME_END