#include "great_world_base/cellmgr/balance/cell_node.h"

#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/common/common_private.h"

GW_CELL_MGR_BEGIN

GWCLASS_DECL_IMPLEMENT(CellNode)

CellNode::CellNode() {
  type = RegionNodeType::kCell;
}

CellNode::CellNode(const Rect& rect, RegionNode* parent) : BalanceNode(rect, parent) {
}

CellNode::~CellNode() {
  if (space_) {
    if (main_cell_.gapp_) {
      space_->UnregisterCell(main_cell_.gapp_->id());
    }
    for (auto& it : parallel_cells_) {
      space_->UnregisterCell(it.first);
    }
  }
}

bool CellNode::AssignGameApps(ExcludeGameAppFilter& filter) {
  main_cell_.gapp_ = CELLMGR.SearchGood(filter);
  if (main_cell_.gapp_ != nullptr) {
    filter.Cache(main_cell_.gapp_->id());
  }
  return main_cell_.gapp_ != nullptr;
}

bool CellNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << static_cast<uint8_t>(RegionNodeType::kCell);
  os << main_cell_.gapp_->id();
  os << id();
  os << static_cast<uint32_t>(UpperAvatarNumPerCell());
  os << parallel_visible_;
  os << parallel_cells_.size();
  for (auto& it : parallel_cells_) {
    os << it.first;
  }
  if (detail_for_viewer) {
    os << entity_rect_;
  }
  return true;
}

bool CellNode::ReadFromStream(IMemoryIStream& in) {
  in >> min_parallel_num_;
  GW_LOG_DEBUG("min_parallel_num %d", min_parallel_num_);
  return true;
}

void CellNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

GameAppEntry* CellNode::FindCell(float x, float y) {
  if (rect_.Contains(x, y) && main_cell_.gapp_) {
    return ShmNew<GameAppEntry>(main_cell_.gapp_->id());
  } else {
    return nullptr;
  }
}

std::shared_ptr<std::unordered_map<uint64_t, GameAppEntry*>> CellNode::FindAllCell(float x, float y) {
  if (rect_.Contains(x, y) && main_cell_.gapp_) {
    auto ret = std::make_shared<std::unordered_map<uint64_t, GameAppEntry*>>();
    ret->emplace(main_cell_.gapp_->id(), main_cell_.gapp_);
    for (auto& it : parallel_cells_) {
      ret->emplace(it.first, it.second.gapp_);
    }
    return ret;
  } else {
    return nullptr;
  }
}

bool CellNode::available() const {
  return main_cell_.gapp_ && main_cell_.gapp_->available();
}

bool CellNode::CanDelete() const {
  return removing() && !rect_.HasArea() && main_cell_.num_entities_ == 0;
}

bool CellNode::created() const {
  return main_cell_.state_ == CellState::kCreated;
}

float CellNode::curr_load() const {
  return main_cell_.gapp_->curr_load();
}

float CellNode::revised_load() const {
  return main_cell_.gapp_->revised_load();
}

float CellNode::conv_load() const {
  return main_cell_.gapp_->conv_load();
}

uint32_t CellNode::memsize() const {
  return main_cell_.gapp_->memsize();
}

CellState CellNode::state(uint64_t gapp_id) const {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    return it->second.state_;
  }
  return main_cell_.state_;
}

bool CellNode::set_state(CellState state, uint64_t gapp_id) {
  if (main_cell_.gapp_->id() == gapp_id) {
    main_cell_.state_ = state;
    return true;
  }
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.state_ = state;
  }
  return true;
}

bool CellNode::set_main_state(CellState state) {
  main_cell_.state_ = state;
  return true;
}

uint32_t CellNode::num_entities(bool use_parallel, uint64_t gapp_id) const {
  if (use_parallel) {
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      return it->second.num_entities_;
    }
  }
  return main_cell_.num_entities_;
}

void CellNode::set_num_entities(uint32_t num, uint64_t gapp_id) {
  if (main_cell_.gapp_->id() == gapp_id) {
    main_cell_.num_entities_ = num;
    return;
  }
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.num_entities_ = num;
  }
}

uint32_t CellNode::num_reals(bool use_parallel, uint64_t gapp_id) const {
  if (use_parallel) {
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      return it->second.num_reals_;
    }
  }
  return main_cell_.num_reals_;
}

void CellNode::set_num_reals(uint32_t num, uint64_t gapp_id) {
  if (main_cell_.gapp_->id() == gapp_id) {
    main_cell_.num_reals_ = num;
    return;
  }
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.num_reals_ = num;
  }
}

uint32_t CellNode::num_avatars(bool use_parallel, uint64_t gapp_id) const {
  if (use_parallel) {
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      return it->second.num_avatars_;
    }
  }
  return main_cell_.num_avatars_;
}

void CellNode::set_num_avatars(uint32_t num, uint64_t gapp_id) {
  if (main_cell_.gapp_->id() == gapp_id) {
    main_cell_.num_avatars_ = num;
    return;
  }
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.num_avatars_ = num;
  }
}

bool CellNode::removing() const {
  return main_cell_.state_ == CellState::kRemoving;
}

bool CellNode::AddCell(GameAppEntry* gapp) {
  if (!gapp) {
    GW_LOG_INFO("add cell fail, gapp is null");
    return false;
  }
  if (parallel_cells_.find(gapp->id()) != parallel_cells_.end() || gapp->id() == main_cell_.gapp_->id()) {
    GW_LOG_INFO("add cell fail,%d,%d", gapp->id(), main_cell_.gapp_->id());
    return false;
  }
  parallel_cells_[gapp->id()] = {gapp};
  return true;
}

uint32_t CellNode::UpperAvatarNumPerCell() {
  if (parallel_visible_) {
    return CELLMGR_CFG.upper_normal_avatar_num();
  } else {
    return CELLMGR_CFG.upper_invisible_normal_avatar_num();
  }
}

bool CellNode::RemoveCell(uint64_t gapp_id) {
  if (parallel_cells_.find(gapp_id) == parallel_cells_.end()) {
    return false;
  }
  parallel_cells_.erase(gapp_id);
  return true;
}
/** 什么时候应该增加 cell 数量
 * 触发条件：某game内存过高、cpu过高，或者某cell人数过多
 * ==> 判断其他 cell 能不能放下，能放下就先不迁移，否则创建新分线
 * ==> todo 增加一个 ForceCreate 的接口，给项目组去创建，这个创建是有较长 cd 的
 * 
 * 分线之间迁移的时候通过握手+人数预加的方式做
 * -----------------------
 */
void CellNode::CheckAddOrRemoveCell(bool& need_add, bool& need_remove, uint64_t& remove_gapp_id, bool& visible_changed) {
  need_add = false;
  need_remove = false;
  auto now_ts = GetWalltimeMs();
  if (now_ts - auto_change_cell_num_ts_ < CELLMGR_CFG.auto_change_cell_num_cd()) {
    return;
  }

  if (parallel_cells_.size() < min_parallel_num_) {
    GW_LOG_INFO("space %d, cellnode %d, add cell, currect %d, min_parallel_num_ %d", parent_->id(), id_, parallel_cells_.size(), min_parallel_num_);
    need_add = true;
    if (CELLMGR_CFG.visible_cell_parallel_max_size() < min_parallel_num_) {
      parallel_visible_ = true;
    }
    return;
  }

  uint32_t in_avatar = 0;   // cell还能接收多少
  uint32_t out_avatar = 0;  // cell需要弄出去多少

  if (main_cell_.num_avatars_ > UpperAvatarNumPerCell()) {
    out_avatar = main_cell_.num_avatars_ - UpperAvatarNumPerCell();
  } else if (main_cell_.gapp_->curr_load() < CELLMGR_CFG.upper_normal_load() &&
             main_cell_.gapp_->memsize() < CELLMGR_CFG.upper_normal_memsize() &&
             UpperAvatarNumPerCell() - main_cell_.num_avatars_ > 0) {
    in_avatar = UpperAvatarNumPerCell() - main_cell_.num_avatars_;
  }
  for (auto& it : parallel_cells_) {
    if (it.second.num_avatars_ > UpperAvatarNumPerCell()) {
      out_avatar += (it.second.num_avatars_ - UpperAvatarNumPerCell());
    } else if (it.second.gapp_->curr_load() < CELLMGR_CFG.upper_normal_load() &&
               it.second.gapp_->memsize() < CELLMGR_CFG.upper_normal_memsize() &&
               UpperAvatarNumPerCell() - it.second.num_avatars_ > 0) {
      in_avatar += (UpperAvatarNumPerCell() - it.second.num_avatars_);
    }
  }
  // GW_LOG_TRACE("space %d, cellnode %d, in %d, out %d, main_cell %d, parallel_cells_size %d", parent_->id(), id_, in_avatar, out_avatar, main_cell_.num_avatars_, parallel_cells_.size());
  if (out_avatar > in_avatar) {
    // 如果已经到达最大互通分线数量，就把分线模式切分为隔离的，然后后面性能阈值就读隔离分线的阈值了
    if (parallel_visible_ && CELLMGR_CFG.visible_cell_parallel_max_size() < parallel_cells_.size() + 1) {
      GW_LOG_INFO("space %d, cellnode %d, use_invisible_parallel, parallel_cells_size %d", parent_->id(), id_, parallel_cells_.size());
      parallel_visible_ = false;
      visible_changed = true;
      auto_change_cell_num_ts_ = now_ts;
      return;
    }
    if (!parallel_visible_ && CELLMGR_CFG.invisible_cell_parallel_max_size() < parallel_cells_.size() + 1) {
      GW_LOG_INFO("space %d, cellnode %d, add cell fail, parallel_cells_size %d", parent_->id(), id_, parallel_cells_.size());
      return;
    }
    need_add = true;
    auto_change_cell_num_ts_ = now_ts;
    GW_LOG_INFO("space %d, cellnode %d, out_avatar %d, in_avatar %d need add cell", parent_->id(), id_, out_avatar, in_avatar);
    return;
  }

  // 此处需要做性能测试
  // ！！！负载均衡的缩容依赖以下假设：
  // 1. 一个隔离分线能放下的人数比2个互通分线能放下的少很多，比如1主2分的三互通的分线结构要比 一主一分 的隔离分线多放下很多
  if (out_avatar == 0 && parallel_cells_.size() > min_parallel_num_) {
    // 考虑合并，如果删掉某个cell，其余的能接收就删掉
    uint32_t cell_avatar_min_num = 0xffffffff;
    uint32_t cell_avatar_min_gapp_id = main_cell_.gapp_->id();
    for (auto& it : parallel_cells_) {
      if (it.second.num_avatars_ < cell_avatar_min_num) {
        cell_avatar_min_num = it.second.num_avatars_;
        cell_avatar_min_gapp_id = it.first;
      }
    }

    if (cell_avatar_min_gapp_id != main_cell_.gapp_->id()) {
      // 调参数 todo evanyu 结合历史信息和逻辑数据调整参数
      auto final_in_avatar = in_avatar - (UpperAvatarNumPerCell() - parallel_cells_[cell_avatar_min_gapp_id].num_avatars_);
      // 给还能接收的数据打个折扣，暂且算是做个预测
      final_in_avatar *= 0.9;
      if (parallel_cells_[cell_avatar_min_gapp_id].num_avatars_ < final_in_avatar) {
        need_remove = true;
        remove_gapp_id = cell_avatar_min_gapp_id;
        auto_change_cell_num_ts_ = now_ts;
        if (parallel_cells_.size() == 1) {
          parallel_visible_ = true;
        }
        GW_LOG_INFO("space %d, cellnode %d, cell %d, final_in_avatar %d, in_avatar %d need remove cell", parent_->id(), id_, cell_avatar_min_gapp_id, final_in_avatar, in_avatar);
        return;
      }
    }
  }
}

// 返回一个良好的id，如果实在找不到合适的话就随机给一个
uint64_t CellNode::ChooseFreeGame() {
  uint64_t gapp_id = 0;
  std::vector<uint64_t> normal_game_ids;
  if (main_cell_.num_avatars_ <= UpperAvatarNumPerCell() && main_cell_.state_ == CellState::kCreated)
    normal_game_ids.push_back(main_cell_.gapp_->id());
  for (auto& it : parallel_cells_) {
    if (it.second.num_avatars_ <= UpperAvatarNumPerCell() && it.second.state_ == CellState::kCreated)
      normal_game_ids.push_back(it.first);
  }
  if (!normal_game_ids.empty()) {
    auto index = MathUtility::RandRange(0, normal_game_ids.size() - 1);
    gapp_id = normal_game_ids[index];
  } else {
    normal_game_ids = CreatedParallelCells();
    if (normal_game_ids.empty())
      return gapp_id;
    auto index = MathUtility::RandRange(0, normal_game_ids.size() - 1);
    gapp_id = normal_game_ids[index];
    GW_LOG_INFO("cannot find normal cell, space %d, node %d, cell_num %d, final_gapp_id %d", parent_->id(), id_, parallel_cells_.size() + 1, gapp_id);
  }
  if (main_cell_.gapp_->id() == gapp_id) {
    main_cell_.num_avatars_++;
    main_cell_.num_entities_++;
    main_cell_.num_reals_++;
    return gapp_id;
  } else {
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      it->second.num_avatars_++;
      it->second.num_entities_++;
      it->second.num_reals_++;
    }
  }
  return gapp_id;
}

std::vector<uint64_t> CellNode::CreatedParallelCells() {
  std::vector<uint64_t> gapp_ids;
  if (main_cell_.state_ == CellState::kCreated)
    gapp_ids.push_back(main_cell_.gapp_->id());
  for (auto& it : parallel_cells_) {
    if (it.second.state_ == CellState::kCreated) {
      gapp_ids.push_back(it.first);
    }
  }
  return gapp_ids;
}

GW_CELL_MGR_END