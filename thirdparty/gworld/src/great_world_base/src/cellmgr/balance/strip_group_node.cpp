#include "great_world_base/cellmgr/balance/strip_group_node.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/default_balance_policy.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/balance/strip_node.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/common/common_private.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(StripGroupNode)

StripGroupNode::StripGroupNode() {
  type = RegionNodeType::kStripGroup;
}

StripGroupNode::~StripGroupNode() {
  for (auto it : nodes_) {
    it->Dispose();
  }
  nodes_.clear();
}

void StripGroupNode::set_vertical(bool vertical) {
  if (num_strips()) {
    GW_LOG_ERROR("cannot set vertical, it is not empty");
    return;
  }
  vertical_ = vertical;
}

StripNode* StripGroupNode::AppendStripNode(GameAppEntry* gapp) {
  StripNode* strip = ShmNew<StripNode>(this);
  nodes_.emplace_back(strip);
  strip->set_rect(rect_);           // miny, maxy 后续调整
  strip->set_vertical(!vertical_);  // 子区域的排布方向需要相反
  Rect& strip_rect = strip->mutable_rect();
  strip_rect.set_max(rect_.max(vertical_), vertical_);
  strip_rect.set_min(strip_rect.max(vertical_), vertical_);
  // strip自身的数据设置完再加cell，cell会用到strip数据
  strip->AppendCellNode(gapp);  // 每个strip至少包含一个CellNode
  return strip;
}

StripNode* StripGroupNode::InsertStripNode(GameAppEntry* gapp, uint16_t index) {
  G6_ASSERT(gapp);
  uint16_t n = num_strips();
  if (n == 0) {
    return AppendStripNode(gapp);
  }
  if (index >= n) {
    GW_LOG_ERROR("invalid index : %d size : %d", index, n);
    return nullptr;
  }
  StripNode* strip = AppendStripNode(gapp);
  StripNode* temp = nodes_[index];
  for (int i = n; i > index; --i) {
    nodes_[i] = nodes_[i - 1];
  }
  nodes_[index] = strip;
  const Rect& next_rect = temp->rect();
  Rect& strip_rect = strip->mutable_rect();
  strip_rect.set_max(next_rect.min(vertical_), vertical_);
  strip_rect.set_min(strip_rect.max(vertical_), vertical_);
  return strip;
}

bool StripGroupNode::AssignGameApps(ExcludeGameAppFilter& filter) {
  bool result = false;
  G6_ASSERT(min_strips_ > 0);
  G6_ASSERT(min_cells_per_strip_ > 0);
  // 先确保有尽可能多的strip——满足min_strips_的最低要求
  for (uint16_t i = 0, n = min_strips_; i < n; ++i) {
    GameAppEntry* gapp = filter.SearchSuitable();
    if (!gapp) {
      break;
    } else {
      result = true;  // 只要有1个就算成功
    }
    AppendStripNode(gapp);
    filter.Cache(gapp->id());
  }
  if (!result) {
    GW_LOG_ERROR("no suitable game app, no strip is created");
    return false;
  }
  uint16_t snum = num_strips();
  GW_LOG_INFO("strip num=%d", snum);
  // 调整每个strip的区域，比较多，不单独用set_min, set_max那几个rect的接口了
  if (vertical_) {
    float strip_height = rect_.height() / snum;
    nodes_[0]->mutable_rect().set_miny(rect_.miny());
    for (uint16_t i = 0, l = snum - 1; i < l; ++i) {
      float edge = rect_.miny() + strip_height * (i + 1);
      nodes_[i]->mutable_rect().set_maxy(edge);
      nodes_[i + 1]->mutable_rect().set_miny(edge);
    }
    nodes_[snum - 1]->mutable_rect().set_maxy(rect_.maxy());
  } else {
    float strip_width = rect_.width() / snum;
    nodes_[0]->mutable_rect().set_minx(rect_.minx());
    for (uint16_t i = 0, l = snum - 1; i < l; ++i) {
      float edge = rect_.minx() + strip_width * (i + 1);
      nodes_[i]->mutable_rect().set_maxx(edge);
      nodes_[i + 1]->mutable_rect().set_minx(edge);
    }
    nodes_[snum - 1]->mutable_rect().set_maxx(rect_.maxx());
  }
  // 每个strip依次分配一个game app，循环往复，直至满足min_cells_per_strip_的最低要求
  for (uint16_t j = 1; j < min_cells_per_strip_; ++j) {
    result = true;
    for (uint16_t i = 0; i < snum; ++i) {
      StripNode* strip = nodes_[i];
      if (!strip->AssignGameApps(filter)) {
        result = false;
        break;
      }
    }
    if (!result) {  // 如果没有合适的game app，结束分配
      break;
    }
  }
  //调整每个strip内的cell node的区域
  for (uint16_t i = 0; i < snum; ++i) {
    nodes_[i]->ResizeCellRect();
    GW_LOG_TRACE("strip %d with %d cells", i, nodes_[i]->num_nodes());
  }

  return true;
}

bool StripGroupNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << static_cast<uint8_t>(RegionNodeType::kStripGroup);
  os << id_;
  os << static_cast<uint8_t>(vertical_ ? 1 : 0);
  uint16_t n = num_strips();
  os << n;
  // 写入当前的节点id序列
  for (auto it : nodes_) {
    os << it->id();
  }
  // 写入分隔的边界
  for (uint16_t i = 0, l = n - 1; i < l; ++i) {
    os << nodes_[i]->rect().max(vertical_);
  }
  for (auto it : nodes_) {
    it->WriteToStream(os, update_only, detail_for_viewer);
  }
  return true;
}

bool StripGroupNode::ReadFromStream(IMemoryIStream& in) {
  in >> min_strips_ >> max_strips_;
  uint16_t min_val = 1;
  min_strips_ = std::max(min_val, std::min(min_strips_, max_strips_));
  max_strips_ = std::max(min_val, std::max(min_strips_, max_strips_));

  in >> min_cells_per_strip_ >> max_cells_per_strip_;
  min_cells_per_strip_ = std::max(min_val, std::min(min_cells_per_strip_, max_cells_per_strip_));
  max_cells_per_strip_ = std::max(min_val, std::max(min_cells_per_strip_, max_cells_per_strip_));

  uint8_t vertical = 0;
  in >> vertical;
  vertical_ = vertical ? true : false;

  GW_LOG_TRACE("strips=[%d, %d], cells_per_strip=[%d, %d]", min_strips_, max_strips_, min_cells_per_strip_, max_cells_per_strip_);

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

uint16_t StripGroupNode::GetNumCells() const {
  uint16_t num_cells = 0;
  for (auto it : nodes_) {
    num_cells += it->num_nodes();
  }
  return num_cells;
}

StripNode* StripGroupNode::SearchMinLoadNode(uint16_t& idx) const {
  uint16_t n = num_strips();
  if (!n) {
    return nullptr;
  }
  StripNode* result = nodes_[0];
  float min_load = result->conv_load();
  for (uint16_t i = 1; i < n; ++i) {
    StripNode* node = nodes_[i];
    if (node->conv_load() < min_load) {
      min_load = node->conv_load();
      result = node;
      idx = i;
    }
  }
  return result;
}

StripNode* StripGroupNode::SearchMaxLoadNodeWithMin(uint16_t& idx, StripNode** found_min_load_node) const {
  uint16_t n = num_strips();
  *found_min_load_node = nullptr;
  if (!n) {
    return nullptr;
  }
  StripNode* result = nodes_[0];
  StripNode* min_load_node = result;
  float min_load = result->conv_load();
  float max_load = result->conv_load();
  for (uint16_t i = 1; i < n; ++i) {
    StripNode* node = nodes_[i];
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

StripNode* StripGroupNode::SearchMinMemsizeNode(uint16_t& idx) const {
  uint16_t n = num_strips();
  if (!n) {
    return nullptr;
  }
  StripNode* result = nodes_[0];
  uint32_t min_mem = result->memsize();
  for (uint16_t i = 1; i < n; ++i) {
    StripNode* node = nodes_[i];
    if (node->memsize() < min_mem) {
      min_mem = node->memsize();
      result = node;
      idx = i;
    }
  }
  return result;
}

StripNode* StripGroupNode::SearchMaxMemsizeNodeWithMin(uint16_t& idx, StripNode** found_min_mem_node) const {
  uint16_t n = num_strips();
  *found_min_mem_node = nullptr;
  if (!n) {
    return nullptr;
  }
  StripNode* result = nodes_[0];
  StripNode* min_mem_node = result;
  uint32_t min_mem = result->memsize();
  uint32_t max_mem = result->memsize();
  for (uint16_t i = 1; i < n; ++i) {
    StripNode* node = nodes_[i];
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
  *found_min_mem_node = min_mem_node;
  return result;
}

StripNode* StripGroupNode::SearchMinTileAreaNode(uint16_t& idx) const {
  uint16_t n = num_strips();
  if (!n) {
    return nullptr;
  }
  StripNode* result = nodes_[0];
  float min_area = result->rect().Area();
  for (uint16_t i = 1; i < n; ++i) {
    StripNode* node = nodes_[i];
    float area = node->rect().Area();
    if (area < min_area) {
      min_area = area;
      result = node;
      idx = i;
    }
  }
  return result;
}

StripNode* StripGroupNode::SearchLowLoadNodeToRemove(float load, uint16_t& idx) const {
  uint16_t n = num_strips();
  if (!n) {
    return nullptr;
  } else if (n <= 2) {
    uint16_t ridx = 0;
    StripNode* node = SearchMinLoadNode(ridx);
    if (node->conv_load() < load) {
      idx = ridx;
      return node;
    }
  } else if (nodes_[0]->conv_load() < load && nodes_[1]->conv_load() < load && nodes_[0]->num_nodes() <= min_cells_per_strip_ && nodes_[1]->num_nodes() <= min_cells_per_strip_) {  // 考虑最边上
    idx = 0;
    return nodes_[0];
  } else if (nodes_[n - 2]->conv_load() < load && nodes_[n - 1]->conv_load() < load && nodes_[n - 2]->num_nodes() <= min_cells_per_strip_ && nodes_[n - 1]->num_nodes() <= min_cells_per_strip_) {  // 考虑最边上
    idx = n - 1;
    return nodes_[n - 1];
  } else {
    // 临近3个节点负载都小于load，取中间
    for (uint16_t i = 2, l = n - 2; i < l; ++i) {
      StripNode* prev = nodes_[i - 1];
      StripNode* node = nodes_[i];
      StripNode* next = nodes_[i + 1];
      if (prev->conv_load() < load && node->conv_load() < load && next->conv_load() < load && prev->num_nodes() <= min_cells_per_strip_ && node->num_nodes() <= min_cells_per_strip_ && next->num_nodes() <= min_cells_per_strip_) {
        idx = i;
        return node;
      }
    }
  }
  return nullptr;
}

bool StripGroupNode::IsSomeStripRemoving() const {
  for (auto it : nodes_) {
    if (it->removing()) {
      return true;
    }
  }
  return false;
}

void StripGroupNode::RecalcLoad() {
  cell_conv_load_ = 0;
  cell_memsize_ = 0;
  strip_conv_load_ = 0;
  strip_memsize_ = 0;
  total_conv_load_ = 0;
  total_memsize_ = 0;
  uint16_t n = num_strips();
  if (!n) {
    return;
  }
  uint16_t cell_num = 0;
  // 重算总负载与总平均负载
  for (auto it : nodes_) {
    it->RecalcLoad();
    total_conv_load_ += it->total_conv_load();
    total_memsize_ += it->total_memsize();
    cell_num += it->num_nodes();
  }
  if (!cell_num) {
    return;
  }
  cell_conv_load_ = total_conv_load_ / cell_num;
  cell_memsize_ = total_memsize_ / cell_num;
  strip_conv_load_ = total_conv_load_ / n;
  strip_memsize_ = total_memsize_ / n;
}

bool StripGroupNode::Balance(bool tracing) {
  bool updated = false;
  // 更新子区域的负载情况
  RecalcLoad();
  // 处理子区域内的负载均衡
  for (auto it : nodes_) {
    if (it->Balance(tracing)) {
      updated = true;
    }
  }
  uint16_t n = num_strips();
  if (n <= 1) {
    return updated;
  }
  BalanceResult result;
  DefaultBalancePolicy policy(vertical_, tracing);
  for (uint16_t i = balance_first_ ? 0 : 1, l = n - 1; i < l; i += 2) {
    StripNode* curr_node = nodes_[i];
    StripNode* next_node = nodes_[i + 1];
    if (policy.Balance(curr_node, next_node, result)) {
      updated = true;
      if (tracing) {
        GW_LOG_TRACE("[balance] space:%d strip:%d - %d, edge:%f->%f",
                   space_->id(), i, i + 1, result.old_edge(), result.edge());
      }
      // 内部的cell边界也要设置一下
      for (auto curr_cell_node : curr_node->nodes()) {
        curr_cell_node->mutable_node_rect().set_max(result.edge(), vertical_);
      }
      for (auto next_cell_node : next_node->nodes()) {
        next_cell_node->mutable_node_rect().set_min(result.edge(), vertical_);
      }
    }
    // 一次最多删除一个StripNode，这里先标记
    if (curr_node->CanDelete()) {
      curr_node->set_removed(true);
      updated = true;
    } else if (next_node->CanDelete()) {
      next_node->set_removed(true);
      updated = true;
    }
  }
  // 删除标记的StripNode
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    if ((*it)->removed()) {
      if (tracing) {
        GW_LOG_TRACE("[balance] space:%d remove strip", space_->id());
      }
      StripNode* remove_node = *it;
      nodes_.erase(it);
      CELLMGR.UpdateLayout(space_->id());
      remove_node->Dispose();
      unsigned long t_now = GetWalltimeMs();
      space_->set_last_remove_strip_time(t_now);
      space_->set_last_remove_time(t_now);
      break;
    }
  }
  balance_first_ = !balance_first_;
  return updated;
}

bool StripGroupNode::CheckToAddCell(ExcludeGameAppFilter& filter, bool tracing) {
  for (uint16_t i = 0, n = num_strips(); i < n; ++i) {
    StripNode* strip = nodes_[i];
    uint16_t num_cells = strip->num_nodes();
    if (num_cells >= max_cells_per_strip_) {
      continue;
    }
    CellNode* min_node = nullptr;
    CellNode* max_node = nullptr;
    uint16_t ridx = 0;  // index of max_node
    bool need_to_add = false;
    do {
      max_node = strip->SearchMaxLoadNodeWithMin(ridx, &min_node);
      // 是否最小负载仍大于负载上限
      if (max_node && min_node && (min_node->conv_load() > CELLMGR_CFG.upper_normal_load())) {
        need_to_add = true;
        break;
      }
      // 是否最小内存仍大于内存上限
      max_node = strip->SearchMaxMemsizeNodeWithMin(ridx, &min_node);
      if (max_node && min_node && (min_node->memsize() > CELLMGR_CFG.upper_normal_memsize())) {
        need_to_add = true;
        break;
      }
    } while (false);

    if (need_to_add) {
      GameAppEntry* gapp = filter.SearchGood();
      if (gapp) {
        filter.Cache(gapp->id());
        CellNode* new_cell_node = nullptr;
        if (tracing) {
          GW_LOG_TRACE("add new cell, space_id:%d, strip:%d, max node:%d, least load:%f, memsize:%d, gapp_id:%d",
                     space_->id(), i, ridx, min_node->conv_load(), min_node->memsize(), gapp->id());
        }
        if (ridx == 0) {
          new_cell_node = strip->InsertCellNode(gapp, 0);  // 在最前面加入新增的cell
        } else if (ridx == num_cells - 1) {
          new_cell_node = strip->AppendCellNode(gapp);  // 在最后面加入新增的cell
        } else {
          auto& cell_nodes = strip->nodes();
          // 加入新增的cell到负载相对高的一侧
          if (cell_nodes[ridx - 1]->conv_load() > cell_nodes[ridx + 1]->conv_load()) {
            new_cell_node = strip->InsertCellNode(gapp, ridx);
          } else {
            new_cell_node = strip->InsertCellNode(gapp, ridx + 1);
          }
        }
        new_cell_node->set_init_expanding(true);
        gapp->AddRevisedLoad(CELLMGR_CFG.extra_load_for_new_cell());
        unsigned long t_now = GetWalltimeMs();
        space_->set_last_add_cell_time(t_now);
        space_->set_last_add_time(t_now);
        // visitor的一些初始化工作要手动执行一次
        new_cell_node->set_id(space_->Nextuint64_t());
        new_cell_node->set_space(space_);
        space_->RegisterCell(new_cell_node);  // UpdateLayout时game会自动AddCell
      } else {
        if (tracing) {
          GW_LOG_TRACE("no suitable game app, space_id:%d, strip:%d, max node:%d, least load:%f, memsize:%d",
                     space_->id(), i, ridx, min_node->conv_load(), min_node->memsize());
        }
        return false;
      }
    }
  }
  return true;
}

bool StripGroupNode::CheckToRemoveCell() {
  float upper_good_load = CELLMGR_CFG.upper_good_load();
  // 移除更要从全局考虑, 如果cell平均负载大于，不考虑移除
  if (cell_conv_load_ > upper_good_load) {
    return false;
  }
  uint16_t total_num_cells = GetNumCells();
  if (total_num_cells <= 1) {
    return false;
  }
  // 确保移除后平均负载仍然在一个较低水平
  if (total_conv_load_ / (total_num_cells - 1) > upper_good_load) {
    return false;
  }
  float upper_remove_load = upper_good_load * CELLMGR_CFG.load_ratio_for_remove_cell();
  bool updated = false;
  for (uint16_t i = 0, n = num_strips(); i < n; ++i) {
    StripNode* strip = nodes_[i];
    uint16_t num_cells = strip->num_nodes();
    if (num_cells <= min_cells_per_strip_ || num_cells <= 1) {
      continue;
    }
    // 同时最多只有1个cell在移除中
    if (strip->IsSomeCellRemoving()) {
      return false;
    }
    uint16_t ridx = 0;
    CellNode* cell_node = strip->SearchLowLoadNodeToRemove(upper_remove_load, ridx);
    if (cell_node) {
      GW_LOG_DEBUG("removing cell, space_id:%d, idx:%d, gapp:%d",
                 space_->id(), ridx, cell_node->main_gapp()->id());
      cell_node->set_main_state(CellState::kRemoving);  // 标记开始移除
      updated = true;
    }
  }
  return updated;
}

bool StripGroupNode::CheckToAddStrip(ExcludeGameAppFilter& filter, bool tracing) {
  uint16_t n = num_strips();
  if (n >= max_strips_) {
    return false;
  }
  StripNode* min_node = nullptr;
  StripNode* max_node = nullptr;
  uint16_t ridx = 0;  // index of max node
  bool need_to_add = false;
  do {
    max_node = SearchMaxLoadNodeWithMin(ridx, &min_node);
    // 是否最小负载仍大于负载上限
    if (max_node && min_node && (min_node->conv_load() > CELLMGR_CFG.upper_normal_load())) {
      need_to_add = true;
      break;
    }
    // 是否最小内存仍大于内存上限
    max_node = SearchMaxMemsizeNodeWithMin(ridx, &min_node);
    if (max_node && min_node && (min_node->memsize() > CELLMGR_CFG.upper_normal_memsize())) {
      need_to_add = true;
      break;
    }
  } while (false);

  if (need_to_add) {
    GameAppEntry* gapp = filter.SearchGood();
    StripNode* new_strip_node = nullptr;
    if (gapp) {
      filter.Cache(gapp->id());
      if (tracing) {
        GW_LOG_TRACE("add new strip, space_id:%d, strip:%d, least load:%f, memsize:%d, gapp_id:%d",
                   space_->id(), ridx, min_node->conv_load(), min_node->memsize(), gapp->id());
      }
      if (ridx == 0) {
        new_strip_node = InsertStripNode(gapp, 0);  // 在最前面加入新增的strip
      } else if (ridx == n - 1) {
        new_strip_node = AppendStripNode(gapp);  // 在最后面加入新增的strip
      } else {
        // 加入新增的cell到负载相对高的一侧
        if (nodes_[ridx - 1]->conv_load() > nodes_[ridx + 1]->conv_load()) {
          new_strip_node = InsertStripNode(gapp, ridx);
        } else {
          new_strip_node = InsertStripNode(gapp, ridx + 1);
        }
      }
      CellNode* new_cell_node = new_strip_node->mutable_nodes()[0];
      new_cell_node->set_init_expanding(true);
      gapp->AddRevisedLoad(CELLMGR_CFG.extra_load_for_new_cell());
      unsigned long t_now = GetWalltimeMs();
      space_->set_last_add_strip_time(t_now);
      space_->set_last_add_time(t_now);
      // visitor的一些初始化工作要手动执行一次
      new_strip_node->set_id(space_->Nextuint64_t());
      new_strip_node->set_space(space_);
      new_cell_node->set_id(space_->Nextuint64_t());
      new_cell_node->set_space(space_);
      space_->RegisterCell(new_cell_node);
    } else {
      if (tracing) {
        GW_LOG_TRACE("no suitable game app, space_id:%d, strip:%d, least load:%f, memsize:%d",
                   space_->id(), ridx, min_node->conv_load(), min_node->memsize());
      }
      return false;
    }
  }
  return true;
}

bool StripGroupNode::CheckToRemoveStrip() {
  if (num_strips() <= min_strips_) {
    return false;
  }
  float upper_good_load = CELLMGR_CFG.upper_good_load();
  // 移除更要从全局考虑, 如果cell平均负载大于，不考虑移除
  if (cell_conv_load_ > upper_good_load) {
    return false;
  }
  // 同时只有一个strip在移除
  if (IsSomeStripRemoving()) {
    return false;
  }
  uint16_t total_num_cells = GetNumCells();
  if (total_num_cells <= min_cells_per_strip_) {  // 移除的strip内的cell数目必定小于min_cells_per_strip_
    return false;
  }
  // 确保移除strip之后的负载仍然在一个比较低的水平
  if (total_conv_load_ / (total_num_cells - min_cells_per_strip_) > upper_good_load) {
    return false;
  }
  float upper_remove_load = upper_good_load * CELLMGR_CFG.load_ratio_for_remove_cell();
  uint16_t ridx = 0;
  StripNode* strip = SearchLowLoadNodeToRemove(upper_remove_load, ridx);
  if (strip) {
    GW_LOG_DEBUG("removing strip space_id:%d, idx:%d", space_->id(), ridx);
    strip->set_removing(true);  // 标记开始移除
    return true;
  }
  return false;
}

bool StripGroupNode::CellNumBalance(ExcludeGameAppFilter& filter, bool tracing) {
  bool updated = false;
  unsigned long t_now = GetWalltimeMs();
  if (t_now - space_->last_add_cell_time() > CELLMGR_CFG.space_add_cell_cd() &&
      t_now - space_->last_add_time() > CELLMGR_CFG.space_add_cd()) {
    updated = CheckToAddCell(filter, tracing);
  }
  if (t_now - space_->last_remove_cell_time() > CELLMGR_CFG.space_remove_cell_cd() &&
      t_now - space_->last_remove_time() > CELLMGR_CFG.space_remove_cd()) {
    updated |= CheckToRemoveCell();
  }
  return updated;
}

bool StripGroupNode::StripNumBalance(ExcludeGameAppFilter& filter, bool tracing) {
  bool updated = false;
  unsigned long t_now = GetWalltimeMs();
  if (t_now - space_->last_add_strip_time() > CELLMGR_CFG.space_add_strip_cd() &&
      t_now - space_->last_add_time() > CELLMGR_CFG.space_add_cd()) {
    updated |= CheckToAddStrip(filter, tracing);
  }
  if (t_now - space_->last_remove_strip_time() > CELLMGR_CFG.space_remove_strip_cd() &&
      t_now - space_->last_remove_time() > CELLMGR_CFG.space_remove_cd()) {
    updated |= CheckToRemoveStrip();
  }
  return updated;
}

bool StripGroupNode::GAppBalance(ExcludeGameAppFilter& filter, bool tracing) {
  bool updated = false;
  // TODO(cleve) cell num与strip num的检查频率需要不同
  updated |= CellNumBalance(filter, tracing);
  updated |= StripNumBalance(filter, tracing);
  return updated;
}

GW_CELL_MGR_END