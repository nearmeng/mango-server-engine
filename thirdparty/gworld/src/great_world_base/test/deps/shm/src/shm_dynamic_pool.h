#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <new>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "shm_allocator.h"

class ShmDynamicPool {
 public:
  using ElemPtrVector = std::vector<void *, ShmAllocator<void *>>;
  using FreeIndexVector = std::vector<int, ShmAllocator<int>>;
  using ResumeFunc = std::function<bool(void *, void *)>;
  using PostResumeFunc = std::function<bool(void *)>;
  using NameString = std::basic_string<char, std::char_traits<char>, ShmAllocator<char>>;
  ShmDynamicPool();
  void Init(const std::string &object_name, std::size_t object_unit_sz, std::size_t object_align_sz);
  // 这里allocate的时候会额外分配sizeof(size_t)大小的空间在末尾，用来当作shared_ptr的引用计数counter
  int Allocate();

  bool Deallocate(int i);

  void *GetPtr(int i) const;
  std::size_t *GetPtrRefCounterPtr(int i);

  bool CheckNameMatch(const std::string &object_name) const;
  const NameString &GetTypeName() const;
  bool Resume(const ResumeFunc &resume_func);
  bool PostResume(const PostResumeFunc &post_resume_func);
  bool CheckSubTypePtrMatch(int i, void *ptr) const;
  // 返回对象分配数量与当前活跃对象数量
  std::pair<std::size_t, std::size_t> Stat() const;

 private:
  NameString type_name_;
  std::size_t type_size_;
  std::size_t align_size_;
  ElemPtrVector elements_;
  FreeIndexVector freeindex_;
  std::size_t alloc_counter_;
};
