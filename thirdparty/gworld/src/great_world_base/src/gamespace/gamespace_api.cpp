#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/common/memory_stream_private.h"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/common/shm/shm_mgr.h"
#include "great_world_base/gamespace/managers.h"
#include "great_world_base/gamespace/space/balance_message.h"
#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/common/log.h"

GW_GAME_BEGIN

RetCode GameSpaceApi::Init(IGameSpaceAdapter* adapter) {
  Logger::InitLogger(adapter->GetLogger());
  for (auto& it : SHM_CLASS_REGISTER.name_to_id()) {
    GW_LOG_DEBUG("name %s id %d", it.first.c_str(), it.second);
  }

  uint64_t gapp_id;
  GWString config_path;
  adapter->Init(gapp_id, config_path);
  uint8_t* buffer = adapter->resume_buffer();
  SetWalltimeMs([adapter]() -> uint64_t {
    return adapter->GetTimeMs();
  });
  gw::TimerManager::Get()->Init([tmp = adapter]()->ITimeMgr*{
    return tmp->CreateTimerMgr();
  }, [tmp = adapter](ITimeMgr* mgr){
    tmp->DestoryTimerMgr(mgr);
  });

  SHM_MGR.Init();  // 必须这个最先调用，初始化内存池
  struct Managers* mgrs_ = reinterpret_cast<struct Managers*>(buffer);
  mgrs_->space_mgr_ = &SPACE_MGR;
  mgrs_->aoi_world_ = &AOI_WORLD;
  mgrs_->poll_mgr_ = SHM_MGR.pool_mgr();
  GW_LOG_DEBUG("SPACE_MGR:%p, AOI_WORLD:%p, poll_mgr_:%p, Managers_size %d", mgrs_->space_mgr_, mgrs_->aoi_world_,
               mgrs_->poll_mgr_, sizeof(Managers));
  SPACE_MGR.set_adapter(adapter);

  auto ret = SPACE_MGR.Init(config_path);
  if (ret == SUCCESS) {
    SPACE_MGR.set_gapp_id(gapp_id);
  }
  else {
    return ret;
  }
  ret = AOI_WORLD.Init(config_path);
  if(ret != SUCCESS) {
    return ret;
  }
  SPACE_MGR.adapter()->GetLoad();
  NetworkMgr::sender_ = ShmNew<GameSpaceNetworkSender>();
  return SUCCESS;
}

RetCode GameSpaceApi::Resume(IGameSpaceAdapter* adapter) {
  uint64_t gapp_id;
  GWString config_path;
  adapter->Init(gapp_id, config_path);
  uint8_t* buffer = adapter->resume_buffer();

  // 恢复池子里面的对象状态
  struct Managers* mgrs_ = reinterpret_cast<struct Managers*>(buffer);
  SHM_MGR.set_pools(mgrs_->poll_mgr_);

  SHM_CLASS_REGISTER.PushSpecialResumeFunc<AoIWorld>([](void* p) { AoIWorld::OwnResume(p); });

  SHM_MGR.Resume();
  SPACE_MGR.InstanceResume(mgrs_->space_mgr_);
  AOI_WORLD.InstanceResume(mgrs_->aoi_world_);

  SetWalltimeMs([adapter]() -> uint64_t {
    return adapter->GetTimeMs();
  });
  SPACE_MGR.set_adapter(adapter);
  return SUCCESS;
}

void GameSpaceApi::UnInit(bool retain_shm) {
  SPACE_MGR.UnInit(retain_shm);
  AOI_WORLD.Uninit();
}

ISpace* GameSpaceApi::GetSpace(uint64_t space_id) { return SPACE_MGR.GetSpace(space_id); }

void GameSpaceApi::SetResLoaderScheduler(IResLoaderScheduler* res_loader_scheduler) {
  SPACE_MGR.set_res_loader_scheduler(res_loader_scheduler);
}

// cell 2 cell
namespace {

#define CHECK_SPACE_BALANCE(space_id)                                                \
  auto space = SPACE_MGR.GetSpace(space_id);                                         \
  if (!space) {                                                                      \
    GW_LOG_ERROR("space not found, space_id %lu, gapp_id %lu", space_id, gapp_id);   \
    return;                                                                          \
  }                                                                                  \
  auto balance = space->cell()->parallel_balance();                                  \
  if (!balance) {                                                                    \
    GW_LOG_ERROR("balance not found, space_id %lu, gapp_id %lu", space_id, gapp_id); \
    return;                                                                          \
  };

void RequestMigrateAvatarNum(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t space_id;
  is >> space_id;
  CHECK_SPACE_BALANCE(space_id);
  balance->Receiver_RequestMigrateAvatarNum(space_id);
}

void ResponseMigrateAvatarNum(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  MigrateAvatarNum data;
  data.FromString(&is);
  auto space_id = data.space_id();
  CHECK_SPACE_BALANCE(space_id);
  balance->Sender_MigrateTo(gapp_id, data.num());
}

void NotityCellStateChanged(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  CellStateChangeInfo req;
  req.FromString(&is);
  auto space = SPACE_MGR.FindSpace(req.space_id());
  if (!space) {
    GW_LOG_WARN("space not found:%d", req.space_id());
    return;
  }
  space->ChangeOtherCellState(static_cast<CellState>(req.new_state()), req.gapp_id());
  NetworkOStream os(NetworkMessage::OnNotityCellStateChanged);
  os << req.index() << req.space_id() << static_cast<uint8_t>(space->curr_cell_node()->state(SPACE_MGR.gapp_id()));
  SEND_MSG_TO_CELL(gapp_id, os);
}

void OnNotityCellStateChanged(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint32_t index;
  uint64_t space_id;
  uint8_t cell_state;
  is >> index >> space_id >> cell_state;
  auto space = SPACE_MGR.FindSpace(space_id);
  if (!space) {
    GW_LOG_WARN("space not found:%d", space_id);
    return;
  }
  space->OnChangeCellStateSuccessFromGame(index, gapp_id, static_cast<CellState>(cell_state));
}

}  // namespace

// cellmgr 2 cell
namespace {

void OnAddCell(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  gw::AddCellMsg msg;
  msg.FromString(&is);
  gw::StringSlice slice(msg.layout_data().c_str(), msg.layout_data().size());

  if (SPACE_MGR.AddCell(msg.space_id(), msg.tag(), slice, msg.is_static())) {
    NetworkOStream os(NetworkMessage::ConfirmAddCell);
    os << msg.space_id();
    SEND_MSG_TO_CELLMGR(os);
    SPACE_MGR.adapter()->OnCreateSpace(msg);
    SPACE_MGR.InitSpaceResource(msg.space_id());
  } else {
    GW_LOG_ERROR("create spcae fail:space %lu", msg.space_id());
  }
}

void DestroyCell(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  gw::DestroySpaceNotice msg;
  msg.FromString(&is);
  if (!SPACE_MGR.DestroySpace(msg.space_id())) {
    GW_LOG_ERROR("DestroySpace fail:space_id, %lu", msg.space_id());
  }
}

void UpdateSpaceLayout(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  gw::SpaceLayoutMsg msg;
  msg.FromString(&is);

  for (const auto& it : msg.layouts()) {
    gw::StringSlice slice(it.layout_data().data(), it.layout_data().size());
    auto ret = SPACE_MGR.UpdateLayout(it.space_id(), it.tag(), slice, it.is_static());
    GW_LOG_DEBUG("update space %lu layout, ret %d", it.space_id(), ret);
    if (ret == gw::RetCode::SUCCESS || ret == gw::RetCode::SPACE_ADD_CELL) {
      if (ret == gw::RetCode::SPACE_ADD_CELL) {
        NetworkOStream os(NetworkMessage::ConfirmAddCell);
        os << it.space_id();
        SEND_MSG_TO_CELLMGR(os);
        SPACE_MGR.adapter()->OnUpdateLayout(it, true);
      } else {
        SPACE_MGR.adapter()->OnUpdateLayout(it, false);
      }
    } else {
      GW_LOG_ERROR("UpdateSpaceLayout falue:space_id %lu, ret %u", it.space_id(), ret);
    }
  }
}

void AllCellCreated(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  gw::AllCellCreatedMsg msg;
  msg.FromString(&is);
  auto space = SPACE_MGR.GetSpace(msg.space_id());
  if (!space) {
    GW_LOG_DEBUG("AllCellCreated:space_id %lu", msg.space_id());
  }
  else {
    space->OnAllCellCreated();
  }
  SPACE_MGR.adapter()->AllCellCreated(msg);
}

}  // namespace

// realghost
namespace {
void CreateGhost(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t eid;
  uint64_t space_id;
  is >> eid;
  is >> space_id;
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found:%lu", space_id);
    return;
  }
  if(space->cell_entities().GetCellEntity(eid)) {
    GW_LOG_ERROR("repeat create:space_id %lu, eid %lu", space_id, eid);
    return;
  }
  ICellEntityAdapter* adapter = SPACE_MGR.adapter()->OnCreateGhost(eid, is);
  if (!adapter) {
    GW_LOG_ERROR("adapter is null:eid %lu, space_id %lu", eid, space_id);
    return;
  }
  CellEntity* cell_entity = ShmNew<CellEntity>(eid, space, adapter, is);
  auto ret = space->Enter(cell_entity, cell_entity->pos());
  cell_entity->InnerReadFromMemoryStreamGhost(is);
  if (ret.second != SUCCESS) {
    cell_entity->Dispose();
    GW_LOG_ERROR("create ghost fail:eid %lu, space_id %lu", eid, space_id);
  } else {
    GW_LOG_DEBUG("create ghost success:eid %lu, space_id %lu", eid, space_id);
  }
  adapter->OnCreateGhostFinish(ret.second, cell_entity);
}

void DeleteGhost(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t eid;
  uint64_t space_id;
  is >> eid >> space_id;
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found:eid, %lu, space_id %lu", eid, space_id);
    return;
  }
  auto cell_entity = space->cell_entities().GetCellEntity(eid);
  if (!cell_entity) {
    GW_LOG_ERROR("cell_entity not found:eid, %lu, space_id %lu", eid, space_id);
    return;
  }
  space->Leave(cell_entity, LeaveSpaceReason::GhostDeleted);
}

void EntityMigrate(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t cell_entity_id;
  uint64_t space_id;
  is >> cell_entity_id;
  is >> space_id;
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found");
    return;
  }
  auto cell_entity = space->cell_entities().GetCellEntity(cell_entity_id);
  if (!cell_entity || !cell_entity->IsGhost()) {
    GW_LOG_ERROR("[real_ghost]target actor entity is not a ghost, entity_id:%lu", cell_entity_id);
    return;
  }

  GW_LOG_DEBUG("%lu before MoveMigrateStream:%u", cell_entity_id, is.LeftSize());
  cell_entity->MoveMigrateStream(is);
  if(!cell_entity->adapter()->RequestMigrateIn()) {
    GW_LOG_WARN("%lu RequestMigrateIn fail", cell_entity->GetEid());
  }
}

void UpdateGhostDstAppId(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t real_gapp_id;
  uint64_t cell_entity_id;
  uint64_t space_id;
  is >> real_gapp_id;
  is >> cell_entity_id;
  is >> space_id;
  GW_LOG_TRACE("[real_ghost]UpdateGhostDstAppId, entity_id:%lu, real_app_id:%d", cell_entity_id, real_gapp_id);
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found:%lu", space_id);
    return;
  }
  auto cell_entity = space->cell_entities().GetCellEntity(cell_entity_id);
  if (cell_entity == nullptr || !cell_entity->IsGhost()) {
    GW_LOG_ERROR("[real_ghost]actor ghost not found, entity_id:%lu", cell_entity_id);
    return;
  }
  cell_entity->entity_entry()->set_dst_gapp_id(gapp_id);
}

void UpdateGhostRealAppId(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t eid;
  uint64_t space_id;
  uint64_t real_app_id;
  is >> eid >> space_id >> real_app_id;
  GW_LOG_TRACE("[real_ghost]UpdateGhostRealAppId, eid:%ld, real_app_id:%d", eid, real_app_id);

  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found");
    return;
  }
  auto cell_entity = space->cell_entities().GetCellEntity(eid);
  if (!cell_entity || !cell_entity->IsGhost()) {
    GW_LOG_ERROR("[real_ghost]target actor entity is not a ghost, entity_id:%lu", eid);
    return;
  }
  cell_entity->entity_entry()->set_real_gapp_id(real_app_id);
}

void NotifyToGhost(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t eid;
  uint64_t space_id;
  is >> space_id >> eid;

  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found");
    return;
  }
  auto cell_entity = space->cell_entities().GetCellEntity(eid);
  if (!cell_entity || !cell_entity->IsGhost()) {
    GW_LOG_ERROR("[real_ghost]target actor entity is not a ghost, entity_id:%lu", eid);
    return;
  }
  cell_entity->OnNotifyData(is);
}

void TelportIn(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t eid;
  uint64_t space_id;
  Vector3 pos;
  is >> eid >> space_id >> pos.x >> pos.y >> pos.z;
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_ERROR("space not found:%lu", space_id);
    return;
  }
  if(space->cell_entities().GetCellEntity(eid)) {
    GW_LOG_ERROR("repeat TelportIn:space_id %lu, eid %lu", space_id, eid);
    return;
  }
  auto adapter = SPACE_MGR.adapter()->OnTeleportIn(eid, is);
  if(adapter) {
    GW_LOG_ERROR("adapter create fail %lu, eid %lu", space_id, eid);
  }
  CellEntity* cell_entity = ShmNew<CellEntity>(eid, space, adapter, is);
  cell_entity->set_pos(pos);
  auto ret = space->Enter(cell_entity, cell_entity->pos());
  cell_entity->InnerReadFromMemoryStreamGhost(is);
  if (ret.second != SUCCESS) {
    cell_entity->Dispose();
    GW_LOG_ERROR("teleport fail:eid %lu, space_id %lu", eid, space_id);
  } else {
    GW_LOG_DEBUG("teleport success:eid %lu, space_id %lu", eid, space_id);
  }
  adapter->OnTeleportFinish(ret.second, cell_entity);
}
}  // namespace

void GameSpaceApi::DispatchMessage(uint64_t gapp_id, StringSlice ss) {
  NetworkIStream is(ss);
  switch (is.msg_id()) {
    DISPATCH_MESSAGE(NetworkMessage::RequestMigrateAvatarNum, RequestMigrateAvatarNum);
    DISPATCH_MESSAGE(NetworkMessage::ResponseMigrateAvatarNum, ResponseMigrateAvatarNum);
    DISPATCH_MESSAGE(NetworkMessage::NotityCellStateChanged, NotityCellStateChanged);
    DISPATCH_MESSAGE(NetworkMessage::OnNotityCellStateChanged, OnNotityCellStateChanged);
    DISPATCH_MESSAGE(NetworkMessage::AddCell, OnAddCell);
    DISPATCH_MESSAGE(NetworkMessage::DestroyCell, DestroyCell);
    DISPATCH_MESSAGE(NetworkMessage::UpdateSpaceLayout, UpdateSpaceLayout);
    DISPATCH_MESSAGE(NetworkMessage::AllCellCreated, AllCellCreated);
    DISPATCH_MESSAGE(NetworkMessage::CreateGhost, CreateGhost);
    DISPATCH_MESSAGE(NetworkMessage::DeleteGhost, DeleteGhost);
    DISPATCH_MESSAGE(NetworkMessage::EntityMigrate, EntityMigrate);
    DISPATCH_MESSAGE(NetworkMessage::UpdateGhostDstAppId, UpdateGhostDstAppId);
    DISPATCH_MESSAGE(NetworkMessage::UpdateGhostRealAppId, UpdateGhostRealAppId);
    DISPATCH_MESSAGE(NetworkMessage::NotifyToGhost, NotifyToGhost);
    DISPATCH_MESSAGE(NetworkMessage::TelportTo, TelportIn);
    default:
      GW_LOG_ERROR("dispatch fail, gapp_id:%lu, msg_id:%u", gapp_id, is.msg_id());
      break;
  }
  GW_LOG_TRACE("DispatchMessage %lu to %lu, msg_id %lu", gapp_id, SPACE_MGR.gapp_id(), is.msg_id());
}

GW_GAME_END
