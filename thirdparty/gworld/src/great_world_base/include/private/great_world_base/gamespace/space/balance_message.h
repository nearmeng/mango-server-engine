// 头文件
#pragma once
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/common/memory_stream.h"

namespace gw {
namespace game {

class MigrateAvatarNum {
 private:
  // member
  uint64_t space_id_;
  uint32_t num_;
  uint32_t balance_id_;
   
 public:
  MigrateAvatarNum() = default;
  MigrateAvatarNum(const MigrateAvatarNum&) = default;
  MigrateAvatarNum& operator=(const MigrateAvatarNum&) = default;
  // constructor
  MigrateAvatarNum(uint64_t space_id, uint32_t num, uint32_t balance_id) : space_id_(space_id), num_(num), balance_id_(balance_id) { }
  // move constructor
  MigrateAvatarNum(MigrateAvatarNum&& other) : space_id_(std::move(other.space_id_)), num_(std::move(other.num_)), balance_id_(std::move(other.balance_id_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << num_;
    *os << balance_id_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> num_;
    *is >> balance_id_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline uint32_t num() const { return num_; }
  inline uint32_t& mutable_num() { return num_; }
  inline uint32_t balance_id() const { return balance_id_; }
  inline uint32_t& mutable_balance_id() { return balance_id_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_num(const uint32_t& tmp) { num_ = tmp; }
  inline void set_num(uint32_t&& tmp) { num_ = std::move(tmp); }
  inline void set_balance_id(const uint32_t& tmp) { balance_id_ = tmp; }
  inline void set_balance_id(uint32_t&& tmp) { balance_id_ = std::move(tmp); }


};


}
}
