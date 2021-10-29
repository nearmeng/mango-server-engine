#include <iostream>
#include <fstream>
#include "great_world_base/cellmgr/cellmgr_api.h"
#include "great_world_base/common/ilog.h"
#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/cell_message.h"
#include "gtest/gtest.h"

#include "mock/log.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/timer/timer_mgr.h"

int main() {
  SHM_MGR.set_malloc_func(std::malloc);
  SHM_MGR.set_free_func(std::free);

  // gw::Logger::InitLogger(new gw::MockLogger());
  // cellmgr
  // 初始化cellmgr
  uint8_t buffer_[256];
  gw::cellmgr::CellMgrApi::Init(new gw::CellMgrAdapter(buffer_, "./space/", "./cell_conf.json"));

  // 设置 gapp
  gw::cellmgr::CellMgrApi::RegisterGameApp(301);
  gw::cellmgr::CellMgrApi::RegisterGameApp(302);
  gw::cellmgr::CellMgrApi::RegisterGameApp(303);
  gw::cellmgr::CellMgrApi::RegisterGameApp(304);

  gw::CreateSpaceMsg msg;
  msg.set_name("spacename");
  msg.set_tag("spacetag");
  gw::Rect& proto_rect = msg.mutable_rect();
  proto_rect.set_minx(0);
  proto_rect.set_miny(0);
  proto_rect.set_maxx(20000);
  proto_rect.set_maxy(10000);
  gw::mock::MockMemoryOStream os;
  uint16_t rows = 2;
  uint16_t cols = 2;
  os << static_cast<uint8_t>(gw::RegionNodeType::kEvenlyDivided);
  os << rows;
  os << cols;
  os << static_cast<uint8_t>(0);  // 无子区域数据
  msg.set_layout_data(os.str());
  msg.set_is_static(true);
  auto space_id = gw::cellmgr::CellMgrApi::CreateSpace(msg);
  gw::timer::TimerManager::Get().SpeedUp(2000);
  gw::timer::TimerManager::Get().Update();
  return 0;
}