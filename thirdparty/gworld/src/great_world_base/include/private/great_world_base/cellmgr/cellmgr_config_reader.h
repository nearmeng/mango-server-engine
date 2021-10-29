#pragma once
#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

class ICellMgrConfigReader {
 public:
  virtual bool readConfig(const char* name, float& val) = 0;
  virtual bool readConfig(const char* name, uint8_t& val) = 0;
  virtual bool readConfig(const char* name, uint32_t& val) = 0;
  virtual bool readConfig(const char* name, uint64_t& val) = 0;
  virtual bool readConfig(const char* name, bool& val) = 0;
  virtual bool readConfig(const char* name, GWString& val) = 0;
};

GW_CELL_MGR_END