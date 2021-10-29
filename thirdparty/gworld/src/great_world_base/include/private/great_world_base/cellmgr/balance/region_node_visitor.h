#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

class CellNode;
class EvenlyDividedNode;
class StripNode;
class StripGroupNode;
class FixedDividedNode;

class RegionNodeVisitor {
 public:
  virtual void Visit(CellNode& node) = 0;
  virtual void Visit(EvenlyDividedNode& node) = 0;
  virtual void Visit(StripNode& node) = 0;
  virtual void Visit(StripGroupNode& node) = 0;
  virtual void Visit(FixedDividedNode& node) = 0;
};

GW_CELL_MGR_END
