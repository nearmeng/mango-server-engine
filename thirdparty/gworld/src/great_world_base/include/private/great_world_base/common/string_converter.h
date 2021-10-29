#pragma once
#include <string>
#include "great_world_base/common/common_private.h"

namespace gw {

class StringConverter {
 public:
  //返回是否成功
  static bool ToInt32(const char* str, int32_t& num);

  static bool ToUint32(const char* str, uint32_t& num);

  static bool ToInt64(const char* str, int64_t& num);

  static bool ToUint64(const char* str, uint64_t& num);

  static bool ToFloat(const char* str, float& num);

  static bool ToUint8(const char* str, uint8_t& num);

  static bool ToUint16(const char* str, uint16_t& num);
};

#define TRY_STR_TO_INT32(str, num, fail_return_value) \
  if (!StringConverter::ToInt32(str, num)) {          \
    GW_LOG_ERROR("ToInt32 fail, str:%s", str);          \
    return (fail_return_value);                       \
  }
#define TRY_STR_TO_NEW_INT32(str, num, fail_return_value) \
  int32_t num = 0;                                        \
  TRY_STR_TO_INT32(str, num, fail_return_value)

#define TRY_STR_TO_UINT32(str, num, fail_return_value) \
  if (!StringConverter::ToUint32(str, num)) {          \
    GW_LOG_ERROR("ToInt32 fail, str:%s", str);           \
    return (fail_return_value);                        \
  }
#define TRY_STR_TO_NEW_UINT32(str, num, fail_return_value) \
  uint32_t num = 0;                                        \
  TRY_STR_TO_UINT32(str, num, fail_return_value)

#define TRY_STR_TO_INT64(str, num, fail_return_value) \
  if (!StringConverter::ToInt64(str, num)) {          \
    GW_LOG_ERROR("ToInt32 fail, str:%s", str);          \
    return (fail_return_value);                       \
  }
#define TRY_STR_TO_NEW_INT64(str, num, fail_return_value) \
  int64_t num = 0;                                        \
  TRY_STR_TO_INT64(str, num, fail_return_value)

#define TRY_STR_TO_UINT64(str, num, fail_return_value) \
  if (!StringConverter::ToUint64(str, num)) {          \
    GW_LOG_ERROR("ToInt32 fail, str:%s", str);           \
    return (fail_return_value);                        \
  }
#define TRY_STR_TO_NEW_UINT64(str, num, fail_return_value) \
  uint64_t num = 0;                                        \
  TRY_STR_TO_UINT64(str, num, fail_return_value)

#define TRY_STR_TO_UINT16(str, num, fail_return_value) \
  if (!StringConverter::ToUint16(str, num)) {          \
    GW_LOG_ERROR("ToInt16 fail, str:%s", str);           \
    return (fail_return_value);                        \
  }
#define TRY_STR_TO_NEW_UINT16(str, num, fail_return_value) \
  uint64_t num = 0;                                        \
  TRY_STR_TO_UINT16(str, num, fail_return_value)

#define TRY_STR_TO_UINT8(str, num, fail_return_value) \
  if (!StringConverter::ToUint8(str, num)) {          \
    GW_LOG_ERROR("ToInt8 fail, str:%s", str);           \
    return (fail_return_value);                       \
  }
#define TRY_STR_TO_NEW_UINT8(str, num, fail_return_value) \
  uint64_t num = 0;                                       \
  TRY_STR_TO_UINT8(str, num, fail_return_value)

#define TRY_STR_TO_FLOAT(str, num, fail_return_value) \
  if (!StringConverter::ToFloat(str, num)) {          \
    GW_LOG_ERROR("ToFloat fail, str:%s", str);          \
    return (fail_return_value);                       \
  }
#define TRY_STR_TO_NEW_FLOAT(str, num, fail_return_value) \
  float num = 0f;                                         \
  TRY_STR_TO_FLOAT(str, num, fail_return_value)
};  //namespace gw
