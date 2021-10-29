#pragma once
#include <tuple>
#include <type_traits>

template <typename U, std::size_t N>
struct TupleSearchType {
  template <typename T>
  constexpr static int Index(
      typename std::enable_if<std::is_same<
          typename std::remove_reference<decltype(std::get<N - 1>(std::declval<T>()))>::type,
          U>::value>::type*) {
    return N - 1;
  }

  template <typename T>
  constexpr static int Index(...) {
    return TupleSearchType<U, N - 1>::template Index<T>(nullptr);
  }
};

template <typename U>
struct TupleSearchType<U, 0> {
  template <typename T>
  constexpr static int Index(...) {
    return -1;
  }
};
