#include "great_world_base/cellmgr/balance/fixed_divided_node.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(FixedDividedNode)

FixedDividedNode::FixedDividedNode() {
  type = RegionNodeType::kFixedDivided;
}

FixedDividedNode::~FixedDividedNode() {
  for (auto& it : nodes_) {
    it->Dispose();
  }
}

void FixedDividedNode::Clear() {
  for (auto& it : nodes_) {
    it->Dispose();
  }
}

bool FixedDividedNode::Balance(bool tracing) {
  bool updated = false;
  for (auto node : nodes_) {
    if (node->Balance(tracing)) {
      updated = true;
    }
  }
  return updated;
}

bool FixedDividedNode::GAppBalance(ExcludeGameAppFilter& filter, bool tracing) {
  bool updated = false;
  for (auto node : nodes_) {
    if (node->GAppBalance(filter, tracing)) {
      updated = true;
    }
  }
  return updated;
}

bool FixedDividedNode::AssignGameApps(ExcludeGameAppFilter& filter) {
  for (auto node : nodes_) {
    if (!node->AssignGameApps(filter)) {
      GW_LOG_INFO("not enough apps to assign FixedDividedNode:%d", nodes_.size());
      return false;  // 如果没有足够的game app，按创建失败处理
    }
  }
  return true;
}

bool FixedDividedNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << static_cast<uint8_t>(RegionNodeType::kFixedDivided);
  os << id_;
  for (auto node : nodes_) {
    os << node->rect();
    if (!node->WriteToStream(os, update_only, detail_for_viewer))
      return false;
  }
  return true;
}

bool FixedDividedNode::CheckLayout(GWVector<RegionNode*> nodes) {
  // 应该属于低频操作，暴力判断是否有重叠
  for (auto nodeA : nodes) {
    for (auto nodeB : nodes) {
      if (!((nodeA == nodeB) ||
            MathUtility::IsMoreEqu(nodeA->rect().minx(), nodeB->rect().maxx()) ||
            MathUtility::IsMoreEqu(nodeA->rect().miny(), nodeB->rect().maxy()) ||
            MathUtility::IsMoreEqu(nodeB->rect().minx(), nodeA->rect().maxx()) ||
            MathUtility::IsMoreEqu(nodeB->rect().miny(), nodeA->rect().maxy()))) {
        GW_LOG_DEBUG("FixedDividedNode Area Cover FAIL:%s=%s", nodeA->to_string().c_str(), nodeB->to_string().c_str());
        return false;
      }
    }
  }
  // 判断下是否已经把rect装满
  float areaSum = 0;
  for (auto nodeA : nodes) {
    areaSum += nodeA->rect().Area();
  }
  if (!MathUtility::IsEqual(areaSum, rect_.Area())) {
    GW_LOG_DEBUG("FixedDividedNode Area SUM FAIL:%f,%f", areaSum, rect_.Area());
    return false;
  }
  return true;
}

bool FixedDividedNode::ReadFromStream(IMemoryIStream& in) {
  GW_LOG_TRACE("FixedDividedNode begin");
  Clear();
  Rect sub_rect;
  while (!in.empty()) {
    in >> sub_rect;

    if (MathUtility::IsMore(rect_.minx(), sub_rect.minx()) ||
        MathUtility::IsMore(sub_rect.maxx(), rect_.maxx()) ||
        MathUtility::IsMore(rect_.miny(), sub_rect.miny()) ||
        MathUtility::IsMore(sub_rect.maxy(), rect_.maxy())) {
      GW_LOG_DEBUG("FixedDividedNode Area ERROR");
      return false;
    }
    RegionNode* node = RegionNode::Parse(in);
    if (!node) {
      GW_LOG_DEBUG("FixedDividedNode Parse Fail");
      return false;
    }
    node->set_rect(sub_rect);
    node->set_parent(this);
    nodes_.push_back(node);
    minx_to_nodes_[sub_rect.minx()].push_back(node);
  }

  if (!CheckLayout(nodes_)) {
    Clear();
    return false;
  }
  return true;
}

void FixedDividedNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void FixedDividedNode::VisitNodes(RegionNodeVisitor& visitor) {
  for (auto node : nodes_) {
    node->Accept(visitor);
  }
}

// 备选节点里面，根据 y 选一个最接近的，然后取出这个最接近的 MaxX 值找这个x对应的y
GameAppEntry* FixedDividedNode::FindCell(float x, float y) {
  if (MathUtility::IsMore(y, rect_.maxy_) || MathUtility::IsMore(rect_.miny_, y) || MathUtility::IsMore(x, rect_.maxx_) || MathUtility::IsMore(rect_.minx_, x)) {
    GW_LOG_INFO("FixedDividedNode FindCell fail:%d,%d,%d", id_, x, y);
    return nullptr;
  }

  float currectX = 0;
  for (uint32_t i = 0; i < nodes_.size() || MathUtility::IsMoreEqu(currectX, rect_.maxx_); i++) {
    auto targetNodes = minx_to_nodes_.find(currectX);
    for (auto node : targetNodes->second) {
      if (MathUtility::IsMoreEqu(y, node->rect().miny()) && MathUtility::IsMore(node->rect().maxy(), y)) {
        if (MathUtility::IsMoreEqu(node->rect().maxx(), x)) {
          return node->FindCell(x, y);
        }
        currectX = node->rect().maxx();
        break;
      }
    }
  }

  return nullptr;
}

GW_CELL_MGR_END