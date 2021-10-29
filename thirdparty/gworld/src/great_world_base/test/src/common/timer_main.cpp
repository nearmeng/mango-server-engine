
#include "gtest/gtest.h"
#include "great_world_base/common/ilog.h"
#include "mock/log.h"
#include "mock/timer/timer_mgr.h"
#include "great_world_base/common/timer_mgr_private.h"

class CommonTest : public testing::Test {
 public:
  ~CommonTest() { 
  }

  CommonTest() {
  }

  static void SetUpTestCase() {
    gw::Logger::InitLogger(new gw::MockLogger());
  }

  static void TearDownTestCase() {
    // shm_mgr.UnInit(true);
    // SPACE_MGR.UnInit(false);
  }

  virtual void TearDown() {
  }

 protected:
};

TEST_F(CommonTest, TestTimer) {
  auto timer_id = gw::timer::TimerManager::Get().CreateTimer([](gw::timer::timerid_t timer_id)->void{
    GW_LOG_DEBUG("timer===================");
  }, 1000, true);
  
  ASSERT_NE(timer_id, gw::TimerManager::kInvalidTimerId);
  gw::timer::TimerManager::Get().Update();
  gw::timer::TimerManager::Get().SpeedUp(1000);
  gw::timer::TimerManager::Get().Update();
  gw::timer::TimerManager::Get().SpeedUp(1000);
  gw::timer::TimerManager::Get().Update();
}