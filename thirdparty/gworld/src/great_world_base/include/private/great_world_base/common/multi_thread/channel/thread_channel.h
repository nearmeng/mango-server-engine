
#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>

#include "thread_cmd.h"

SMT_NS_BEGIN

class IMainThreadHandle {
 public:
  virtual void start() = 0;
  virtual void stop() = 0;

  virtual bool push_to_child(ChildThreadCmd* cmd) = 0;
  virtual MainThreadCmd* pop_cmd_from_child() = 0;
};

class IChildThreadHandle {
 public:
  virtual bool push_to_main(MainThreadCmd* cmd) = 0;
  virtual ChildThreadCmd* pop_cmd_from_main() = 0;
};

class ThreadChannel : public IMainThreadHandle, public IChildThreadHandle {
 public:
  virtual ~ThreadChannel() {}

  void notify() { cv_.notify_one(); }

  std::thread::id thread_id() const { return thread_->get_id(); }
  
  void start() override {
    thread_ = std::make_shared<std::thread>([this]() -> void { run(); });
  }

  void stop() override {
    push_to_child(new SafeChildThreadCmd<>(ThreadChannel::EndThreadCmd));
    notify();
    if (thread_) {
      thread_->join();
      thread_ = nullptr;
    }
  }

 protected:  
  std::shared_ptr<std::thread> thread_;

 private:
  bool end_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;

  static void EndThreadCmd(IChildThreadHandle* thread) {
    auto real = dynamic_cast<ThreadChannel*>(thread);
    real->do_end();
  }
  void do_end() { end_ = true; }
  
  void execute() {
    ChildThreadCmd* ret = pop_cmd_from_main();
    while (ret) {
      ret->run(this);
      delete ret;
      ret = pop_cmd_from_main();
    }
  }

  virtual void run() {
    std::unique_lock<std::mutex> lock(mutex_);
    std::chrono::duration<uint32_t, std::milli> wait_time(100);
    while (!end_) {
      cv_.wait_for(lock, wait_time);
      execute();
    }
  }
};

SMT_NS_END
