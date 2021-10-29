#pragma once
#include <stdarg.h>
#include <stdio.h>

#include "great_world_base/common/common.h"
#include "great_world_base/common/log.h"

namespace gw {
static const char *g_priority_str[] = {"TRACE", "DEBUG", "INFO", "ERROR", "FATAL"};
static constexpr uint32_t max_logger_length_ = 10000;

class MockLogger : public ILogger {
 public:
  void Write(LOG_PRIORITY pri, const char *file, uint32_t line, const char *func, const char *fmt, ...) {
    if (pri < priority_) {
      return;
    }
    int pre_len = 0;
    pre_len = snprintf(logger_buffer_, max_logger_length_, "[%s][%d][(%s:%d)(%s)][%s] ",
                       "",
                       0,
                       file,
                       line,
                       func,
                       g_priority_str[pri]);
    if (pre_len < 0) {
      pre_len = 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(logger_buffer_ + pre_len, max_logger_length_ - pre_len, fmt, ap);
    va_end(ap);
    if (len < 0) {
      len = 0;
    }
    fprintf(stdout, "%s\n", logger_buffer_);
  }
  void set_priority(LOG_PRIORITY priority) { priority_ = priority; }

 private:
  char logger_buffer_[max_logger_length_];
  LOG_PRIORITY priority_ = LOG_PRIORITY::LOG_PRIORITY_DEBUG;
};
}  // namespace gw

#define GW_LOG_ERROR(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_INFO(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_DEBUG(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_TRACE(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);