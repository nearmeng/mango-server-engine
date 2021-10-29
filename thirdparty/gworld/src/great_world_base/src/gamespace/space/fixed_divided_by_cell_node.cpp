#include "great_world_base/gamespace/space/fixed_divided_by_cell_node.h"

#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/space.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(FixedDividedByCellNode)

bool FixedDividedByCellNode::UpdateFromStream(IMemoryIStream& in) {
  GWVector<RegionNode*> new_nodes;
  while (!in.empty()) {
    Rect sub_rect;
    in >> sub_rect;
    CellNode* node = static_cast<CellNode*>(RegionNode::Parse(in, space_));
    if (node == nullptr) {
      return false;
    }
    if (node->keep()) {
      if (!sub_rect.Equal(node->rect())) {
        node->set_rect(sub_rect);
      }
    } else {
      node->set_parent(this);
      node->set_rect(rect_);
    }
    GW_LOG_TRACE("UpdateFromStream node_id %d keep %d", node->id(), node->keep());
    new_nodes.push_back(node);
  }
  GW_LOG_TRACE("FixedDividedByCellNode UpdateFromStream end:%d,%d", nodes_.size(), new_nodes.size());
  nodes_.swap(new_nodes);

  return true;
}

GW_GAME_END
