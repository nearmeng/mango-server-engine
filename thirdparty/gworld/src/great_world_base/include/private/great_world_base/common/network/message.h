#pragma once
#include "great_world_base/common/common_private.h"

namespace gw {

enum NetworkMessage : uint32_t {
  // cell 之间
  RequestMigrateAvatarNum = 0,
  ResponseMigrateAvatarNum = 1,
  NotityCellStateChanged = 2,
  OnNotityCellStateChanged = 3,
  CreateGhost = 4,
  DeleteGhost = 5,
  EntityMigrate = 6,
  UpdateGhostDstAppId = 7,
  UpdateGhostRealAppId = 8,
  NotifyToGhost = 9,
  TelportTo = 10,

  // cellmgr -> cell
  AddCell = 10000,
  DestroyCell = 10001,
  UpdateSpaceLayout = 10002,
  AllCellCreated = 10003,

  // cell -> cellmgr
  ConfirmAddCell = 20000,
  ReportLoad = 20001
};

}
