#include <iostream>

#include <fstream>
#include <memory>

#include "great_world_base/common/ilog.h"
#include "gtest/gtest.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/shm/shm_allocator.h"
#include "great_world_base/cellmgr/cellmgr_api.h"

#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/log.h"
#include "mock/timer/timer_mgr.h"


// 1. 创建space调通，能收到各种回调
// 2. gamespace断space创建能通
// 3. aoi 能用

class TestSpaceLayout : public testing::Test {
 public:
  ~TestSpaceLayout() { 
  }

  TestSpaceLayout() {
  }

  static void SetUpTestCase() {
    SHM_MGR.set_malloc_func(std::malloc); // 可选，默认使用 std::malloc
    SHM_MGR.set_free_func(std::free); // 可选，默认使用 std::free
    gw::cellmgr::CellMgrApi::Init(new gw::CellMgrAdapter(buffer_, "./space/", "./cell_conf.json"));
    // 设置 gapp，gappid不能是 0
    gw::cellmgr::CellMgrApi::RegisterGameApp(301);
    gw::cellmgr::CellMgrApi::StartUp();
  }

  static void TearDownTestCase() {
  }

  virtual void TearDown() {
  }

 protected:
  static uint64_t cellmgr_space_;
  static uint8_t buffer_[256];
};

// gw::ShmMgr TestSpaceLayout::shm_mgr;
uint64_t TestSpaceLayout::cellmgr_space_ = 0;
uint8_t TestSpaceLayout::buffer_[256];

TEST_F(TestSpaceLayout, Cellmgr_CreateSpace) {
  gw::CreateSpaceMsg msg;
  msg.set_name("mock");
  msg.set_tag("1");
  msg.set_min_cell_num(1);
  msg.set_max_cell_num(100);
  msg.set_rect({0, -8100, 8100, 0});
  msg.set_map_name("test_space.xml");
  msg.set_is_static(true);
  cellmgr_space_ = gw::cellmgr::CellMgrApi::CreateSpace(msg);
  GW_LOG_TRACE("space:%lu", cellmgr_space_);
  ASSERT_NE(cellmgr_space_, 0);
  gw::timer::TimerManager::Get().SpeedUp(1000);
  gw::timer::TimerManager::Get().Update();
}

TEST_F(TestSpaceLayout, Debug_String) {
  GW_LOG_INFO("%s", gw::cellmgr::CellMgrApi::DebugString().c_str());
  gw::cellmgr::CellMgrApi::UpdateMinParallelNum(1, 1, 1);
}