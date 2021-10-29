#pragma once

// #include "world/common/world_common.h"
// #include "great_world_base/common/timer.h"
#include <functional>
#include <deque>

#include "great_world_base/common/memory_stream.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/common/common.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

typedef uint32_t AoINotifyId;
typedef uint32_t PropLabel;

// 带序号的同步属性
struct AoIOrderlyNotifyProp {
  GWCLASS_DECL(AoIOrderlyNotifyProp)
 public:
  AoINotifyId id_;
  uint32_t type_;
  GWString data_;
  PropLabel labels_;
  uint64_t timestamp_;

  virtual ~AoIOrderlyNotifyProp() {}
  AoIOrderlyNotifyProp() {}

  AoIOrderlyNotifyProp(IMemoryIStream& is) {
    ReadFromMemoryStreamReal(is);
  }

  AoIOrderlyNotifyProp(AoINotifyId id, uint32_t type, const char* data, std::size_t data_size, PropLabel labels,
                       uint64_t timestamp)
      : id_(id), type_(type), data_(data, data_size), labels_(labels), timestamp_(timestamp) {
  }

  bool WriteToMemoryStreamReal(IMemoryOStream& os) {
    os << id_ << timestamp_ << StringSlice(data_.c_str(), data_.size()) << type_ << labels_;
    return true;
  }

  bool ReadFromMemoryStreamReal(IMemoryIStream& is) {
    is >> id_ >> timestamp_ >> data_ >> type_ >> labels_;
    return true;
  }
};

template <class T>
class SimpleQueue {
 public:
  SimpleQueue(std::size_t max_size) { data_.resize(max_size + 1); }
  inline std::size_t size() {
    return data_.size();
  }
  inline void push(T t) {
    data_.push_back(t);
  }

  inline T& top() {
    return data_.front();
  }

  inline void pop() {
    data_.pop_front();
  }

  inline bool empty() {
    return data_.empty();
  }

 private:
  std::deque<T> data_;
};

GW_GAME_END