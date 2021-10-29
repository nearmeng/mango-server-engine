#pragma once
#include "great_world_base/cellmgr/cellmgr_config_reader.h"
#include "great_world_base/common/xml/pugixml.hpp"

GW_CELL_MGR_BEGIN

class CellMgrConfigXmlReader : public ICellMgrConfigReader {
  GWCLASS_DECL(CellMgrConfigXmlReader)
 private:
  const GWString config_file_name_ = "../cfg/cell_conf.xml";
  gw_pugi::xml_document cfg_doc_;
  gw_pugi::xml_parse_result parse_result_;

 public:
  virtual bool readConfig(const char* name, float& val) override;
  virtual bool readConfig(const char* name, uint8_t& val) override;
  virtual bool readConfig(const char* name, uint32_t& val) override;
  virtual bool readConfig(const char* name, bool& val) override;
  virtual bool readConfig(const char* name, GWString& val) override;
  virtual bool readConfig(const char* name, uint64_t& val) override;

  CellMgrConfigXmlReader();
  ~CellMgrConfigXmlReader();
};

GW_CELL_MGR_END