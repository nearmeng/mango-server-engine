#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/cellmgr/proxy/mgr_service_proxy.h"
#include "great_world_base/cellmgr/cellmgr_api.h"
#include "great_world_base/common/network/network_mgr.h"

GW_CELL_MGR_BEGIN

class MgrServiceProxy;
class GameAppFilter;
class GameAppEntry;
class Space;
class RegionNode;
class CellNode;
class MemWeightPolicy;

class CellMgr : public ShmSingletonRef<CellMgr> {
  GWCLASS_DECL(CellMgr)
 public:
  CellMgr();
  ~CellMgr();

  bool Init(GWString);
  void StartUp();
  void UnInit(bool retain_shm);
  bool Clear();

  static void OwnResume(void*);

  // 创建space
  Space* CreateSpace(const GWString& name, const GWString& tag, uint16_t min_cell_num, uint16_t max_cell_num, 
                      const Rect& rect, RegionNode* root, bool is_static, const GWString& meta_data);
  // 销毁space
  bool DestroySpace(uint64_t id);
  // game app确认创建cell的处理
  void ConfirmAddCell(uint64_t gapp_id, uint64_t space_id);
  // game app上报的负载处理
  void ReportLoad(uint64_t gapp_id, ReportLoadMsg& msg);
  // 更新layout
  bool UpdateLayout(uint64_t id, std::shared_ptr<std::vector<uint64_t>> extra_gapp_ids = nullptr);
  void UpdateLayoutToGame(uint64_t space_id, uint64_t gapp_id);

  // space的cell间的负载均衡处理
  void Balance();
  // 添加或者移除cell
  void GappBalance();

  // 在Space中查找给定的位置（x，y）所在的CellNode
  CellNode* FindCell(uint64_t space_id, float x, float y) const;
  Space* FindSpace(uint64_t space_id) const;

  GameAppEntry* SearchGood(GameAppFilter& filter) const;
  GameAppEntry* SearchLeastLoaded(GameAppFilter& filter) const;

  bool RegisterGameApp(uint64_t gapp_id);
  bool UnregisterGameApp(uint64_t gapp_id);
  bool HasGameApp(uint64_t gapp_id) { return this->gapps_.find(gapp_id) != this->gapps_.end(); }
  const GameAppEntries& game_apps() const { return this->gapps_; }
  GameAppEntries& mutable_game_apps() { return this->gapps_; }

  GWHashMap<uint64_t, Space*> spaces() const { return this->spaces_; }

  const MemWeightPolicy& mem_weight_policy() const { return *mem_weight_policy_; }

  CellMgrConfig& config() { return config_; }

  ICellMgrAdapter* adapter() { return adapter_; }
  void set_adapter(ICellMgrAdapter* adapter) { adapter_ = adapter; }

 private:
  uint64_t last_space_id_ = 0;
  GWHashMap<uint64_t, Space*> spaces_;
  GameAppEntries gapps_;  /// 由 GameApp 上报数据、注册生成，CellApp 上报给 CELLMGR，BaseApp 上报给 BASEMGR
  uint8_t space_balance_round_ = 0;
  uint8_t gapp_balance_round_ = 0;

  MgrServiceProxy mgr_service_;

  uint64_t balance_timer_ = TimerManager::kInvalidTimerId;

  MemWeightPolicy* mem_weight_policy_;

  CellMgrConfig config_;
  GWString config_path_ = "";

  ICellMgrAdapter* adapter_;
};

class CellMgrNetworkSender : public NetworkSender {
  GWCLASS_DECL(CellMgrNetworkSender)
 public:
  inline void SendMessageToCell(uint64_t gapp_id, const StringSlice& ss) override {
    CellMgr::GetInstance().adapter()->SendMessageToCell(gapp_id, ss);
  }
  inline void SendMessageToCellMgr(const StringSlice& ss) override {
    assert(false);
  }
};

#define CELLMGR gw::cellmgr::CellMgr::GetInstance()
#define CELLMGR_CFG CELLMGR.config()
#define NOWMS CELLMGR.adapter()->NowMs()

GW_CELL_MGR_END