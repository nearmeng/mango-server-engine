//
// Created by bondshi on 2020/5/22.
//

#include "timer_i.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define OS_WINDOWS
#else
#include <time.h>
#include <unistd.h>
#endif

namespace gw {
namespace timer {

static const auto kInvalidTickMs = static_cast<unsigned long>(-1);
static timestamp_t g_bootTime = 0;

#ifndef OS_WINDOWS
static inline timestamp_t GetClockTime(clockid_t clock) {
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

class TimeBootstrap {
 public:
  TimeBootstrap() {
    if (g_bootTime == 0) {
      g_bootTime = GetSysTickMs();
    }
  }
};

// Run before main
static TimeBootstrap g_timeBootstrap;

static void DestroyWheel(MultiLayerWheel *w) {
  assert(w != NULL);
  if (w->layer == 0) {
    for (int i = 0; i < SLOT_ENTRY_NUM; ++i) {
      TimerList *timer_list = w->slots[i];
      w->slots[i] = NULL;
      if (timer_list != NULL) {
        delete timer_list;
      }
    }
  } else {
    for (int i = 0; i < LAYER_ENTRY_NUM; ++i) {
      MultiLayerWheel *child = w->layers[i];
      w->layers[i] = NULL;
      if (child != NULL) {
        DestroyWheel(child);
      }
    }
  }

  w->child_num = 0;
  if (w->parent != NULL) {
    delete w;
  }
}

TimerManagerImpl::TimerManagerImpl() {
  memset(cursor_, 0, sizeof(cursor_));
  memset(&wheel_, 0, sizeof(wheel_));
}

TimerManagerImpl::~TimerManagerImpl() {
  Cleanup();
}

int TimerManagerImpl::Init() {
  if (wheel_.layer > 0) {
    // already inited
    return 0;
  }

  if (g_bootTime == kInvalidTickMs) {
    return ERR_TIMER_SYSTEM;
  }

  last_time_ = GetTimeStamp();
  wheel_.layer = MAX_LAYER_NUM - 1;
  return 0;
}

void TimerManagerImpl::Cleanup() {
  DestroyWheel(&wheel_);
}

int TimerManagerImpl::Update(int max_call_num) {
  unsigned long now = GetTimeStamp();
  int ticks = (now - last_time_) / kTickUnit;
  int call_num = 0;

  for (int i = 0; i < ticks; ++i) {
    MultiLayerWheel *w = GetLowestWheel(cursor_, false);
    TimerList *targets = GetTimerList(w, cursor_[0], false);
    if (targets == NULL) {
      IncCursor();
      continue;
    }

    while (!targets->empty() &&
           (max_call_num < 0 || call_num < max_call_num)) {
      TimerNode *node = *targets->begin();
      targets->pop_front();
      assert(node != NULL);

      if (node->removed) {
        DeleteNode(node);
        continue;
      }

      node->cb(node->id);
      node->call_num++;
      call_num++;

      if (!node->repeatable) {
        DeleteNode(node);
      } else {
        InsertTimer(node, ticks - i);
      }
    }

    if (!targets->empty())
      break;

    FreeWheelSlot(w, cursor_);
    delete targets;
    IncCursor();
  }

  return call_num;
}

int TimerManagerImpl::CreateTimer(
    timerid_t timer_id,
    const TimerCb &cb,
    unsigned int interval_ms,
    bool repeatable) {
  if (wheel_.layer == 0)
    return ERR_TIMER_FAIL;
  if (interval_ms < kTickUnit || !cb || timer_id == kInvalidTimerId)
    return ERR_TIMER_ARG;
  if (timers_.size() >= kMaxTimerNum)
    return ERR_TIMER_OVER_LIMIT;

  if (timers_.find(timer_id) != timers_.end())
    return ERR_DUP_TIMER;

  unsigned long now = GetTimeStamp();
  int ticks = (now - last_time_) / kTickUnit;

  TimerNode *node = new TimerNode();
  node->id = timer_id;
  node->interval = interval_ms;
  node->call_num = 0;
  node->repeatable = repeatable;
  node->removed = false;
  node->cb = cb;
  timers_[timer_id] = node;
  //InsertTimer(node, 0);
  InsertTimer(node, ticks);

  return 0;
}

void TimerManagerImpl::DeleteTimer(timerid_t timer_id) {
  TimerMap::iterator it = timers_.find(timer_id);
  if (it == timers_.end()) {
    return;
  }

  TimerNode *node = it->second;
  timers_.erase(it);

  assert(!node->removed);
  node->removed = true;
}

TimerInfo *TimerManagerImpl::GetTimerInfo(timerid_t timer_id, TimerInfo *info) const {
  static TimerInfo default_data;
  TimerMap::const_iterator it = timers_.find(timer_id);
  if (it == timers_.end()) return NULL;
  const TimerNode *node = it->second;
  assert(node != NULL);
  if (info == NULL) {
    info = &default_data;
  }

  info->timer_id = timer_id;
  info->call_num = node->call_num;
  info->expire_time = node->expire_time;
  info->interval = node->interval;
  info->repeatable = node->repeatable;
  return info;
}

void TimerManagerImpl::SpeedUp(timestamp_t t) {
  speed_up_ts_ += t;
}

static ATimerManager *g_defaultInstance = NULL;

ATimerManager *ATimerManager::Default() {
  static TimerManagerImpl instance;
  if (g_defaultInstance == NULL) {
    instance.Init();
    g_defaultInstance = &instance;
  }

  return &instance;
}

ATimerManager *ATimerManager::Create() {
  return new TimerManagerImpl();
}

void ATimerManager::Destroy(ATimerManager *timer_manager) {
  if (timer_manager != g_defaultInstance) {
    delete timer_manager;
  }
}

// global functions
timestamp_t GetProcTickMs() {
  auto now = GetSysTickMs();
  if (now == kInvalidTickMs || now < g_bootTime) {
    assert(false);
    return 0;
  }

  return now - g_bootTime;
}

}  // namespace timer
}  // namespace gw
