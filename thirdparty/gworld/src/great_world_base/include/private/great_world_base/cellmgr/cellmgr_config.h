#pragma once
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/cellmgr_config_reader.h"
#include "great_world_base/common/reader/reader.h"
#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/reader/config.h"

GW_CELL_MGR_BEGIN

class CellMgrConfig : public IConfig {
  GWCLASS_DECL(CellMgrConfig)
 public:
  CellMgrConfig() {}

  bool reload() override;

 private:

  GW_CONFIG_MEM(float, upper_good_load, GW_CONFIG_PATH("CellConfig", "upper_good_load"),
                0.2f)  // 良好情况下的cpu负载上限
  GW_CONFIG_MEM(float, upper_normal_load, GW_CONFIG_PATH("CellConfig", "upper_normal_load"),
                0.8f)  // 良好情况下的内存占用上限，单位：MB
  GW_CONFIG_MEM(uint32_t, upper_good_memsize, GW_CONFIG_PATH("CellConfig", "upper_good_memsize"), 1024)
  GW_CONFIG_MEM(uint32_t, upper_normal_memsize, GW_CONFIG_PATH("CellConfig", "upper_normal_memsize"),
                2048)  // 普通情况下的内存占用上限，单位：MB
  GW_CONFIG_MEM(uint32_t, critical_memsize, GW_CONFIG_PATH("CellConfig", "critical_memsize"),
                4096)  // 内存占用的警戒线，单位：MB
  GW_CONFIG_MEM(uint32_t, upper_good_avatar_num, GW_CONFIG_PATH("CellConfig", "upper_good_avatar_num"),
                200)  // 良好情况下的玩家人数上限
  GW_CONFIG_MEM(uint32_t, upper_normal_avatar_num, GW_CONFIG_PATH("CellConfig", "upper_normal_avatar_num"),
                300)  // 互通情况或者不开分线下的玩家人数上限
  GW_CONFIG_MEM(uint32_t, upper_invisible_normal_avatar_num,
                GW_CONFIG_PATH("CellConfig", "upper_invisible_normal_avatar_num"), 400)  // 隔离情况下的玩家人数上限
  GW_CONFIG_MEM(uint32_t, auto_change_cell_num_cd, GW_CONFIG_PATH("CellConfig", "auto_change_cell_num_cd"), 1000 * 10)
  GW_CONFIG_MEM(float, load_min_diff, GW_CONFIG_PATH("CellConfig", "load_min_diff"),
                0.15)  // 负载差距要求下限（小于该值不移动边界）
  GW_CONFIG_MEM(float, load_min_ratio, GW_CONFIG_PATH("CellConfig", "load_min_ratio"), 0.3)  // 用于调整负载下限的系数
  GW_CONFIG_MEM(float, max_mem_weight, GW_CONFIG_PATH("CellConfig", "max_mem_weight"), 0.6)  // [0,1] 最大内存权重
  GW_CONFIG_MEM(float, good_mem_weight, GW_CONFIG_PATH("CellConfig", "good_mem_weight"),
                0.1)  // [0,1] 内存小于upper_good_memsize_时的固定权重
  GW_CONFIG_MEM(float, normal_mem_weight, GW_CONFIG_PATH("CellConfig", "normal_mem_weight"),
                0.2)  // [0,1] 内存在upper_good_memsize_/upper_normal_memsize_之间的额外系数
  GW_CONFIG_MEM(float, warning_mem_weight, GW_CONFIG_PATH("CellConfig", "warning_mem_weight"),
                0.3)  // [0,1] 内存超出upper_normal_memsize_时的额外权重系数
  GW_CONFIG_MEM(uint8_t, space_balance_round_mask, GW_CONFIG_PATH("CellConfig", "space_balance_round_mask"), 7)
  GW_CONFIG_MEM(uint8_t, gapp_balance_round_mask, GW_CONFIG_PATH("CellConfig", "gapp_balance_round_mask"), 7)
  GW_CONFIG_MEM(float, extra_load_for_new_cell, GW_CONFIG_PATH("CellConfig", "extra_load_for_new_cell"),
                0.1)  // [0,1] 添加新cell时预估增加的负载
  GW_CONFIG_MEM(float, load_ratio_for_remove_cell, GW_CONFIG_PATH("CellConfig", "load_ratio_for_remove_cell"),
                0.7)  // [0, 1] 相对于与upper_good_load_的比例
  GW_CONFIG_MEM(uint32_t, space_add_cell_cd, GW_CONFIG_PATH("CellConfig", "space_add_cell_cd"),
                1000)  // 往space添加新的cell的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, space_remove_cell_cd, GW_CONFIG_PATH("CellConfig", "space_remove_cell_cd"),
                2000)  // 从space移除cell的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, space_add_strip_cd, GW_CONFIG_PATH("CellConfig", "space_add_strip_cd"),
                5000)  // 往space添加新的strip的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, space_remove_strip_cd, GW_CONFIG_PATH("CellConfig", "space_remove_strip_cd"),
                6000)  // 从space移除cell的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, space_add_cd, GW_CONFIG_PATH("CellConfig", "space_add_cd"),
                3000)  // 往space添加的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, space_remove_cd, GW_CONFIG_PATH("CellConfig", "space_remove_cd"),
                4000)  // 从space移除的CD间隔 单位：毫秒
  GW_CONFIG_MEM(uint32_t, balance_interval, GW_CONFIG_PATH("CellConfig", "balance_interval"), 100)  // 平衡频率 单位：毫秒
  GW_CONFIG_MEM(bool, tracing_balance, GW_CONFIG_PATH("CellConfig", "tracing_balance"), false)
  GW_CONFIG_MEM(float, move_step, GW_CONFIG_PATH("CellConfig", "move_step"), 0.1)  // 边界移动步长，单位：米
  GW_CONFIG_MEM(float, removing_move_step, GW_CONFIG_PATH("CellConfig", "removing_move_step"),
                1.0)  // 处于移除状态中的cell的边界移动步长，单位：米
  GW_CONFIG_MEM(float, min_edge_length, GW_CONFIG_PATH("CellConfig", "min_edge_length"),
                100.0)  // 区域边界的下限(除非在移除过程中)，单位：米
  GW_CONFIG_MEM(bool, entity_rect_opt, GW_CONFIG_PATH("CellConfig", "entity_rect_opt"),
                false)  // balance的entity_rect优化
  GW_CONFIG_MEM(float, entity_rect_gap, GW_CONFIG_PATH("CellConfig", "entity_rect_gap"),
                1.0)  // balance的entity_rect优化间隔
  GW_CONFIG_MEM(uint32_t, visible_cell_parallel_max_size,
                GW_CONFIG_PATH("CellConfig", "visible_cell_parallel_max_size"),
                5)  // 互通 cell 分线的最大数量，这个值不包括主线
  GW_CONFIG_MEM(uint32_t, invisible_cell_parallel_max_size,
                GW_CONFIG_PATH("CellConfig", "invisible_cell_parallel_max_size"),
                50)  // 隔离 cell 分线的最大数量，这个值不包括主线
};

GW_CELL_MGR_END