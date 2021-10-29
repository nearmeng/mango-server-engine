//
// Created by bondshi on 2020/5/22.
//

/*
 基于多层时间轮算法的定时器实现，算法介绍参见：https://www.ibm.com/developerworks/cn/linux/l-cn-timers/
 实现规格与Linux 低精度定时器相同：

 1. 总共分为5层，从下到上，每层单元数分别为：256，64，64，64, 64. 每一格容纳一个时间间隔，总共支持 0xFFFFFFFF 种时间间隔。
 2. 最小定时精度为 10ms，执行间隔向下按精度倍数取整，即如果设置间隔为 10~19ms, 均为每10ms 执行一次，20~29ms，均为每20ms执行一次。
 3. 不允许创建间隔10ms 以下的定时器。
 4. 支持一次性与可重复两类定时器。
 5. 支持设置每次调度(TimerManager::Update(max_call_num)) 最大触发时钟数，以便平滑时钟时间的回调。
 */

#pragma once

#include <assert.h>

#include <functional>

namespace gw {
namespace timer {

#if defined(_WIN32) || defined(_WIN64)
typedef unsigned long long timestamp_t;
#else
typedef unsigned long timestamp_t;
#endif

#if defined(_WIN32) || defined(_WIN64)
typedef unsigned long long timerid_t;
#else
typedef unsigned long timerid_t;
#endif

static const timerid_t kInvalidTimerId = static_cast<timerid_t>(-1);

using TimerCb = std::function<void(timerid_t timer_id)>;

// 每个时钟Tick 周期是 10ms
static const int kTickUnit = 10;
// 最大允许100W 个Timer并存
static const int kMaxTimerNum = 1000000;

// 获取进程启动依赖的 Tick 计数，以毫秒为单位
timestamp_t GetProcTickMs();

// Error Codes
enum {
  ERR_TIMER_FAIL = -1,
  ERR_TIMER_ARG = -2,
  ERR_TIMER_OVER_LIMIT = -3,
  ERR_TIMER_SYSTEM = -4,
  ERR_DUP_TIMER = -5,  // timer id 重复
  ERR_TIMER_OK = 0
};

struct TimerInfo {
  timerid_t timer_id;
  bool repeatable;
  unsigned int interval;
  unsigned int call_num;
  timestamp_t expire_time;
};

// 时钟调度接口
class ATimerManager {
 protected:
  // should use Destroy(scheduler)
  virtual ~ATimerManager() = default;

 public:
  // 系统默认调度器
  static ATimerManager *Default();
  // 创建独立的实例
  static ATimerManager *Create();
  // 删除实例
  static void Destroy(ATimerManager *timer_manager);

  // 初始化操作，系统启动时显示调用
  virtual int Init() = 0;
  virtual void Cleanup() = 0;

  // 更新操作，在该接口中触发时钟事件回调
  // \max_call_num  一次Update()中最大执行的时钟回调次数，如果存在大量相同间隔的时钟,
  //                可以分批多次触发，以平滑负载。如果为 -1，则执行所有到期时钟回调。
  // \ return 触发的时钟回调次数
  virtual int Update(int max_call_num = -1) = 0;

  // \timer_id 时钟ID, 不能重复
  // \interval_ms 触发间隔，毫秒单位，实际执行间隔会以 kTickUnit(10ms) 为单位向下取整，
  //              即间隔为 1-0~19ms，都是每10ms执行一次
  // \cb callback function
  // \repeatable 是否重复执行
  // \return 0 为成功，其它为错误码
  virtual int CreateTimer(
      timerid_t timer_id,
      const TimerCb &cb,
      unsigned int interval_ms,
      bool repeatable) = 0;

  // 在回调中删除对应 timer 是安全的
  virtual void DeleteTimer(timerid_t timer_id) = 0;

  // 获取当前时钟总数
  virtual unsigned int GetTimerNum() const = 0;

  // 获取 Timer 信息
  // \timer_id target timer id
  // \info 保存时钟状态信息，如果为 NULL，则返回返回共享空间指针(static, thread unsafe)
  virtual TimerInfo *GetTimerInfo(timerid_t timer_id,
                                  TimerInfo *info) const = 0;

  virtual void SpeedUp(timestamp_t t) = 0;
};

// 对Timer 的包装类，提供操作便利（简化调用、自动删除）
class Timer {
 protected:
  ATimerManager *tm_ = NULL;
  timerid_t id_ = kInvalidTimerId;
  unsigned int interval_ = 0;
  bool repeatable_ = false;
  bool started_ = false;
  TimerCb cb_;

 public:
  explicit Timer(timerid_t id, ATimerManager *tm = NULL) {
    if (tm == NULL) {
      tm = ATimerManager::Default();
    }

    assert(tm != NULL);
    id_ = id;
    tm_ = tm;
  }

  Timer(timerid_t id, const TimerCb &cb, unsigned int interval_ms,
        bool repeatable,
        bool auto_start = false,
        ATimerManager *tm = NULL) {
    if (tm == NULL) {
      tm = ATimerManager::Default();
    }

    assert(tm != NULL);
    id_ = id;
    cb_ = cb;
    tm_ = tm;
    interval_ = interval_ms;
    repeatable_ = repeatable;
    if (auto_start) {
      Start();
    }
  }

  Timer(Timer &&other) {
    id_ = other.id_;
    tm_ = other.tm_;
    interval_ = other.interval_;
    cb_ = other.cb_;
    repeatable_ = other.repeatable_;
    started_ = other.started_;

    other.id_ = kInvalidTimerId;
    other.started_ = false;
  }

  virtual ~Timer() {
    Stop();
  }

  // \return  0 is success, otherwise failed
  int Start() {
    if (is_valid())
      return 0;

    assert(tm_ != NULL);
    int ret = tm_->CreateTimer(id_, cb_, interval_, repeatable_);
    if (ret == 0) {
      started_ = true;
    }

    return ret;
  }

  void Stop() {
    if (started_) {
      tm_->DeleteTimer(id_);
      started_ = false;
    }
  }

  timerid_t id() const {
    return id_;
  }

  bool is_valid() const {
    if (!started_) return false;
    if (tm_->GetTimerInfo(id_, NULL) == NULL) {
      return false;
    }

    return true;
  }

  unsigned int interval() const {
    return interval_;
  }

  bool repeatable() const {
    return repeatable_;
  }

  ATimerManager *timer_manager() const {
    return tm_;
  }

  TimerInfo *GetInfo(TimerInfo *info) const {
    if (id_ == kInvalidTimerId) return NULL;
    assert(tm_ != NULL);
    return tm_->GetTimerInfo(id_, info);
  }

  bool Reset(const TimerCb &cb, unsigned int interval_ms, bool repeatable) {
    if (is_valid()) {
      return false;
    }

    cb_ = cb;
    interval_ = interval_ms;
    repeatable_ = repeatable;
    return true;
  }
};

}  // namespace timer
}  // namespace gw
