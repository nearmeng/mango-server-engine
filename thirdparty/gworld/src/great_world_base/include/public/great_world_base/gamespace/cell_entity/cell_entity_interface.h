#pragma once
#include "great_world_base/common/common.h"
#include "great_world_base/common/memory_stream.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/gamespace/cell_entity/mail_box.h"
#include <functional>

GW_GAME_BEGIN

enum NotifyFlag : uint8_t {
  other_client = 1
};

struct AoIDiffProp
{
  StringSlice data_;
  uint32_t type_;
  uint32_t labels_;
};
typedef std::function<void(uint64_t, uint64_t, const Vector3&)> AOIFUNC;

enum LeaveSpaceReason : uint8_t {
  GhostDeleted, // ghost 被销毁
  TeleportOut,  // 传送
  SpaceDestory, // space 被销毁了
  CallLeave     // 外部调用 
};

class ICellEntity;
// 用户对象继承
class ICellEntityAdapter {
 public:
  // 是否是一个有客户端的玩家对象
  virtual bool IsAvatar() = 0;

  // 准备迁出和回调
  virtual bool RequestMigrateOut(uint64_t gapp_id) = 0;
  virtual void OnMigrateOut(uint64_t gapp_id) = 0;

  // 准备迁入和回调
  virtual bool RequestMigrateIn() = 0;
  virtual void OnMigrateIn() = 0;

  // 离开 space
  virtual void OnLeaveSpace(LeaveSpaceReason reason) = 0;

  // OnCreateGhostFinish 创建完成
  virtual void OnCreateGhostFinish(RetCode success, ICellEntity* cell_entity) = 0;

  // 传入space的回调
  virtual void OnTeleportFinish(RetCode success, ICellEntity* cell_entity) = 0;

  // ghost 收到同步数据的回调
  virtual void OnNotifyToGhost(uint8_t notify_flag, uint32_t data_type, StringSlice raw_data) = 0;
  
  // aoi 半径
  virtual uint32_t GetAoIRadius() = 0;
  // 序列化
  virtual bool WriteToMemoryStreamGhost(IMemoryOStream& os) = 0;
  virtual bool ReadFromMemoryStreamGhost(IMemoryIStream& is) = 0;
  virtual bool WriteToMemoryStreamReal(IMemoryOStream& os) = 0;
  virtual bool ReadFromMemoryStreamReal(IMemoryIStream& is) = 0;
};

class ICellEntity {
 public:
  ICellEntity(ICellEntityAdapter* adapter) : adapter_(adapter) {}
  virtual uint64_t GetEid() const = 0;
  virtual uint64_t GetBaseEid() const = 0;
  virtual uint64_t GetBaseGappId() const = 0;
  virtual uint64_t GetSpaceId() const = 0;
  virtual bool IsGhost() const = 0;
  virtual void Move(const Vector3& pos) = 0;
  virtual void GetRealEntityMailBox(EntityMailBox& mailbox) = 0;
  virtual void GetGhostEntityMailBoxes(std::vector<EntityMailBox>& mailboxes) = 0;
  virtual void NotifyRealGappId() = 0;

  virtual void ConfirmMigrateOut(uint64_t gapp_id) = 0;
  virtual void ConfirmMigrateIn() = 0;
  
  // 给aoi/ghost 做数据同步
  virtual RetCode NotifyData(uint8_t notify_flag, uint32_t data_type, StringSlice raw_data, uint32_t label) = 0;
  // 传送
  virtual bool Teleport(const uint64_t gapp_id, const uint64_t space_id, float x, float y, float z) = 0;

  // aoi
  virtual StringSlice Notify() = 0;
  virtual void ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, uint32_t>& begin_eids) = 0;
  virtual void ForceChangeLabels(bool is_add, uint64_t eid, uint32_t labels) = 0;
  virtual bool AddTrigger(uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave) = 0;
  virtual void RemoveTrigger(uint32_t radius) = 0;
  virtual void AddLabels(uint32_t new_labels, uint64_t eid) = 0;
  virtual void RemoveLabels(uint32_t removed_labels, uint64_t eid) = 0;

  // 分线
  virtual void SetParallelVisible(bool always_visible) = 0;

  ICellEntityAdapter* adapter() { return adapter_; }

 protected:
  ICellEntityAdapter* adapter_;
};

GW_GAME_END
