#include "great_world_base/gamespace/multi_cell_aoi/aoi_conf.h"

GW_GAME_BEGIN

bool AoIConfig::reload() {
  if (reader_ == nullptr) {
    GW_LOG_ERROR("AoIConfig need a reader");
    return false;
  }
  if (!reader_->Reload()) {
    GW_LOG_ERROR("reader reload fail");
    return false;
  }

  GW_CONFIG_MEM_RELOAD(observed_prop_buffer_max_num)
  GW_CONFIG_MEM_RELOAD(observed_prop_max_wait_num)
  GW_CONFIG_MEM_RELOAD(observer_entity_max_num)
  GW_CONFIG_MEM_RELOAD(observer_notify_entity_num_one_time)
  GW_CONFIG_MEM_RELOAD(observer_notify_bytes_one_time)
  GW_CONFIG_MEM_RELOAD(observer_notify_bytes_num_per_frame)
  GW_CONFIG_MEM_RELOAD(observer_label_check_ms_cd)

  return true;
}

GW_GAME_END