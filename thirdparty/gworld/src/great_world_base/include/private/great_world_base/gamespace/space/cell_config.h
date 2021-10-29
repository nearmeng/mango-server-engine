#pragma once

#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/reader/config.h"

GW_GAME_BEGIN

class CellConfig : public IConfig {
  GWCLASS_DECL(CellConfig)
 public:
  CellConfig() {}
  bool reload() override;

  // ghost离开边界比进入边界范围稍微大一点，防止来边界出反复来回触发。
  float ghost_dist_extend() const { return ghost_dist_ + 20; }

  GW_CONFIG_MEM(float, ghost_dist, GW_CONFIG_PATH("Cell", "ghost_dist"), 100.0)  // 全局ghost边界长度，单位：米
  GW_CONFIG_MEM(float, migrate_dist, GW_CONFIG_PATH("Cell", "migrate_dist"), 0)  // real_entity迁移边界长度，单位：米
  GW_CONFIG_MEM(uint32_t, max_create_ghost_num, GW_CONFIG_PATH("Cell", "max_create_ghost_num"),
                100)  // 每帧最大创建ghost的entity个数
  GW_CONFIG_MEM(uint32_t, max_migrate_num, GW_CONFIG_PATH("Cell", "max_migrate_num"), 20)  // 每帧最大迁移entity个数
  GW_CONFIG_MEM(bool, enable_ordered_cell_rpc, GW_CONFIG_PATH("Cell", "enable_ordered_cell_rpc"), false)
  GW_CONFIG_MEM(uint32_t, migrate_check_interval, GW_CONFIG_PATH("Cell", "migrate_check_interval"), 100)
};

GW_GAME_END