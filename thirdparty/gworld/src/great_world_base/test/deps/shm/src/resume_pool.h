#pragma once
//#include "log/log.h"
#include <cstdio>
#include "resume_typed_storage.h"
#include "shm_log.h"

struct ResumePoolStorage : ResumeTypedStorage<void> {
  using Super = ResumeTypedStorage<void>;

  ResumePoolStorage(const std::string& type_name, std::size_t type_size)
      : Super(type_size), type_name_(type_name.c_str(), type_name.size()) {}

  const char* GetTypeName() const { return type_name_.c_str(); }

  bool IsValid(const char* expect_type, std::size_t expect_size) {
    auto* actual_type = GetTypeName();
    auto actual_size = GetTypeSize();
    if (strcmp(actual_type, expect_type) == 0 && actual_size == expect_size) return true;

    SHMLOG_ERROR( 
        "[ResumePoolStorage.IsValid] type:actual=%s, expect=%s. type_size:actual=%zu, expect=%zu",
        actual_type, expect_type, actual_size, expect_size);
    return false;
  }

  shm::string type_name_;
};

class ResumePoolImpl {
 public:
  using ResumeFunc = ResumePoolStorage::ResumeFunc;
  using PostResumeFunc = ResumePoolStorage::PostResumeFunc;

  bool Bind(ResumePoolStorage* storage) {
    if (storage_) return false;
    storage_ = storage;
    return true;
  }

  bool Resume(const ResumeFunc& func) { return storage_->ResumeElements(func); }

  bool PostResume(const PostResumeFunc& func) { return storage_->PostResumeElements(func); }

  template <typename T, typename... Args>
  T* New(Args&&... args) {
    return storage_->template NewElement<T>(std::forward<Args>(args)...);
  }

  template <typename T>
  void Delete(T* elem) {
    storage_->DeleteElement(elem);
  }

  template <typename T>
  T* Singleton() {
    return static_cast<T*>(storage_->SingletonElement());
  }

  uint32_t GetSerialNumber(void* elem) { return storage_->GetElementSerialNumber(elem); }

  template <typename T>
  bool IsValid() const {
    return storage_->IsValid(GetTypeName<T>(), sizeof(T));
  }

  template <typename T>
  static const char* GetTypeName() {
    return typeid(T).name();
  }

 private:
  ResumePoolStorage* storage_ = nullptr;
};

template <typename T>
struct ResumePool {
  ResumePool() = default;
  ResumePool(const ResumePool&) = delete;
  ResumePool(ResumePool&&) = delete;
  ResumePool& operator=(const ResumePool&) = delete;
  ResumePool& operator=(ResumePool&&) = delete;

  bool Bind(ResumePoolImpl* impl) {
    impl_ = impl;
    if (impl_->IsValid<T>()) return true;

    assert(false);
    impl_ = nullptr;
    return false;
  }

  template <typename... Args>
  T* New(Args&&... args) {
    return impl_->New<T>(std::forward<Args>(args)...);
  }

  void Delete(T* elem) { impl_->Delete<T>(elem); }

  T* Singleton() { return impl_->Singleton<T>(); }

  uint32_t GetSerialNumber(T* elem) { return impl_->GetSerialNumber(elem); }

  ResumePoolImpl* impl_ = nullptr;
};
