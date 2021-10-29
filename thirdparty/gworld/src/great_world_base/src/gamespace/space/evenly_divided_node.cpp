#include "great_world_base/gamespace/space/evenly_divided_node.h"

#include "great_world_base/gamespace/space/region_node_visitor.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(EvenlyDividedNode)

EvenlyDividedNode::EvenlyDividedNode() {
}

EvenlyDividedNode::~EvenlyDividedNode() {
}

void EvenlyDividedNode::Destroy() {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      if (nodes_[i][j]) {
        nodes_[i][j]->Destroy();
      }
    }
  }
  nodes_.clear();
  Dispose();
}
bool EvenlyDividedNode::ReadFromStream(IMemoryIStream& in) {
  in >> rows_ >> cols_;
  GW_LOG_TRACE("rows_=%d cols_=%d", rows_, cols_);
  auto row_val = GWVector<RegionNode*>(cols_);
  nodes_ = GWVector<GWVector<RegionNode*>>(rows_, row_val);

  float w = rect_.width() / cols_;
  float h = rect_.height() / rows_;
  float x = 0;
  float y = 0;
  for (int i = 0; i < rows_; ++i) {
    y = rect_.miny() + i * h;
    for (int j = 0; j < cols_; ++j) {
      x = rect_.minx() + j * w;
      Rect sub_rect(x, y, x + w, y + h);
      RegionNode* node = RegionNode::Parse(in, space_, &sub_rect);
      GW_RETURN_IF_ERROR(node == nullptr, false, "failed to parse region node at %d, %d", i, j);
      node->set_rect(sub_rect);
      node->set_parent(this);
      nodes_[i][j] = node;
    }
  }
  return true;
}

bool EvenlyDividedNode::UpdateFromStream(IMemoryIStream& in) {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      RegionNode::Parse(in, space_);  // 逐一更新子区域
    }
  }
  return true;
}

void EvenlyDividedNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void EvenlyDividedNode::VisitNodes(RegionNodeVisitor& visitor) {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      nodes_[i][j]->Accept(visitor);
    }
  }
}

GW_GAME_END