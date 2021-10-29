#pragma once
#include "great_world_base/gamespace/gamespace_api.h"

#include "mock/timer/timer_mgr.h"
#include "mock/log.h"

namespace gw {

using namespace gw::game;

class GameSpaceAdapter : public gw::game::IGameSpaceAdapter {
 public:
  ITimeMgr* CreateTimerMgr() override {
    return &gw::timer::TimerManager::Get();
  }

  void DestoryTimerMgr(ITimeMgr* mgr) override {

  }

  void SendMessageToCell(uint64_t gapp_id, const gw::StringSlice& ss) override {
    printf("SendMessageToCell %lu, %d\n", gapp_id, ss.size());
  }

  void SendMessageToCellMgr(const gw::StringSlice& ss) override {
    printf("SendMessageToCellMgr %lu\n", ss.size());
  }

  ICellEntityAdapter* OnTeleportIn(uint64_t eid, IMemoryIStream& is) {
    return nullptr;
  }

  void Init(uint64_t& gapp_id, GWString& config_path) override {
    gapp_id = 301;
    config_path = "";
  }

  uint8_t* resume_buffer() {
    return buffer_;
  }
  
  void OnCreateSpace(const gw::AddCellMsg& msg) override {
    printf("OnCreateSpace\n");
  }

  void OnDestroySpace(uint64_t space_id, const GWString& tag) override {
    printf("OnDestroySpace\n");
  }

  void OnUpdateLayout(const gw::LayoutMsg& msg, bool add_cell) override {
    printf("OnUpdateLayout\n");
  }

  void AllCellCreated(const gw::AllCellCreatedMsg& msg) override {
    printf("AllCellCreated\n");
  }

  ICellEntityAdapter* OnCreateGhost(uint64_t eid, IMemoryIStream& is) override {
    return nullptr;
  }

  float GetLoad() override {
    return 0;
  }

  float GetMemsize() override {
    return 0;
  }

  void OnBeginObserver(const gw::GWVector<uint64_t>& self, uint64_t other) override {
    aoi_observed_num++;
    printf("test OnBeginObserver self %lu, other %lu\n", self.size(), other);
  }

  void OnEndObserver(uint64_t self, uint64_t other) override {
    aoi_observed_num--;
    printf("test OnEndObserver self %lu, other %lu\n", self, other);
  }

  GWVector<AoIDiffProp>&& GetDiffProps(uint64_t, uint32_t) override {
    gw::GWVector<gw::game::AoIDiffProp> v;
    return std::move(v);
  }

  IMemoryOStream* NotifyOS() override { return &os_; }

  uint64_t GetTimeMs() override {
    return gw::timer::TimerManager::Get().GetWalltimeMs();
  }

  ILogger* GetLogger() override {
    auto logger = new MockLogger();
    logger->set_priority(gw::LOG_PRIORITY::LOG_PRIORITY_TRACE);
    return logger;
  }

  static uint32_t aoi_observed_num;

 private:
  gw::mock::MockMemoryOStream os_;

 private:
  uint8_t buffer_[256];
};

uint32_t GameSpaceAdapter::aoi_observed_num = 0;

class Avatar : public ICellEntityAdapter {
 public:
  void OnNotifyToGhost(uint8_t notify_flag, uint32_t data_type, StringSlice raw_data) {
  
  }

  void OnTeleportFinish(RetCode success, ICellEntity* cell_entity) {

  }

  bool IsAvatar() {
    return true;
  }

  void OnMigrateBegin(uint64_t real_gapp_id) {
    GW_LOG_DEBUG("OnMigrateBegin\n");
  }

  void OnCreateGhostFinish(RetCode success, ICellEntity* cell_entity) {
    GW_LOG_DEBUG("OnCreateGhostFinish\n");
  }

  void UpdateRouter(std::function<void(bool)> call_back) {
    GW_LOG_DEBUG("UpdateRouter\n");
  }

  // 准备迁出和回调
  bool RequestMigrateOut(uint64_t gapp_id) {
    GW_LOG_DEBUG("RequestMigrateOut\n");
    cell_entity_->ConfirmMigrateOut(gapp_id);
    return true;
  }

  void OnMigrateOut(uint64_t gapp_id) {
    GW_LOG_DEBUG("OnMigrateOut\n");
    migrate_count_++;
  }

  // 准备迁入和回调
  bool RequestMigrateIn() {
    GW_LOG_DEBUG("RequestMigrateIn\n");
    return true;
  }
  void OnMigrateIn() {
    GW_LOG_DEBUG("OnMigrateIn\n");
  }

  void OnDeleteGhost() {
    GW_LOG_DEBUG("OnDeleteGhost\n");
    delete_count_++;
  }

  void OnLeaveSpace(gw::game::LeaveSpaceReason reason) override {
    GW_LOG_DEBUG("OnLeaveSpace:%d\n", reason);
  }

  bool WriteToMemoryStreamGhost(IMemoryOStream& os) {
    return true;
  }

  bool ReadFromMemoryStreamGhost(IMemoryIStream& is) {
    return true;
  }

  bool WriteToMemoryStreamReal(IMemoryOStream& os) {
    return true;
  }

  bool ReadFromMemoryStreamReal(IMemoryIStream& is) {
    return true;
  }

  int32_t migrate_count() { return migrate_count_; }
  int32_t delete_count() { return delete_count_; }

  uint32_t GetAoIRadius() { return 10; }

  void set_cell_entity(ICellEntity* cell_entity) { cell_entity_ = cell_entity; }
  ICellEntity* cell_entity() { return cell_entity_; }

 private:
  int32_t migrate_count_ = 0;
  int32_t delete_count_ = 0;
  ICellEntity* cell_entity_;
};
}