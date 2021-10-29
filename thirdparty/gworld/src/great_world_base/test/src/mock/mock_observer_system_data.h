// #pragma once

// #include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"
// #include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observed.h"

// GW_GAME_BEGIN

// class MockMultiCellAoIObserverSystemData : public IMultiCellAoIObserverSystemData {
//  public:
//   ~MockMultiCellAoIObserverSystemData() {

//   }
  
//   MultiCellAoIObserved* get_observed(uint64_t eid) override {
//     auto it = observed_map_.find(eid);
//     if(it == observed_map_.end()) return nullptr;
//     return it->second;
//   }
  
//   GWVector<AoIOrderlyNotifyProp*> get_diff_prop(PropLabel, uint64_t) override {
//     return {};
//   }

//   void RegistObserved(MultiCellAoIObserved* observed) {
//     observed_map_.emplace(observed->owner_id(), observed);
//   }

//  private:
//   std::unordered_map<uint64_t, MultiCellAoIObserved*> observed_map_;
// };

// GW_GAME_END