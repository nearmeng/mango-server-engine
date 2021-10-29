#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <unordered_map>
#include <string>
#include <type_traits>
#include "great_world_base/common/common.h"
#include "great_world_base/common/log.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/shm/shm_mgr.h"
#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/common/simple_simd_hash_map.hpp"

namespace gw {

using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

#define CAST_VALUE(type, value) static_cast<type>(value)
#define CAST_U64(value) CAST_VALUE(u64, (value))
#define CAST_S64(value) CAST_VALUE(s64, (value))
#define CAST_U32(value) CAST_VALUE(u32, (value))
#define CAST_S32(value) CAST_VALUE(s32, (value))
#define CAST_U16(value) CAST_VALUE(u16, (value))
#define CAST_S16(value) CAST_VALUE(s16, (value))
#define CAST_U8(value) CAST_VALUE(u8, (value))
#define CAST_S8(value) CAST_VALUE(s8, (value))
#define CAST_INT(value) CAST_VALUE(int, (value))
#define CAST_SIZE(value) CAST_VALUE(size_t, (value))
#define CAST_FLOAT(value) CAST_VALUE(float, (value))
#define CAST_DOUBLE(value) CAST_VALUE(double, (value))

#ifdef _WIN32
#define LIKELY(exp) (exp)
#define UNLIKELY(exp) (exp)
#else
#define LIKELY(exp) __builtin_expect(!!(exp), 1)
#define UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#endif

uint64_t GetWalltimeMs();
void SetWalltimeMs(std::function<uint64_t()> func);

#define G6_ASSERT(gapp) assert(gapp);

#define GW_LOG_ERROR(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_WARN(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_WARN, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_INFO(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_DEBUG(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define GW_LOG_TRACE(fmt, ...) gw::Logger::logger_->Write(gw::LOG_PRIORITY::LOG_PRIORITY_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);

#define GW_LOG_CD_ERROR(fmt, ...) GW_LOG_ERROR(fmt, ##__VA_ARGS__);
#define GW_LOG_CD_INFO(fmt, ...) GW_LOG_INFO(fmt, ##__VA_ARGS__);
#define GW_LOG_CD_DEBUG(fmt, ...) GW_LOG_DEBUG(fmt, ##__VA_ARGS__);
#define GW_LOG_CD_TRACE(fmt, ...) GW_LOG_TRACE(fmt, ##__VA_ARGS__);

// 条件日志
#define GW_LOG_IF_ERROR(condition, fmt, ...) \
  if (condition) {                         \
    GW_LOG_ERROR(fmt, ##__VA_ARGS__);        \
  }
#define GW_LOG_IF_INFO(condition, fmt, ...) \
  if (condition) {                        \
    GW_LOG_INFO(fmt, ##__VA_ARGS__);        \
  }
#define GW_LOG_IF_DEBUG(condition, fmt, ...) \
  if (condition) {                         \
    GW_LOG_DEBUG(fmt, ##__VA_ARGS__);        \
  }
#define GW_LOG_IF_TRACE(condition, fmt, ...) \
  if (condition) {                         \
    GW_LOG_TRACE(fmt, ##__VA_ARGS__);        \
  }

// 记录日志并返回
#define GW_RETURN_IF_ERROR(condition, ret, fmt, ...) \
  if (condition) {                                \
    GW_LOG_ERROR(fmt, ##__VA_ARGS__);               \
    return (ret);                                 \
  }
#define GW_RETURN_IF_INFO(condition, ret, fmt, ...) \
  if (condition) {                               \
    GW_LOG_INFO(fmt, ##__VA_ARGS__);               \
    return (ret);                                \
  }
#define GW_RETURN_IF_DEBUG(condition, ret, fmt, ...) \
  if (condition) {                                \
    GW_LOG_DEBUG(fmt, ##__VA_ARGS__);               \
    return (ret);                                 \
  }
#define GW_RETURN_IF_TRACE(condition, ret, fmt, ...) \
  if (condition) {                                \
    GW_LOG_TRACE(fmt, ##__VA_ARGS__);               \
    return (ret);                                 \
  }

#define GW_JUMP_IF_ERROR(condition, fmt, ...) \
  if (condition) {                         \
    GW_LOG_ERROR(fmt, ##__VA_ARGS__);        \
    goto FEXIT;                            \
  }

#ifndef WIN32

template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using QUICK_HASH_MAP = SIMDHashTable<K, V>; // SIMDHashTable

#else

template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using QUICK_HASH_MAP = std::unordered_map<K, V, H, E, ShmAllocator<std::pair<const K, V>>>; // SIMDHashTable

#endif

}  // namespace gw
