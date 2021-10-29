#include "great_world_base/common/reader/json_reader.h"
#include <fstream>
#include <streambuf>

namespace gw {

bool JsonReader::Init() { return Reload(); }

bool JsonReader::Reload() {
  std::string s = ReadFromFile();
  if (s.empty()) {
    s = "{}";
  }
  d.Parse(s.c_str());
  if (d.HasParseError()) {
    GW_LOG_INFO("load json config error:code %d", d.GetParseError());
    return false;
  }
  return true;
}

RAPIDJSON_NAMESPACE::Value* JsonReader::readConfig(GWVector<const char*>& name) {
  if (d.HasParseError()) return nullptr;
  RAPIDJSON_NAMESPACE::Value* v = &d;
  for (auto& it : name) {
    if (v->HasMember(it)) {
      v = &((*v)[it]);
    } else {
      return nullptr;
    }
  }
  return v;
}

#define GW_JSON_CONFIG(TYPE, IS_TYPE, GET_TYPE)                           \
  bool JsonReader::readConfig(GWVector<const char*> name, TYPE& val) {    \
    auto rapid_value = readConfig(name);                                  \
    if (rapid_value && rapid_value->IS_TYPE()) {                          \
      val = static_cast<TYPE>(rapid_value->GET_TYPE());                   \
      return true;                                                        \
    } else {                                                              \
      GW_LOG_WARN("json reader type error, rapid_value_type %d",         \
                   rapid_value == nullptr ? -1 : rapid_value->GetType()); \
      return false;                                                       \
    }                                                                     \
  }

GW_JSON_CONFIG(float, IsNumber, GetFloat)
GW_JSON_CONFIG(uint8_t, IsNumber, GetUint)
GW_JSON_CONFIG(uint32_t, IsNumber, GetUint)
GW_JSON_CONFIG(uint64_t, IsNumber, GetUint64)
GW_JSON_CONFIG(bool, IsBool, GetBool)
GW_JSON_CONFIG(GWString, IsString, GetString)

#undef GW_JSON_CONFIG

}  // namespace gw