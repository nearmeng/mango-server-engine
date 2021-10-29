#pragma once
#include <vector>
#include <string>
#include "thread_common.h"
#include "thread_singleton.h"

SMT_NS_BEGIN

typedef enum {
  LOG_PRIORITY_TRACE = 0,
  LOG_PRIORITY_DEBUG,
  LOG_PRIORITY_INFO,
  LOG_PRIORITY_WARN,
  LOG_PRIORITY_ERROR,
  LOG_PRIORITY_FATAL,
} LOG_PRIORITY;

class ThreadLogger : public ThreadSingleton<ThreadLogger> {
 public:
  void set_is_main_thread(bool b) { is_main_thread_ = b; }
  void set_priority(LOG_PRIORITY priority) { priority_ = priority; }
  void Write(LOG_PRIORITY pri, const char *file, uint32_t line, const char *func, const char *fmt, ...) {
    if(pri < priority_) return;
    // 这里把真实时间戳也拼进去，然后 buffer_ 里面做下存包，到一定数量之后再拷贝过去

  }

 private:
  bool is_main_thread_;
  LOG_PRIORITY priority_;
  std::vector<const char*> buffer_;
};

SMT_NS_END