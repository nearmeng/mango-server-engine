#include <fstream>
#include <iostream>
#include "great_world_base/common/ilog.h"
#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "gtest/gtest.h"
#include "mock/log.h"
#include "mock/mock_aoi.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/resume_helper.h"
#include "mock/timer/timer_mgr.h"
#include "mock/mock_gamespace_adapter.h"

#include "shm.h"
#include "shm_config.h"

#include "mock/mock_gamespace_adapter.h"

bool is_resume = false; // 本次是 resume 运行
bool retain_shm = false; // 本次进程关闭的时候保留shm

class TestSpaceLayout : public testing::Test {
 public:
  ~TestSpaceLayout() {
  }

  TestSpaceLayout() {
  }

  static void Init() {
#ifndef WIN32
    ShmCreate(1008611, 1, false);
    uint8_t* buffer = reinterpret_cast<uint8_t*>(ShmMalloc(sizeof(uint8_t) * 256));
#else
    uint8_t* buffer = reinterpret_cast<uint8_t*>(std::malloc(sizeof(uint8_t) * 256));
#endif  // WIN32
    auto ret = gw::game::GameSpaceApi::Init(new gw::GameSpaceAdapter());
    ASSERT_EQ(ret, gw::RetCode::SUCCESS);
    SaveGlobal(buffer);
  }

  static void Resume() {
#ifndef WIN32
    ShmCreate(1008611, 1, true);
#endif  // WIN32
    uint8_t* buffer = LoadGlobal();
    gw::game::GameSpaceApi::Resume(new gw::GameSpaceAdapter());
  }

  static void SetUpTestCase() {
    // auto mock_log = new gw::MockLogger();
    // mock_log->set_priority(gw::LOG_PRIORITY_TRACE);
    // gw::Logger::InitLogger(mock_log);
    if (is_resume) {
      Resume();
    } else {
      Init();
    }
  }

  static void TearDownTestCase() {
    if (!retain_shm) {
      gw::game::GameSpaceApi::UnInit(false);
#ifndef WIN32
      ShmDestroy();
#endif
    }
  }

  virtual void TearDown() {
  }

 protected:
  static gw::mock::MockMemoryOStream os_;
  static gw::game::ISpace* space_;
};
gw::game::ISpace* TestSpaceLayout::space_ = nullptr;
gw::mock::MockMemoryOStream TestSpaceLayout::os_;

TEST_F(TestSpaceLayout, GameSpace_CreateSpace) {
  if (is_resume)
    return;
  char space_data[1000];
  std::size_t space_data_size;
  std::ifstream infile;
  infile.open("../1_mock.data");
  infile >> space_data_size;
  infile.read(space_data, space_data_size);
  infile.close();

  gw::StringSlice ss(space_data, space_data_size);
  gw::mock::MockMemoryIStream is(ss);
  gw::AddCellMsg add_msg;
  add_msg.FromString(&is);
  gw::StringSlice tmp_ss(add_msg.mutable_layout_data().c_str(), add_msg.mutable_layout_data().size());
  GW_LOG_DEBUG("space.data len %d,%p", space_data_size, tmp_ss.data());

// uint64_t space_id, const StringSlice& data, bool is_static

  SPACE_MGR.AddCell(1, "", tmp_ss, false);
  space_ = SPACE_MGR.GetSpace(1);
  ASSERT_NE(space_, nullptr);
  auto ret_code = SPACE_MGR.UpdateLayout(1, "", tmp_ss, true);
  ASSERT_EQ(ret_code, gw::RetCode::SUCCESS);
  ASSERT_EQ(SPACE_MGR.DestroySpace(1), true);
  gw::timer::TimerManager::Get().SpeedUp(6000);
  gw::timer::TimerManager::Get().Update();
  SPACE_MGR.AddCell(1, "", tmp_ss, false);
  space_ = SPACE_MGR.GetSpace(1);
  ASSERT_NE(space_, nullptr);
}

TEST_F(TestSpaceLayout, GameSpace_CheckSpace) {
  space_ = SPACE_MGR.GetSpace(1);
  ASSERT_NE(space_, nullptr);
  space_->DebugOutput();
  printf("space_vtbl:%x\n", *(reinterpret_cast<uint64_t*>(space_)));
}

// AoI
TEST_F(TestSpaceLayout, AoI) {
  gw::Avatar avatar1;
  gw::Avatar avatar2;
  auto ret = space_->Enter(100, 1, 1, {0, 2, 0}, &avatar1);
  ASSERT_EQ(ret.second, gw::RetCode::SUCCESS);
  avatar1.set_cell_entity(ret.first);
  ret = space_->Enter(101, 1, 1, {1, 0, 0}, &avatar2);
  ASSERT_EQ(ret.second, gw::RetCode::SUCCESS);
  avatar2.set_cell_entity(ret.first);
  gw::timer::TimerManager::Get().SpeedUp(4000);
  gw::timer::TimerManager::Get().Update();
  GW_LOG_DEBUG("aoi_observed_num:%d", gw::GameSpaceAdapter::aoi_observed_num)
  ASSERT_EQ(gw::GameSpaceAdapter::aoi_observed_num, 2);
  avatar1.cell_entity()->Move({10, 100, 100});
  gw::timer::TimerManager::Get().SpeedUp(4000);
  gw::timer::TimerManager::Get().Update();
  ASSERT_EQ(gw::GameSpaceAdapter::aoi_observed_num, 0);
  space_->Leave(avatar1.cell_entity());
  space_->Leave(avatar2.cell_entity());
}

// entity
TEST_F(TestSpaceLayout, Migrate) {
  space_->DebugOutput();
  gw::Avatar avatar;
  auto ret = space_->Enter(1, 1, 1, {1,2,3}, &avatar);
  ASSERT_EQ(ret.second, gw::RetCode::SUCCESS);
}

// entity
TEST_F(TestSpaceLayout, CellStateChangeInfo) {
  gw::mock::MockMemoryOStream os;
  gw::CellStateChangeInfo req(1, 303, static_cast<uint8_t>(gw::CellState::kInit), static_cast<uint8_t>(gw::CellState::kInit), 1);
  req.ToString(&os);
  gw::mock::MockMemoryIStream is(os.view());
  gw::CellStateChangeInfo req2;
  req2.FromString(&is);
  ASSERT_EQ(req.space_id(), req2.space_id());
}

// stream move
TEST_F(TestSpaceLayout, StreamMove) {
  gw::mock::MockMemoryOStream os;
  uint64_t i_v1 = 1;
  uint64_t i_v2 = 234234234;
  uint64_t i_v3 = 0;
  float f_v = 1.555;
  gw::GWString tmp_s = "asdasdasd";
  os << i_v1 << i_v2 << i_v3 << tmp_s << f_v << "as";
  
  gw::mock::MockMemoryIStream is(os.view());
  is >> i_v1 >> i_v2;
  ASSERT_EQ(i_v1, 1);
  ASSERT_EQ(i_v2, 234234234);
  gw::GWVector<char> tmp;
  printf("before move %u\n", is.LeftSize());
  is.MoveLeftTo(tmp);
  printf("%u after move %u\n", os.size(), tmp.size());
  ASSERT_EQ(is.LeftSize(), 0);

  gw::StringSlice ss(&tmp[0], tmp.size());
  gw::mock::MockMemoryIStream is2(ss);
  is2 >> i_v3 >> tmp_s >> f_v;
  printf("f_v:%f\n", f_v);
  ASSERT_EQ(i_v3, 0);
  ASSERT_EQ(tmp_s, "asdasdasd");
  ASSERT_TRUE(std::abs(f_v - 1.555) < 0.002);
}

int main(int argc, char** argv) {
  for (size_t i = 0; i < argc; i++) {
    if (!is_resume && strstr(argv[i], "is_resume") > 0) {
      is_resume = true;
    }
    if (!retain_shm && strstr(argv[i], "retain_shm") > 0) {
      retain_shm = true;
    }
  }
  if (is_resume || retain_shm) {
#ifndef WIN32
    SHM_MGR.set_malloc_func(ShmMalloc);
    SHM_MGR.set_free_func(ShmFree);
    ShmConfig::Instance().SetEnable(true);
#endif
  } else {
    SHM_MGR.set_malloc_func(std::malloc);
    SHM_MGR.set_free_func(std::free);
#ifndef WIN32
    ShmConfig::Instance().SetEnable(false);
#endif
  }
  printf("-->is_resume %d, retain_shm %d\n", is_resume, retain_shm);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}