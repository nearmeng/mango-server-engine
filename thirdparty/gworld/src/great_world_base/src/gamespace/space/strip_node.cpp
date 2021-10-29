#include "great_world_base/gamespace/space/strip_node.h"

#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/region_node_visitor.h"
#include "great_world_base/gamespace/space/space.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(StripNode)

StripNode::StripNode() {
}

StripNode::StripNode(RegionNode* parent) : RegionNode(parent) {
  if (parent_) {
    space_ = parent_->space();
  }
}

StripNode::~StripNode() {
}

void StripNode::Destroy() {
  for (auto subNode : nodes_) {
    subNode->Destroy();
  }
  Dispose();
}

bool StripNode::ReadFromStream(IMemoryIStream& in) {
  uint16_t n;
  in >> n;  // cell node num
  GW_RETURN_IF_ERROR(n <= 0, false, "invalid cell node num : %d", n);
  CellNode* node = nullptr;
  // 处理cell的信息
  for (int i = 0; i < n; ++i) {
    node = static_cast<CellNode*>(RegionNode::Parse(in, space_));
    nodes_.emplace_back(node);
    node->set_parent(this);
    node->set_rect(rect_);  // minx, maxx 后续调整
  }
  float edge;
  // 处理前n-1个cell node的区域
  for (int i = 0, l = n - 1; i < l; ++i) {
    in >> edge;
    nodes_[i]->mutable_rect().set_max(edge, vertical_);
    nodes_[i + 1]->mutable_rect().set_min(edge, vertical_);
  }
  return true;
}

bool StripNode::UpdateFromStream(IMemoryIStream& in) {
  uint16_t n;  // cell num
  in >> n;
  GW_RETURN_IF_ERROR(n <= 0, false, "invalid cell node num : %d", n);
  uint16_t curr_num = num_nodes();
  bool exist = true;
  int curr_idx = 0;  // index for current nodes
  uint64_t gapp_id = 0;
  // 新的cell gapp_id数组与当前的对比，增加或者删除
  for (uint16_t i = 0; i < n; ++i) {
    CellNode* node = static_cast<CellNode*>(RegionNode::Parse(in, space_));
    gapp_id = node->main_gapp_id();
    exist = false;
    for (uint16_t j = curr_idx; j < curr_num; ++j) {
      if (gapp_id == nodes_[j]->main_gapp_id()) {
        nodes_[j]->set_keep(true);
        exist = true;
        // 删除跳过的节点
        if (j > curr_idx) {
          for (uint16_t k = curr_idx; k < j; ++k) {
            space_->RemoveCellNode(nodes_[k]->main_gapp_id());
            nodes_[k]->Destroy();
          }
          nodes_.erase(nodes_.begin() + curr_idx, nodes_.begin() + j);
        }
        ++curr_idx;
        break;
      }
    }
    if (!exist) {  // 插入新的节点
      nodes_.insert(nodes_.begin() + curr_idx, node);
      node->set_parent(this);
      node->set_rect(rect_);  // minx, maxx后续调整
      ++curr_idx;
    }
    curr_num = num_nodes();
  }
  // G6_ASSERT(n == curr_num);
  if (n < curr_num) {
    for (uint16_t i = n; i < curr_num; ++i) {
      space_->RemoveCellNode(nodes_[i]->main_gapp_id());
      nodes_[i]->Destroy();
    }
    nodes_.erase(nodes_.begin() + n, nodes_.end());
    curr_num = num_nodes();
  }

  float edge;
  // 调整区域边界
  for (uint16_t i = 0, l = curr_num - 1; i < l; ++i) {
    in >> edge;
    nodes_[i]->mutable_rect().set_max(edge, vertical_);
    nodes_[i + 1]->mutable_rect().set_min(edge, vertical_);
  }
  return true;
}

void StripNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void StripNode::VisitNodes(RegionNodeVisitor& visitor) {
  for (auto it : nodes_) {
    it->Accept(visitor);
  }
}

GW_GAME_END
