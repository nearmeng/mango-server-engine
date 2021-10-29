#pragma once
#include "great_world_base/cellmgr/basemgr.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/spacelayout/space_layout_mgr.h"
#include "great_world_base/common/shm/shm_mgr.h"

GW_CELL_MGR_BEGIN

struct Managers {
  BaseMgr* base_mgr_;
  CellMgr* cell_mgr_;
  SpaceLayoutMgr* space_layout_;
  ShmPoolMgr* poll_mgr_;
};

GW_CELL_MGR_END