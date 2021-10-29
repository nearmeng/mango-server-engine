#include "shm_dynamic_singleton.h"
#include <cassert>

ShmSingletonArena::ShmSingletonArena() { return; }

ShmSingletonArena &ShmSingletonArena::Instance() {
  static ShmSingletonArena the_one;
  return the_one;
}

void *ShmSingletonArena::GetDataImpl(const std::string &name) {
  auto cur_iter = name_to_data_.find(name);
  if (cur_iter == name_to_data_.end()) {
    return nullptr;
  } else {
    return singleton_data_->singleton_vec[cur_iter->second].second;
  }
}
void *ShmSingletonArena::CreateDataImpl(const std::string &name, std::size_t align, std::size_t sz) {
  auto cur_iter = name_to_data_.find(name);
  if (cur_iter != name_to_data_.end()) {
    return nullptr;
  }
  if (!singleton_data_) {
    return nullptr;
  }
  assert(align <= ShmAlignSize);
  auto temp_result = ShmAlignedAlloc(align, sz);
  if (!temp_result) {
    return nullptr;
  }
  singleton_data_->singleton_vec.emplace_back(
      ShmDynamicSingletonData::SingletonName(name.data(), name.size()), temp_result);
  name_to_data_[name] = singleton_data_->singleton_vec.size() - 1;
  return temp_result;
}
bool ShmSingletonArena::Init(ShmDynamicSingletonData *singleton_data_ptr) {
  auto &cur_instance = Instance();
  cur_instance.singleton_data_ = singleton_data_ptr;
  return true;
}

bool ShmSingletonArena::Resume(ShmDynamicSingletonData *singleton_data_ptr) {
  auto &cur_instance = Instance();
  if (!cur_instance.name_to_data_.empty()) {
    return false;
  }
  cur_instance.singleton_data_ = singleton_data_ptr;
  std::size_t counter = 0;
  for (auto &one_item : singleton_data_ptr->singleton_vec) {
    std::string cur_name(one_item.first.c_str(), one_item.first.size());
    cur_instance.name_to_data_[cur_name] = counter;
    counter++;
  }
  return true;
}

void ShmSingletonArena::DisposeDataImpl(const std::string &name, void *ptr) {
  ShmFree(ptr);
  auto cur_idx = name_to_data_[name];
  singleton_data_->singleton_vec[cur_idx].second = nullptr;
  name_to_data_.erase(name);
}