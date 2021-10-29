#pragma once
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <memory>
#include "thread_common.h"

SMT_NS_BEGIN

namespace {
// 简单类型检查
template <typename T>
struct is_base_type {
  static const bool value = std::is_integral<T>::value ||
                            std::is_void<T>::value ||
                            std::is_enum<T>::value ||
                            std::is_floating_point<T>::value ||
                            std::is_same<std::string, T>::value;
};

// 检查stl模板类型
template <typename T>
struct is_cont {
  static const bool value = false;
};
template <typename T, typename Alloc>
struct is_cont<std::vector<T, Alloc> > {
  static const bool value = is_base_type<T>::value;
};
template <typename T, typename Alloc>
struct is_cont<std::queue<T, Alloc> > {
  static const bool value = is_base_type<T>::value;
};
template <typename K, typename V, typename H, typename E, typename Alloc>
struct is_cont<std::unordered_map<K, V, H, E, Alloc> > {
  static const bool value = is_base_type<K>::value && is_base_type<V>::value;
};

// 检查shared_ptr
template <typename T>
struct is_shared_ptr {
  static const bool value = false;
};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T> > {
  static const bool value = std::is_const<T>::value || is_base_type<T>::value;
};

// 检查是否是 const point
template <typename T>
struct is_const_ptr {
  static const bool value = false;
};
template <typename T>
struct is_const_ptr<T*> {
  // using remove_pointer_t = typename std::remove_pointer<T>::type;
  static const bool value = std::is_const<T>::value;
};

//递归终止函数
void check() {
}
//展开函数
template <class CHECK_T, class... CHECK_Args>
void check(CHECK_T head, CHECK_Args&&... rest) {
  // 是基础类型
  // 或者是stl类型
  // 或者是指向const类型的shared ptr
  static_assert(is_base_type<CHECK_T>::value ||
                    is_cont<CHECK_T>::value || is_shared_ptr<CHECK_T>::value,
                "type error");
  check(std::forward<CHECK_Args>(rest)...);
}
}  // namespace

namespace helper {
template <int... Is>
struct index {};

template <int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is>
struct gen_seq<0, Is...> : index<Is...> {};
}  // namespace helper

class MainThreadCmd {
 public:
  virtual void run() = 0;
};

class IChildThreadHandle;
class ChildThreadCmd {
 public:
  virtual void run(IChildThreadHandle*) = 0;
};

// 一共会有2-3次拷贝，外面拷贝到构造函数，构造函数移动到tuple，tuple拷贝到实际的调用函数
// 关于 check 函数，主要纠结的点是使用白名单还是黑名单
// 使用黑名单的话可以禁止掉非const指针/引用，但是对于类中包含的指针没法指定，而且也没法判断这个类实现了深拷贝，所以还是危险的
// 所以个人倾向于使用白名单，如果要直接传结构的话
// todo evanyu 需要一个 SafeChildThreadCmd、unsafe... 子线程的命令都需要知道自己的线程对象是哪个
template <class... Args>
class SafeMainThreadCmd : public MainThreadCmd {
  typedef void (*FuncType)(Args...);

 public:
  SafeMainThreadCmd(FuncType func, Args... args) : func_(func), args_(std::move(args)...) {
    if (false) check(args...);
  }

  void run() override {
    func(args_);
  }

 protected:
  SafeMainThreadCmd(int a, FuncType func, Args... args) : func_(func), args_(std::move(args)...) {
  }
  template <int... Is>
  void func(std::tuple<Args...>& tup, helper::index<Is...>) {
    func_(std::get<Is>(tup)...);
  }

  void func(std::tuple<Args...>& tup) {
    func(tup, helper::gen_seq<sizeof...(Args)>{});
  }

  std::tuple<Args...> args_;
  FuncType func_;
};

template <class... Args>
class UnsafeMainThreadCmd : public SafeMainThreadCmd<Args...> {
  typedef void (*FuncType)(Args...);

 public:
  UnsafeMainThreadCmd(FuncType func, Args... args) : SafeMainThreadCmd<Args...>(1, func, args...) {
  }
};

template <class... Args>
class SafeChildThreadCmd : public ChildThreadCmd {
  typedef void (*FuncType)(IChildThreadHandle*, Args...);

 public:
  SafeChildThreadCmd(FuncType func, Args... args) : func_(func), args_(std::move(args)...) {
    if (false) check(args...);
  }

  void run(IChildThreadHandle* child_handle) {
    func(child_handle, args_);
  }

 protected:
  SafeChildThreadCmd(int a, FuncType func, Args... args) : func_(func), args_(std::move(args)...) {
  }
  template <int... Is>
  void func(IChildThreadHandle* child_handle, std::tuple<Args...>& tup, helper::index<Is...>) {
    func_(child_handle, std::get<Is>(tup)...);
  }

  void func(IChildThreadHandle* child_handle, std::tuple<Args...>& tup) {
    func(child_handle, tup, helper::gen_seq<sizeof...(Args)>{});
  }

  std::tuple<Args...> args_;
  FuncType func_;
};

template <class... Args>
class UnsafeChildThreadCmd : public SafeChildThreadCmd<Args...> {
  typedef void (*FuncType)(IChildThreadHandle*, Args...);

 public:
  UnsafeChildThreadCmd(FuncType func, Args... args) : SafeChildThreadCmd<Args...>(1, func, args...) {
  }
};
SMT_NS_END