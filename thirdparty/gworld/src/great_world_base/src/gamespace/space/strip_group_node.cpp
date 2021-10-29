#include "great_world_base/gamespace/space/strip_group_node.h"

#include "great_world_base/gamespace/space/region_node_visitor.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/strip_node.h"
GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(StripGroupNode)

StripGroupNode::StripGroupNode() {
}

StripGroupNode::~StripGroupNode() {
}

void StripGroupNode::Destroy() {
  for (auto subNode : nodes_) {
    subNode->Destroy();
  }
  Dispose();
}
bool StripGroupNode::ReadFromStream(IMemoryIStream& in) {
  uint8_t v;  // vertical
  in >> v;
  vertical_ = v ? true : false;
  uint16_t n;  // strip num
  in >> n;
  GW_RETURN_IF_ERROR(n <= 0, false, "invalid strip node num : %d", n);
  GW_LOG_TRACE("strip num=%d", n);
  StripNode* strip = nullptr;
  uint64_t sid = 0;
  for (uint16_t i = 0; i < n; ++i) {
    in >> sid;
    strip = ShmNew<StripNode>(this);
    nodes_.emplace_back(strip);
    strip->set_id(sid);
    strip->set_rect(rect_);  // 边界后续调整
    strip->set_vertical(!vertical_);
    space_->AddNonCellNode(strip);
  }
  float edge;
  // 调整区域边界
  for (uint16_t i = 0, l = n - 1; i < l; ++i) {
    in >> edge;
    nodes_[i]->mutable_rect().set_max(edge, vertical_);
    nodes_[i + 1]->mutable_rect().set_min(edge, vertical_);
  }
  // 处理每个strip node的数据
  bool result = true;
  for (uint16_t i = 0; i < n; ++i) {
    if (!nodes_[i]->ReadFromStream(in)) {
      result = false;
    }
    GW_LOG_TRACE("strip %d with %d cells", i, nodes_[i]->num_nodes());
  }
  return result;
}

bool StripGroupNode::UpdateFromStream(IMemoryIStream& in) {
  uint8_t v;  // vertical
  in >> v;
  vertical_ = v ? true : false;
  uint16_t n;  // strip num
  in >> n;
  GW_RETURN_IF_ERROR(n <= 0, false, "invalid strip node num : %d", n);
  uint16_t curr_num = num_strips();
  uint64_t sid = 0;
  bool exist = true;
  int curr_idx = 0;  // index for current nodes
  // 新的strip id数组与当前的对比，增加或者删除
  for (uint16_t i = 0; i < n; ++i) {
    in >> sid;
    exist = false;
    for (uint16_t j = curr_idx; j < curr_num; ++j) {
      if (sid == nodes_[j]->id()) {
        nodes_[j]->set_keep(true);
        exist = true;
        // 删除跳过的节点
        if (j > curr_idx) {
          for (uint16_t k = curr_idx; k < j; ++k) {
            space_->RemoveNonCellNode(nodes_[k]->id());
            nodes_[k]->Destroy();
          }
          nodes_.erase(nodes_.begin() + curr_idx, nodes_.begin() + j);
        }
        ++curr_idx;
        break;
      }
    }
    if (!exist) {  // 插入新的节点
      StripNode* strip = ShmNew<StripNode>(this);
      nodes_.insert(nodes_.begin() + curr_idx, strip);
      strip->set_rect(rect_);  // miny, maxy后续调整
      strip->set_vertical(!vertical_);
      ++curr_idx;
    }
    curr_num = num_strips();
  }
  // G6_ASSERT(n == curr_num);
  if (n < curr_num) {
    for (uint16_t i = n; i < curr_num; ++i) {
      space_->RemoveNonCellNode(nodes_[i]->id());
      nodes_[i]->Destroy();
    }
    nodes_.erase(nodes_.begin() + n, nodes_.begin() + curr_num);
    curr_num = num_strips();
  }

  float edge;
  // 调整区域边界
  for (uint16_t i = 0, l = curr_num - 1; i < l; ++i) {
    in >> edge;
    nodes_[i]->mutable_rect().set_max(edge, vertical_);
    nodes_[i + 1]->mutable_rect().set_min(edge, vertical_);
  }
  // 更新每个strip node
  for (auto it : nodes_) {
    it->UpdateFromStream(in);
  }
  return true;
}

void StripGroupNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void StripGroupNode::VisitNodes(RegionNodeVisitor& visitor) {
  for (auto it : nodes_) {
    it->Accept(visitor);
  }
}

GW_GAME_END