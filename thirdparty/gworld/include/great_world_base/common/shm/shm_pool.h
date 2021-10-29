#pragma once
#include <functional>

#include "great_world_base/common/shm/shm_stl.h"

namespace gw {

// 这个不能有虚函数
class ShmPool {
 public:
  ShmPool(uint32_t id, GWString class_name) : id_(id), class_name_(std::move(class_name)) {}
  void Init(uint32_t id, GWString class_name) {
    id_ = id;
    class_name_ = std::move(class_name);
  }

  uint32_t Allocate(void* p) {
    if (!free_indexs_.empty()) {
      auto ret = free_indexs_.back();
      free_indexs_.pop_back();
      return ret;
    }
    elements_.push_back(p);
    return elements_.size() - 1;
  }

  void Deallocate(uint32_t i) {
    free_indexs_.push_back(i);
  }

  void Resume(std::function<void(void*)> resume_func) {
    for(uint32_t it:free_indexs_) {
      elements_[it] = 0;
    }
    for(void* p:elements_) {
      if(p) {
        resume_func(p);
      }
    }
  }

 private:
  uint32_t id_;
  GWString class_name_;
  GWVector<void*> elements_;
  GWVector<uint32_t> free_indexs_;
};

// 这个不能有虚函数
class ShmPoolMgr {
 public:
  inline ShmPool* pools() { return pools_; }
  void Init(const std::unordered_map<std::string, uint32_t>& name_to_id) {
    if (pools_) return;
    pools_ = (ShmPool*)(ShmMalloc(sizeof(ShmPool) * name_to_id.size()));
    for (auto& it : name_to_id) {
      new (pools_ + it.second) ShmPool(it.second, GW_TOSHMSTR(it.first));
    }
  }
  void Resume(
      const std::unordered_map<std::string, uint32_t>& name_to_id,
      std::unordered_map<std::string, std::function<void(void*)>> name_to_resume_func) {
    for (auto& it : name_to_id) {
      auto pool = pools_[it.second];
      pool.Resume(name_to_resume_func[it.first]);
    }
  }

  void SpecialResume(const std::vector<std::pair<uint32_t, std::function<void(void*)>>>& special_funcs) {
    for (auto& it : special_funcs) {
      auto pool = pools_[it.first];
      pool.Resume(it.second);
    }
  }

 private:
  ShmPool* pools_ = nullptr;
};

}  // namespace gw