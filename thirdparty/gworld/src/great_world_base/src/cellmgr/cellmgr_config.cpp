#include "great_world_base/cellmgr/cellmgr_config.h"

#include "great_world_base/cellmgr/balance/default_mem_weight_policy.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(CellMgrConfig)

bool CellMgrConfig::reload() {
  if (reader_ == nullptr) {
    GW_LOG_ERROR("CellMgrConfig need a reader");
    return false;
  }
  if(!reader_->Reload()) {
    GW_LOG_ERROR("reader reload fail");
    return false;
  }

  GW_CONFIG_MEM_RELOAD(upper_normal_load)
  GW_CONFIG_MEM_RELOAD(upper_good_load)
  GW_CONFIG_MEM_RELOAD(upper_normal_load)
  GW_CONFIG_MEM_RELOAD(upper_good_memsize)
  GW_CONFIG_MEM_RELOAD(upper_normal_memsize)
  GW_CONFIG_MEM_RELOAD(critical_memsize)
  GW_CONFIG_MEM_RELOAD(upper_good_avatar_num)
  GW_CONFIG_MEM_RELOAD(upper_normal_avatar_num)
  GW_CONFIG_MEM_RELOAD(upper_invisible_normal_avatar_num)
  GW_CONFIG_MEM_RELOAD(auto_change_cell_num_cd)
  GW_CONFIG_MEM_RELOAD(load_min_diff)
  GW_CONFIG_MEM_RELOAD(load_min_ratio)
  GW_CONFIG_MEM_RELOAD(max_mem_weight)
  GW_CONFIG_MEM_RELOAD(good_mem_weight)
  GW_CONFIG_MEM_RELOAD(normal_mem_weight)
  GW_CONFIG_MEM_RELOAD(warning_mem_weight)
  GW_CONFIG_MEM_RELOAD(space_balance_round_mask)
  GW_CONFIG_MEM_RELOAD(gapp_balance_round_mask)
  GW_CONFIG_MEM_RELOAD(extra_load_for_new_cell)
  GW_CONFIG_MEM_RELOAD(load_ratio_for_remove_cell)
  GW_CONFIG_MEM_RELOAD(space_add_cell_cd)
  GW_CONFIG_MEM_RELOAD(space_remove_cell_cd)
  GW_CONFIG_MEM_RELOAD(space_add_strip_cd)
  GW_CONFIG_MEM_RELOAD(space_remove_strip_cd)
  GW_CONFIG_MEM_RELOAD(space_add_cd)
  GW_CONFIG_MEM_RELOAD(space_remove_cd)
  GW_CONFIG_MEM_RELOAD(balance_interval)
  GW_CONFIG_MEM_RELOAD(tracing_balance)
  GW_CONFIG_MEM_RELOAD(move_step)
  GW_CONFIG_MEM_RELOAD(removing_move_step)
  GW_CONFIG_MEM_RELOAD(min_edge_length)
  GW_CONFIG_MEM_RELOAD(entity_rect_opt)
  GW_CONFIG_MEM_RELOAD(entity_rect_gap)
  GW_CONFIG_MEM_RELOAD(visible_cell_parallel_max_size)
  GW_CONFIG_MEM_RELOAD(invisible_cell_parallel_max_size)

  if (visible_cell_parallel_max_size_ > 0 && upper_invisible_normal_avatar_num_ >= upper_normal_avatar_num_ * 2) {
    // 这种情况下可能出现循环的互通、隔离分线转换，而且如果差距这么大也不适合做互通了
    GW_LOG_ERROR("upper_invisible_normal_avatar_num error, please check load balance data");
    return false;
  }
  return true;
}

GW_CELL_MGR_END