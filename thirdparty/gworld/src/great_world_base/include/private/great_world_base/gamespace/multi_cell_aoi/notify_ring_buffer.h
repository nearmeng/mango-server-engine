#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_common.h"

GW_GAME_BEGIN

struct NotifyRingBuffer {
  static const AoINotifyId invalid_order_id = 0;

  GWVector<AoIOrderlyNotifyProp*> buffer_;
  uint32_t head_ = 0; // 占位点
  uint32_t tail_ = 0;

  ~NotifyRingBuffer() {
    if (empty()) {
      return;
    }
    for (uint32_t index = tail_; index != head_; index = (buffer_.size() + index - 1) % buffer_.size()) {
      buffer_[index]->Dispose();
    }
  }

  inline uint32_t size() { return tail_ >= head_ ? tail_ - head_ : tail_ + buffer_.size() - head_; }
  inline AoINotifyId next_id() { return empty() ? 1 : buffer_[tail_]->id_ + 1; }
  inline AoINotifyId max_id() { return empty() ? invalid_order_id : buffer_[tail_]->id_; }
  inline bool empty() { return tail_ == head_; }

  inline AoIOrderlyNotifyProp* first() {
    if (empty())
      return nullptr;
    return buffer_[(head_ + 1) % buffer_.size()];
  }

  inline AoIOrderlyNotifyProp* last() {
    if (empty())
      return nullptr;
    return buffer_[tail_];
  }

  inline AoIOrderlyNotifyProp* GetByIndex(uint32_t index) {
    if (index >= buffer_.size() || empty() ||
        (tail_ > head_ && (index > tail_ || index <= head_)) ||
        (index > tail_ && index <= head_)) {
      return nullptr;
    }
    return buffer_[index];
  }

  inline AoIOrderlyNotifyProp* GetById(AoINotifyId id) {
    if (empty() || id > last()->id_ || id < first()->id_)
      return nullptr;
    uint32_t target_index = (id - first()->id_ + head_ + 1) % buffer_.size();
    return buffer_[target_index];
  }

  bool Push(AoIOrderlyNotifyProp* data) {
    if (!empty() && data->id_ != last()->id_ + 1) {
      gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, "AoIOrderlyNotifyProp newid error:%d,%d", data->id_, buffer_[tail_]->id_);
      return false;
    }
    if ((tail_ + 1) % buffer_.size() == head_) {
      head_ = (head_ + 1) % buffer_.size();
      if (buffer_[head_])
        buffer_[head_]->Dispose();
    }
    tail_ = (tail_ + 1) % buffer_.size();
    buffer_[tail_] = data;
    return true;
  }

  void Resize(uint32_t new_size) {
    if (new_size++ <= buffer_.size()) {
      return;
    }
    GWVector<AoIOrderlyNotifyProp*> new_buffer(new_size, nullptr);
    for (uint32_t offset = 1; offset <= size(); offset++) {
      uint32_t index = (head_ + offset) % buffer_.size();
      new_buffer[offset] = buffer_[index];
    }
    tail_ = size();
    head_ = 0;
    buffer_ = new_buffer;
  }

  bool WriteToMemoryStreamReal(IMemoryOStream& os) {
    os << size();
    if (!empty()) {
      auto notify_id = first()->id_;
      while (notify_id <= last()->id_) {
        auto target = GetById(notify_id);
        if (!target)
          return false;
        if (!target->WriteToMemoryStreamReal(os))
          return false;
        notify_id++;
      }
    }
    return true;
  }

  bool ReadFromMemoryStreamReal(IMemoryIStream& is) {
    std::size_t size;
    is >> size;
    for (std::size_t i = 0; i != size; i++) {
      auto data = ShmNew<AoIOrderlyNotifyProp>();
      if (!data->ReadFromMemoryStreamReal(is)) {
        data->Dispose();
        return false;
      }
      if (!Push(data)) {
        data->Dispose();
        return false;
      }
    }
    return true;
  }
};

GW_GAME_END