#include "great_world_base/gamespace/space/space.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/common/memory_stream_private.h"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/gamespace/cell_entity/cell_entity.h"

#include "great_world_base/gamespace/space/cell.h"
#include "great_world_base/gamespace/space/res_loader_interface.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/gamespace/space/space_resource.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(Space)

Space::Space(uint64_t id)
    : id_(id), root_(nullptr), cell_(nullptr), state_(SpaceState::kInit), cell_state_rsp_(this), ghost_range_(0) {
  RetCode ret = AOI_WORLD.CreateSpace(id);
  if (ret != RetCode::SUCCESS) GW_LOG_ERROR("aoi create space code:%d", ret);
}

Space::~Space() {
  if (cell_state_rsp_.check_timerid_ != 0) {
    TimerManager::Get()->DeleteTimer(cell_state_rsp_.check_timerid_);
    cell_state_rsp_.check_timerid_ = 0;
  }
  std::unordered_set<CellNode*> to_delete;
  for (auto it : cell_nodes_) {
    if (to_delete.find(it.second) == to_delete.end()) {
      to_delete.insert(it.second);
      it.second->Dispose();
    }
  }
  if (cell_) {
    cell_->Dispose();
    cell_ = nullptr;
  }
  if (space_resource_) {
    delete space_resource_;
    space_resource_ = nullptr;
  }
  AOI_WORLD.DestroySpace(id_);
}

void Space::OnAllCellCreated() {
  for (auto& it : cell_nodes_) {
    it.second->OnAllCellCreated();
  }
}

RetCode Space::AddCellEntity(CellEntity* cell_entity) { 
  auto ret = cell_entities_.AddEntity(cell_entity);
  if (ret == SUCCESS) {
    cell_->AddEntityEntry(cell_entity->GetEid(), cell_entity->entity_entry()); 
  }
  return ret;
}

void Space::DelCellEntity(uint64_t id) {
  cell_->DelEntityEntry(id);
}

std::pair<ICellEntity*, RetCode> Space::Enter(CellEntity* cell_entity, const Vector3& pos, bool is_real) {
  RetCode ret = AddCellEntity(cell_entity);
  if (ret != SUCCESS) {
    return {nullptr, ret};
  }
  cell_entity->set_pos(pos);
  auto aoi_entity =
      AOI_WORLD.Enter(id_, cell_entity->GetEid(), cell_entity->adapter()->GetAoIRadius(), cell_entity->pos());
  if (!aoi_entity) {
    cell_entity->Dispose();
    return {nullptr, ret};
  }
  cell_entity->set_aoi_entity(aoi_entity);
  aoi_entity->set_cell_entity(cell_entity);
  cell_entity->set_space(this);

  if(is_real) {
    cell_entity->ConvertToReal();
  }

  aoi_entity->AddTrigger(cell_entity->adapter()->GetAoIRadius(),
                         std::bind(&AoIEntity::OnEnterBaseAoI, aoi_entity, std::placeholders::_1, std::placeholders::_2,
                                   std::placeholders::_3),
                         std::bind(&AoIEntity::OnLeaveBaseAoI, aoi_entity, std::placeholders::_1, std::placeholders::_2,
                                   std::placeholders::_3));
  return {cell_entity, SUCCESS};
}

std::pair<ICellEntity*, RetCode> Space::Enter(uint64_t eid, uint64_t base_eid, uint64_t base_gapp_id, Vector3 pos,
                                              ICellEntityAdapter* apapter) {
  CellEntity* cell_entity = cell_entities_.GetCellEntity(eid);
  if (cell_entity) {
    return {cell_entity, CELL_ENTITY_ADD_REPEAT};
  }
  cell_entity = ShmNew<CellEntity>(eid, base_eid, base_gapp_id, pos, this, apapter);
  cell_entity->entity_entry()->set_real_gapp_id(SPACE_MGR.gapp_id());
  return Enter(cell_entity, pos, true);
}

bool Space::LeaveWithoutDel(CellEntity* cell_entity, LeaveSpaceReason reason) {
  if(!cell_entity) {
    GW_LOG_ERROR("dynamic_cast to cell_entity error");
    return false;
  }
  DelCellEntity(cell_entity->GetEid());
  cell_entities_.DelEntity(cell_entity->GetEid());
  AOI_WORLD.Leave(cell_entity->space()->id(), cell_entity->GetEid());
  cell_entity->adapter()->OnLeaveSpace(reason);
  return true;
}

void Space::Leave(CellEntity* cell_entity, LeaveSpaceReason reason) {
  if(LeaveWithoutDel(cell_entity, reason)) {
    cell_entity->Dispose();
  }
}

void Space::Leave(ICellEntity* icell_entity) { 
  CellEntity* cell_entity = dynamic_cast<CellEntity*>(icell_entity);
  Leave(cell_entity, LeaveSpaceReason::CallLeave);
}

void Space::DebugOutput() {
  GW_LOG_INFO("space_id %lu, num_entities %lu", id_, num_entities());
  for (auto cell_node : cell_nodes_) {
    GW_LOG_INFO("==>nodes:id, %lu, rect, %s", cell_node.second->id(), cell_node.second->rect().DebugString().c_str())
  }
}

CellState Space::GetCellNodeState(uint64_t gapp_id) {
  for (auto& it : cell_nodes_) {
    GameCellInfo* cell_info = it.second->FindCell(gapp_id);
    if (cell_info) {
      return cell_info->state_;
    }
  }
  return CellState::kNone;
}

void Space::PrepareRemove() {
  GW_LOG_DEBUG("PrepareRemove:%d", id_);
  state_ = SpaceState::kRemoving;
}

void Space::PrepareDestroy() {
  state_ = SpaceState::kDestroying;
  if (cell_) {
    cell_->PrepareDestroy();
  }
}

void Space::OnDestroy() {
  state_ = SpaceState::kDestroyed;
  if (cell_) {
    cell_->OnDestroy();
  }
}

void Space::set_cell(Cell* cell) {
  G6_ASSERT(!cell_);
  cell_ = cell;
}

Cell* Space::cell() const { return cell_; }

CellNode* Space::curr_cell_node() const { 
  if (!cell_) {
    return nullptr;
  }
  return cell_->node(); 
}

bool Space::UpdateLayout(IMemoryIStream& in) {
  in >> rect_;
  for (auto it : cell_nodes_) {
    it.second->set_keep(false);
  }
  try {
    RegionNode* node = RegionNode::Parse(in, this, &rect_);
    GW_RETURN_IF_ERROR(root_ && node != root_, false, "invalid to change root");
    if (!root_) {
      root_ = node;
    }
  } catch (const UnarchiveError& e) {
    GW_LOG_ERROR("unarchive error");
    return false;
  }
  std::unordered_set<uint64_t> deleted;
  for (auto it : cell_nodes_) {
    if (!it.second->keep()) {
      deleted.insert(it.first);
    }
    if (!it.second->cell_keep(it.first)) {
      GW_LOG_INFO("space %d, delete parallel game %d, cell_id %d", id_, it.first, it.second->id());
    }
  }
  std::unordered_set<CellNode*> deleted_cell_node;
  for (auto game_app_id : deleted) {
    auto it = cell_nodes_.find(game_app_id);
    it->second->mutable_rect().reset();
    GW_LOG_INFO("space %d, delete main game %d, cell_id %d", id_, game_app_id, it->second->id());
    if (curr_cell_node() != it->second) {
      cell_nodes_.erase(it->first);
      if (deleted_cell_node.find(it->second) == deleted_cell_node.end()) {
        deleted_cell_node.insert(it->second);
        it->second->Dispose();
      }
    }
  }
  return true;
}

bool Space::AddCellNode(uint64_t gapp_id, CellNode* node) {
  GW_RETURN_IF_ERROR(cell_nodes_.find(gapp_id) != cell_nodes_.end(), false, "cell node already added");
  cell_nodes_.emplace(gapp_id, node);
  return true;
}

bool Space::RemoveCellNode(uint64_t gapp_id) {
  auto it = cell_nodes_.find(gapp_id);
  GW_RETURN_IF_ERROR(it == cell_nodes_.end(), false, "cell node not exist");
  cell_nodes_.erase(it);
  return true;
}

CellNode* Space::FindCellNode(uint64_t gapp_id) {
  auto it = cell_nodes_.find(gapp_id);
  return it != cell_nodes_.end() ? it->second : nullptr;
}

CellNode* Space::FindCellNode(float x, float z) {
  for (auto it = cell_nodes_.begin(); it != cell_nodes_.end(); it++) {
    const Rect& rect = it->second->rect();
    if (rect.Contains(x, z)) {
      return it->second;
    }
  }
  return nullptr;
}

bool Space::RemoveNonCellNode(uint64_t node_id) {
  auto it = non_cell_nodes_.find(node_id);
  GW_RETURN_IF_ERROR(it == non_cell_nodes_.end(), false, "non cell node not exist");
  non_cell_nodes_.erase(it);
  return true;
}

RegionNode* Space::FindNonCellNode(uint64_t node_id) {
  auto it = non_cell_nodes_.find(node_id);
  return it != non_cell_nodes_.end() ? it->second : nullptr;
}

bool Space::AddNonCellNode(RegionNode* node) {
  GW_RETURN_IF_ERROR(node == nullptr, false, "node is null");
  GW_RETURN_IF_ERROR(non_cell_nodes_.find(node->id()) != non_cell_nodes_.end(), false, "node already added");
  non_cell_nodes_[node->id()] = node;
  return true;
}

bool Space::InitSpaceResource() {
  if (space_resource_) {
    GW_LOG_ERROR("space resource already inited, space id: %ld", id_);
    return false;
  }
  space_resource_ = new SpaceResource(this);
  if (!space_resource_->Init(resource_mask_)) {
    GW_LOG_ERROR("space resource init fail, space id: %ld", id_);
    return false;
  }
  return true;
}

void Space::ChangeCellState(CellState new_state) {
  auto self_gapp_id = SPACE_MGR.gapp_id();
  cell_state_rsp_.update(cell_nodes_.size() - 1);
  CellStateChangeInfo req(id_, SPACE_MGR.gapp_id(), static_cast<uint8_t>(curr_cell_node()->state(self_gapp_id)),
                          static_cast<uint8_t>(new_state), cell_state_rsp_.index_);
  NetworkOStream os(NetworkMessage::NotityCellStateChanged);
  req.ToString(&os);
  GW_LOG_DEBUG("ChangeCellState:space %d, cell_id %d, main game %d, mygame %d, new_state %d, cell_size %d, os_size %u",
               id_, curr_cell_node()->id(), curr_cell_node()->main_gapp_id(), SPACE_MGR.gapp_id(),
               (int32_t)new_state, cell_nodes_.size(), os.size());
  for (auto& it : cell_nodes_) {
    if (SPACE_MGR.gapp_id() != it.first) {
      SEND_MSG_TO_CELL(it.first, os);
    }
  }
  curr_cell_node()->set_state(new_state, self_gapp_id);
}

bool Space::ChangeOtherCellState(CellState new_state, uint64_t gapp_id) {
  auto iter = cell_nodes_.find(gapp_id);
  if (iter != cell_nodes_.end()) {
    iter->second->set_state(new_state, gapp_id);
    return true;
  }
  else {
    GW_LOG_ERROR("change state fail:space_id %lu, new_state %d, gapp_id %lu", id_, new_state, gapp_id);
    return false;
  }
}

void Space::OnChangeCellStateSuccessFromGame(uint32_t sid, uint64_t other_gapp_id, CellState other_state) {
  if (cell_state_rsp_.index_ != sid) {
    GW_LOG_WARN("OnChangeCellStateSuccessFromGame_old_index:%d,%d", cell_state_rsp_.index_, sid);
    return;
  }
  cell_state_rsp_.back_num_++;
  GW_LOG_DEBUG("space %d, sid %d, back_num %d, target_num %d, curr_node_state %d, other_gapp %d, other_state %d", id(),
               sid, cell_state_rsp_.back_num_, cell_state_rsp_.target_num_,
               curr_cell_node()->state(SPACE_MGR.gapp_id()), other_gapp_id, other_state);
  if (cell_state_rsp_.back_num_ == cell_state_rsp_.target_num_) {
    if (curr_cell_node()->state(SPACE_MGR.gapp_id()) == CellState::kRemoving) {
      PrepareRemove();
    }
    if (cell_state_rsp_.check_timerid_ != 0) {
      TimerManager::Get()->DeleteTimer(cell_state_rsp_.check_timerid_);
      cell_state_rsp_.check_timerid_ = 0;
    }
  }
  auto other_cell_node = FindCellNode(other_gapp_id);
  if (other_cell_node->state(other_gapp_id) == CellState::kInit) {
    other_cell_node->set_state(other_state, other_gapp_id);
  }
}

void Space::OnChangeCellStateTimeOutFromGame() {
  GW_LOG_WARN("wait change cell state callback timeout");
  cell_state_rsp_.index_++;
  if (curr_cell_node()->state(SPACE_MGR.gapp_id()) == CellState::kRemoving) {
    PrepareRemove();
  }
  if (cell_state_rsp_.check_timerid_ != 0) {
    TimerManager::Get()->DeleteTimer(cell_state_rsp_.check_timerid_);
    cell_state_rsp_.check_timerid_ = 0;
  }
}

bool Space::set_ghost_range(float range) {
  if (range > CELL_CFG.ghost_dist()) {
    GW_LOG_ERROR("ghost range should not greater than ghost dist");
    return false;
  }
  ghost_range_ = range;
  return true;
}

void Space::IncreaseNumEntities(CellEntity* cell_entity) {
  set_num_entities(num_entities() + 1);
  if (cell_entity->adapter()->IsAvatar()) {
    set_num_entity_avatars(num_entity_avatars() + 1);
  }
}

void Space::DecreaseNumEntities(CellEntity* cell_entity) {
  if (num_entities() > 0) {
    set_num_entities(num_entities() - 1);
    if (cell_entity->adapter()->IsAvatar()) {
      set_num_entity_avatars(num_entity_avatars() - 1);
    }
  } 
  else {
    GW_LOG_WARN(
        "num_entities is zero, but there is still some entity to leave, "
        "space_id:%d",
        id_);
  }
}

void Space::IncreaseNumReals(CellEntity* cell_entity) {
  set_num_reals(num_reals() + 1);
  if (cell_entity->adapter()->IsAvatar()) {
    set_num_real_avatars(num_real_avatars() + 1);
  }
}

void Space::DecreaseNumReals(CellEntity* cell_entity) {
  if (num_reals() > 0) {
    set_num_reals(num_reals() - 1);
    if (cell_entity->adapter()->IsAvatar() && num_real_avatars() > 0) {
      set_num_real_avatars(num_real_avatars() - 1);
    }
  } 
  else {
    GW_LOG_WARN(
        "num_reals is zero, but there is still some real entity to leave, "
        "space_id:%d",
        id_);
  }
}

GW_GAME_END
