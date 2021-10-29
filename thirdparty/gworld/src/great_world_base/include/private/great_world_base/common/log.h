#pragma once
#include "great_world_base/common/ilog.h"

namespace gw {

class Logger : public ILogger {
 public:
  void Write(LOG_PRIORITY pri, const char *file, uint32_t line, const char *func, const char *fmt, ...) {

  }
  static inline void InitLogger(ILogger* logger) { logger_ = logger; }
  static ILogger* logger_;
};

}