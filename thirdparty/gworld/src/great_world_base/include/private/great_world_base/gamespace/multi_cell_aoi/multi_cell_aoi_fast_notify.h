#pragma once

#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"

GW_GAME_BEGIN

class MultiCellAoiFastNotify : public MultiCellAoIObserver {
  GWCLASS_DECL(MultiCellAoiFastNotify)
 public:
  MultiCellAoiFastNotify();
  MultiCellAoiFastNotify(AoIEntity* owner, IMemoryOStream* os);

  StringSlice NotifyProp() override;
  uint32_t PushOneObservedData(MultiCellAoIObservedActor* observed_node, MultiCellAoIObserved* observed);

  bool BeginObserve(MultiCellAoIObserved* observed, PropLabel labels) override;

  // 迁移
  bool ReadFromMemoryStream(IMemoryIStream& is) override;

 private:
  SimpleQueue<uint64_t> observed_actors_;
};

GW_GAME_END