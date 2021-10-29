#pragma once
#include "thread_common.h"
#include "thread_channel.h"
#include <vector>
#include <deque>
#include <mutex>

SMT_NS_BEGIN

// 生产者只负责push
// 消费者发现自己的buffer没了之后主动进行一次指针转换

class LockBuffer {
 public:
  void push(void* data) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(data);
  }

  std::vector<void*> pop() {
    std::vector<void*> ret;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      ret = std::move(buffer_);
    }
    return ret;
  }

 private:
  std::vector<void*> buffer_;
  std::mutex mutex_;
};

class LockChannel : public ThreadChannel {
 private:
  static void* pop(LockBuffer& buffer, std::vector<void*>& tmp) {
    if(tmp.empty()) {
      tmp = std::move(buffer.pop());
    }
    if(!tmp.empty()) {
      auto ret = tmp.back();
      tmp.pop_back();
      return ret;
    }
    else {
      return nullptr;
    }
  }
 public:
  bool push_to_child(ChildThreadCmd* cmd) override {
    from_main_.push(cmd);
    notify();
    return true;
  }
  
  MainThreadCmd* pop_cmd_from_child() override {
    return reinterpret_cast<MainThreadCmd*>(pop(to_main_, tmp_to_main_));
  }
  
  bool push_to_main(MainThreadCmd* cmd) override {
    to_main_.push(cmd);
    return true;
  }
  
  ChildThreadCmd* pop_cmd_from_main() override {
    return reinterpret_cast<ChildThreadCmd*>(pop(from_main_, tmp_from_main_));
  }

 private:
  LockBuffer from_main_;
  std::vector<void*> tmp_from_main_;

  LockBuffer to_main_;
  std::vector<void*> tmp_to_main_;
};

SMT_NS_END