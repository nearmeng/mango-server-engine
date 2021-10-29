#pragma once
#include "channel/thread_channel.h"

// 多线程调度器:
// 可以预计的需求：指定投递到某个线程/指定投递到某一组线程的其中之一/根据负载确定要投递的线程
// ==>增加一个 ghoup 的概念，group 内部的投递方案自己实现，

SMT_NS_BEGIN

class ThreadGroup {
 public:
  virtual ~ThreadGroup() {}
  ThreadGroup(uint32_t id) : id_(id) {}
  inline uint32_t id() const { return id_; }

  virtual void Start() = 0;
  virtual void Stop() = 0;

  virtual void AddChannel(ThreadChannel* channel) = 0;

  virtual void Push(ChildThreadCmd*) = 0;
  virtual MainThreadCmd* Pop() = 0;

 protected:
  uint32_t id_;
};

class SingleThreadGroup : public ThreadGroup {
 public:
  virtual ~SingleThreadGroup() {
    if (main_handle_) {
      delete main_handle_;
    }
  }
  SingleThreadGroup(uint32_t id) : ThreadGroup(id) {}
  void Start() override {
    main_handle_->start();
  }

  void Stop() override {
    main_handle_->stop();
  }

  void AddChannel(ThreadChannel* channel) override {
    main_handle_ = channel;
  }

  void Push(ChildThreadCmd* cmd) override {
    if(!main_handle_->push_to_child(cmd)) {
      LOG_ERR("push error,%p", cmd);
    }
  }
  MainThreadCmd* Pop() override {
    return main_handle_->pop_cmd_from_child();
  }

 private:
  IMainThreadHandle* main_handle_ = nullptr;
};

SMT_NS_END