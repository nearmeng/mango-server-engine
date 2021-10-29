#pragma once
#include "tuple_search_type.h"

template <typename T, typename... Args>
auto ResumeLayerInit(bool is_resume, Args&&... args)
    -> decltype(T::Init(std::forward<Args>(args)...)) {
  if (!is_resume) {
    return T::Init(std::forward<Args>(args)...);
  } else {
    return T::ResumeInit(std::forward<Args>(args)...);
  }
}

template <typename T, std::size_t N>
struct ResumeTypedLayerTraverse {
  static bool Resume(T& t) {
    return ResumeTypedLayerTraverse<T, N - 1>::Resume(t) && std::get<N - 1>(t).Resume();
  }

  static bool PostResume(T& t) {
    return ResumeTypedLayerTraverse<T, N - 1>::PostResume(t) && std::get<N - 1>(t).PostResume();
  }
};

template <typename T>
struct ResumeTypedLayerTraverse<T, 0> {
  static bool Resume(T&) { return true; }
  static bool PostResume(T&) { return true; }
};

template <typename... Args>
struct ResumeTypedLayer {
  using ResumeTuple = std::tuple<Args...>;
  static ResumeTuple* Init(void* memory, size_t size) {
    if (size < sizeof(ResumeTuple)) return nullptr;
    return new (memory) ResumeTuple;
  }

  static ResumeTuple* ResumeInit(void* memory, size_t size) {
    if (size < sizeof(ResumeTuple)) return nullptr;

    auto* data = static_cast<ResumeTuple*>(memory);

    if (!ResumeTypedLayerTraverse<ResumeTuple, sizeof...(Args)>::Resume(*data)) return nullptr;

    if (!ResumeTypedLayerTraverse<ResumeTuple, sizeof...(Args)>::PostResume(*data)) return nullptr;

    return data;
  }

  template <
      typename T, typename ResumeElemTuple = std::tuple<typename Args::ElementType*...>,
      int Idx = TupleSearchType<T*, sizeof...(Args)>::template Index<ResumeElemTuple>(nullptr)>
  static auto Allocator(ResumeTuple& t) -> decltype(&std::get<Idx>(t)) {
    return &std::get<Idx>(t);
  }
};
