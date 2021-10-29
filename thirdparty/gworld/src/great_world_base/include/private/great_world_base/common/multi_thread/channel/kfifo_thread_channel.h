#pragma once

#include "thread_channel.h"
#include "kfifo/kfifo.h"

SMT_NS_BEGIN

class KfifoChannel : public ThreadChannel {
 public:
  virtual ~KfifoChannel() {
    KFIFO::kfifo_free(&to_main_kfifo_);
    KFIFO::kfifo_free(&from_main_kfifo_);
  }

  KfifoChannel() {
    KFIFO::kfifo_alloc(&to_main_kfifo_, kfifo_size_);
    KFIFO::kfifo_alloc(&from_main_kfifo_, kfifo_size_);
  }
  bool push_to_child(ChildThreadCmd* cmd) override {
    KFIFO::kfifo_in(&from_main_kfifo_, &cmd, sizeof(cmd));
    notify();
    return true;
  }

  bool push_to_main(MainThreadCmd* cmd) override {
    KFIFO::kfifo_in(&to_main_kfifo_, &cmd, sizeof(cmd));
    return true;
  }
  MainThreadCmd* pop_cmd_from_child() override {
    MainThreadCmd* ret = nullptr;
    KFIFO::kfifo_out(&to_main_kfifo_, &ret, sizeof(ret));
    return ret;
  }

  ChildThreadCmd* pop_cmd_from_main() {
    ChildThreadCmd* ret = nullptr;
    KFIFO::kfifo_out(&from_main_kfifo_, &ret, sizeof(ret));
    return ret;
  }

 private:
  kfifo_t to_main_kfifo_; // 给主线程读写的
  kfifo_t from_main_kfifo_; // 给子线程读写的
  const uint32_t kfifo_size_ = static_cast<uint32_t>(sizeof(MainThreadCmd*) * 1024 * 1024 * 10);
};

SMT_NS_END