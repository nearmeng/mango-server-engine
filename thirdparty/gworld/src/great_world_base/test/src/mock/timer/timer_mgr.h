#pragma once

#include <list>
#include <unordered_map>

#include "timer.h"
#include "great_world_base/common/timer_mgr.h"

namespace gw {
namespace timer {

typedef std::unordered_map<timerid_t, Timer*> TimerMap;

/*
 TimerManager提供基于时间轮算法的定时器的简单包装，详细实现请参考src/common/time_util/timer_i.h(.cc)
 1. 通过CreateTimer()创建Timer，返回的g6::util::Timer可用于Timer的信息查询和管理。
 2. 删除Timer请通过DeleteTimer()接口进行，TimerManager不会主动对Timer进行删除。
 3. Timer的使用请参考src/common/time_util/timer.h中类Timer对应的接口。
 4. 可以通过修改 kMaxCallbackTriggerPerTick 设置每个tick触发的Callback调用数量。
 */

class TimerManager : public ITimeMgr {
 public:
  static TimerManager& Get();

  //添加一个timer, 如果auto_start设置为false, timer需要后续调用Start()启动，启动后id()才会有具体值，之前为零。
  uint64_t CreateTimer(
    const std::function<void(timerid_t timer_id)>& cb, 
    unsigned int interval_ms, 
    bool repeatable) override;
  int DeleteTimer(uint64_t id) override;
  
  int Update();

  void SpeedUp(timestamp_t t) { tm_->SpeedUp(t); }

  unsigned long GetWalltimeMs();

 private:
  TimerManager();
  timerid_t GetNextTimerID();

 private:
  ATimerManager* tm_ = nullptr;
  timerid_t timerid_next_ = 0;
  TimerMap timers_;
};

}  // namespace timer
}  // namespace gw