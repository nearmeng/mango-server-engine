#pragma once

#include <map>
#include <unordered_map>
#include "shm_dynamic_pool.h"

struct ShmDynamicArenaData {
  using PoolPtrVector = std::vector<ShmDynamicPool *, ShmAllocator<ShmDynamicPool *>>;
  PoolPtrVector object_pools;
  static std::string StaticClassName() { return "ShmDynamicArenaData"; }
};
class ShmDynamicArena;

template <typename T>
struct ShmDynBasePtr {
  std::size_t pool_idx = 0;
  int offset_idx = -1;
  T *Get() const;
  typename std::add_lvalue_reference<T>::type operator*() const { return *Get(); }
  T *operator->() const noexcept { return Get(); }
  operator bool() const {
    if (offset_idx != -1) {
      return true;
    } else {
      return false;
    }
  }
  ShmDynBasePtr<T> &operator=(std::nullptr_t &&other) {
    pool_idx = 0;
    offset_idx = -1;
    return *this;
  }
};

class ShmDynamicArena {
 public:
  using ResumeFunc = ShmDynamicPool::ResumeFunc;
  using PostResumeFunc = ShmDynamicPool::PostResumeFunc;
  using ShmDtorFunc = std::function<void(void *)>;
 private:
  struct ShmObjectTypeInfo {
    std::size_t unit_size;
    std::size_t type_idx;
    std::size_t align_size;
    std::string name;
    ResumeFunc resume_func;
    PostResumeFunc post_resume_func;
    ShmDtorFunc dtor_func;
  };

 private:
  template <typename T>
  static std::size_t RegisterTypeIdx() {
    static const std::size_t id = last_used_type_id_++;
    return id;
  }
  bool RegisterTypeImpl(const std::string &cur_type_name, std::size_t cur_type_idx,
                        std::size_t cur_unit_size, std::size_t cur_align_size,
                        ResumeFunc resume_func, PostResumeFunc post_resume_func,
                        ShmDtorFunc dtor_func);
  bool InitImpl(ShmDynamicArenaData *arena_data_ptr);
  bool ResumeImpl(ShmDynamicArenaData *arena_data_ptr);
  bool PostResumeImpl();
  void *GetPtrImpl(std::size_t cur_type_idx, int index) const;
  std::size_t *GetPtrRefCounterPtrImpl(std::size_t cur_type_idx, int index) const;
  int Allocate(std::size_t cur_type_idx, const std::string& name);
  bool DeAllocate(std::size_t cur_type_idx, int index);
  bool DestroyImpl(std::size_t type_idx, int offset_idx);
  ShmDynamicPool *GetPool(std::size_t cur_type_idx) const;
  template <typename T>
  std::size_t TypeToPoolIdx() {
    auto cur_type_idx = RegisterTypeIdx<T>();
    if (cur_type_idx >= type_idx_to_pool_idx_vec_.size()) {
      return type_idx_to_pool_idx_vec_.size();
    } else {
      return type_idx_to_pool_idx_vec_[cur_type_idx];
    }
  }

 public:
  ShmDynamicArena();
  ~ShmDynamicArena();
  static ShmDynamicArena &Instance();

  ShmDynamicArena(const ShmDynamicArena &other) = delete;
  ShmDynamicArena &operator=(const ShmDynamicArena &other) = delete;
  template <typename T>
  static bool RegisterType(ResumeFunc resume_func, PostResumeFunc post_resume_func,
                           ShmDtorFunc dtor_func) {
    auto cur_type_idx = RegisterTypeIdx<T>();
    static_assert(alignof(T) <= ShmAlignSize, "alignment exceed ShmAlignSize");

    return Instance().RegisterTypeImpl(T::StaticClassName(), cur_type_idx, sizeof(T), alignof(T),
                                       resume_func, post_resume_func, dtor_func);
  }
  static bool Init(ShmDynamicArenaData *arena_data_ptr) {
    return Instance().InitImpl(arena_data_ptr);
  }
  static bool Resume(ShmDynamicArenaData *arena_data_ptr) {
    return Instance().ResumeImpl(arena_data_ptr);
  }
  static bool PostResume() { return Instance().PostResumeImpl(); }

  template <typename T>
  static T *GetPtr(int index) {
    if (index < 0) {
      return nullptr;
    }
    auto &cur_instance = Instance();
    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();
    return reinterpret_cast<T *>(Instance().GetPtrImpl(cur_pool_idx, index));
  }
  template <typename T>
  static T *GetPtr(std::size_t pool_idx, int index) {
    if (index < 0) {
      return nullptr;
    }
    auto &cur_instance = Instance();
    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();
    if (cur_pool_idx != pool_idx) {
      return nullptr;
    }
    return reinterpret_cast<T *>(Instance().GetPtrImpl(pool_idx, index));
  }
  template <typename T>
  static std::size_t *GetPtrRefCounterPtr(std::size_t pool_idx, int index) {
    if (index < 0) {
      return nullptr;
    }
    auto &cur_instance = Instance();
    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();
    if (cur_pool_idx != pool_idx) {
      return nullptr;
    }
    return Instance().GetPtrRefCounterPtrImpl(pool_idx, index);
  }
  template <typename T, typename... Args>
  static ShmDynBasePtr<T> Create(Args &&...args) {
    auto &cur_instance = Instance();
    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();

    ShmDynBasePtr<T> result;
    auto index = cur_instance.Allocate(cur_pool_idx, T::StaticClassName());
    result.pool_idx = cur_pool_idx;
    result.offset_idx = index;
    if (index == -1) {
      return result;
    }
    T *addr = result.Get();
    new (addr) T(std::forward<Args>(args)...);
    return result;
  }

  template <typename T>
  static bool Destroy(ShmDynBasePtr<T> &index) {
    auto &cur_instance = Instance();

    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();
    if (cur_pool_idx != index.pool_idx) {
      return false;
    }
    auto result = cur_instance.DestroyImpl(cur_pool_idx, index.offset_idx);
    index.offset_idx = -1;
    return result;
  }
  template <typename T>
  static ShmDynBasePtr<T> ForceConstructShmDynBasePtr(std::size_t pool_idx, int index) {
    ShmDynBasePtr<T> result;
    auto &cur_instance = Instance();

    auto cur_pool_idx = cur_instance.TypeToPoolIdx<T>();
    if (cur_pool_idx != pool_idx) {
      return result;
    }
    result.pool_idx = cur_pool_idx;
    result.offset_idx = index;
    return result;
  }
  static bool DestroyDynPtr(std::size_t pool_idx, int offset_idx, void *ptr);
  // 返回所有类型的总分配对象数量与现存对象数量
  static std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> Stat();

 private:
  std::map<std::string, ShmObjectTypeInfo> registed_type_info_;
  std::vector<ShmDtorFunc> pool_dtors_;
  // 类型注册索引到对应的pool索引的映射
  std::vector<std::size_t> type_idx_to_pool_idx_vec_;
  ShmDynamicArenaData *arena_data_;
  static std::size_t last_used_type_id_;
};

template <typename T>
T *ShmDynBasePtr<T>::Get() const {
  return ShmDynamicArena::GetPtr<T>(pool_idx, offset_idx);
}