#pragma once
#include <memory>
#include <new>

#include "great_world_base/common/common.h"
#include "great_world_base/common/ilog.h"

namespace gw {

typedef void* (*ShmMallocFunc)(std::size_t);
typedef void (*ShmFreeFunc)(void*);

// gw使用的内存分配销毁函数
extern void* ShmMalloc(std::size_t size);
extern void ShmFree(void* p);

// 给容器用的 allocator
template <typename T>
struct ShmAllocator {
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;

  template <class T1>
  struct rebind {
    typedef ShmAllocator<T1> other;
  };

  ShmAllocator() {}
  ShmAllocator(const ShmAllocator&) {}
  template <class T1>
  ShmAllocator(const ShmAllocator<T1>&) {}
  ~ShmAllocator() {}

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }

  size_type max_size() const { return size_t(-1) / sizeof(T); }

  void construct(pointer p, const T& val) { ::new (p) T(val); }
  void construct(pointer p) { ::new (p) T(); }
  template <class U, class... Args>
  void construct(U* p, Args&&... args) {
    ::new (p) U(std::forward<Args>(args)...);
  }
  void destroy(pointer p) { p->~T(); }

  // There's no state, so these allocators are always equal
  bool operator==(const ShmAllocator&) const { return true; }
  bool operator!=(const ShmAllocator&) const { return false; }

  pointer allocate(size_type n, const void* = nullptr) {
    pointer p = reinterpret_cast<pointer>(ShmMalloc(n * sizeof(T)));
    return p;
  }

  void deallocate(pointer p, size_type = 0) { ShmFree(p); }
};

}