#include "great_world_base/gamespace/space/cell_config.h"

GW_GAME_BEGIN

GWCLASS_DECL_IMPLEMENT(CellConfig)

bool CellConfig::reload() {
  if (reader_ == nullptr) {
    GW_LOG_ERROR("AoIConfig need a reader");
    return false;
  }
  if (!reader_->Reload()) {
    GW_LOG_ERROR("reader reload fail");
    return false;
  }

  GW_CONFIG_MEM_RELOAD(ghost_dist)
  GW_CONFIG_MEM_RELOAD(migrate_dist)
  GW_CONFIG_MEM_RELOAD(max_create_ghost_num)
  GW_CONFIG_MEM_RELOAD(max_migrate_num)
  GW_CONFIG_MEM_RELOAD(enable_ordered_cell_rpc)
  GW_CONFIG_MEM_RELOAD(migrate_check_interval)
  return true;
}

GW_GAME_END