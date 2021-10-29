#pragma once

#define RAPIDJSON_NAMESPACE gw_rapidjson

#include "great_world_base/common/rapidjson/document.h"
#include "great_world_base/common/rapidjson/stringbuffer.h"
#include "great_world_base/common/rapidjson/writer.h"
#include "great_world_base/common/reader/reader.h"

namespace gw {

class JsonReader : public IReader {
 public:
  JsonReader() {}
  JsonReader(GWString path) : IReader(path) {}

  virtual bool Init() override;
  virtual bool Reload() override;

  virtual bool readConfig(GWVector<const char*> name, float& val) override;
  virtual bool readConfig(GWVector<const char*> name, uint8_t& val) override;
  virtual bool readConfig(GWVector<const char*> name, uint32_t& val) override;
  virtual bool readConfig(GWVector<const char*> name, uint64_t& val) override;
  virtual bool readConfig(GWVector<const char*> name, bool& val) override;
  virtual bool readConfig(GWVector<const char*> name, GWString& val) override;

 private:
  virtual RAPIDJSON_NAMESPACE::Value* readConfig(GWVector<const char*>& name);

 private:
  RAPIDJSON_NAMESPACE::Document d;
};

}  // namespace gw