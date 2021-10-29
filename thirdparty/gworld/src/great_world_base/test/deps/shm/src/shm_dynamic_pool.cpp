#include "shm_dynamic_pool.h"

#include <cstring>
#include "shm_log.h"

// 这里的sizeof(std::size_t) 的意思是引用计数变量
// 计算对齐 寻找object后面的第一个size_t对齐地址，作为counter 所以会额外分配2* sizeof(std::size_t)
ShmDynamicPool::ShmDynamicPool() {}

void ShmDynamicPool::Init(const std::string &object_name, std::size_t object_unit_sz, std::size_t object_align_sz) {
  type_name_ = NameString(object_name.c_str(), object_name.size());
  type_size_ = object_unit_sz;
  align_size_ = object_align_sz;
}

int ShmDynamicPool::Allocate() {
  void *chunk = ShmAlignedAlloc(align_size_, type_size_ + 2 * sizeof(std::size_t));
  if (chunk == nullptr) {
    SHMLOG_ERROR("fail to allocate for class %s", type_name_.c_str());
    return -1;
  }
  memset(chunk, 0, type_size_ + 2* sizeof(std::size_t));
  SHMLOG_TRACE("ShmDynamicPool::Allocate for class %s with ptr %p", type_name_.c_str(), chunk);
  alloc_counter_++;
  if (freeindex_.empty()) {
    elements_.push_back(chunk);
    return static_cast<int>(elements_.size()) - 1;
  } else {
    auto idx = freeindex_.back();
    freeindex_.pop_back();
    assert(!elements_[idx]);
    elements_[idx] = chunk;
    return idx;
  }
}

bool ShmDynamicPool::Deallocate(int i) {
  auto element = elements_[i];
  if (!element) {
    SHMLOG_ERROR("Deallocate for class %s with ptr null", type_name_.c_str());
    return false;
  }
  std::size_t counter_ptr = reinterpret_cast<std::size_t>(element) + type_size_;
  // 计算对齐 寻找object后面的第一个size_t对齐地址，作为counter
  auto st_size = sizeof(std::size_t);
  counter_ptr = ((counter_ptr + st_size - 1) / st_size) * st_size;
  if (*(reinterpret_cast<std::size_t*>(counter_ptr)) != 0) {
    SHMLOG_ERROR("Deallocate for class %s with ptr %p while ref counter not 0", type_name_.c_str(),
                 element);
    return false;
  }
  SHMLOG_TRACE("Deallocate for class %s with ptr %p", type_name_.c_str(), element);
  ShmFree(reinterpret_cast<void *>(element));
  elements_[i] = nullptr;
  freeindex_.push_back(i);
  return true;
}

void *ShmDynamicPool::GetPtr(int i) const {
  if (i < 0 || i > elements_.size()) {
    return nullptr;
  }
  auto element = elements_[i];
  return element;
}

bool ShmDynamicPool::CheckNameMatch(const std::string &object_name) const {
  if (object_name.size() != type_name_.size()) {
    return false;
  }
  for (std::size_t i = 0; i < object_name.size(); i++) {
    if (type_name_[i] != object_name[i]) {
      return false;
    }
  }
  return true;
}
const ShmDynamicPool::NameString &ShmDynamicPool::GetTypeName() const { return type_name_; }

bool ShmDynamicPool::Resume(const ResumeFunc &resume_func) {
  std::vector<char> temp_buffer(type_size_ + 2* sizeof(std::size_t) + align_size_, 0);
  void *temp_object =
      reinterpret_cast<void *>((reinterpret_cast<std::size_t>(temp_buffer.data() + align_size_ -1) / align_size_) * align_size_);
  for (auto one_ele : elements_) {
    if (!one_ele) {
      continue;
    }
    std::memcpy(temp_object, one_ele, type_size_ + 2* sizeof(std::size_t));
    if (!resume_func(one_ele, temp_object)) {
      return false;
    }
  }
  return true;
}

bool ShmDynamicPool::PostResume(const PostResumeFunc &post_resume_func) {
  for (auto one_ele : elements_) {
    if (!one_ele) {
      continue;
    }
    if (!post_resume_func(one_ele)) {
      return false;
    }
  }
  return true;
}
std::size_t *ShmDynamicPool::GetPtrRefCounterPtr(int i) {
  auto cur_raw_ptr = GetPtr(i);
  if (!cur_raw_ptr) {
    return nullptr;
  }
  std::size_t counter_ptr = reinterpret_cast<std::size_t>(cur_raw_ptr) + type_size_;
  // 计算对齐 寻找object后面的第一个size_t对齐地址，作为counter
  auto st_size = sizeof(std::size_t);
  counter_ptr = ((counter_ptr + st_size - 1) / st_size) * st_size;
  return reinterpret_cast<std::size_t *>(counter_ptr);
}
bool ShmDynamicPool::CheckSubTypePtrMatch(int i, void *ptr) const {
  auto cur_raw_ptr = GetPtr(i);
  if (!cur_raw_ptr) {
    return false;
  }
  auto temp_1 = reinterpret_cast<std::size_t>(cur_raw_ptr);
  auto temp_2 = reinterpret_cast<std::size_t>(ptr);
  return (temp_2 >= temp_1) && (temp_2 < temp_1 + type_size_);
}
std::pair<std::size_t, std::size_t> ShmDynamicPool::Stat() const {
  return std::make_pair(alloc_counter_, elements_.size() - freeindex_.size());
}