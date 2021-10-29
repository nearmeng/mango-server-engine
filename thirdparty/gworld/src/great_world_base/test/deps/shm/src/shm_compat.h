#pragma once
#include "resume_typed_layer.h"
#include "resume_typed_pool.h"
#include "utility_memory_block.h"

template <typename T>
struct ShmTypeWrapper : ResumeTypeWrapper<T> {
  T& operator()() { return this->Ref(); }
  const T& operator()() const { return this->Ref(); }
  T* operator->() { return this->Ptr(); }
};

template <std::size_t TotalSize>
using ShmBlock = MemoryBlock<TotalSize>;