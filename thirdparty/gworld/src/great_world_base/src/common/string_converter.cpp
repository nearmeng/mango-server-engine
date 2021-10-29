#include "great_world_base/common/string_converter.h"

#include <limits>

#include "great_world_base/common/ilog.h"
#include "great_world_base/common/common_private.h"

namespace gw {

bool StringConverter::ToInt32(const char* str, int32_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  long int n = strtol(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE || n > std::numeric_limits<int32_t>::max() || n < std::numeric_limits<int32_t>::min()) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  num = (int32_t)n;
  return true;
}

bool StringConverter::ToUint32(const char* str, uint32_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  unsigned long int n = strtoul(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE || n > std::numeric_limits<uint32_t>::max()) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  num = (uint32_t)n;
  return true;
}

bool StringConverter::ToInt64(const char* str, int64_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  num = strtoll(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  return true;
}

bool StringConverter::ToUint64(const char* str, uint64_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  num = strtoull(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  return true;
}

bool StringConverter::ToFloat(const char* str, float& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  num = strtof(str, &end);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  return true;
}

bool StringConverter::ToUint8(const char* str, uint8_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  unsigned long int n = strtoul(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE || n > std::numeric_limits<uint8_t>::max()) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  num = (uint8_t)n;
  return true;
}

bool StringConverter::ToUint16(const char* str, uint16_t& num) {
  if (nullptr == str) {
    GW_LOG_ERROR("str is null");
    return false;
  }
  char* end = NULL;
  unsigned long int n = strtoul(str, &end, 0);
  if (end <= str) {
    GW_LOG_ERROR("str(%s) is not a number", str);
    return false;
  }
  if (errno == ERANGE || n > std::numeric_limits<uint16_t>::max()) {
    GW_LOG_ERROR("str(%s) is out of range", str);
    return false;
  }
  num = (uint16_t)n;
  return true;
}

};  //namespace gw
