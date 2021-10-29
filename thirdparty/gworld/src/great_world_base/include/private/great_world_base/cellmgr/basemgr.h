#pragma once
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/cell_message.h"

#define BASEMGR gw::cellmgr::BaseMgr::GetInstance()

GW_CELL_MGR_BEGIN

class BaseMgr : public ShmSingletonRef<BaseMgr> {
  GWCLASS_DECL(BaseMgr);
 public:
  void Init();
  void Uninit(bool retain_shm);
  void ReportLoad(uint64_t gapp_id, ReportLoadMsg& msg);

  uint64_t ChooseFreeGame() const;
  bool RegisterGameApp(uint64_t gapp_id);
  bool UnregisterGameApp(uint64_t gapp_id);
  bool HasGameApp(uint64_t gapp_id) { return this->gapps_.find(gapp_id) != this->gapps_.end(); }
  const GameAppEntries& game_apps() const { return this->gapps_; }

 private:
  GameAppEntries gapps_;
};

GW_CELL_MGR_END
