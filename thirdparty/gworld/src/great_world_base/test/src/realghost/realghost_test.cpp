#include <fstream>
#include <iostream>
#include <functional>

#include "great_world_base/common/ilog.h"
#include "great_world_base/common/common.h"
#include "great_world_base/gamespace/space/cell.h"
#include "great_world_base/gamespace/space/cell_config.h"
#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/common/cell_message.h"


#include "gtest/gtest.h"
#include "mock/log.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/timer/timer_mgr.h"
#include "mock/mock_varint_stream.hpp"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

#include "great_world_base/gamespace/gamespace_api.h"
#include "mock/mock_aoi.h"

#include "mock/mock_gamespace_adapter.h"

using namespace gw::game;
using namespace gw;

typedef uint64_t EntityId;

class TestCellMigration : public testing::Test {
 public:
  TestCellMigration() : space_width_(20000), space_height_(10000) {}
  static gw::mock::MockMemoryOStream os_;
  static gw::game::Space* space_;

  static void SetUpTestCase() {    
    uint8_t buffer[256];
    gw::game::GameSpaceApi::Init(new gw::GameSpaceAdapter());
    char space_data[1000];
    std::size_t space_data_size;
    std::ifstream infile;
    infile.open("../spacetag_mock.data");
    infile >> space_data_size;
    infile.read(space_data, space_data_size);
    infile.close();
    GW_LOG_DEBUG("space.data len %d", space_data_size);

    gw::StringSlice ss(space_data, space_data_size);
    gw::mock::MockMemoryIStream is(ss);
    AddCellMsg add_msg;
    add_msg.FromString(&is);
    gw::StringSlice tmp_ss(add_msg.layout_data().c_str(), add_msg.layout_data().size());

    SPACE_MGR.AddCell(1, "", tmp_ss, false);
    space_ = reinterpret_cast<gw::game::Space*>(SPACE_MGR.GetSpace(1));

    space_->ChangeOtherCellState(CellState::kCreated, 1);
    space_->ChangeOtherCellState(CellState::kCreated, 2);
    space_->ChangeOtherCellState(CellState::kCreated, 3);
    ASSERT_NE(space_, nullptr);
  }

  static void TearDownTestCase() {
  }

  virtual void SetUp() {
    ASSERT_EQ(SPACE_MGR.DestroySpace(1), true);
    gw::timer::TimerManager::Get().SpeedUp(6000);
    gw::timer::TimerManager::Get().Update();

    char space_data[1000];
    std::size_t space_data_size;
    std::ifstream infile;
    infile.open("../spacetag_mock.data");
    infile >> space_data_size;
    infile.read(space_data, space_data_size);
    infile.close();
    GW_LOG_DEBUG("space.data len %d", space_data_size);

    gw::StringSlice ss(space_data, space_data_size);
    gw::mock::MockMemoryIStream is(ss);
    AddCellMsg add_msg;
    add_msg.FromString(&is);
    gw::StringSlice tmp_ss(add_msg.layout_data().c_str(), add_msg.layout_data().size());
    SPACE_MGR.AddCell(1, "", tmp_ss, false);
    space_ = reinterpret_cast<gw::game::Space*>(SPACE_MGR.GetSpace(1));

    space_->ChangeOtherCellState(CellState::kCreated, 302);
    space_->ChangeOtherCellState(CellState::kCreated, 303);
    space_->ChangeOtherCellState(CellState::kCreated, 304);
    ASSERT_NE(space_, nullptr);
  }

  virtual void TearDown() { 

  }

 protected:
  int space_width_;
  int space_height_;
};
gw::mock::MockMemoryOStream TestCellMigration::os_;
gw::game::Space* TestCellMigration::space_;

TEST_F(TestCellMigration, OnMigrate) {
  EntityId id = 1;

  gw::Avatar avatar;
  auto ret = space_->Enter(id, 1, 1, {
    static_cast<float>(space_width_ / 2 + CELL_CFG.migrate_dist() + 10),
    0, 
    static_cast<float>(space_height_ / 4)
  }, &avatar);
  ASSERT_EQ(ret.second, SUCCESS);
  GW_LOG_DEBUG("avatar pos:%f,%f", static_cast<float>(space_width_ / 2 + CELL_CFG.migrate_dist() + 10), 
      static_cast<float>(space_height_ / 4));
  avatar.set_cell_entity(ret.first);

  EXPECT_EQ(avatar.migrate_count(), 0);
  gw::game::SpaceManager::GetInstance().CheckSpacesMigration();  // 先创ghost
  gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
  EXPECT_EQ(avatar.migrate_count(), 1);
  gw::game::SpaceManager::GetInstance().CheckSpacesMigration();  // 多次调用，只出发一次。
  EXPECT_EQ(avatar.migrate_count(), 1);
}

// TEST_F(TestCellMigration, OnCreateGhost) {
//     EntityId id = 1;

//   gw::Avatar avatar;
//   auto ret = space_->Enter(id, {
//     static_cast<float>(space_width_ / 2 - CELL_CFG.ghost_dist() + 10),
//     0, 
//     static_cast<float>(space_height_ / 4)
//   }, &avatar);
//   ASSERT_EQ(ret.second, SUCCESS);
//   GW_LOG_DEBUG("avatar pos:%f,%f", static_cast<float>(space_width_ / 2 + CELL_CFG.migrate_dist() + 10), 
//       static_cast<float>(space_height_ / 4));

//   EXPECT_EQ(avatar->create_count(), 0);
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   EXPECT_EQ(avatar->create_count(), 1);
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();  // 多次调用，只出发一次。
//   EXPECT_EQ(avatar->create_count(), 1);
// }

// TEST_F(TestCellMigration, OnDeleteGhost) {
//   EntityId id = 1;
//   MockEntityEntry* entry = MockEntityEntry::Create(id);
//   auto space = gw::game::SpaceManager::GetInstance().GetSpace(1);
//   space->cell()->AddEntityEntry(id, entry);

//   entry->set_position(Vector3(
//       space_width_ / 2 - CELL_CFG.ghost_dist() + 10, 0, space_height_ / 4));

//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   EXPECT_EQ(entry->create_count(), 1);
//   EXPECT_EQ(entry->delete_count(), 0);
//   entry->set_position(
//       Vector3(space_width_ / 2 - CELL_CFG.ghost_dist_extend() - 10, 0,
//                         space_height_ / 4));
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   EXPECT_EQ(entry->delete_count(), 1);
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();  // 此时不应该还有触发。
//   EXPECT_EQ(entry->create_count(), 1);
//   EXPECT_EQ(entry->delete_count(), 1);
// }

// TEST_F(TestCellMigration, MaxMigrateNum) {
//   std::vector<MockEntityEntry*> entries;
//   for (uint64_t id = 1; id <= CELL_CFG.max_migrate_num() + 1; id++) {
//     MockEntityEntry* entry = MockEntityEntry::Create(id);
//     entries.push_back(entry);
//     auto space = gw::game::SpaceManager::GetInstance().GetSpace(1);
//     space->cell()->AddEntityEntry(id, entry);
//     entry->set_position(Vector3(
//         space_width_ / 2 + CELL_CFG.migrate_dist() + 10, 0, space_height_ / 4));
//   }
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   int total = 0;
//   for (auto& it : entries) {
//     total += it->migrate_count() > 0 ? 1 : 0;
//   }
//   EXPECT_EQ(total, CELL_CFG.max_migrate_num());

//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   total = 0;
//   for (auto& it : entries) {
//     total += it->migrate_count() > 0 ? 1 : 0;
//   }
//   EXPECT_EQ(total, CELL_CFG.max_migrate_num() + 1);
// }

// TEST_F(TestCellMigration, MaxCreateGhostNum) {
//   std::vector<MockEntityEntry*> entries;
//   for (uint64_t id = 1; id <= CELL_CFG.max_create_ghost_num() + 1; id++) {
//     MockEntityEntry* entry = MockEntityEntry::Create(id);
//     entries.push_back(entry);
//     auto space = gw::game::SpaceManager::GetInstance().GetSpace(1);
//     space->cell()->AddEntityEntry(id, entry);
//     entry->set_position(Vector3(
//         space_width_ / 2 - CELL_CFG.ghost_dist() + 10, 0, space_height_ / 4));
//   }
//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   int total = 0;
//   for (auto& it : entries) {
//     total += it->create_count() > 0 ? 1 : 0;
//   }
//   EXPECT_EQ(total, CELL_CFG.max_create_ghost_num());

//   gw::game::SpaceManager::GetInstance().CheckSpacesMigration();
//   total = 0;
//   for (auto& it : entries) {
//     total += it->create_count() > 0 ? 1 : 0;
//   }
//   EXPECT_EQ(total, CELL_CFG.max_create_ghost_num() + 1);
// }

int main(int argc, char* argv[]) {
  SHM_MGR.set_malloc_func(std::malloc);
  SHM_MGR.set_free_func(std::free);

  ::testing::InitGoogleTest(&argc, argv);
  auto mock_log = new gw::MockLogger();
  mock_log->set_priority(gw::LOG_PRIORITY_TRACE);
  // gw::Logger::InitLogger(mock_log);
  // gw::ShmMgr shm_mgr;
  // shm_mgr.SetEnable(false);
  // shm_mgr.Init(false, 1);
  return RUN_ALL_TESTS();
}