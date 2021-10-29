#include "great_world_base/gamespace/space/region_node_visitor.h"

#include "great_world_base/gamespace/space/evenly_divided_node.h"
#include "great_world_base/gamespace/space/fixed_divided_node.h"
#include "great_world_base/gamespace/space/strip_group_node.h"
#include "great_world_base/gamespace/space/strip_node.h"

GW_GAME_BEGIN

void RegionNodeVisitor::Visit(EvenlyDividedNode& node) {
  node.VisitNodes(*this);
}

void RegionNodeVisitor::Visit(StripNode& node) {
  node.VisitNodes(*this);
}

void RegionNodeVisitor::Visit(StripGroupNode& node) {
  node.VisitNodes(*this);
}

void RegionNodeVisitor::Visit(FixedDividedNode& node) {
  node.VisitNodes(*this);
}

GW_GAME_END