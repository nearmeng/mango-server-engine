#pragma once
#include <type_traits>

template <typename T>
class ResumeDefaultStrategy {
 public:
  static void Resume(T* elem) {
#ifdef __cpp_if_constexpr
    if constexpr (!std::is_pod<T>())
#else
    if (!std::is_pod<T>())
#endif
      new (elem) T;
  }
  static bool PostResume(T* elem) { return OnResume(*elem, 0); }

 private:
  template <class U>
  static auto OnResume(U& elem, int) -> decltype(std::declval<U>().OnResume()) {
    return elem.OnResume();
  }

  template <class U>
  static bool OnResume(U&, ...) {
    return true;
  }
};
