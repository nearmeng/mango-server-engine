#pragma once
#include "great_world_base/cellmgr/proxy/mgr_service_proxy.h"
#include "great_world_base/cellmgr/cellmgr_api.h"
#include "mock/mock_varint_stream.hpp"
#include "mock/log.h"
#include "great_world_base/common/cell_message.h"
#include <fstream>

#include "mock/mock_varint_stream.hpp"
#include "mock/timer/timer_mgr.h"

namespace gw {
using namespace gw::cellmgr;

class MockMgrServiceProxy {
 public:
  ~MockMgrServiceProxy() {}
  void OnAddCell(uint64_t gapp_id, const AddCellMsg &msg) { 
    gw::mock::MockMemoryOStream os;
    msg.ToString(&os);
    GW_LOG_INFO("OnAddCell:%lu, %lu, %s, size:%d", gapp_id, msg.space_id(), msg.tag().c_str(), os.size());

    gw::mock::MockMemoryIStream is(os.view());
    AddCellMsg add_msg;
    add_msg.FromString(&is);
    GW_LOG_INFO("OnAddCell:%lu, %lu, %s, %d", gapp_id, add_msg.space_id(), add_msg.tag().c_str());

    std::ofstream outfile;
    GWString file_name(GWString("../") + add_msg.tag() + GWString("_mock.data"));
    outfile.open(GW_TOSTDSTR(file_name));
    outfile << os.str().size() << os.str();
    outfile.close();
  }
  void OnDestroyCell(uint64_t gapp_id, const DestroySpaceNotice& msg) { GW_LOG_INFO("OnDestroyCell:%llu", gapp_id); }
  void OnUpdateSpaceLayout(uint64_t gapp_id, const SpaceLayoutMsg& msg) { 
    for(auto &it:msg.layouts()) {
      GW_LOG_DEBUG("layout_size:%d", it.layout_data().size());
    }
  }
  void OnAllCellCreated(uint64_t gapp_id, const AllCellCreatedMsg &msg) { GW_LOG_INFO("OnAllCellCreated:%llu", gapp_id); }
  void OnSyncCell(uint64_t gapp_id, const AddCellMsg& msg) { GW_LOG_INFO("OnSyncCell:%llu", gapp_id); }
};

class MockCellMgrNetworkSender : public gw::NetworkSender {
 public:
  void SendMessageToCell(uint64_t gapp_id, const gw::StringSlice& ss) {
    gw::mock::MockNetworkIStream is(ss);
    printf("SendMessageToCell %lu %d, %d\n", gapp_id, ss.size(), is.msg_id());
    if(is.msg_id() == NetworkMessage::AddCell) {
      AddCellMsg msg;
      msg.FromString(&is);
      MockMgrServiceProxy proxy;
      proxy.OnAddCell(gapp_id, msg);
    }
    else {
    }
  }
  
  void SendMessageToCellMgr(const StringSlice& ss) override {
    printf("SendMessageToCellMgr");
  }
};
class CellMgrAdapter : public ICellMgrAdapter {
 public:
  CellMgrAdapter(uint8_t* buffer, GWString SpaceConfPath, GWString CellMgrConfPath)
      : buffer_(buffer), SpaceConfPath_(SpaceConfPath), CellMgrConfPath_(CellMgrConfPath) {}
  ~CellMgrAdapter() {}

  
  void SendMessageToCell(uint64_t gapp_id, const gw::StringSlice& ss) override {
    sender_.SendMessageToCell(gapp_id, ss);
    printf("SendMessageToCell %lu, %d\n", gapp_id, ss.size());
  }

  GWString SpaceConfPath() override { return SpaceConfPath_; }
  GWString CellMgrConfPath() override { return CellMgrConfPath_; }
  uint8_t* MetaData() override { return buffer_; }

  uint64_t GetTimeMs() override {
    return gw::timer::TimerManager::Get().GetWalltimeMs();
  }

  ITimeMgr* CreateTimerMgr() {
    return &gw::timer::TimerManager::Get();
  }
  void DestoryTimerMgr(ITimeMgr* mgr) {

  }

  ILogger* GetLogger() override {
    return new gw::MockLogger();
  }

  void OnDestroySpace(uint64_t space_id, const GWString& tag) override {

  }

  void AllCellCreated(const gw::AllCellCreatedMsg& msg) override {
    
  }

 private:
  uint8_t* buffer_;
  GWString SpaceConfPath_;
  GWString CellMgrConfPath_;
  MockMgrServiceProxy proxy_;
  MockCellMgrNetworkSender sender_;
};

}  // namespace gw