#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_manager.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"

#include "great_world_base/gamespace/multi_cell_aoi/notify_ring_buffer.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/gamespace/multi_cell_aoi/default_multi_cell_aoi_filter.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"
#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/vector3.h"
#include "mock/mock_varint_stream.hpp"
#include "gtest/gtest.h"
#include "mock/log.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/timer/timer_mgr.h"
#include "great_world_base/common/common_private.h"
#include "mock/mock_aoi.h"

#include "mock/mock_gamespace_adapter.h"

namespace {

void on_enter_aoi(uint64_t self, uint64_t other, const gw::Vector3& pos) {
  GW_LOG_INFO("test on_enter_aoi self %lu, other %lu", self, other);
}

void on_leave_aoi(uint64_t self, uint64_t other, const gw::Vector3& pos) {
  GW_LOG_INFO("test on_leave_aoi self %lu, other %lu", self, other);
}

}  // namespace


class TestAoI : public testing::Test {
 public:
  ~TestAoI() {
  }

  TestAoI() {
  }

  static void SetUpTestCase() {
    SHM_MGR.set_malloc_func(std::malloc);
    SHM_MGR.set_free_func(std::free);

    auto mock_log = new gw::MockLogger();
    mock_log->set_priority(gw::LOG_PRIORITY_TRACE);
    gw::Logger::InitLogger(mock_log);

    // IMemoryOStream* NotifyOS
    uint8_t buffer[256];
    gw::game::GameSpaceApi::Init(new gw::GameSpaceAdapter());
  }
  static gw::mock::MockMemoryOStream os_;
};
gw::mock::MockMemoryOStream TestAoI::os_;

TEST_F(TestAoI, EntityEnterLeave) {
  const uint64_t space_id = 2;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1000, 15, {2750, -2695, 0}));
  // auto aoi_entity_2 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1001, 15, {2750, -2695, 0}));
  // ASSERT_EQ(aoi_mgr->entity_size(), 2);
  // AOI_WORLD.Leave(space_id, 1001);
  // ASSERT_EQ(aoi_mgr->entity_size(), 1);
  auto aoi_entity_3 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1002, 15, {2746, -2693, 0}));
  // DebugOutput
  aoi_mgr->DebugOutput();
  ASSERT_EQ(aoi_mgr->entity_size(), 2);
  aoi_entity_3->Move({2706, -2693, 0});
  aoi_mgr->DebugOutput();
  ASSERT_EQ(aoi_entity_3->aoi_base_all_eids().size(), 0);
  ASSERT_EQ(aoi_entity_1->aoi_base_all_eids().size(), 0);
  gw::timer::TimerManager::Get().SpeedUp(2000);
  gw::timer::TimerManager::Get().Update();
}

TEST_F(TestAoI, Trigger) {
  const uint64_t space_id = 3;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = AOI_WORLD.Enter(space_id, 1000, 10, {1, 2, 0});
  aoi_entity_1->AddTrigger(10, on_enter_aoi, on_leave_aoi);
  auto aoi_entity_2 = AOI_WORLD.Enter(space_id, 1001, 10, {1, 2, 0});
  AOI_WORLD.Leave(space_id, 1001);
}

TEST_F(TestAoI, RingBuffer) {
  gw::game::NotifyRingBuffer buffer;
  buffer.Resize(3);
  for (size_t i = 0; i < 10; i++) {
    gw::game::AoIOrderlyNotifyProp* p = gw::ShmNew<gw::game::AoIOrderlyNotifyProp>(i, 1, nullptr, 0, 1, 1);
    buffer.Push(p);
  }
  ASSERT_EQ(buffer.size(), 3);
  ASSERT_EQ(buffer.first()->id_, 7);
}

TEST_F(TestAoI, AddNotifyData) {
  const uint64_t space_id = 4;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1000, 10, {1, 2, 0}));
  gw::StringSlice s("111");
  ASSERT_EQ(aoi_entity_1->observed()->next_id(), 1);
  uint32_t order_id = aoi_entity_1->AddNotifyData(1, s, 0, gw::GetWalltimeMs());
  ASSERT_NE(order_id, 2);
  ASSERT_EQ(aoi_entity_1->observed()->next_id(), 2);
}

TEST_F(TestAoI, Filter) {
  const uint64_t space_id = 5;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1000, 10, {1, 2, 0}));
  auto aoi_entity_2 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1001, 10, {1, 2, 0}));
  auto aoi_entity_3 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1002, 10, {1, 2, 0}));
  auto aoi_entity_4 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1003, 10, {1, 2, 0}));
  auto filter1 = dynamic_cast<gw::game::DefaultMultiCellAoIFilter*>(aoi_entity_1->filter());
  ASSERT_EQ(aoi_entity_1->aoi_base_all_eids().size(), 3);
}

TEST_F(TestAoI, Notify) {
  const uint64_t space_id = 6;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1000, 10, {1, 2, 0}));
  auto aoi_entity_2 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1001, 10, {1, 2, 0}));
  // gw::StringSlice s("111");
  // uint32_t order_id = aoi_entity_1->AddNotifyData(1, s, 0);
  // aoi_entity_2->Notify();
}

TEST_F(TestAoI, RWStream) {
  const uint64_t space_id = 7;
  AOI_WORLD.CreateSpace(space_id);
  auto aoi_mgr = dynamic_cast<gw::game::AoIManager*>(AOI_WORLD.GetAoIManager(space_id));
  auto aoi_entity_1 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1000, 10, {1, 2, 0}));
  auto aoi_entity_2 = dynamic_cast<gw::game::AoIEntity*>(AOI_WORLD.Enter(space_id, 1001, 10, {1, 2, 0}));
  gw::mock::MockMemoryOStream os;
  ASSERT_EQ(aoi_entity_1->WriteToMemoryStreamGhost(os), true);
  ASSERT_EQ(aoi_entity_1->WriteToMemoryStreamReal(os), true);
  gw::mock::MockMemoryIStream is(os.view());
  ASSERT_EQ(aoi_entity_2->ReadFromMemoryStreamGhost(is), true);
  ASSERT_EQ(aoi_entity_2->ReadFromMemoryStreamReal(is), true);
}