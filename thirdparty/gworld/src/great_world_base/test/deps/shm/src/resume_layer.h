#pragma once
#include "resume_pool.h"
#include "resume_strategy.h"
#include "utility_memory_block.h"

using ResumeLayerStorage = shm::vector<ResumePoolStorage>;
using ResumeLayerStorageWrapper = ResumeTypeWrapper<ResumeLayerStorage>;

class ResumeLayerImpl {
 public:
  using ResumeFunc = ResumePoolImpl::ResumeFunc;
  using PostResumeFunc = ResumePoolImpl::PostResumeFunc;

  bool RegisterType(const char* type_name, std::size_t type_size, const ResumeFunc& resume,
                    const PostResumeFunc& post_resume);

  void Clear();

  bool Init(ResumeLayerStorage* data_array);

  bool ResumeInit(ResumeLayerStorage* data_array);

  ResumePoolImpl* Allocator(const char* type_name);

  struct ResumeType {
    ResumeFunc resume;
    PostResumeFunc post_resume;
    std::size_t type_size;
    ResumePoolImpl pool;
  };
  using ResumeTypeMap = std::map<std::string, ResumeType>;

 private:
  ResumeTypeMap type_map_;
  ResumeLayerStorage* data_array_ = nullptr;
};

struct ResumeLayer {
  template <typename T, template <typename> class ResumeStrategy = ResumeDefaultStrategy>
  static bool RegisterType() {
    return Impl().RegisterType(
        ResumePoolImpl::GetTypeName<T>(), sizeof(T),
        [](void* elem) { ResumeStrategy<T>::Resume(static_cast<T*>(elem)); },
        [](void* elem) { return ResumeStrategy<T>::PostResume(static_cast<T*>(elem)); });
  }

  static bool Init(ResumeLayerStorage* data_array) { return Impl().Init(data_array); }

  static bool ResumeInit(ResumeLayerStorage* data_array) { return Impl().ResumeInit(data_array); }

  template <typename T>
  static ResumePool<T>* Allocator() {
    auto* impl = Impl().Allocator(ResumePoolImpl::GetTypeName<T>());
    if (impl) {
      static ResumePool<T> pool;
      pool.Bind(impl);
      return &pool;
    }
    return nullptr;
  }

  static ResumeLayerImpl& Impl() {
    static ResumeLayerImpl s_impl;
    return s_impl;
  }
};