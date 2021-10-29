#include "great_world_base/cellmgr/balance/default_balance_policy.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/balance/balance_node.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/common/math_utility.h"

GW_CELL_MGR_BEGIN

GWCLASS_DECL_IMPLEMENT(DefaultBalancePolicy)

DefaultBalancePolicy::DefaultBalancePolicy(bool vertical, bool tracing) : vertical_(vertical), tracing_(tracing) {
}

bool DefaultBalancePolicy::Balance(BalanceNode* curr_node, BalanceNode* next_node, BalanceResult& result) {
  GW_LOG_DEBUG("DBP Balance1, [%s], [%s]", curr_node->to_string().c_str(), next_node->to_string().c_str());
  if (curr_node->removing() || next_node->removing()) {
    return BalanceRemoving(curr_node, next_node, result);
  }
  float curr_origin_load = curr_node->conv_load();
  uint32_t curr_mem = curr_node->memsize();
  // 节点的内存权重
  float curr_mem_weight = CELLMGR.mem_weight_policy().CalcWeight(curr_mem);

  float next_origin_load = next_node->conv_load();
  uint32_t next_mem = next_node->memsize();
  // 临近节点的内存权重
  float next_mem_weight = CELLMGR.mem_weight_policy().CalcWeight(next_mem);

  float origin_load_diff = curr_origin_load - next_origin_load;
  GW_LOG_DEBUG("DBP Balance2, curr load:%f, next load:%f, curr mem:%d, next mem:%d", curr_origin_load, next_origin_load, curr_mem, next_mem);
  // 归一化后的内存差
  float mem_diff = std::min(1.0f, static_cast<int32_t>(curr_mem - next_mem) * 1.0f / CELLMGR_CFG.critical_memsize());
  if (origin_load_diff * mem_diff < 0) {  // cpu负载和内存差异保持一致
    return false;
  }
  float mem_weight = (curr_mem_weight + next_mem_weight) / 2;
  // 统一归一化后的负载差
  float load_diff = origin_load_diff * (1 - mem_weight) + mem_diff * mem_weight;
  // TODO(cleve) when game app is not available
  float abs_load_diff = std::abs(origin_load_diff);
  // 如果负载差距不大或者内存较小则不调整
  if (abs_load_diff < CELLMGR_CFG.load_min_diff() ||
      std::min(curr_mem, next_mem) < CELLMGR_CFG.load_min_ratio() * CELLMGR_CFG.upper_good_memsize()) {
    return false;
  }
  const Rect& curr_rect = curr_node->node_rect();
  const Rect& next_rect = next_node->node_rect();
  float edge = curr_rect.max(vertical_);
  float min_edge = curr_rect.min(vertical_);
  float max_edge = next_rect.max(vertical_);
  float old_edge = edge;
  float entity_rect_edge = 0.0f;
  float tile_rect_edge = 0.0f;
  if (load_diff > 0.0f) {  // 移动边界 from next to curr
    entity_rect_edge = curr_node->entity_rect().max(vertical_) + CELLMGR_CFG.entity_rect_gap();
    tile_rect_edge = next_node->tile_rect().min(vertical_) + CELLMGR_CFG.min_edge_length();
    // 1.curr长度大于ghost距离，防止在同一方向上产生过多的ghost
    // 2.curr边界大于entity边界，尽快降低负载
    // 3.next边界小于tile边界，只能移动到已加载资源区域
    if (curr_rect.length(vertical_) > CELLMGR_CFG.min_edge_length()) {
      if (CELLMGR_CFG.entity_rect_opt() && edge > entity_rect_edge && edge > tile_rect_edge) {
        edge = std::max(min_edge + CELLMGR_CFG.min_edge_length(), entity_rect_edge);
        edge = std::max(edge, tile_rect_edge);
      } else {
        edge -= CELLMGR_CFG.move_step();
      }
    }
    if (curr_node->init_expanding()) {
      curr_node->set_init_expanding(false);
    }
  } else {  // 移动边界 from curr to next
    entity_rect_edge = next_node->entity_rect().min(vertical_) - CELLMGR_CFG.entity_rect_gap();
    tile_rect_edge = curr_node->tile_rect().max(vertical_) - CELLMGR_CFG.min_edge_length();
    if (next_rect.length(vertical_) > CELLMGR_CFG.min_edge_length()) {
      if (CELLMGR_CFG.entity_rect_opt() && edge < entity_rect_edge && edge < tile_rect_edge) {
        edge = std::min(max_edge - CELLMGR_CFG.min_edge_length(), entity_rect_edge);
        edge = std::min(edge, tile_rect_edge);
      } else {
        edge += CELLMGR_CFG.move_step();
      }
    }
    if (next_node->init_expanding()) {
      next_node->set_init_expanding(false);
    }
  }
  edge = MathUtility::Clamp(min_edge, edge, max_edge);
  curr_node->mutable_node_rect().set_max(edge, vertical_);
  next_node->mutable_node_rect().set_min(edge, vertical_);
  result.set_edge(edge);
  result.set_old_edge(old_edge);
  return !MathUtility::IsEqual(old_edge, edge);  // 边界是否发生了变化
}

bool DefaultBalancePolicy::BalanceRemoving(BalanceNode* curr_node, BalanceNode* next_node, BalanceResult& result) {
  // TODO(cleve) refactor with Balance
  const Rect& curr_rect = curr_node->node_rect();
  const Rect& next_rect = next_node->node_rect();
  float edge = curr_rect.max(vertical_);
  float min_edge = curr_rect.min(vertical_);
  float max_edge = next_rect.max(vertical_);
  float old_edge = edge;
  if (curr_node->removing()) {
    edge -= CELLMGR_CFG.removing_move_step();
  } else {
    edge += CELLMGR_CFG.removing_move_step();
  }
  edge = MathUtility::Clamp(min_edge, edge, max_edge);
  curr_node->mutable_node_rect().set_max(edge, vertical_);
  next_node->mutable_node_rect().set_min(edge, vertical_);
  result.set_edge(edge);
  result.set_old_edge(old_edge);
  return !MathUtility::IsEqual(old_edge, edge);  // 边界是否发生了变化
}

GW_CELL_MGR_END
