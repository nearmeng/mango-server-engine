#include "great_world_base/gamespace/space/cell_node.h"

#include "great_world_base/gamespace/space/region_node_visitor.h"
#include "great_world_base/gamespace/space/space_manager.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(CellNode)

CellNode::CellNode(const Rect& rect, RegionNode* parent) : RegionNode(rect, parent), main_cell_(CellState::kInit) {
}

CellNode::CellNode(RegionNode* parent) : RegionNode(parent), main_cell_(CellState::kInit) {
}

CellNode::~CellNode() {
}

void CellNode::Destroy() {
  Dispose();
}

uint32_t CellNode::num_avatars(uint64_t gapp_id) const {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    return it->second.num_avatars_;
  }
  return main_cell_.num_avatars_;
}

void CellNode::set_num_avatars(uint32_t num, uint64_t gapp_id) {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.num_avatars_ = num;
  }
  if (gapp_id == main_cell_.gapp_id_) {
    main_cell_.num_avatars_ = num;
  }
}

CellState CellNode::state(uint64_t gapp_id) const {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    return it->second.state_;
  }
  return main_cell_.state_;
}

void CellNode::set_state(CellState state, uint64_t gapp_id) {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.state_ = state;
  }
  if (gapp_id == main_cell_.gapp_id_) {
    main_cell_.state_ = state;
  }
}

bool CellNode::cell_keep(uint64_t gapp_id) {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    return it->second.keep_;
  }
  return main_cell_.keep_;
}

void CellNode::set_cell_keep(bool keep, uint64_t gapp_id) {
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end()) {
    it->second.keep_ = keep;
  }
  if (gapp_id == main_cell_.gapp_id_) {
    main_cell_.keep_ = keep;
  }
}

void CellNode::OnAllCellCreated() {
  if (main_cell_.state_ == CellState::kInit)
    main_cell_.state_ = CellState::kCreated;
  for (auto& it : parallel_cells_) {
    if (it.second.state_ == CellState::kInit) {
      it.second.state_ = CellState::kCreated;
    }
  }
}

bool CellNode::ReadFromStream(IMemoryIStream& in) {
  in >> id_;
  in >> upper_normal_avatar_num_;
  in >> parallel_visible_;
  std::size_t size;
  in >> size;
  uint64_t gapp_id;
  for (auto& it : parallel_cells_) {
    it.second.keep_ = false;
  }
  for (size_t i = 0; i < size; i++) {
    in >> gapp_id;
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      it->second.keep_ = true;
    } else {
      struct GameCellInfo cell_info(gapp_id, CellState::kInit);
      parallel_cells_.emplace(gapp_id, cell_info);
    }
  }
  return true;
}

bool CellNode::UpdateFromStream(IMemoryIStream& in) {
  in >> id_;
  in >> upper_normal_avatar_num_;
  bool new_parallel_visible;
  in >> new_parallel_visible;
  if (new_parallel_visible != parallel_visible_ && !new_parallel_visible) {
    parallel_visible_ = new_parallel_visible;
  }
  std::size_t size;
  in >> size;
  uint64_t gapp_id;
  for (auto& it : parallel_cells_) {
    it.second.keep_ = false;
  }
  for (size_t i = 0; i < size; i++) {
    in >> gapp_id;
    auto it = parallel_cells_.find(gapp_id);
    if (it != parallel_cells_.end()) {
      it->second.keep_ = true;
    } else {
      struct GameCellInfo cell_info(gapp_id, CellState::kInit);
      parallel_cells_.emplace(gapp_id, cell_info);
      space_->AddCellNode(gapp_id, this);
    }
  }
  return true;
}

void CellNode::Accept(RegionNodeVisitor& visitor) {
  visitor.Visit(*this);
}

void CellNode::OnMigrateIn(uint64_t entity_id) {
  GW_LOG_DEBUG("CellNode OnMigrateIn %ld", entity_id);
  migratings_.insert(entity_id);
}

void CellNode::OnMigrateDone(uint64_t entity_id) {
  GW_LOG_DEBUG("CellNode OnMigrateDone %ld", entity_id);
  if (migratings_.erase(entity_id) != 1) {
    GW_LOG_ERROR("should not happen");
  }
  if (migratings_.size() == 0 && main_cell_.state_ == CellState::kRemoving) {
    // TODO, 同步给其他game，本game所以迁移完成，可以安全删除。注意，state变成
  }
}

GameCellInfo* CellNode::FindCell(uint64_t gapp_id) {
  if (main_cell_.gapp_id_ == gapp_id) {
    return &main_cell_;
  }
  auto it = parallel_cells_.find(gapp_id);
  if (it != parallel_cells_.end())
    return &it->second;
  return nullptr;
}

uint64_t CellNode::ChooseGameForMigrate() {
  std::vector<uint64_t> all_game_ids;
  all_game_ids.push_back(main_cell_.gapp_id_);
  for (auto& it : parallel_cells_) {
    all_game_ids.push_back(it.first);
  }
  auto index = MathUtility::RandRange(0, all_game_ids.size() - 1);
  return all_game_ids[index];
}

uint64_t CellNode::RandomCell() {
  auto index = MathUtility::RandRange(0, parallel_cells_.size());
  if (index == 0) {
    return main_gapp_id();
  } else {
    auto it = std::next(parallel_cells_.begin(), index - 1);
    return it->second.gapp_id_;
  }
}

GWHashSet<uint64_t> CellNode::AllGameIds() {
  GWHashSet<uint64_t> target_ids;
  if (main_gapp_id() != SPACE_MGR.gapp_id()) {
    target_ids.insert(main_gapp_id());
  } 
  else {
    for (auto& it : parallel_cells()) {
      if (it.first != SPACE_MGR.gapp_id()) {
        target_ids.insert(it.first);
      }
    }
  }
  return target_ids;
}

GW_GAME_END