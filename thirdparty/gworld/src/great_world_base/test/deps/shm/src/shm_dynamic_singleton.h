#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "shm_allocator.h"

struct ShmDynamicSingletonData {
  using SingletonName = std::basic_string<char, std::char_traits<char>, ShmAllocator<char>>;
  using SingletonInfo = std::pair<SingletonName, void *>;
  using SingletonVector = std::vector<SingletonInfo, ShmAllocator<SingletonInfo>>;
  SingletonVector singleton_vec;
};

class ShmSingletonArena {
  // 只支持pod类型的数据，不能有虚表 不能有继承 支持默认构造
 private:
  ShmDynamicSingletonData *singleton_data_ = nullptr;
  std::unordered_map<std::string, std::size_t> name_to_data_;

 private:
  bool ResumeImpl();
  void *GetDataImpl(const std::string &name);
  void *CreateDataImpl(const std::string &name, std::size_t align, std::size_t sz);
  void DisposeDataImpl(const std::string& name, void *ptr);
 public:
  ShmSingletonArena();
  ShmSingletonArena(const ShmSingletonArena &other) = delete;
  ShmSingletonArena &operator=(const ShmSingletonArena &other) = delete;
  static ShmSingletonArena &Instance();
  static bool Init(ShmDynamicSingletonData *singleton_data_ptr);
  static bool Resume(ShmDynamicSingletonData *singleton_data_ptr);
  template <typename T>
  static T *Get() {
    const auto &cur_type_name = T::StaticClassName();
    auto &cur_ins = Instance();
    static_assert(alignof(T) <= ShmAlignSize, "alignment exceed ShmAlignSize");
    auto temp_result = cur_ins.GetDataImpl(cur_type_name);
    if (!temp_result) {
      auto malloc_result = cur_ins.CreateDataImpl(cur_type_name, alignof(T), sizeof(T));
      if (!malloc_result) {
        return nullptr;
      }
      return new (malloc_result) T;
    } else {
      return reinterpret_cast<T *>(temp_result);
    }
  }
  template <typename T>
  static void Dispose() {
    const auto &cur_type_name = T::StaticClassName();
    auto &cur_ins = Instance();
    auto temp_result = cur_ins.GetDataImpl(cur_type_name);
    if (!temp_result) {
      return;
    }
    reinterpret_cast<T*>(temp_result)->~T();
    cur_ins.DisposeDataImpl(cur_type_name, temp_result);
  }
};