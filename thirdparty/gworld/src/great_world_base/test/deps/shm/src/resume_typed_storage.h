#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <type_traits>
#include <cstdio>
#include "resuming_guard.h"
#include "utility_container.h"

template <typename T>
struct ResumeTypedStorage {
  using ElementType = T;
  using ResumeFunc = std::function<void(ElementType*)>;
  using PostResumeFunc = std::function<bool(ElementType*)>;
  using ElementsMapOld = shm::map<uint32_t, ElementType*>;
  using ElementsMap = shm::map<ElementType*, uint32_t>;

  ResumeTypedStorage(size_t type_size) : type_size_(type_size) {}

  bool ResumeElements(const ResumeFunc& func) {
    ResumingGuard guard;
    for (auto& pair : elements_) {
      func(const_cast<ElementType*>(pair.first));
    }
    return true;
  }

  bool PostResumeElements(const PostResumeFunc& func) {
    ResumingGuard guard;
    for (auto& pair : elements_) {
      if (!func(const_cast<ElementType*>(pair.first))) return false;
    }
    return true;
  }

  uint32_t GetElementSerialNumber(ElementType* elem) {
    auto it = elements_.find(elem);
    if (it == elements_.end()) {
      return 0;
    }
    return it->second;
  }

  ElementType* SingletonElement() {
    auto size = elements_.size();
    if (size == 0) return nullptr;
    if (size == 1) return elements_.begin()->first;

    assert(false);
    return nullptr;
  }

  template <typename U, typename... Args>
  U* NewElement(Args&&... args) {
    auto* chunk = ShmAlignedAlloc(alignof(U), sizeof(U));
    if (chunk == nullptr) {
      return 0;
    }
    memset(chunk, 0, sizeof(U));

    auto* elem = new (chunk) U(std::forward<Args>(args)...);
    if (elem == nullptr) {
      ShmFree(chunk);
      return 0;
    }

    do {
      serial_number_++;
    } while (serial_number_ == 0);
    elements_.insert(std::make_pair(elem, serial_number_));
    return elem;
  }

  template <typename U>
  void DeleteElement(U* elem) {
    auto it = elements_.find(elem);
    if (it == elements_.end()) {
      assert(false);
      return;
    }

    auto serial_number = it->second;
    elements_.erase(it);

    elem->~U();
    ShmFree(elem);
  }

  std::size_t GetCount() const { return elements_.size(); }

  std::size_t GetTypeSize() const { return type_size_; }

  uint32_t serial_number_ = 0;
  std::size_t type_size_;
  ElementsMap elements_;
};
