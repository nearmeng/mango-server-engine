#pragma once
#include <functional>

namespace gw {

// 有个约定，值 static_cast<timerid_t>(-1) 是个标识，意思是无效 timer_id
class ITimeMgr {
 public:
  virtual ~ITimeMgr() {}
  virtual uint64_t CreateTimer(const std::function<void(uint64_t timer_id)>& cb, unsigned int interval_ms, bool repeatable) = 0;
  virtual int DeleteTimer(uint64_t id) = 0;
};

}  // namespace gw