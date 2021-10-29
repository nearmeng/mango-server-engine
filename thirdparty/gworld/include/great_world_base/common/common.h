#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <type_traits>
#include "great_world_base/common/ret_code.h"

#define EXPORT_VISIBLE __attribute__((visibility ("default")))

namespace gw {

enum class RegionNodeType {
  kNone,
  kEvenlyDivided,
  kFixedDivided,
  kStripGroup,
  KStrip,
  kCell,
  kFixedDividedByCell,
};

enum class CellState : uint8_t {
  kNone,
  // 初始化，game端尚未创建
  kInit,
  // game端已创建
  kCreated,
  // 移除中
  kRemoving,
  // 已经被移除
  kRemoved,
  // 对应的game app崩溃
  kCrash,
};

enum class GameAppState {
  kStarting,
  kNormal,
  kShuttingDown
};


#define GW_GAME_BEGIN \
  namespace gw {      \
  namespace game {
#define GW_GAME_END \
  }                 \
  }

#define REALGHOST_NS_BEGIN \
  namespace gw {      \
  namespace game {
#define REALGHOST_NS_END \
  }                 \
  }
#define REALGHOST_NS gw::game

#define GW_CELL_MGR_BEGIN \
  namespace gw {       \
  namespace cellmgr {
#define GW_CELL_MGR_END \
  }                  \
  }  // namespace gw { namespace cellmgr {
#define USING_CELL_MGR using namespace gw::cellmgr;

}  // namespace gw
