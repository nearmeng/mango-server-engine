#include "great_world_base/gamespace/cell_entity/cell_entity.h"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_manager.h"

GW_GAME_BEGIN

GWCLASS_DECL_IMPLEMENT(CellEntity)

RetCode CellEntities::AddEntity(CellEntity* cell_entity) {
  if (!cell_entity) {
    GW_LOG_ERROR("cell_entity is null");
    return FAIL;
  }
  auto ret = GetCellEntity(cell_entity->GetEid());
  if (ret) {
    GW_LOG_ERROR("repeat add cell_entity");
    return CELL_ENTITY_ADD_REPEAT;
  }
  cell_entities_.emplace(cell_entity->GetEid(), cell_entity);
  return SUCCESS;
}

void CellEntities::DelEntity(uint64_t eid) { cell_entities_.erase(eid); }

CellEntity* CellEntities::GetCellEntity(uint64_t eid) {
  auto ret = cell_entities_.find(eid);
  if (ret != cell_entities_.end()) {
    return ret->second;
  } else {
    return nullptr;
  }
}

uint64_t CellEntity::GetSpaceId() const {
  return space_->id();
}

void CellEntity::GetRealEntityMailBox(EntityMailBox& mailbox) {
  GameAppId gapp_id = entity_entry_.real_gapp_id();
  EntityId entity_id = eid_;
  mailbox = EntityMailBox(MailBoxType::CELL, gapp_id, entity_id, 0, 0);
}

void CellEntity::GetGhostEntityMailBoxes(std::vector<EntityMailBox>& mailboxes) {
  auto& ghosts = entity_entry_.real_entry()->ghosts();
  for (auto ghost : ghosts.entries()) {
    mailboxes.push_back(EntityMailBox(MailBoxType::CELL, ghost->gapp()->id(), GetEid(), 0, 0));
  }
}

void CellEntity::Move(const Vector3& pos) {
  pos_ = pos;
  aoi_entity_->Move(pos);
}

RetCode CellEntity::NotifyData(uint8_t notify_flag, uint32_t data_type, StringSlice raw_data, uint32_t label) {
  if(IsGhost()) {
    GW_LOG_ERROR("ghost cannot notify data:eid %lu", eid_);
    return FAIL;
  }
  bool aoi_need = false;
  uint32_t aoi_order_id = 0;
  uint64_t now = GetWalltimeMs();
  if (notify_flag & NotifyFlag::other_client) {
    aoi_order_id = aoi_entity_->AddNotifyData(data_type, raw_data, label, now);
    aoi_need = true;
  }

  NetworkOStream os(NetworkMessage::NotifyToGhost);
  os << space_->id() << eid_;
  os << notify_flag << data_type << raw_data;
  if(aoi_need)
    os << label << aoi_order_id << now;
  for (auto& it : entity_entry_.real_entry()->ghosts().entries()) {
    SEND_MSG_TO_CELL(it->gapp()->id(), os);
  }
  return SUCCESS;
}

void CellEntity::OnNotifyData(MemoryIStream& is) {
  uint8_t notify_flag;
  uint32_t data_type;
  GWString raw_data;
  is >> notify_flag >> data_type >> raw_data;
  StringSlice s(raw_data.c_str(), raw_data.size());
  if (notify_flag & NotifyFlag::other_client) {
    uint32_t label;
    uint32_t aoi_order_id;
    uint64_t now;
    is >> label >> aoi_order_id >> now;
    aoi_entity_->AddNotifyData(data_type, s, label, now, aoi_order_id);
  }
  adapter_->OnNotifyToGhost(notify_flag, data_type, s);
}

bool CellEntity::ConvertToGhost() {
  return entity_entry_.ConvertToGhost() && aoi_entity_->ConvertToGhost();
}

bool CellEntity::ConvertToReal() {
  return entity_entry_.ConvertToReal() && aoi_entity_->ConvertToReal();
}

void CellEntity::ConfirmMigrateOut(uint64_t gapp_id) {
  space()->curr_cell_node()->OnMigrateDone(GetEid());
  if(gapp_id != entity_entry_.dst_gapp_id()) {
    GW_LOG_ERROR("ConfirmMigrateOut:%lu, %lu", gapp_id, entity_entry_.dst_gapp_id());
    return;
  }
  const gw::game::GhostEntries::Entries& entries = entity_entry_.real_entry()->ghosts().entries();

  NetworkOStream migrate_os(NetworkMessage::EntityMigrate);
  migrate_os << eid_;
  migrate_os << space()->id();

  // GW_LOG_DEBUG("%lu HEAD size:%u", GetEid(), migrate_os.size());

  migrate_os << entries.size() + 1;
  for (auto ghost_entry : entries) {
    migrate_os << ghost_entry->gapp()->id();
  }
  migrate_os << SPACE_MGR.gapp_id();

  // GW_LOG_DEBUG("%lu gappids size:%u", GetEid(), migrate_os.size());

  InnerWriteToMemoryStreamReal(migrate_os);

  // GW_LOG_DEBUG("%lu InnerWriteToMemoryStreamReal size:%u", GetEid(), migrate_os.size());

  adapter()->WriteToMemoryStreamReal(migrate_os);

  // GW_LOG_DEBUG("%lu WriteToMemoryStreamReal size:%u", GetEid(), migrate_os.size());

  entity_entry_.set_real_gapp_id(gapp_id);

  // 暂时没用到的调用
  NetworkOStream os(NetworkMessage::UpdateGhostDstAppId);
  os << gapp_id;
  os << eid_;
  os << space()->id();
  for(auto& it:entries) {
    if (it->gapp()->id() == gapp_id) {
      continue;
    }
    SEND_MSG_TO_CELL(it->gapp()->id(), os);
  }
  SEND_MSG_TO_CELL(gapp_id, migrate_os);

  if (!ConvertToGhost()) {
    GW_LOG_ERROR("entity %lu fail to convert to ghost", GetEid());
  }
  adapter_->OnMigrateOut(gapp_id);
}

void CellEntity::ConfirmMigrateIn() {
  StringSlice ss(&migrate_tmp_data_.at(0), migrate_tmp_data_.size());
  MemoryIStream is(ss);
  GW_LOG_DEBUG("%lu ConfirmMigrateIn:%u %u", eid_, migrate_tmp_data_.size(), is.LeftSize());

  // 调整这个即将转变成real的ghost的entry信息
  ConvertToReal();
  entity_entry_.set_dst_gapp_id(0);
  entity_entry_.set_real_gapp_id(SPACE_MGR.gapp_id());

  std::unordered_set<uint64_t> gapp_ids;
  std::size_t gapp_ids_size;
  is >> gapp_ids_size;
  for (size_t i = 0; i < gapp_ids_size; i++) {
    uint64_t tmp;
    is >> tmp;
    gapp_ids.insert(tmp);
  }

  // GW_LOG_DEBUG("%lu gapp_ids: %u", eid_, is.LeftSize());

  std::vector<uint64_t> ghost_info;
  for (auto it : gapp_ids) {
    if (it == SPACE_MGR.gapp_id()) {
      continue;
    }
    ghost_info.push_back(it);
  }
  entity_entry()->UpdateGhostEntries(ghost_info);
  InnerReadFromMemoryStreamReal(is);
  // GW_LOG_DEBUG("%lu InnerReadFromMemoryStreamReal: %u", eid_, is.LeftSize());

  adapter()->ReadFromMemoryStreamReal(is);
  // GW_LOG_DEBUG("%lu ReadFromMemoryStreamReal: %u", eid_, is.LeftSize());

  adapter()->OnMigrateIn();
  migrate_tmp_data_.clear();

  CheckAoIData();
}

bool CellEntity::InnerWriteToMemoryStreamGhost(IMemoryOStream& os) {
  os << base_eid_ << base_gapp_id_;
  os << pos().x << pos().y << pos().z;
  os << entity_entry_.real_gapp_id();
  return aoi_entity_->WriteToMemoryStreamGhost(os);
}

CellEntity::CellEntity(uint64_t eid, Space* space, ICellEntityAdapter* adapter, IMemoryIStream& is)
    : ICellEntity(adapter), eid_(eid), entity_entry_(this), space_(space) {
  is >> base_eid_ >> base_gapp_id_;
  is >> pos_.x >> pos_.y >> pos_.z;
  uint64_t tmp_id;
  is >> tmp_id;
  entity_entry_.set_real_gapp_id(tmp_id);
}

bool CellEntity::InnerReadFromMemoryStreamGhost(IMemoryIStream& is) {
  return aoi_entity_->ReadFromMemoryStreamGhost(is);
}

bool CellEntity::InnerWriteToMemoryStreamReal(IMemoryOStream& os) {
  return aoi_entity_->WriteToMemoryStreamReal(os);
}

bool CellEntity::InnerReadFromMemoryStreamReal(IMemoryIStream& is) {
  return aoi_entity_->ReadFromMemoryStreamReal(is);
}

namespace {

bool DoTeleport(CellEntity* cell_entity, const uint64_t gapp_id, const uint64_t space_id, float x, float y, float z) {
  if (gapp_id == SPACE_MGR.gapp_id()) {
    auto new_space = SPACE_MGR.GetSpace(space_id);
    if (new_space == nullptr) {
      GW_LOG_ERROR("[teleportbw]: space %llu is not found in this game server", space_id);
      return false;
    }
    auto old_space = cell_entity->space();
    cell_entity->space()->LeaveWithoutDel(cell_entity, LeaveSpaceReason::TeleportOut);
    new_space->Enter(cell_entity, {x, y, z});
    cell_entity->adapter()->OnTeleportFinish(SUCCESS, cell_entity);
    GW_LOG_DEBUG("[teleportbw]: local teleport, eid: %ld", cell_entity->GetEid());
    return true;
  } 
  else {
    NetworkOStream os(NetworkMessage::TelportTo);
    os << cell_entity->GetEid() << space_id << x << y << z;

    auto ghosts_entries = cell_entity->entity_entry()->real_entry()->ghosts().entries();

    cell_entity->adapter()->WriteToMemoryStreamGhost(os);
    cell_entity->adapter()->WriteToMemoryStreamReal(os);

    cell_entity->InnerWriteToMemoryStreamGhost(os);
    cell_entity->InnerWriteToMemoryStreamReal(os);

    cell_entity->space()->Leave(cell_entity, LeaveSpaceReason::TeleportOut);
    SEND_MSG_TO_CELL(gapp_id, os);
    return true;
  }
}
}

bool CellEntity::Teleport(const uint64_t gapp_id, const uint64_t space_id, float x, float y, float z) {
  GW_LOG_DEBUG("[teleportbw]: to game: %llu, space: %llu", gapp_id, space_id);
  if (IsGhost()) {
    GW_LOG_ERROR("[teleportbw]: ghost can not teleport, eid: %llu", GetEid());
    return false;
  }
  return DoTeleport(this, gapp_id, space_id, x, y, z);
}

void CellEntity::NotifyRealGappId() {
  if (IsGhost()) {
    GW_LOG_ERROR("ghost cannot notify");
    return;
  }
  const gw::game::GhostEntries::Entries& entries = entity_entry()->real_entry()->ghosts().entries();
  NetworkOStream os(NetworkMessage::UpdateGhostRealAppId);
  os << GetEid() << space_->id() << entity_entry_.real_gapp_id();
  for (auto& it : entries) {
    SEND_MSG_TO_CELL(it->gapp()->id(), os);
  }
}

GW_GAME_END