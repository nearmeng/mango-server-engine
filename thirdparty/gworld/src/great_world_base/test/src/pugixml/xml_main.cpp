#include <fstream>

#include "gtest/gtest.h"
#include "mock/log.h"
#include "great_world_base/common/xml/pugixml.hpp"

class TestXML : public testing::Test {
 public:
  ~TestXML() {
  }

  TestXML() {
  }

  static void SetUpTestCase() {
    std::ofstream outfile;
    std::string file_name("xml_test.xml");
    outfile.open(file_name);

    std::string d = R"delimiter(
<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<!-- fixed Divided -->
<Space>
        <width>8100</width>
        <height>8100</height>
</Space>
)delimiter";

    outfile << d;
    outfile.close();
  }
};


TEST_F(TestXML, TEST_READ) {
  gw_pugi::xml_document cfg_doc;
  gw_pugi::xml_parse_result parse_result = cfg_doc.load_file("xml_test.xml");
  gw_pugi::xml_node spaceNode = cfg_doc.child("Space");
  auto width = atoi(spaceNode.child_value("width"));
  printf("width:%d\n", width);
  ASSERT_EQ(width, 8100);
}
