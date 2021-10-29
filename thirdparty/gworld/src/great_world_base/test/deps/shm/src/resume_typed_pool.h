#pragma once
//#include "log/log.h"
#include "resume_strategy.h"
#include "resume_typed_storage.h"
#include "shm_log.h"

template <typename T, template <typename> class ResumeStrategy = ResumeDefaultStrategy>
class ResumeTypedPool final : ResumeTypedStorage<T> {
 public:
  using ElementType = T;
  using Super = ResumeTypedStorage<T>;
  using Super::GetCount;

  ResumeTypedPool() : Super(sizeof(T)) {}

  bool Resume() {
    if (!IsValid()) return false;

    this->ResumeElements([](ElementType* elem) { ResumeStrategy<ElementType>::Resume(elem); });
    return true;
  }

  bool PostResume() {
    return this->PostResumeElements(
        [](ElementType* elem) { return ResumeStrategy<ElementType>::PostResume(elem); });
  }

  template <typename... Args>
  ElementType* New(Args&&... args) {
    return this->template NewElement<ElementType>(std::forward<Args>(args)...);
  }

  void Delete(ElementType* elem) { this->DeleteElement(elem); }

  ElementType* Singleton() { return this->SingletonElement(); }

  uint32_t GetSerialNumber(ElementType* elem) { return this->GetElementSerialNumber(elem); }

 private:
  bool IsValid() {
    if (this->GetTypeSize() == sizeof(ElementType)) return true;

    SHMLOG_ERROR("[ResumeTypedPool.IsValid] type:%s. type_size actual=%zu, expect=%zu",
              typeid(ElementType).name(), this->GetTypeSize(), sizeof(ElementType));
    return false;
  }
};
