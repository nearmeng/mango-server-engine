#pragma once
#include "great_world_base/common/common_private.h"
#include "great_world_base/common/shm/shm_stl.h"

#include <fstream>
#include <streambuf>

namespace gw {

class IReader {
 public:
  IReader() {}
  IReader(GWString path) : path_(path) { }
  // 初始化
  virtual bool Init() = 0;
  // 初始化
  virtual bool Reload() = 0;

  std::string ReadFromFile() {
    if(path_.empty()) {
      return "";
    }
    else {
      std::ifstream infile(GW_TOSTDSTR(path_));
      std::string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
      return str;
    }
  }

  virtual bool readConfig(GWVector<const char*> name, float& val) = 0;
  virtual bool readConfig(GWVector<const char*> name, uint8_t& val) = 0;
  virtual bool readConfig(GWVector<const char*> name, uint32_t& val) = 0;
  virtual bool readConfig(GWVector<const char*> name, uint64_t& val) = 0;
  virtual bool readConfig(GWVector<const char*> name, bool& val) = 0;
  virtual bool readConfig(GWVector<const char*> name, GWString& val) = 0;
 protected:
  GWString path_ = "";
};

}  // namespace

#define GW_CONFIG_PATH(...) { __VA_ARGS__ }
#define GW_CONFIG_MEM(TYPE, NAME, PATH, DEFAULT) private: \
TYPE NAME##_ = DEFAULT; \
inline GWVector<const char*> NAME##_path() const { \
  return PATH; } \
inline bool reload_##NAME() { return reader_->readConfig(NAME##_path(), NAME##_); } \
public:\
inline TYPE NAME() const { return NAME##_; }

#define GW_CONFIG_MEM_RELOAD(NAME) if(!reload_##NAME()) { GW_LOG_WARN("reload %s fail", #NAME); }
