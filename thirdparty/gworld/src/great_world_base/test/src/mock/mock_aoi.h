#pragma once
#include <iostream>


#include "mock/mock_varint_stream.hpp"

GW_GAME_BEGIN

// extern uint32_t& aoi_observed_num();

// class MockAoICallBack : public IAoICallBack {
//   GWCLASS_DECL(MockAoICallBack)
//  public:
//   void OnBeginObserver(uint64_t self, uint64_t other) override {
//     aoi_observed_num()++;
//     printf("test OnBeginObserver self %lu, other %lu\n", self, other);
//   }

//   void OnEndObserver(uint64_t self, uint64_t other) override {
//     aoi_observed_num()--;
//     printf("test OnEndObserver self %lu, other %lu\n", self, other);
//   }

//   GWVector<AoIDiffProp>&& GetDiffProps(uint64_t, uint32_t) override {
//     gw::GWVector<gw::game::AoIDiffProp> v;
//     return std::move(v);
//   }

//   IMemoryOStream* NotifyOS() override { return &os_; }

//  private:
//   gw::mock::MockMemoryOStream os_;
// };

GW_GAME_END