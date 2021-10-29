#include "great_world_base/cellmgr/balance/default_mem_weight_policy.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include <algorithm>

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(DefaultMemWeightPolicy)
DefaultMemWeightPolicy::DefaultMemWeightPolicy() {
  upper_good_memsize_ = CELLMGR_CFG.upper_good_memsize();
  upper_normal_memsize_ = CELLMGR_CFG.upper_normal_memsize();
  critical_memsize_ = CELLMGR_CFG.critical_memsize();
  max_mem_weight_ = CELLMGR_CFG.max_mem_weight();
  good_mem_weight_ = CELLMGR_CFG.good_mem_weight();
  normal_mem_weight_ = CELLMGR_CFG.normal_mem_weight();
  warning_mem_weight_ = CELLMGR_CFG.warning_mem_weight();
}

DefaultMemWeightPolicy::~DefaultMemWeightPolicy() {
}

// 一个分段函数的实现
float DefaultMemWeightPolicy::CalcWeight(uint32_t memsize) const {
  float weight = 0;
  if (memsize < upper_good_memsize_) {
    weight = good_mem_weight_;
  } else if (memsize < upper_normal_memsize_) {
    weight = (memsize - upper_good_memsize_) / (upper_normal_memsize_ - upper_good_memsize_) * normal_mem_weight_ + good_mem_weight_;
  } else if (memsize < critical_memsize_) {
    weight = (memsize - upper_normal_memsize_) / (critical_memsize_ - upper_normal_memsize_) * warning_mem_weight_ + normal_mem_weight_ + good_mem_weight_;
  }
  return std::min(weight, max_mem_weight_);
}

GW_CELL_MGR_END