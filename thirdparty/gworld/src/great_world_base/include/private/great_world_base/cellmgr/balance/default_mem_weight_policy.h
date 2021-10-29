#pragma once

#include "great_world_base/cellmgr/balance/balance_policy.h"

GW_CELL_MGR_BEGIN

class DefaultMemWeightPolicy : public MemWeightPolicy {
  GWCLASS_DECL(DefaultMemWeightPolicy)
 public:
  DefaultMemWeightPolicy();
  ~DefaultMemWeightPolicy();
  float CalcWeight(uint32_t memsize) const override;

 private:
  uint32_t upper_good_memsize_;
  uint32_t upper_normal_memsize_;
  uint32_t critical_memsize_;
  float max_mem_weight_;
  float good_mem_weight_;
  float normal_mem_weight_;
  float warning_mem_weight_;
};

GW_CELL_MGR_END