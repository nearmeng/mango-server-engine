#include "great_world_base/gamespace/space/fixed_divided_node.h"

#include "great_world_base/gamespace/space/region_node_visitor.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(FixedDividedNode)

void FixedDividedNode::Destroy() {
  for (auto subNode : nodes_) {
    subNode->Destroy();
  }
  Dispose();
}

bool FixedDividedNode::ReadFromStream(IMemoryIStream& in) {
  while (!in.empty()) {
    Rect subRect;
    in >> subRect;
    RegionNode* node = RegionNode::Parse(in, space_);
    if (node == nullptr) {
      return false;
    }
    node->set_parent(this);
    node->set_rect(subRect);
    nodes_.push_back(node);
  }
  return true;
}

bool FixedDividedNode::UpdateFromStream(IMemoryIStream& in) {
  // 这个节点不支持更新
  return false;
}

void FixedDividedNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void FixedDividedNode::VisitNodes(RegionNodeVisitor& visitor) {
  for (auto node : nodes_) {
    node->Accept(visitor);
  }
}

GW_GAME_END