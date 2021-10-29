#pragma once
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class CellNode;
class EvenlyDividedNode;
class StripNode;
class StripGroupNode;
class FixedDividedNode;

class RegionNodeVisitor {
 public:
  virtual void Visit(CellNode& node) = 0;
  virtual void Visit(EvenlyDividedNode& node);
  virtual void Visit(StripNode& node);
  virtual void Visit(StripGroupNode& node);
  virtual void Visit(FixedDividedNode& node);
};

GW_GAME_END