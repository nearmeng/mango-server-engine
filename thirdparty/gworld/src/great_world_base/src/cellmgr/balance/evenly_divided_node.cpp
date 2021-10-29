#include "great_world_base/cellmgr/balance/evenly_divided_node.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/game_app_entry.h"

GW_CELL_MGR_BEGIN

GWCLASS_DECL_IMPLEMENT(EvenlyDividedNode)

EvenlyDividedNode::EvenlyDividedNode() {
  type = RegionNodeType::kEvenlyDivided;
}

EvenlyDividedNode::EvenlyDividedNode(const Rect& rect, uint16_t rows, uint16_t cols, RegionNode* parent) : RegionNode(rect, parent), rows_(rows), cols_(cols) {
  auto row_val = GWVector<RegionNode*>(cols);
  nodes_ = GWVector<GWVector<RegionNode*>>(rows, row_val);
  float w = rect.width() / cols;
  float h = rect.height() / rows;
  float x = 0;
  float y = 0;
  for (int i = 0; i < rows; ++i) {
    y = rect.miny() + i * h;
    for (int j = 0; j < cols; ++j) {
      x = rect.minx() + j * w;
      nodes_[i][j] = ShmNew<CellNode>(Rect(x, y, x + w, y + h));
    }
  }
}

EvenlyDividedNode::~EvenlyDividedNode() {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      if (nodes_[i][j]) {
        nodes_[i][j]->Dispose();
        nodes_[i][j] = nullptr;
      }
    }
  }
  nodes_.clear();
}

bool EvenlyDividedNode::AssignGameApps(ExcludeGameAppFilter& filter) {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      RegionNode* node = nodes_[i][j];
      if (!node->AssignGameApps(filter)) {
        GW_LOG_INFO("not enough apps to assign");
        return false;  // 如果没有足够的game app，按创建失败处理
      }
    }
  }
  return true;
}

bool EvenlyDividedNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << static_cast<uint8_t>(RegionNodeType::kEvenlyDivided);
  os << id_;
  if (!update_only) {
    os << rows_ << cols_;
  }
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      nodes_[i][j]->WriteToStream(os, update_only, detail_for_viewer);
    }
  }
  return true;
}

bool EvenlyDividedNode::ReadFromStream(IMemoryIStream& in) {
  GW_LOG_TRACE("begin");
  in >> rows_ >> cols_;
  GW_LOG_TRACE("rows_=%d cols_=%d", rows_, cols_);
  auto row_val = GWVector<RegionNode*>(cols_);
  nodes_ = GWVector<GWVector<RegionNode*>>(rows_, row_val);

  float w = rect_.width() / cols_;
  float h = rect_.height() / rows_;
  float x = 0;
  float y = 0;
  uint8_t more = 0;
  in >> more;
  for (int i = 0; i < rows_; ++i) {
    y = rect_.miny() + i * h;
    for (int j = 0; j < cols_; ++j) {
      x = rect_.minx() + j * w;
      Rect sub_rect(x, y, x + w, y + h);
      RegionNode* node = RegionNode::Parse(in);
      if (node == nullptr) {
        node = ShmNew<CellNode>(sub_rect);
      } else {
        node->set_rect(sub_rect);
      }
      node->set_parent(this);
      nodes_[i][j] = node;
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

bool EvenlyDividedNode::Balance(bool tracing) {
  bool updated = false;
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      if (nodes_[i][j]->Balance(tracing)) {
        updated = true;
      }
    }
  }
  return updated;
}

GW_CELL_MGR_END
