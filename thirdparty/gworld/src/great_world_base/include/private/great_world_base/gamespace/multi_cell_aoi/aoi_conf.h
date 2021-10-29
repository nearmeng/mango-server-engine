#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_common.h"
#include "great_world_base/common/reader/reader.h"
#include "great_world_base/common/reader/config.h"
GW_GAME_BEGIN

class AoIConfig : public IConfig {
  GWCLASS_DECL(AoIConfig)
  
 public:
  AoIConfig() { }
  bool reload() override;

  GW_CONFIG_MEM(uint32_t, observed_prop_buffer_max_num, GW_CONFIG_PATH("AoI", "observed_prop_buffer_max_num"),
                5000)  // 被观察者的buffer里面最多放多少个节点
  GW_CONFIG_MEM(uint32_t, observed_prop_max_wait_num, GW_CONFIG_PATH("AoI", "observed_prop_max_wait_num"),
                200)  // 乱序情况下最多等多少个乱序节点
  GW_CONFIG_MEM(uint32_t, observer_entity_max_num, GW_CONFIG_PATH("AoI", "observer_entity_max_num"),
                100)  // 每个人最多观察多少个entity
  GW_CONFIG_MEM(uint32_t, observer_notify_entity_num_one_time,
                GW_CONFIG_PATH("AoI", "observer_notify_entity_num_one_time"), 65536)  // 单次同步的遍历的玩家数
  GW_CONFIG_MEM(uint32_t, observer_notify_bytes_one_time, GW_CONFIG_PATH("AoI", "observer_notify_bytes_one_time"),
                8388608)  // 单次同步的字节
  GW_CONFIG_MEM(uint32_t, observer_notify_bytes_num_per_frame,
                GW_CONFIG_PATH("AoI", "observer_notify_bytes_num_per_frame"), 8388608)  // 单帧最多同步的数据量
  GW_CONFIG_MEM(uint32_t, observer_label_check_ms_cd, GW_CONFIG_PATH("AoI", "observer_label_check_ms_cd"),
                5000)  // label差值同步的cd
};

GW_GAME_END