#pragma once
#include "thread_common.h"

SMT_NS_BEGIN

template <class TYPE>
class ThreadSingleton {
 public:
  static TYPE& GetInstance(void) {
    // 这样写主要是为了以后 shm resume
    thread_local TYPE* instance_ = nullptr;
    if(!instance_) {
      instance_ = new TYPE();
    }
    return *instance_;
  }

 protected:
  ThreadSingleton() {}
  ~ThreadSingleton() {}

 private:
  ThreadSingleton(const ThreadSingleton& src){};
  ThreadSingleton& operator=(const ThreadSingleton& src){};
};

SMT_NS_END