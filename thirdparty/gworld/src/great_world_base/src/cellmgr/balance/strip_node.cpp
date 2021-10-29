#include "great_world_base/cellmgr/balance/strip_node.h"

#include <algorithm>
#include <cstdlib>

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/default_balance_policy.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/common/common_private.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(StripNode)

StripNode::StripNode() {
  type = RegionNodeType::KStrip;
}

StripNode::StripNode(RegionNode* parent) : BalanceNode(parent) {
}

StripNode::~StripNode() {
  for (auto it : nodes_) {
    it->Dispose();
  }
  nodes_.clear();
}

bool StripNode::AssignGameApps(ExcludeGameAppFilter& filter) {
  GameAppEntry* gapp = CELLMGR.SearchGood(filter);
  if (!gapp) {
    return false;
  }
  AppendCellNode(gapp);
  filter.Cache(gapp->id());
  return true;
}

bool StripNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  // StripNode肯定在StripGroupNode中，这里不再附加type与id
  uint16_t n = num_nodes();
  os << n;
  // 写入各节点的额外具体信息
  for (auto it : nodes_) {
    it->WriteToStream(os, update_only, detail_for_viewer);
  }
  // 写入边界
  for (uint16_t i = 0, l = n - 1; i < l; ++i) {
    os << nodes_[i]->rect().max(vertical_);
  }
  return true;
}

bool StripNode::ReadFromStream(IMemoryIStream& in) {
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

CellNode* StripNode::AppendCellNode(GameAppEntry* gapp) {
  G6_ASSERT(gapp);
  GW_LOG_DEBUG("%d", gapp->id());
  CellNode* node = ShmNew<CellNode>();
  node->set_parent(this);
  nodes_.emplace_back(node);
  node->set_main_gapp(gapp);
  node->set_rect(rect_);
  Rect& node_rect = node->mutable_rect();
  node_rect.set_max(rect_.max(vertical_), vertical_);
  node_rect.set_min(node_rect.max(vertical_), vertical_);
  return node;
}

CellNode* StripNode::InsertCellNode(GameAppEntry* gapp, uint16_t index) {
  G6_ASSERT(gapp);
  uint16_t n = num_nodes();
  if (n == 0) {
    return AppendCellNode(gapp);
  }
  if (index >= n) {
    GW_LOG_ERROR("invalid index : %d size : %d", index, n);
    return nullptr;
  }
  CellNode* node = AppendCellNode(gapp);
  CellNode* temp = nodes_[index];
  for (int i = n; i > index; --i) {
    nodes_[i] = nodes_[i - 1];
  }
  nodes_[index] = node;
  const Rect& next_rect = temp->rect();
  Rect& node_rect = node->mutable_rect();
  node_rect.set_max(next_rect.min(vertical_), vertical_);
  node_rect.set_min(node_rect.max(vertical_), vertical_);
  return node;
}

CellNode* StripNode::first_node() const {
  uint16_t n = num_nodes();
  return n > 0 ? nodes_[0] : nullptr;
}

CellNode* StripNode::last_node() const {
  uint16_t n = num_nodes();
  return n > 0 ? nodes_[n - 1] : nullptr;
}

void StripNode::ResizeCellRect() {
  if (nodes_.empty()) {
    GW_LOG_INFO("no nodes to resize");
    return;
  }
  if (!vertical_) {
    float cell_width = rect_.width() / nodes_.size();
    float miny = rect_.miny();
    float maxy = rect_.maxy();
    float minx = rect_.minx();
    float x;
    for (int i = 0, n = nodes_.size(); i < n; ++i) {
      x = minx + i * cell_width;
      nodes_[i]->set_rect(Rect(x, miny, x + cell_width, maxy));
    }
  } else {
    float cell_height = rect_.height() / nodes_.size();
    float minx = rect_.minx();
    float maxx = rect_.maxx();
    float miny = rect_.miny();
    float y;
    for (int i = 0, n = nodes_.size(); i < n; ++i) {
      y = miny + i * cell_height;
      nodes_[i]->set_rect(Rect(minx, y, maxx, y + cell_height));
    }
  }
}

float StripNode::curr_load() const {
  return curr_load_;
}

float StripNode::revised_load() const {
  return revised_load_;
}

float StripNode::conv_load() const {
  return conv_load_;
}

uint32_t StripNode::memsize() const {
  return memsize_;
}

bool StripNode::removing() const {
  return removing_;
}

void StripNode::set_removing(bool removing) {
  removing_ = removing;
}

bool StripNode::RecalcLoad() {
  curr_load_ = 0;
  revised_load_ = 0;
  conv_load_ = 0;
  memsize_ = 0;
  total_curr_load_ = 0;
  total_revised_load_ = 0;
  total_conv_load_ = 0;
  total_memsize_ = 0;
  num_entities_ = 0;
  uint16_t n = num_nodes();
  GW_RETURN_IF_ERROR(n <= 0, 0.0f, "empty strip");
  // 重算总负载与总平均负载
  for (auto it : nodes_) {
    total_curr_load_ += it->curr_load();
    total_revised_load_ += it->revised_load();
    total_conv_load_ += it->conv_load();
    total_memsize_ += it->memsize();
    num_entities_ += it->num_entities();
  }
  curr_load_ = total_curr_load_ / n;
  revised_load_ = total_revised_load_ / n;
  conv_load_ = total_conv_load_ / n;
  memsize_ = total_memsize_ / n;
  entity_rect_ = nodes_[0]->entity_rect();
  // tile区域x取外边界(!vertical_)
  tile_rect_ = nodes_[0]->tile_rect();
  tile_rect_.set_max(nodes_[n - 1]->tile_rect().max(vertical_), vertical_);
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    // 合并重算entity区域 (最外的边界)
    entity_rect_.Merge(node->entity_rect());
    // 合并重算tile区域 （最内的边界）
    const Rect& r = node->tile_rect();
    // tile区域y取内边界(!vertical_)
    if (!vertical_) {
      tile_rect_.set_miny(std::max(tile_rect_.miny(), r.miny()));
      tile_rect_.set_maxy(std::min(tile_rect_.maxy(), r.maxy()));
    } else {
      tile_rect_.set_minx(std::max(tile_rect_.minx(), r.minx()));
      tile_rect_.set_maxx(std::min(tile_rect_.maxx(), r.maxx()));
    }
  }

  return total_curr_load_;
}

bool StripNode::Balance(bool tracing) {
  uint16_t n = num_nodes();
  if (n <= 1) {
    return false;
  }
  bool updated = false;
  DefaultBalancePolicy policy(vertical_, tracing);
  BalanceResult result;
  for (uint16_t i = balance_first_ ? 0 : 1, l = n - 1; i < l; i += 2) {
    CellNode* curr_node = nodes_[i];
    CellNode* next_node = nodes_[i + 1];
    GameAppEntry* curr_gapp = curr_node->main_gapp();
    GameAppEntry* next_gapp = next_node->main_gapp();
    if (!curr_gapp->available() || !next_gapp->available()) {
      continue;
    }
    if (policy.Balance(curr_node, next_node, result)) {
      updated = true;
      if (tracing) {
        GW_LOG_TRACE("[balance] space_id:%d gapp:%d - %d, edge:%f->%f",
                   space_->id(), curr_gapp->id(), next_gapp->id(), result.old_edge(), result.edge());
      }
    }
    // 一次最多删除一个cell，这里先标记
    if (curr_node->CanDelete()) {
      curr_node->set_main_state(CellState::kRemoved);
      updated = true;
    } else if (next_node->CanDelete()) {
      next_node->set_main_state(CellState::kRemoved);
      updated = true;
    }
  }
  // 删除标记的cell node
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    if ((*it)->main_state() == CellState::kRemoved) {
      if (tracing) {
        GW_LOG_TRACE("[balance] space_id:%d remove gapp:%d", space_->id(), (*it)->main_gapp()->id());
      }
      CellNode* remove_node = *it;
      nodes_.erase(it);
      CELLMGR.UpdateLayout(space_->id());  // 保证delete节点的game更新到删除后的layout，自动删除space
      remove_node->Dispose();
      unsigned long t_now = GetWalltimeMs();
      space_->set_last_remove_cell_time(t_now);
      space_->set_last_remove_time(t_now);
      break;
    }
  }
  balance_first_ = !balance_first_;
  return updated;
}

CellNode* StripNode::SearchMinLoadNode(uint16_t& idx) const {
  uint16_t n = num_nodes();
  if (!n) {
    return nullptr;
  }
  CellNode* result = nodes_[0];
  float min_load = result->conv_load();
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    if (node->conv_load() < min_load) {
      min_load = node->conv_load();
      result = node;
      idx = i;
    }
  }
  return result;
}

CellNode* StripNode::SearchMaxLoadNodeWithMin(uint16_t& idx, CellNode** found_min_load_node) const {
  uint16_t n = num_nodes();
  *found_min_load_node = nullptr;
  if (!n) {
    return nullptr;
  }
  CellNode* result = nodes_[0];
  CellNode* min_load_node = result;
  float min_load = result->conv_load();
  float max_load = result->conv_load();
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    if (node->conv_load() < min_load) {
      min_load = node->conv_load();
      min_load_node = node;
    }
    if (node->conv_load() > max_load) {
      max_load = node->conv_load();
      result = node;
      idx = i;
    }
  }
  *found_min_load_node = min_load_node;
  return result;
}

bool StripNode::SearchLowLoadAdjacentNodes(float load, uint16_t& idx, CellNode** found_node, CellNode** found_next_node) const {
  uint16_t n = num_nodes();
  if (n <= 1) {
    return false;
  }
  for (uint16_t i = 0, l = n - 1; i < l; ++i) {
    CellNode* node = nodes_[i];
    CellNode* next = nodes_[i + 1];
    if (node->conv_load() < load && next->conv_load() < load) {
      *found_node = node;
      *found_next_node = next;
      idx = i;
      return true;
    }
  }
  return false;
}

CellNode* StripNode::SearchLowLoadNodeToRemove(float load, uint16_t& idx) const {
  uint16_t n = num_nodes();
  if (!n) {
    return nullptr;
  } else if (n <= 2) {
    uint16_t ridx = 0;
    CellNode* node = SearchMinLoadNode(ridx);
    if (node->conv_load() < load && !node->init_expanding()) {
      idx = ridx;
      return node;
    }
  } else if (nodes_[0]->conv_load() < load && nodes_[1]->conv_load() < load) {  // 考虑最边上
    idx = 0;
    return nodes_[0];
  } else if (nodes_[n - 2]->conv_load() < load && nodes_[n - 1]->conv_load() < load && !nodes_[n - 2]->init_expanding() && !nodes_[n - 1]->init_expanding()) {  // 考虑最边上
    idx = n - 1;
    return nodes_[n - 1];
  } else {
    // 临近3个节点负载都小于load，取中间
    for (uint16_t i = 2, l = n - 2; i < l; ++i) {
      CellNode* prev = nodes_[i - 1];
      CellNode* node = nodes_[i];
      CellNode* next = nodes_[i + 1];
      if (prev->conv_load() < load && node->conv_load() < load && next->conv_load() < load && !prev->init_expanding() && !node->init_expanding() && !node->init_expanding()) {
        idx = i;
        return node;
      }
    }
  }
  return nullptr;
}

CellNode* StripNode::SearchMinMemsizeNode(uint16_t& idx) const {
  uint16_t n = num_nodes();
  if (!n) {
    return nullptr;
  }
  CellNode* result = nodes_[0];
  uint32_t min_mem = result->memsize();
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    if (node->memsize() < min_mem) {
      min_mem = node->memsize();
      result = node;
      idx = i;
    }
  }
  return result;
}

CellNode* StripNode::SearchMaxMemsizeNodeWithMin(uint16_t& idx, CellNode** found_min_men_node) const {
  uint16_t n = num_nodes();
  *found_min_men_node = nullptr;
  if (!n) {
    return nullptr;
  }
  CellNode* result = nodes_[0];
  CellNode* min_mem_node = result;
  uint32_t min_mem = result->memsize();
  uint32_t max_mem = result->memsize();
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    if (node->memsize() < min_mem) {
      min_mem = node->memsize();
      min_mem_node = node;
    }
    if (node->memsize() > max_mem) {
      max_mem = node->memsize();
      result = node;
      idx = i;
    }
  }
  *found_min_men_node = min_mem_node;
  return result;
}

CellNode* StripNode::SearchMinTileAreaNode(uint16_t& idx) const {
  uint16_t n = num_nodes();
  if (!n) {
    return nullptr;
  }
  CellNode* result = nodes_[0];
  float min_area = result->rect().Area();
  for (uint16_t i = 1; i < n; ++i) {
    CellNode* node = nodes_[i];
    float area = node->rect().Area();
    if (area < min_area) {
      min_area = area;
      result = node;
      idx = i;
    }
  }
  return result;
}

bool StripNode::IsSomeCellRemoving() {
  for (auto it : nodes_) {
    if (it->removing()) {
      return true;
    }
  }
  return false;
}

bool StripNode::CanDelete() const {
  return removing() && !rect_.HasArea() && num_entities_ == 0;
}

GW_CELL_MGR_END