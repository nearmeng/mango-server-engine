/*
 * Copyright(c) 2018 Tencent Games, Inc.
 * All Rights Reserved.
 *
 * Author: tktian <tktian@tencent.com>
 */
#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "great_world_base/common/vector3.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

namespace gw {
namespace game {

class AoiImp {
 public:
  AoiImp() {}
  virtual ~AoiImp() {}

  virtual bool Enter(uint64_t guid, const Vector3& pos) = 0;
  virtual void Leave(uint64_t guid) = 0;
  virtual void Move(uint64_t guid, const Vector3& pos) = 0;
  virtual bool AddTrigger(uint64_t guid, uint32_t range, AOIFUNC on_enter, AOIFUNC on_leave) = 0;
  virtual void RemoveTrigger(uint64_t guid, uint32_t range) = 0;
  virtual void DebugOutput() = 0;
};

}  // end namespace game
}  // end namespace gw
