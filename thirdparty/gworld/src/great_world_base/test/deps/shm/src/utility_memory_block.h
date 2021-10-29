#pragma once
#include <cstddef>
#include <utility>
#include "resuming_guard.h"

constexpr std::size_t kArraySizeMax = 0x7fffffff;
static_assert(sizeof(std::size_t) >= 8, "sizeof(std::size_t) >= 8");

template <std::size_t TotalSize>
struct MemoryBlockSize {
  constexpr static std::size_t kSize = TotalSize > kArraySizeMax ? kArraySizeMax : TotalSize;
  constexpr static std::size_t kRest = TotalSize > kArraySizeMax ? TotalSize - kArraySizeMax : 0;
};

template <std::size_t TotalSize>
struct MemoryBlock : MemoryBlock<MemoryBlockSize<TotalSize>::kRest> {
  char block[MemoryBlockSize<TotalSize>::kSize];
};
template <>
struct MemoryBlock<0> {};

template <typename T>
struct ResumeTypeWrapper {
  template <typename... Args>
  ResumeTypeWrapper(Args&&... args) {
    if (!ShmIsResuming()) new (&object_) T(std::forward<Args>(args)...);
  }

  ~ResumeTypeWrapper() { object_.~T(); }

  ResumeTypeWrapper(const ResumeTypeWrapper&) = delete;
  ResumeTypeWrapper(ResumeTypeWrapper&&) = delete;
  ResumeTypeWrapper& operator=(const ResumeTypeWrapper&) = delete;
  ResumeTypeWrapper& operator=(ResumeTypeWrapper&&) = delete;

  T* Ptr() { return &object_; }
  T& Ref() { return object_; }

  const T* Ptr() const { return &object_; }
  const T& Ref() const { return object_; }

 private:
  union {
    MemoryBlock<sizeof(T)> _;
    T object_;
  };
};
