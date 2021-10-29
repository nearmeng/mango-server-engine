#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <type_traits>

namespace gw {

typedef enum {
  LOG_PRIORITY_TRACE = 0,
  LOG_PRIORITY_DEBUG,
  LOG_PRIORITY_INFO,
  LOG_PRIORITY_WARN,
  LOG_PRIORITY_ERROR,
  LOG_PRIORITY_FATAL,
} LOG_PRIORITY;

class ILogger {
 public:
  virtual void Write(LOG_PRIORITY pri, const char *file, uint32_t line, const char *func, const char *fmt, ...) = 0;
};

}  // namespace gw