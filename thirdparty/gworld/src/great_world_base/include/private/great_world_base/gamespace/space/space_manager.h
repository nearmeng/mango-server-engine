#pragma once

#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/gamespace/space/game_app_entry.h"
#include "great_world_base/gamespace/space/parallel_balance.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/gamespace/space/cell_config.h"
#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/common/network/network_mgr.h"

GW_GAME_BEGIN

class IResLoaderScheduler;
class GameSpaceNetworkSender;

// game中没有Cell的复杂管理，更多的是Space的维护，因此取名SpaceManager
// 对应gamemgr中的CellMgr
class SpaceManager : public ShmSingletonRef<SpaceManager> {
  GWCLASS_DECL(SpaceManager);
 public:
  typedef GWHashMap<uint64_t, GameAppEntry*> GameAppEntries;

  RetCode Init(GWString conf_path);
  void UnInit(bool retain_shm);
  bool Clear();
  bool AddCell(uint64_t space_id, const GWString& tag, const StringSlice& layout_data, bool is_static);
  bool RemoveCell(uint64_t space_id);
  RetCode UpdateLayout(uint64_t space_id, const GWString& tag, const StringSlice& data, bool is_static);
  bool DestroySpace(uint64_t space_id);
  bool DoDestroySpace(uint64_t space_id);
  Space* GetSpace(uint64_t space_id);
  bool SyncCell(uint64_t space_id, StringSlice& layout_data, bool is_static);

  void ReportLoad();
  void CheckSpacesMigration();

  GameAppEntry* FindGameApp(uint64_t gapp_id, bool create_if_not_found = false);

  void OnChangeCellStateSuccessFromGame(uint64_t space_id, uint32_t index, uint64_t other_gapp_id, CellState other_state);
  bool InitSpaceResource(uint64_t space_id);
  bool QueryCell(uint64_t space_id, float x, float z, uint64_t& gapp_id);

  // 需要依赖注入的属性 begin
  inline uint64_t gapp_id() const { return gapp_id_; }
  inline void set_gapp_id(uint64_t gapp_id) { gapp_id_ = gapp_id; }

  // 需要依赖注入的属性 end

  Space* FindSpace(uint64_t space_id) {
    auto it = this->spaces_.find(space_id);
    if (it == this->spaces_.end()) {
      return nullptr;
    }
    return it->second;
  }

  const GWHashMap<uint64_t, Space*>& spaces() const { return this->spaces_; }

  void set_res_loader_scheduler(IResLoaderScheduler* res_loader_scheduler) { res_loader_scheduler_ = res_loader_scheduler; }
  IResLoaderScheduler* res_loader_scheduler() const { return res_loader_scheduler_; }

  uint64_t choose_migration_cell(uint64_t eid, uint64_t node_id);

  CellConfig& CellConf() { return cell_conf_; }

  inline IGameSpaceAdapter* adapter() { return adapter_; }
  inline void set_adapter(IGameSpaceAdapter* adapter) { adapter_ = adapter; }

 private:
  GWHashMap<uint64_t, Space*> spaces_;
  GameAppEntries gapps_;
  uint64_t gapp_id_ = 0;

  IResLoaderScheduler* res_loader_scheduler_ = nullptr;
  std::function<uint64_t(uint64_t, uint64_t)> choose_migration_cell_;

  uint64_t check_migrate_timer_ = 0;
  uint64_t report_load_timer_ = 0;

  CellConfig cell_conf_;
  
  IGameSpaceAdapter* adapter_;
};

#define SPACE_MGR gw::game::SpaceManager::GetInstance()
#define CELL_CFG SPACE_MGR.CellConf()

class GameSpaceNetworkSender : public NetworkSender {
  GWCLASS_DECL(GameSpaceNetworkSender)
 public:
  inline void SendMessageToCell(uint64_t gapp_id, const StringSlice& ss) override {
    SPACE_MGR.adapter()->SendMessageToCell(gapp_id, ss);
  }
  inline void SendMessageToCellMgr(const StringSlice& ss) override {
    SPACE_MGR.adapter()->SendMessageToCellMgr(ss);
  }
};

GW_GAME_END