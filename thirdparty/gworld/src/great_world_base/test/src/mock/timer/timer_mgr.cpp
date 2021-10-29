//
// Created by rootlin on 2020/6/1.
//

#include "timer_mgr.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define OS_WINDOWS
#else
#include <time.h>
#include <unistd.h>
#endif
#include <assert.h>

namespace gw {
namespace timer {

static const int kMaxCallbackTriggerPerTick = -1;

TimerManager& TimerManager::Get() {
  static TimerManager mgr;
  return mgr;
}

TimerManager::TimerManager() {
  tm_ = ATimerManager::Default();
}

int TimerManager::Update() {
  return tm_->Update(kMaxCallbackTriggerPerTick);
}

uint64_t TimerManager::CreateTimer(const std::function<void(timerid_t timer_id)>& cb, unsigned int interval_ms, bool repeatable) {
  Timer* timer = new Timer(GetNextTimerID(), cb, interval_ms, repeatable, false, tm_);
  int res = timer->Start();
  if (res < 0) {
    delete timer;
    return kInvalidTimerId;
  }
  timers_.insert(std::pair<int, Timer*>(timer->id(), timer));
  return timer->id();
}

timerid_t TimerManager::GetNextTimerID() {
  return timerid_next_++;
}

int TimerManager::DeleteTimer(timerid_t id) {
  if (id == kInvalidTimerId) {
    return -1;
  }

  auto iter = timers_.find(id);
  if (iter != timers_.end()) {
    delete iter->second;
    timers_.erase(iter);
  }
  return 0;
}

static unsigned long g_bootTime = 0;
static const auto kInvalidTickMs = static_cast<unsigned long>(-1);

#ifndef OS_WINDOWS
static inline unsigned long GetClockTime(clockid_t clock) {
  timespec tp;
  int ret = clock_gettime(clock, &tp);
  if (ret != 0) {
    return kInvalidTickMs;
  }
  return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}
#endif

static inline unsigned long GetSysTickMs() {
#ifdef OS_WINDOWS
  return GetTickCount64();
#else
  return GetClockTime(CLOCK_MONOTONIC);
#endif
}

unsigned long TimerManager::GetWalltimeMs() {
#ifdef OS_WINDOWS
  SYSTEMTIME time;
  GetSystemTime(&time);
  LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds;
  return time_ms;
#else
  return GetClockTime(CLOCK_REALTIME);
#endif
}

}  // namespace timer
}  // namespace gw