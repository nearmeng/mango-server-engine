#include <fstream>
#include <iostream>
#include <memory>

#include "great_world_base/common/ilog.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/shm/shm_allocator.h"
#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/space_manager.h"

#include "mock/log.h"

#include "mock/mock_mgr_service_proxy.h"
#include "mock/timer/timer_mgr.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/mock_gamespace_adapter.h"
#include "mock/resume_helper.h"
#include "shm.h"
#include "shm_config.h"
#include "mock/mock_aoi.h"


void OnResume() {
  ShmCreate(1008611, 1, true);
  auto mock_log = new gw::MockLogger();
  gw::Logger::InitLogger(mock_log);
  mock_log->set_priority(gw::LOG_PRIORITY_TRACE);
  uint8_t* buffer = LoadGlobal();
  gw::game::GameSpaceApi::Resume(new gw::GameSpaceAdapter());

  auto space = SPACE_MGR.FindSpace(1);
  GW_LOG_DEBUG("aftere resume space %p", space);
}

void OnInit() {
  // 初始化内存分配函数
  ShmCreate(1008611, 1, false);
  auto mock_log = new gw::MockLogger();
  gw::Logger::InitLogger(mock_log);
  mock_log->set_priority(gw::LOG_PRIORITY_TRACE);

  uint8_t* buffer = reinterpret_cast<uint8_t*>(ShmMalloc(sizeof(uint8_t) * 256));
  gw::game::GameSpaceApi::Init(new gw::GameSpaceAdapter());
  gw::mock::MockMemoryOStream os_;
  SaveGlobal(buffer);

  char space_data[1000];
  std::size_t space_data_size;
  std::ifstream infile;
  infile.open("1_mock.data");
  infile >> space_data_size;
  infile.read(space_data, space_data_size);
  infile.close();

  gw::StringSlice ss(space_data, space_data_size);
  gw::mock::MockMemoryIStream is(ss);
  gw::AddCellMsg add_msg;
  add_msg.FromString(&is);
  gw::StringSlice tmp_ss(add_msg.mutable_layout_data().c_str(), add_msg.mutable_layout_data().size());

  SPACE_MGR.AddCell(1, "", tmp_ss, false);
  auto space = SPACE_MGR.GetSpace(1);
  GW_LOG_DEBUG("space.data len %d,%p,space %p", space_data_size, tmp_ss.data(), space);
}

int main(int argc, char* argv[]) {
  ShmConfig::Instance().SetEnable(true);
  SHM_MGR.set_malloc_func(ShmMalloc);
  SHM_MGR.set_free_func(ShmFree);
  if (argc == 2) {  // 执行resume
    OnResume();
  } else {
    OnInit();
  }
  ShmDestroy();
}