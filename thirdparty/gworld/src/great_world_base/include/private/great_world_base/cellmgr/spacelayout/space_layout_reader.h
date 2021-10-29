#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

struct SpaceLayoutData {
  float width_;
  float height_;
  std::shared_ptr<const std::vector<char>> data_;
};

class ISpaceLayoutReader {
 public:
  virtual std::shared_ptr<SpaceLayoutData> LoadSpaceLayout(const GWString& name) = 0;
};
GW_CELL_MGR_END