#pragma once
#include <sstream>
#include <thread>

#ifndef SMT_NS_BEGIN
#define SMT_NS_BEGIN namespace smt {
#endif
#ifndef SMT_NS_END
#define SMT_NS_END }
#endif

#define SMT_NS smt

#ifndef LOG
#define LOG(fmt, ...) printf(fmt, __VA_ARGS__);
#endif

#ifndef LOG_ERR
#define LOG_ERR(fmt, ...) printf(fmt, __VA_ARGS__);
#endif

SMT_NS_BEGIN

class Common {
 public:
  static std::string thread_id_str() {
    std::stringstream sin;
    sin << std::this_thread::get_id();
    return sin.str();
  }
};

SMT_NS_END