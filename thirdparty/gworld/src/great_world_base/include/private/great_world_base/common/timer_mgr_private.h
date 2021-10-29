#pragma once
#include <functional>
#include "great_world_base/common/timer_mgr.h"

namespace gw {

class TimerManager : public ITimeMgr {
 public:
  virtual ~TimerManager();

  static TimerManager* Get();
  static void Init(std::function<ITimeMgr*(void)> creator, std::function<void(ITimeMgr*)> destructor);

  uint64_t CreateTimer(const std::function<void(uint64_t timer_id)>& cb, unsigned int interval_ms, bool repeatable) override { 
    return real_mgr_->CreateTimer(cb, interval_ms, repeatable); 
  }
  int DeleteTimer(uint64_t id) { 
    return real_mgr_->DeleteTimer(id); 
  }

  static const uint64_t kInvalidTimerId = static_cast<uint64_t>(-1);
 private:
  static std::function<ITimeMgr*(void)> creator_;
  static std::function<void(ITimeMgr*)> destructor_;
  static ITimeMgr* real_mgr_;
  static TimerManager* m_instance;
};

}  // namespace gw