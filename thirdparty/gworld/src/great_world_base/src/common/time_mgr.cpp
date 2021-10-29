#include "great_world_base/common/timer_mgr_private.h"

namespace gw {

std::function<ITimeMgr*(void)> TimerManager::creator_;
std::function<void(ITimeMgr*)> TimerManager::destructor_;

TimerManager::~TimerManager() {
  if (destructor_ && real_mgr_) {
    destructor_(real_mgr_);
  }
}

TimerManager* TimerManager::Get() {
  if (m_instance == NULL) {
    m_instance = new TimerManager();
  }
  return m_instance;
}

void TimerManager::Init(std::function<ITimeMgr*(void)> creator, std::function<void(ITimeMgr*)> destructor) {
  if (destructor_ && real_mgr_) {
    destructor_(real_mgr_);
  }
  creator_ = creator;
  destructor_ = destructor;
  real_mgr_ = creator_();
}

ITimeMgr* TimerManager::real_mgr_;
TimerManager* TimerManager::m_instance;
const uint64_t TimerManager::kInvalidTimerId;

}  // namespace gw