#include "great_world_base/cellmgr/balance/region_node.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/evenly_divided_node.h"
#include "great_world_base/cellmgr/balance/fixed_divided_by_cell_node.h"
#include "great_world_base/cellmgr/balance/fixed_divided_node.h"
#include "great_world_base/cellmgr/balance/strip_group_node.h"
#include "great_world_base/common/common_private.h"

GW_CELL_MGR_BEGIN

RegionNode::RegionNode() {
}

RegionNode::RegionNode(const Rect& rect, RegionNode* parent) : rect_(rect), parent_(parent) {
}

RegionNode::RegionNode(RegionNode* parent) : parent_(parent) {
  if (parent_) {
    space_ = parent_->space();
  }
}

RegionNode::RegionNode(const RegionNode& rhs) {
  id_ = rhs.id_;
  rect_ = rhs.rect_;
  space_ = rhs.space_;
  parent_ = rhs.parent_;
}

RegionNode::~RegionNode() {
}

RegionNode* RegionNode::Parse(IMemoryIStream& in, const Rect* root_rect) {
  if (in.empty()) {
    return nullptr;
  }
  uint8_t node_type_num = 0;
  in >> node_type_num;
  RegionNodeType node_type = static_cast<RegionNodeType>(node_type_num);
  RegionNode* node;
  if (node_type == RegionNodeType::kNone) {
    return nullptr;
  } else if (node_type == RegionNodeType::kEvenlyDivided) {
    node = ShmNew<EvenlyDividedNode>();
  } else if (node_type == RegionNodeType::kCell) {
    node = ShmNew<CellNode>();
  } else if (node_type == RegionNodeType::kStripGroup) {
    node = ShmNew<StripGroupNode>();
  } else if (node_type == RegionNodeType::kFixedDivided) {
    node = ShmNew<FixedDividedNode>();
  } else if (node_type == RegionNodeType::kFixedDividedByCell) {
    node = ShmNew<FixedDividedByCellNode>();
  } else {
    GW_LOG_ERROR("invalid node type : %d", node_type_num);
    return nullptr;
  }
  if (root_rect) {
    node->set_rect(*root_rect);
  }
  if (node) {
    node->ReadFromStream(in);
  }
  return node;
}

GW_CELL_MGR_END