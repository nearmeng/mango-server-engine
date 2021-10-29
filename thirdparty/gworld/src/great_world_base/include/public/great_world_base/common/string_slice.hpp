#pragma once
#include <assert.h>
#include <string.h>

#include <string>
#include <vector>

#include "great_world_base/common/shm/shm_stl.h"

namespace gw {

/**
 * 类似C++17 string_view，传内存地址和长度
 */
class StringSlice {
 public:
  StringSlice() : ptr_(""), size_(0) {}
  explicit StringSlice(const char *data) : ptr_(data), size_(strlen(data)) {}
  explicit StringSlice(const char *data, size_t size) : ptr_(data), size_(size) {}
  explicit StringSlice(const void *data, size_t size)
      : ptr_(reinterpret_cast<const char *>(data)), size_(size) {}
  explicit StringSlice(const unsigned char *data, size_t size)
      : ptr_(reinterpret_cast<const char *>(data)), size_(size) {}

  explicit StringSlice(const GWString &str) : StringSlice(str.data(), str.size()) {
  }
  explicit StringSlice(const std::vector<char> &v) : StringSlice(v.data(), v.size()) {}
  explicit StringSlice(const std::vector<unsigned char> &v) : StringSlice(v.data(), v.size()) {}

  const char *data() const { return ptr_; }
  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  char operator[](size_t n) const {
    assert(n < size());
    return ptr_[n];
  }

  void Clear() {
    ptr_ = "";
    size_ = 0;
  }

  void Remove(size_t n) {
    assert(n < size());
    ptr_ += n;
    size_ -= n;
  }

  GWString toString() const { return GWString(ptr_, size_); }

  StringSlice Sub(size_t start) const {
    assert(start < size());
    return StringSlice(ptr_ + start, size_ - start);
  }

  StringSlice Sub(size_t start, size_t end) const {
    assert(start < size());
    assert(end <= size());
    assert(start <= end);
    return StringSlice(ptr_ + start, end - start);
  }

 private:
  const char *ptr_;
  size_t size_ = 0;
};

inline bool operator==(const StringSlice &lhs, const StringSlice &rhs) {
  return (lhs.size() == rhs.size()) && (memcmp(lhs.data(), rhs.data(), lhs.size()) == 0);
}

inline bool operator!=(const StringSlice &lhs, const StringSlice &rhs) {
  return !(lhs == rhs);
}

}  // namespace gw
