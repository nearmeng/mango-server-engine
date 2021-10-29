#pragma once
#include "shm_dynamic_arena.h"
#include "shm_pool_tag_ctor_handler.h"
#include <iostream>
using ResumeFunc = ShmDynamicArena::ResumeFunc;
using PostResumeFunc = ShmDynamicArena::PostResumeFunc;
using DtorFunc = ShmDynamicArena::ShmDtorFunc;

struct SingletonTest1 {
  int value_1 = 3456;
  static std::string StaticClassName() { return "SingletonTest1"; }

};

struct SingletonTest2 {
  int value_2 = 7890;
  static std::string StaticClassName() { return "SingletonTest2"; }
};

struct DynBaseComponent {
  int value_1;
  DynBaseComponent() { value_1 = 1; }
  DynBaseComponent(DynBaseComponent& other, const ResumeConstructorFlag& tag) { value_1 = 2; }
  virtual const std::string& ClassName() const {
    static std::string name("DynBaseComponent");
    return name;
  }
  static const std::string& StaticClassName() {
    static std::string name("DynBaseComponent");
    return name;
  }
};
template <typename T>
bool DynResumeConstruct(void* addr, void* temp) {

  ResumeConstructorFlag flag;
  new (addr) T(*reinterpret_cast<T*>(temp), flag);
  return true;
}
template <typename T>
bool DynResumePost(void* addr) {
  T* object_addr = reinterpret_cast<T*>(addr);
  return object_addr->ClassResumePostFunc();
}

template <typename T>
void DynDestroy(void* addr)
{
    T* real_addr = reinterpret_cast<T*>(addr);
    std::cout << "DynDestroy for ptr " << real_addr << " with typename " << real_addr->ClassName() << std::endl;
    real_addr->~T();
}
#define DYNCOMPONENT(CLASS)                     \
  const std::string& ClassName() const {        \
    static std::string name = #CLASS;           \
    return name;                                \
  }                                             \
  static const std::string& StaticClassName() { \
    static std::string name = #CLASS;           \
    return name;                                \
  }                                             \
  static bool class_register_;                  \
  static bool ClassRegisterFunc();              \
  bool ClassResumePostFunc();                   \


#define DYNCOMPONENT_IMPL(CLASS)                                                \
  bool CLASS::class_register_ = CLASS::ClassRegisterFunc();                     \
  bool CLASS::ClassRegisterFunc() {                                             \
    ResumeFunc resume_func = DynResumeConstruct<CLASS>;                         \
    PostResumeFunc resume_post_func = DynResumePost<CLASS>;                     \
    DtorFunc dtor_func = DynDestroy<CLASS> ;                                    \
    return ShmDynamicArena::RegisterType<CLASS>(resume_func, resume_post_func, dtor_func); \
  }                                                                             \

struct DynDerivedComponent1 : public DynBaseComponent {
  std::string value_2;
  DynDerivedComponent1() { value_2 = "init"; }
  DynDerivedComponent1(DynDerivedComponent1& other, const ResumeConstructorFlag& tag)
      : value_2(std::move(other.value_2)),
        DynBaseComponent(other, tag) {
    value_2 += "_resume";
  }
  DYNCOMPONENT(DynDerivedComponent1)
};

struct DynDerivedComponent2 : public DynDerivedComponent1 {
  std::vector<int> value_3;
  DynDerivedComponent2() { value_3.push_back(1); }
  DynDerivedComponent2(DynDerivedComponent2& other, const ResumeConstructorFlag& tag)
      : value_3(std::move(other.value_3)),
        DynDerivedComponent1(other, tag) {
    value_3.push_back(2);
  }
  DYNCOMPONENT(DynDerivedComponent2)
};