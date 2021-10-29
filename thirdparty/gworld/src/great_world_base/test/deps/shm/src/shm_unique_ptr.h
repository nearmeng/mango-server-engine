#pragma once
#include <memory>
#include "shm_dynamic_arena.h"
template <class T>
class ShmUniquePtr {
 public:
  using element_type = T;

 private:
  std::size_t pool_idx_;
  int offset_idx_;
  T *ptr_;
  friend class ShmDynamicArena;
  template <typename T2>
  friend class ShmUniquePtr;

  ShmUniquePtr(std::size_t in_pool_idx_, int in_offset_idx_, T *in_ptr)
      : pool_idx_(in_pool_idx_), offset_idx_(in_offset_idx_), ptr_(in_ptr) {}

 public:
  ShmUniquePtr() {
    pool_idx_ = 0;
    offset_idx_ = -1;
    ptr_ = nullptr;
  }
  ShmUniquePtr(const ShmUniquePtr &other) = delete;

  // template <class T2, typename std::enable_if<std::is_convertible<T2, T>,
  // int>::type = 0>
  template <typename T2>
  ShmUniquePtr(ShmUniquePtr<T2> &&other)
      : pool_idx_(std::move(other.pool_idx_)),
        offset_idx_(std::move(other.offset_idx_)),
        ptr_(static_cast<T *>(other.ptr_)) {
    other.offset_idx_ = -1;
    other.ptr_ = nullptr;
  }

  // template <class T2, typename std::enable_if<std::is_convertible<T2, T>,
  // int>::type = 0>
  template <typename T2>
  ShmUniquePtr &operator=(ShmUniquePtr<T2> &&other) {
    reset();
    pool_idx_ = other.pool_idx_;
    offset_idx_ = other.offset_idx_;
    ptr_ = static_cast<T *>(other.ptr_);
    other.ptr = nullptr;
    other.offset_idx_ = -1;
    return *this;
  }
  ShmUniquePtr &operator=(std::nullptr_t &&other) {
    reset();
    return *this;
  }
  void reset() {
    if (offset_idx_ != -1) {
      if (!ShmDynamicArena::DestroyDynPtr(pool_idx_, offset_idx_, reinterpret_cast<void *>(ptr_))) {
        assert(false);
      };
      offset_idx_ = -1;
    }
    ptr_ = nullptr;
  }
  ~ShmUniquePtr() { reset(); }
  T *get() const { return ptr_; }
  typename std::add_lvalue_reference<T>::type operator*() const { return *get(); }
  T *operator->() const noexcept { return get(); }
  operator bool() const {
    if (offset_idx_ != -1) {
      return true;
    } else {
      return false;
    }
  }
  template <typename U, typename... Args>
  friend ShmUniquePtr<U> ShmMakeUnique(Args &&...args);
};
template <typename T, typename... Args>
ShmUniquePtr<T> ShmMakeUnique(Args &&...args) {
  ShmDynBasePtr<T> temp_result = ShmDynamicArena::Create<T>(std::forward<Args>(args)...);
  return ShmUniquePtr<T>(temp_result.pool_idx, temp_result.offset_idx, temp_result.Get());
}