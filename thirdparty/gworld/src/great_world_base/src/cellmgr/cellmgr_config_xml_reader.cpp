#include "great_world_base/cellmgr/cellmgr_config_xml_reader.h"

#include "great_world_base/common/string_converter.h"

#define CHECK_CELL_XML_CONFIG()                                                                                     \
  if (!parse_result_) {                                                                                             \
    GW_LOG_ERROR("readConfig(%s) fail: %s", config_file_name_.c_str(), parse_result_.description());                  \
    return false;                                                                                                   \
  }                                                                                                                 \
  gw_pugi::xml_node cfg = cfg_doc_.child("CellConfig");                                                                \
  if (!cfg) {                                                                                                       \
    GW_LOG_ERROR("readConfig fail:CellConfig is null", name, config_file_name_.c_str(), parse_result_.description()); \
    return false;                                                                                                   \
  }

#define SET_CELL_XML_CONFIG(configName, variable, formatFunc) \
  if (strcmp(cfg.child_value(configName), "") != 0) {         \
    if (!formatFunc(cfg.child_value(configName), variable))   \
      return false;                                           \
  }

GW_CELL_MGR_BEGIN

GWCLASS_DECL_IMPLEMENT(CellMgrConfigXmlReader)
CellMgrConfigXmlReader::CellMgrConfigXmlReader() {
  parse_result_ = cfg_doc_.load_file(config_file_name_.c_str());
  if (!parse_result_) {
    GW_LOG_ERROR("load_file(%s) fail: %s", config_file_name_.c_str(), parse_result_.description());
  }
}

CellMgrConfigXmlReader::~CellMgrConfigXmlReader() {
}

bool CellMgrConfigXmlReader::readConfig(const char* name, float& val) {
  CHECK_CELL_XML_CONFIG()
  SET_CELL_XML_CONFIG(name, val, StringConverter::ToFloat)
  return true;
}

bool CellMgrConfigXmlReader::readConfig(const char* name, uint8_t& val) {
  CHECK_CELL_XML_CONFIG()
  SET_CELL_XML_CONFIG(name, val, StringConverter::ToUint8)
  return true;
}

bool CellMgrConfigXmlReader::readConfig(const char* name, uint32_t& val) {
  CHECK_CELL_XML_CONFIG()
  SET_CELL_XML_CONFIG(name, val, StringConverter::ToUint32)
  return true;
}

bool CellMgrConfigXmlReader::readConfig(const char* name, uint64_t& val) {
  CHECK_CELL_XML_CONFIG()
  SET_CELL_XML_CONFIG(name, val, StringConverter::ToUint64);
  return true;
}

bool CellMgrConfigXmlReader::readConfig(const char* name, bool& val) {
  CHECK_CELL_XML_CONFIG()
  if (strcmp(cfg.child_value(name), "") != 0)
    val = (strcmp("0", cfg.child_value("TracingBalance")) == 0);

  return true;
}

bool CellMgrConfigXmlReader::readConfig(const char* name, GWString& val) {
  CHECK_CELL_XML_CONFIG()
  val = cfg.child_value(name);
  return true;
}

GW_CELL_MGR_END