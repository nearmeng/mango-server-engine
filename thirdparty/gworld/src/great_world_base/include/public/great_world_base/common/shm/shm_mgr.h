#pragma once
#include <algorithm>
#include <cstring>
#include <functional>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

#include "great_world_base/common/shm/shm_allocator.h"
#include "great_world_base/common/shm/shm_pool.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/singleton_template.h"

namespace gw {

class ShmClassRegister : public SingletonRef<ShmClassRegister> {
 public:
  template <typename T>
  bool Registe() {
    name_to_id_.emplace(T::StaticStdClassName(), 0);
    std::vector<std::string> class_names;
    for (auto& it : name_to_id_) {
      class_names.push_back(it.first);
    }
    std::sort(class_names.begin(), class_names.end());
    name_to_id_.clear();
    for (size_t i = 0; i < class_names.size(); i++) {
      name_to_id_[class_names[i]] = i;
    }

    std::function<void(void*)> resume_func = [](void* p) {
      gw::ShmClassRegister::GetInstance().Resume<T>(p);
    };

    name_to_resume_func_[T::StaticStdClassName()] = resume_func;
    return true;
  }

  template <typename T>
  void Resume(void* p) {
    T* tmp = reinterpret_cast<T*>(std::malloc(sizeof(T)));
    std::memcpy(tmp, p, sizeof(T));
    new (p) T();
    // 跳过虚表
    std::memcpy(
        reinterpret_cast<void*>(reinterpret_cast<uint64_t>(p) + sizeof(void*)),
        reinterpret_cast<void*>(reinterpret_cast<uint64_t>(tmp) + sizeof(void*)),
        sizeof(T) - sizeof(void*));
    std::free(tmp);
  }

  template <typename T>
  void PushSpecialResumeFunc(std::function<void(void*)> func) {
    auto id_it = name_to_id_.find(T::StaticStdClassName());
    assert(id_it != name_to_id_.end());
    uint32_t id = id_it->second;
    special_resumes_.push_back({id, func});
  }

  template <typename T>
  uint32_t GetClassId() {
    static uint32_t index = name_to_id_[T::StaticStdClassName()];
    return index;
  }

  const std::unordered_map<std::string, uint32_t>& name_to_id() const { return name_to_id_; }
  const std::unordered_map<std::string, std::function<void(void*)>>& name_to_resume_func() const { return name_to_resume_func_; }
  const std::vector<std::pair<uint32_t, std::function<void(void*)>>>& special_resumes() const { return special_resumes_; }

 private:
  std::unordered_map<std::string, uint32_t> name_to_id_;
  std::unordered_map<std::string, std::function<void(void*)>> name_to_resume_func_;
  std::vector<std::pair<uint32_t, std::function<void(void*)>>> special_resumes_;
};
#define SHM_CLASS_REGISTER gw::ShmClassRegister::GetInstance()

class ShmMgr : public SingletonRef<ShmMgr> {
 public:
  void set_malloc_func(ShmMallocFunc func) { malloc_ = func; }
  void set_free_func(ShmFreeFunc func) { free_ = func; }

  void* malloc(std::size_t size) { return malloc_(size); }
  void free(void* p) { free_(p); }

  void set_pools(ShmPoolMgr* pool_mgr) { pool_mgr_ = pool_mgr; }
  ShmPoolMgr* pool_mgr() { return pool_mgr_; }

  void Init() {
    if (pool_mgr_) return;
    pool_mgr_ = reinterpret_cast<ShmPoolMgr*>(ShmMalloc(sizeof(ShmPoolMgr)));
    new (pool_mgr_) ShmPoolMgr();
    pool_mgr_->Init(SHM_CLASS_REGISTER.name_to_id());
  }

  void Resume() {
    pool_mgr_->Resume(SHM_CLASS_REGISTER.name_to_id(), SHM_CLASS_REGISTER.name_to_resume_func());
    pool_mgr_->SpecialResume(SHM_CLASS_REGISTER.special_resumes());
  }

  // 分配一个 id 出来
  template <typename T>
  uint32_t Allocate(void* p) {
    ShmPool& pool = pool_mgr_->pools()[SHM_CLASS_REGISTER.GetClassId<T>()];
    return pool.Allocate(p);
  }
  // 归还一个 id
  template <typename T>
  void Deallocate(uint32_t index) {
    ShmPool& pool = pool_mgr_->pools()[SHM_CLASS_REGISTER.GetClassId<T>()];
    pool.Deallocate(index);
  }

 private:
  ShmMallocFunc malloc_ = std::malloc;
  ShmFreeFunc free_ = std::free;
  ShmPoolMgr* pool_mgr_ = nullptr;
};
#define SHM_MGR gw::ShmMgr::GetInstance()
#define SHM_POOL_MGR SHM_MGR.pool_mgr()

template <typename T, typename... Args>
T* ShmNew(Args&&... args) {
  while (true) {
    auto p = ShmMalloc(sizeof(T) + 16);  // 给每个对象多分配16个字节，用来做热更新数据结构往后加数据
    if (p) {
      T* t = new (p) T(std::forward<Args>(args)...);
      t->shm_index_ = SHM_MGR.Allocate<T>(p);
      return t;
    }
    std::new_handler globalHandler = std::set_new_handler(NULL);
    std::set_new_handler(globalHandler);
    if (globalHandler) {
      (*globalHandler)();
    } else {
      throw std::bad_alloc();
    }
  }
  return nullptr;
}

template <typename T>
void ShmDelete(T* p) {
  SHM_MGR.Deallocate<T>(p->shm_index_);
  p->~T();
  ShmFree(p);
}

}  // namespace gw

#define GWCLASS_DECL_IMPLEMENT(CLASSNAME)                           \
  bool CLASSNAME::class_register_ = CLASSNAME::ClassRegisterFunc(); \
  bool CLASSNAME::ClassRegisterFunc() {                             \
    return SHM_CLASS_REGISTER.Registe<CLASSNAME>();                 \
  }

#define GWCLASS_DECL(CLASSNAME)                                  \
 public:                                                         \
  template <typename T, typename... Args>                        \
  friend T* gw::ShmNew(Args&&... args);                          \
  template <typename T>                                          \
  friend void gw::ShmDelete(T* p);                               \
  friend void gw::ShmClassRegister::Resume<CLASSNAME>(void*);    \
  gw::GWString ClassName() { return #CLASSNAME; }                \
  static gw::GWString StaticClassName() { return #CLASSNAME; }   \
  static std::string StaticStdClassName() { return #CLASSNAME; } \
  static bool ClassRegisterFunc();                               \
  static bool class_register_;                                   \
  void Dispose() { gw::ShmDelete<CLASSNAME>(this); }             \
  virtual void ___mark_virtual() {}                              \
  uint32_t shm_index_;                                           \
                                                                 \
 protected:                                                      \
  void* operator new(size_t num) throw() { return nullptr; }     \
  void operator delete(void* addr) { return; }                   \
  void* operator new(size_t, void* p) { return p; }              \
                                                                 \
 public:
