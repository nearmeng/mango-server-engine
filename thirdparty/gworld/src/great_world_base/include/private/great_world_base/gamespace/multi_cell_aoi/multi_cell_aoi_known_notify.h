#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"

GW_GAME_BEGIN
// 有明显缺陷，一个boss被万人关注的话，每次更新数据要通知跟多人，这种可能无限制增加的不可靠
// 如果没有上诉情况的话这个是效率最高，延迟最小的

class MultiCellAoiKnownNotify : public MultiCellAoIObserver {
  GWCLASS_DECL(MultiCellAoiKnownNotify)
 public:
  MultiCellAoiKnownNotify() = default;
  MultiCellAoiKnownNotify(AoIEntity* owner, IMemoryOStream* os) : MultiCellAoIObserver(owner, os) {}

  StringSlice NotifyProp() override;
  void OnObservedDataAdded(uint64_t observed_eid) override { buffer_added_observed_eids_.insert(observed_eid); }

  // 迁移
  virtual bool WriteToMemoryStream(IMemoryOStream& os) override;
  virtual bool ReadFromMemoryStream(IMemoryIStream& is) override;

 private:
  GWHashSet<uint64_t> buffer_added_observed_eids_;
};

GW_GAME_END