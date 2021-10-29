#include "great_world_base/cellmgr/balance/balance_node.h"

GW_CELL_MGR_BEGIN

BalanceNode::BalanceNode() {
}

BalanceNode::~BalanceNode() {
}

BalanceNode::BalanceNode(const Rect& rect, RegionNode* parent) : RegionNode(rect, parent) {
}

BalanceNode::BalanceNode(RegionNode* parent) : RegionNode(parent) {
}

BalanceNode::BalanceNode(const RegionNode& rhs) : RegionNode(rhs) {
}

GW_CELL_MGR_END