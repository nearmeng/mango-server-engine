#include <fstream>
#include <iostream>
#include <memory>
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_api.h"
#include "great_world_base/common/ilog.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/shm/shm_allocator.h"
#include "mock/log.h"
#include "mock/mock_mgr_service_proxy.h"
#include "mock/mock_varint_stream.hpp"
#include "shm.h"
#include "shm_config.h"
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/space.h"
#include "great_world_base/cellmgr/balance/region_node.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "mock/resume_helper.h"

class TmpRegionNodeVisitor: public gw::cellmgr::RegionNodeVisitor {
 public:
  virtual void Visit(gw::cellmgr::CellNode& node) { GW_LOG_DEBUG("visit CellNode"); }
  virtual void Visit(gw::cellmgr::EvenlyDividedNode& node) { GW_LOG_DEBUG("visit EvenlyDividedNode"); }
  virtual void Visit(gw::cellmgr::StripNode& node) { GW_LOG_DEBUG("visit StripNode"); }
  virtual void Visit(gw::cellmgr::StripGroupNode& node) { GW_LOG_DEBUG("visit StripGroupNode"); }
  virtual void Visit(gw::cellmgr::FixedDividedNode& node) { GW_LOG_DEBUG("visit FixedDividedNode"); }
};

void OnResume() {
  ShmCreate(1008611, 1, true);
  auto mock_log = new gw::MockLogger();
  gw::Logger::InitLogger(mock_log);
  mock_log->set_priority(gw::LOG_PRIORITY_TRACE);

  uint8_t* buffer = LoadGlobal();
  gw::cellmgr::CellMgrApi::Resume(new gw::CellMgrAdapter(buffer, "./space/", "./cell_conf.json"));
  std::unordered_set<uint64_t> space_ids = gw::cellmgr::CellMgrApi::GetSpaceIds();
  TmpRegionNodeVisitor tmp_visitor;
  for(auto space_id:space_ids) {
    gw::cellmgr::Space* space = CELLMGR.FindSpace(space_id);
    auto root = space->mutable_root();
    GW_LOG_DEBUG("after resume space_id:%lu,root:%s", space_id, root->to_string().c_str());
    root->Accept(tmp_visitor);
  }
}

void OnInit() {
  // 初始化内存分配函数
  ShmCreate(1008611, 1, false);
  auto mock_log = new gw::MockLogger();
  gw::Logger::InitLogger(mock_log);
  mock_log->set_priority(gw::LOG_PRIORITY_TRACE);
  
  uint8_t* buffer = reinterpret_cast<uint8_t*>(ShmMalloc(sizeof(uint8_t) * 256));
  gw::cellmgr::CellMgrApi::Init(new gw::CellMgrAdapter(buffer, "./space/", "./cell_conf.json"));
  
  gw::cellmgr::CellMgrApi::RegisterGameApp(301);
  gw::CreateSpaceMsg msg;
  msg.set_name("mock");
  msg.set_tag("1");
  msg.set_min_cell_num(1);
  msg.set_max_cell_num(100);
  msg.set_rect({0, -8100, 8100, 0});
  msg.set_map_name("test_space.xml");
  msg.set_is_static(true);
  uint64_t cellmgr_space = gw::cellmgr::CellMgrApi::CreateSpace(msg);
  GW_LOG_TRACE("space:%lu", cellmgr_space);
  gw::timer::TimerManager::Get().SpeedUp(1000);
  gw::timer::TimerManager::Get().Update();
  SaveGlobal(buffer);
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
}