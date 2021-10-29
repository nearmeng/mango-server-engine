#include "shm_dynamic_arena.h"
#include <unordered_map>
#include "shm_log.h"
// #include <execinfo.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>


std::size_t ShmDynamicArena::last_used_type_id_ = 0;
// void show_back_trace() {
//    int j, nptrs;
// #define SIZE 100
//    void *buffer[100];
//    char **strings;

//    nptrs = backtrace(buffer, SIZE);

//    strings = backtrace_symbols(buffer, nptrs);
//     if (strings == NULL) {
//         perror("backtrace_symbols");
//         exit(EXIT_FAILURE);
//     }

//    for (j = 0; j < nptrs; j++)
//         printf("%s\n", strings[j]);

//    free(strings);
// }

ShmDynamicArena::ShmDynamicArena() {
  SHMLOG_INFO("ShmDynamicArena create at %p", this);
  // show_back_trace();

}

ShmDynamicArena::~ShmDynamicArena() {
  SHMLOG_INFO("ShmDynamicArena destroy at %p", this);
//   show_back_trace();
}


bool ShmDynamicArena::RegisterTypeImpl(const std::string &cur_type_name, std::size_t cur_type_idx,
                                       std::size_t cur_unit_size, std::size_t cur_align_size,
                                       ResumeFunc resume_func, PostResumeFunc post_resume_func,
                                       ShmDtorFunc dtor_func) {
  if (!pool_dtors_.empty()) {
    SHMLOG_ERROR("pool_dtors not empty while RegisterType, this type %s would be ignored",
                cur_type_name.c_str());
    return false;
  }
  if (cur_type_name.empty()) {
    SHMLOG_ERROR("type name should not empty");
    assert(false);
    return false;
  }
  auto cur_iter = registed_type_info_.find(cur_type_name);
  if (cur_iter != registed_type_info_.end()) {
    SHMLOG_ERROR("type %s already registered",
                cur_type_name.c_str());
    return false;
  }
  if (cur_align_size > ShmAlignSize) {
    // 最大对齐为16字节 更大的需要对源代码进行修改
    SHMLOG_ERROR("type %s align size should not large than %zu cur align %ld ", cur_type_name.c_str(),
                 ShmAlignSize, cur_align_size);
    return false;
  }
  SHMLOG_INFO("RegisterTypeImpl for type %s  align size %ld suc", cur_type_name.c_str(), cur_align_size);

  ShmObjectTypeInfo cur_type_info;
  cur_type_info.name = cur_type_name;
  cur_type_info.unit_size = cur_unit_size;
  cur_type_info.align_size = cur_align_size;
  cur_type_info.resume_func = resume_func;
  cur_type_info.post_resume_func = post_resume_func;
  cur_type_info.type_idx = cur_type_idx;
  cur_type_info.dtor_func = dtor_func;
  registed_type_info_[cur_type_name] = cur_type_info;
  return true;
}

bool ShmDynamicArena::InitImpl(ShmDynamicArenaData *arena_data_ptr) {
  arena_data_ = arena_data_ptr;
  std::size_t total_type_sz = registed_type_info_.size();
  arena_data_->object_pools = ShmDynamicArenaData::PoolPtrVector(total_type_sz, nullptr);
  pool_dtors_ = std::vector<ShmDtorFunc>(total_type_sz);
  type_idx_to_pool_idx_vec_ = std::vector<std::size_t>(total_type_sz);
  for (const auto &one_item : registed_type_info_) {
    auto cur_index = one_item.second.type_idx;
    SHMLOG_INFO("InitImpl with type %s index %zu align_sz %zu unit_sz %zu", one_item.first.c_str(), one_item.second.type_idx, one_item.second.align_size , one_item.second.unit_size);
    if (cur_index >= total_type_sz) {
      SHMLOG_ERROR("fail cur_index %zu cur_type %s total_type_sz %s", cur_index, one_item.first.c_str(), total_type_sz);
      return false;
    }
    auto cur_pool_raw = ShmMalloc(sizeof(ShmDynamicPool));
    if (!cur_pool_raw) {
      SHMLOG_ERROR("cant malloc for ShmDynamicPool");
      return false;
    }
    auto cur_pool = new (cur_pool_raw) ShmDynamicPool();
    cur_pool->Init(one_item.second.name, one_item.second.unit_size, one_item.second.align_size);
    arena_data_->object_pools[cur_index] = cur_pool;
    pool_dtors_[cur_index] = one_item.second.dtor_func;
    type_idx_to_pool_idx_vec_[cur_index] = cur_index;
  }
  return true;
}

bool ShmDynamicArena::ResumeImpl(ShmDynamicArenaData *arena_data_ptr) {
  arena_data_ = arena_data_ptr;
  std::unordered_map<std::string, std::size_t> name_to_pools;
  for (std::size_t i = 0; i < arena_data_->object_pools.size(); i++) {
   

    const auto one_pool = arena_data_->object_pools[i];
    SHMLOG_INFO("ResumeImpl arena_data_ type %s index %zu", one_pool->GetTypeName().c_str(), i);
    const auto &cur_type_name_shm = one_pool->GetTypeName();
    std::string cur_type_name(cur_type_name_shm.c_str(), cur_type_name_shm.size());
    name_to_pools[cur_type_name] = i;
  }
  std::size_t total_type_sz = registed_type_info_.size();
  if (arena_data_->object_pools.size() != total_type_sz) {
    SHMLOG_ERROR("resume fail because pool sz not match pool_sz %zu total_type_sz %zu",
                 arena_data_->object_pools.size(), total_type_sz);
    return false;
  }
  type_idx_to_pool_idx_vec_ = std::vector<std::size_t>(total_type_sz);
  pool_dtors_ = std::vector<ShmDtorFunc>(total_type_sz);
  // 由于注册顺序问题
  // 可能每次注册的类型对应的索引都不一样，所以要根据名字重新调整索引
  for (const auto &one_item : registed_type_info_) {
    SHMLOG_INFO("ResumeImpl registed_type_info_ type %s index %zu", one_item.first.c_str(), one_item.second.type_idx);

    auto cur_index = one_item.second.type_idx;
    auto cur_iter = name_to_pools.find(one_item.second.name);
    if (cur_iter == name_to_pools.end()) {
      SHMLOG_ERROR("resume fail because cant find pool for type %s", one_item.second.name.c_str());
      return false;
    }
    auto cur_pool_idx = name_to_pools[one_item.first];
    type_idx_to_pool_idx_vec_[one_item.second.type_idx] = cur_pool_idx;
    pool_dtors_[cur_pool_idx] = one_item.second.dtor_func;
  }
  for (const auto &one_item : registed_type_info_) {
    auto cur_pool_index = type_idx_to_pool_idx_vec_[one_item.second.type_idx];
    auto cur_pool = GetPool(cur_pool_index);
    if (!cur_pool) {
      SHMLOG_ERROR("resume fail because cant find pool for type %s", one_item.first.c_str());
      return false;
    }
    if (!cur_pool->CheckNameMatch(one_item.second.name)) {
      SHMLOG_ERROR("resume fail because CheckNameMatch fail item name %s pool_name %s",
                   cur_pool->GetTypeName().c_str(), one_item.second.name.c_str());

      return false;
    }
    if (!cur_pool->Resume(one_item.second.resume_func)) {
      SHMLOG_ERROR("resume fail because Type %s resume fail ",
                   cur_pool->GetTypeName().c_str());
      return false;
    }
  }
  return true;
}

bool ShmDynamicArena::PostResumeImpl() {
  for (const auto &one_item : registed_type_info_) {
    auto cur_type_id = one_item.second.type_idx;
    auto cur_pool = GetPool(cur_type_id);
    if (!cur_pool) {
      return false;
    }
    if (!cur_pool->PostResume(one_item.second.post_resume_func)) {
      return false;
    }
  }
  return true;
}

ShmDynamicPool *ShmDynamicArena::GetPool(std::size_t cur_pool_idx) const {
  if (!arena_data_) {
    return nullptr;
  }
  if (arena_data_->object_pools.size() <= cur_pool_idx) {
    return nullptr;
  }
  return arena_data_->object_pools[cur_pool_idx];
}

void *ShmDynamicArena::GetPtrImpl(std::size_t cur_pool_idx, int index) const {
  auto cur_pool = GetPool(cur_pool_idx);
  if (!cur_pool) {
    return nullptr;
  }
  return cur_pool->GetPtr(index);
}

std::size_t *ShmDynamicArena::GetPtrRefCounterPtrImpl(std::size_t cur_pool_idx, int index) const {
  auto cur_pool = GetPool(cur_pool_idx);
  if (!cur_pool) {
    return nullptr;
  }
  return cur_pool->GetPtrRefCounterPtr(index);
}
int ShmDynamicArena::Allocate(std::size_t cur_pool_idx, const std::string& name) {
  auto cur_pool = GetPool(cur_pool_idx);
  if (!cur_pool) {
    SHMLOG_ERROR("fail to Allocate for class %s type not registered", name.c_str());
    return -1;
  }
  return cur_pool->Allocate();
}

bool ShmDynamicArena::DeAllocate(std::size_t cur_pool_idx, int index) {
  auto cur_pool = GetPool(cur_pool_idx);
  if (!cur_pool) {
    return false;
  }
  return cur_pool->Deallocate(index);
}

bool ShmDynamicArena::DestroyImpl(std::size_t cur_pool_idx, int offset_idx) {
  auto cur_pool = GetPool(cur_pool_idx);
  if (!cur_pool) {
    return false;
  }
  void *addr = cur_pool->GetPtr(offset_idx);
  if (!addr) {
    return false;
  }

  if (cur_pool_idx >= pool_dtors_.size()) {
    return false;
  }
  pool_dtors_[cur_pool_idx](addr);
  auto result = DeAllocate(cur_pool_idx, offset_idx);
  return result;
}

bool ShmDynamicArena::DestroyDynPtr(std::size_t cur_pool_idx, int offset_idx, void *ptr) {
  auto &cur_instance = ShmDynamicArena::Instance();
  auto cur_pool = cur_instance.GetPool(cur_pool_idx);
  if (!cur_pool) {
    return false;
  }
  if (!cur_pool->CheckSubTypePtrMatch(offset_idx, ptr)) {
    return false;
  }
  return cur_instance.DestroyImpl(cur_pool_idx, offset_idx);
}
ShmDynamicArena &ShmDynamicArena::Instance() {
  static ShmDynamicArena* the_one = new ShmDynamicArena();
  return *the_one;
}

std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> ShmDynamicArena::Stat() {
  auto &cur_instance = Instance();
  std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> result;

  for (const auto& one_pool: cur_instance.arena_data_->object_pools) 
     {
    result[std::string(one_pool->GetTypeName().c_str())] = one_pool->Stat();
  }
  return result;
}
