#pragma once

#include "shm_dynamic_arena.h"
template <typename T>
class ShmSharedPtr {
 public:
  using element_type = T;

 private:
  template <typename T2>
  friend class ShmSharedPtr;

  ShmSharedPtr(std::size_t *in_use_count_ptr_, std::size_t in_pool_idx_, int in_offset_idx_, T *in_ptr_)
      : use_count_ptr_(in_use_count_ptr_),
        pool_idx_(in_pool_idx_),
        offset_idx_(in_offset_idx_),
        ptr_(in_ptr_) {}

 public:
  ShmSharedPtr() {
    use_count_ptr_ = nullptr;
    pool_idx_ = 0;
    offset_idx_ = -1;
    ptr_ = nullptr;
  }

  template <typename T2>
  ShmSharedPtr(const ShmSharedPtr<T2> &other)
      : use_count_ptr_(other.use_count_ptr_),
        pool_idx_(other.pool_idx_),
        offset_idx_(other.offset_idx_),
        ptr_(static_cast<T *>(other.ptr_)) {
    if (use_count_ptr_) {
      (*use_count_ptr_)++;
    }
  }

  void reset() {
    if (use_count_ptr_) {
      if (*use_count_ptr_ == 0) {
        assert(false);
      }
      *use_count_ptr_ -= 1;
      if (*use_count_ptr_ == 0) {
        if (!ShmDynamicArena::DestroyDynPtr(pool_idx_, offset_idx_,
                                            reinterpret_cast<void *>(ptr_))) {
          assert(false);
        }
      }
    }
    use_count_ptr_ = nullptr;
    offset_idx_ = -1;
    ptr_ = nullptr;
  }

  // template <class T2, typename std::enable_if<std::is_convertible<T2, T>,
  // void>::type  = 0>
  template <typename T2>
  ShmSharedPtr(ShmSharedPtr<T2> &&other)
      : use_count_ptr_(other.use_count_ptr_),
        pool_idx_(std::move(other.pool_idx_)),
        offset_idx_(std::move(other.offset_idx_)),
        ptr_(static_cast<T *>(other.ptr_)) {
    other.use_count_ptr_ = nullptr;
    other.reset();
  }

  // template <class T2, typename std::enable_if<std::is_convertible<T2, T>,
  // int>::type = 0>
  template <typename T2>
  ShmSharedPtr &operator=(ShmSharedPtr<T2> &&other) {
    reset();
    use_count_ptr_ = other.use_count_ptr_;
    pool_idx_ = other.pool_idx_;
    offset_idx_ = other.offset_idx_;
    ptr_ = static_cast<T *>(other.ptr_);

    other.use_count_ptr_ = nullptr;
    other.reset();
    return *this;
  }
  ShmSharedPtr &operator=(std::nullptr_t other) {
    reset();
    return *this;
  }
  ~ShmSharedPtr() { reset(); }
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

 private:
  std::size_t *use_count_ptr_;
  std::size_t pool_idx_;
  int offset_idx_;
  T *ptr_;

  template <typename U, typename... Args>
  friend ShmSharedPtr<U> ShmMakeShared(Args &&...args);
  template <typename T1, typename T2>
  friend ShmSharedPtr<T1> ShmDynamicPointerCast(const ShmSharedPtr<T2> &orig);
};

template <typename T, typename... Args>
ShmSharedPtr<T> ShmMakeShared(Args &&...args) {
  ShmDynBasePtr<T> temp_result = ShmDynamicArena::Create<T>(std::forward<Args>(args)...);
  ShmSharedPtr<T> result;
  auto use_count_ptr_ =
      ShmDynamicArena::GetPtrRefCounterPtr<T>(temp_result.pool_idx, temp_result.offset_idx);
  if (use_count_ptr_) {
    (*use_count_ptr_)++;
  }
  return ShmSharedPtr<T>(use_count_ptr_, temp_result.pool_idx, temp_result.offset_idx,
                         temp_result.Get());
}
template <typename T1, typename T2>
ShmSharedPtr<T1> ShmDynamicPointerCast(const ShmSharedPtr<T2> &orig) {
  auto cast_ptr = dynamic_cast<T1*>(orig.get());
  if (!cast_ptr) {
    return ShmSharedPtr<T1>();
  } else {
    (*orig.use_count_ptr_)++;
    return ShmSharedPtr<T1>(orig.use_count_ptr_, orig.pool_idx_, orig.offset_idx_, cast_ptr);
  }
}