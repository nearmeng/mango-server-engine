#pragma once

#include "great_world_base/cellmgr/balance/balance_node.h"
#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

class MemWeightPolicy {
 public:
  void Dispose() {}
  virtual float CalcWeight(uint32_t memsize) const = 0;
  virtual ~MemWeightPolicy() {}
};

class BalanceResult {
 public:
  float edge() const { return edge_; }
  void set_edge(float edge) { edge_ = edge; }
  float old_edge() const { return old_edge_; }
  void set_old_edge(float old_edge) { old_edge_ = old_edge; }

 private:
  float edge_ = 0;      // 调整后的边界
  float old_edge_ = 0;  // 调整前的边界
};

class BalancePolicy {
 public:
  virtual bool Balance(BalanceNode* curr_node, BalanceNode* next_node, BalanceResult& result) = 0;
};

GW_CELL_MGR_END