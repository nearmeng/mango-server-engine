#pragma once
#include "great_world_base/common/common.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/gamespace/space/res_loader_interface.h"
#include "great_world_base/gamespace/space/space_interface.h"
#include "great_world_base/common/timer_mgr.h"

GW_GAME_BEGIN

class IGameSpaceAdapter {
 public:
  virtual ~IGameSpaceAdapter() { }
  // 设置内部基础参数
  virtual void Init(uint64_t& gapp_id, GWString& config_path) = 0;
  
  // timer 管理
  virtual ITimeMgr* CreateTimerMgr() = 0;
  virtual void DestoryTimerMgr(ITimeMgr* mgr) = 0;

  // 网络通信
  virtual void SendMessageToCell(uint64_t gapp_id, const StringSlice& ss) = 0;
  virtual void SendMessageToCellMgr(const StringSlice& ss) = 0;

  virtual uint8_t* resume_buffer() = 0;

  // space 创建成功的回调，对于大世界来说也就是 space 中的一个cell在当前game创建成功，在这里可以通过 GameSpaceApi::GetSpace 拿到对应的space对象
  virtual void OnCreateSpace(const gw::AddCellMsg& msg) = 0;
  // space 销毁的回调，对于大世界来说也就是这个 space 在当前 game 的 cell 销毁了
  virtual void OnDestroySpace(uint64_t space_id, const GWString& tag) = 0;
  // 动态负载均衡导致的布局变化的回调
  virtual void OnUpdateLayout(const gw::LayoutMsg& msg, bool add_cell) = 0;
  // 在 space 的过程中，如果所有 cell 都创建完成了，每个 cell 所在game都会收到这个回调
  virtual void AllCellCreated(const gw::AllCellCreatedMsg& msg) = 0;

  // ghost 的创建，流里面前面是用户数据，可以先根据流里面的数据把用户对象创建出来，等ghost的创建过程整个完成后再把用户对象和cellentity进行绑定
  virtual ICellEntityAdapter* OnCreateGhost(uint64_t eid, IMemoryIStream& is) = 0;
  // 传送接口，作用同上
  virtual ICellEntityAdapter* OnTeleportIn(uint64_t eid, IMemoryIStream& is) = 0;

  // aoi
  // self 开始 observer other，这个时候可以把 other 同步给 self 的客户端
  virtual void OnBeginObserver(const GWVector<uint64_t>& self, uint64_t other) = 0;
  virtual void OnEndObserver(uint64_t self, uint64_t other) = 0;
  // todo 这个干掉 addlabel的时候直接塞进去，不要用回调了，多线程用起来麻烦
  virtual GWVector<AoIDiffProp>&& GetDiffProps(uint64_t, uint32_t) = 0;
  // 用于做数据同步的流
  virtual IMemoryOStream* NotifyOS() = 0;

  virtual float GetLoad() = 0;
  virtual float GetMemsize() = 0;

  virtual ILogger* GetLogger() = 0;
  virtual uint64_t GetTimeMs() = 0; 
};

class GameSpaceApi {
 public:
  static RetCode Init(IGameSpaceAdapter* adapter);
  static RetCode Resume(IGameSpaceAdapter* adapter);
  static void UnInit(bool retain_shm);
  static void DispatchMessage(uint64_t gapp_id, StringSlice ss);

  static ISpace* GetSpace(uint64_t space_id);
  static void SetResLoaderScheduler(IResLoaderScheduler* res_loader_scheduler);
};

GW_GAME_END
