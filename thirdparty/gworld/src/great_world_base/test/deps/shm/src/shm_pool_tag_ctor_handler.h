#pragma once

#include <cstring>
#include <type_traits>
#include <utility>

template <typename... T>
struct ShmMakeVoid {
  using type = void;
};
template <typename... T>
using ShmVoidT = typename ShmMakeVoid<T...>::type;
struct ResumeConstructorFlag {
  int value = 0;
};

template <typename T, typename = void>
struct HasResumeConstructor : public std::false_type {};
template <typename T>
struct HasResumeConstructor<
    T, ShmVoidT<decltype(T(*(reinterpret_cast<T*>(0)), ResumeConstructorFlag()))>>
    : std::true_type {};
template <typename T>
typename std::enable_if<!HasResumeConstructor<T>::value, void>::type ResumeConstruct(T* addr) {
  new (addr) T();
}
template <typename T>
typename std::enable_if<HasResumeConstructor<T>::value, void>::type ResumeConstruct(T* addr) {
  char temp_buffer[sizeof(T) + 16];
  void* temp_object =
      reinterpret_cast<void*>((reinterpret_cast<std::size_t>(temp_buffer + 15) >> 4) << 4);
  std::memcpy(temp_object, (void*)addr, sizeof(T));
  ResumeConstructorFlag flag;
  new (addr) T(*reinterpret_cast<T*>(temp_object), flag);
}

template <typename T>
class ShmPoolTagCtorHandler {
 public:
  static void Resume(T* elem) { ResumeConstruct<T>(elem); }
  static bool PostResume(T* elem) { return OnResume(*elem, true); }

 private:
  template <class TElem>
  static auto OnResume(TElem& elem, bool is_resume) -> decltype(std::declval<TElem>().OnResume()) {
    return elem.OnResume();
  }

  template <class TElem>
  static bool OnResume(TElem&, ...) {
    return true;
  }
};
