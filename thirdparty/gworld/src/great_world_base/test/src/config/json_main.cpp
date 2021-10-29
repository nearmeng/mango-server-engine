#include <fstream>

#include "gtest/gtest.h"
#include "mock/log.h"
#include "great_world_base/common/xml/pugixml.hpp"
#include "great_world_base/common/reader/json_reader.h"
#include "great_world_base/cellmgr/cellmgr_config.h"
#include "great_world_base/cellmgr/cellmgr_api.h"
#include "mock/mock_mgr_service_proxy.h"

class TestConfig : public testing::Test {
 public:
  ~TestConfig() {
  }

  TestConfig() {
  }

  static void SetUpTestCase() {
    SHM_MGR.set_malloc_func(std::malloc);
    SHM_MGR.set_free_func(std::free);
    auto mock_log = new gw::MockLogger();
    gw::Logger::InitLogger(mock_log);
    mock_log->set_priority(gw::LOG_PRIORITY_TRACE);

    uint8_t buffer[256];
    gw::cellmgr::CellMgrApi::Init(new gw::CellMgrAdapter(buffer, "./space/", "./cell_conf.json"));

    gw::GWString path = "config.json";
    reader_ = new gw::JsonReader(path);
    reader_->Init();
  }
  static gw::JsonReader* reader_;
};

gw::JsonReader* TestConfig::reader_ = nullptr;


TEST_F(TestConfig, TEST_JSON) {
  gw::GWVector<const char*> value;
  value.push_back("CellConfig");
  value.push_back("upper_good_load");
  float f;
  ASSERT_EQ(reader_->readConfig(value, f), true);
  printf("reader_f:%f\n", f);
}

TEST_F(TestConfig, TEST_CellMgrConfig) {
  gw::cellmgr::CellMgrConfig config;
  config.Init(reader_);
  ASSERT_EQ(config.upper_good_avatar_num(), 200000);
  config.reload();
}