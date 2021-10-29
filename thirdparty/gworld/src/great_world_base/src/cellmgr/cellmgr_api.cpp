#include "great_world_base/cellmgr/cellmgr_api.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/evenly_divided_node.h"
#include "great_world_base/cellmgr/balance/fixed_divided_by_cell_node.h"
#include "great_world_base/cellmgr/basemgr.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/cellmgr/space.h"
#include "great_world_base/cellmgr/spacelayout/space_layout_mgr.h"
#include "great_world_base/cellmgr/managers.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/common/ilog.h"
#include "great_world_base/common/memory_stream_private.h"
#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/common/log.h"

GW_CELL_MGR_BEGIN

bool CellMgrApi::Init(ICellMgrAdapter* adapter) {
  Logger::InitLogger(adapter->GetLogger());
  for (auto& it :SHM_CLASS_REGISTER.name_to_id()) {
    GW_LOG_DEBUG("name %s id %d", it.first.c_str(), it.second);
  }
  SHM_MGR.Init(); // 必须这个最先调用，初始化内存池

  SetWalltimeMs([adapter]() -> uint64_t {
    return adapter->GetTimeMs();
  });
  if(!CELLMGR.Init(adapter->CellMgrConfPath())) {
    return false;
  }
  SPACE_LAYOUT_MGR->SetPath(adapter->SpaceConfPath());
  BaseMgr::GetInstance().Init();
  gw::TimerManager::Get()->Init([tmp = adapter]()->ITimeMgr*{
    return tmp->CreateTimerMgr();
  }, [tmp = adapter](ITimeMgr* mgr){
    tmp->DestoryTimerMgr(mgr);
  });

  struct Managers* mgrs_ = reinterpret_cast<struct Managers*>(adapter->MetaData());
  mgrs_->base_mgr_ = &BASEMGR;
  mgrs_->cell_mgr_ = &CELLMGR;
  mgrs_->space_layout_ = SPACE_LAYOUT_MGR;
  mgrs_->poll_mgr_ = SHM_MGR.pool_mgr();

  CELLMGR.set_adapter(adapter);
  NetworkMgr::sender_ = ShmNew<CellMgrNetworkSender>();
  return true;
}

void CellMgrApi::UnInit(bool is_resume) {
  CELLMGR.UnInit(is_resume);
}

void CellMgrApi::StartUp() {
  CellMgr::GetInstance().StartUp();
}

bool CellMgrApi::Resume(ICellMgrAdapter* adapter) {
  struct Managers* mgrs_ = reinterpret_cast<struct Managers*>(adapter->MetaData());
  // 恢复池子里面的对象状态
  SHM_MGR.set_pools(mgrs_->poll_mgr_);
  // 执行resume
  SHM_MGR.Resume();
  // 更新单例指针地址
  BASEMGR.InstanceResume(mgrs_->base_mgr_);
  CELLMGR.InstanceResume(mgrs_->cell_mgr_);
  SPACE_LAYOUT_MGR->InstanceResume(mgrs_->space_layout_);
  SHM_CLASS_REGISTER.PushSpecialResumeFunc<CellMgr>([](void* p) { CellMgr::OwnResume(p); });

  SetWalltimeMs([adapter]() -> uint64_t {
    return adapter->GetTimeMs();
  });
  return true;
}

bool CellMgrApi::ReloadCfg() {
  return CELLMGR_CFG.reload();
}

bool CellMgrApi::RegisterGameApp(uint64_t gapp_id, bool is_base) {
  if (is_base) {
    return BaseMgr::GetInstance().RegisterGameApp(gapp_id);
  } else {
    return CELLMGR.RegisterGameApp(gapp_id);
  }
}

bool CellMgrApi::UnregisterGameApp(uint64_t gapp_id, bool is_base) {
  if (is_base) {
    return BaseMgr::GetInstance().UnregisterGameApp(gapp_id);
  } else {
    return CELLMGR.UnregisterGameApp(gapp_id);
  }
}

std::unordered_set<uint64_t> CellMgrApi::GetGameAppIds(bool is_base) {
  const GWHashMap<uint64_t, GameAppEntry*>* gapps;
  if(is_base) {
    gapps = &BaseMgr::GetInstance().game_apps();
  }
  else {
    gapps = &CELLMGR.game_apps(); 
  }
  std::unordered_set<uint64_t> ret;
  for(auto& it:*gapps) {
    ret.insert(it.first);
  }
  return ret;
}

std::unordered_map<uint64_t, gw::CellState> CellMgrApi::GetGameAppIds(uint64_t space_id) {
  Space* space = CELLMGR.FindSpace(space_id);
  std::unordered_map<uint64_t, gw::CellState> ret;
  if(!space) {
    return ret;
  }
  for(auto& kv:space->cells()) {
    ret.emplace(kv.second->main_gapp()->id(), kv.second->main_state());
    for(auto& p_kv:kv.second->parallel_gapps()) {
      ret.emplace(p_kv.first, kv.second->state(p_kv.first));
    }
  } 
  return ret;
}

std::unordered_set<uint64_t> CellMgrApi::GetSpaceIds() {
  std::unordered_set<uint64_t> ret;
  for(auto& it:CELLMGR.spaces()) {
    ret.insert(it.first);
  }
  return ret;
}

uint64_t CellMgrApi::CreateSpace(CreateSpaceMsg& req) {
  RegionNode* root = nullptr;
  Space* space = nullptr;
  ExcludeGameAppFilter filter;
  const auto& app_ids = req.app_ids();
  if (!app_ids.empty()) {
    for (auto& app_entry : CELLMGR.game_apps()) {
      if (std::find(app_ids.begin(), app_ids.end(), app_entry.first) == app_ids.end()) {
        filter.Cache(app_entry.first);
      }
    }
  }
  const Rect& proto_rect = req.rect();
  Rect rect(proto_rect.minx(), proto_rect.miny(), proto_rect.maxx(), proto_rect.maxy());
  if (req.layout_data().length()) {
    // 指定了 space 的布局配置，则按指定的配置构建
    const GWString& data = req.layout_data();
    try {
      MemoryIStream ins(StringSlice(data.c_str(), data.length()));
      root = RegionNode::Parse(ins, &rect);
    } catch (const UnarchiveError& e) {
      GW_JUMP_IF_ERROR(true, "failed to parse");
    }
  } else {
    // 没有指定 space 的布局配置，则需要指定 space 要加载的布局配置名
    try {
      auto stream_data = SPACE_LAYOUT_MGR->GetSpaceStreamData(req.map_name());
      GW_JUMP_IF_ERROR(!stream_data, "GetSpaceStreamData_Fail");
      GW_JUMP_IF_ERROR(!MathUtility::IsEqual(stream_data->width_, (req.rect().maxx() - req.rect().minx())), "width not match");
      GW_JUMP_IF_ERROR(!MathUtility::IsEqual(stream_data->height_, (req.rect().maxy() - req.rect().miny())), "height not match");
      MemoryIStream ins(StringSlice(stream_data->data_->data(), stream_data->data_->size()));
      root = RegionNode::Parse(ins, &rect);
    } catch (const UnarchiveError& e) {
      GW_JUMP_IF_ERROR(true, "failed to parse");
    }
  }

  if (root == nullptr) {
    root = ShmNew<CellNode>();
    root->set_rect(rect);
  }
  GW_LOG_DEBUG("root node type<%d>", root->GetType());
  GW_JUMP_IF_ERROR(!root->AssignGameApps(filter), "failed to assign game apps");
  space = CELLMGR.CreateSpace(req.name(), req.tag(), req.min_cell_num(), req.max_cell_num(), rect, root, req.is_static(), req.meta_data());
  GW_JUMP_IF_ERROR(!space, "failed to create space");
  return space->id();
FEXIT:
  if (root) {
    delete root;
  }
  return 0;
}

bool CellMgrApi::DestroySpace(uint64_t id) {
  return CELLMGR.DestroySpace(id);
}

uint64_t CellMgrApi::ChooseFreeCellGame(uint64_t space_id, float x, float y) {
  auto cell = CELLMGR.FindCell(space_id, x, y);
  if(!cell) {
    return 0;
  }
  return cell->ChooseFreeGame();
}

uint64_t CellMgrApi::ChooseFreeBaseGame() {
  return BaseMgr::GetInstance().ChooseFreeGame();
}

GWString CellMgrApi::DebugString() {
  std::stringstream ss;
  for(auto& it:CELLMGR.spaces()) {
    ss << "space:" << it.first << "\r\n";
    auto cells = it.second->cells();
    for(auto& cell_it:cells) {
      ss << "==>cellnode:id " << cell_it.second->id() << ", rect " << cell_it.second->rect().DebugString().c_str() 
         << ", gapp_main_id " << cell_it.second->main_gapp()->id() << " parallel_gapps:{";
      for(auto &gapp_it:cell_it.second->parallel_gapps()) {
        ss << gapp_it.second.gapp_->id() << ",";
      }
      ss << "}\r\n";
    }
  }
  return GW_TOSHMSTR(ss.str());
}

RetCode CellMgrApi::UpdateMinParallelNum(uint64_t space_id, uint64_t cell_node_id, uint32_t value) {
  auto space = CELLMGR.FindSpace(space_id);
  if(!space) {
    return SPACE_NOT_FOUND;
  }
  if(dynamic_cast<const FixedDividedByCellNode*>(space->root()) == nullptr) {
    return NODE_TYPE_ERROR;
  }
  for(auto& it:space->cells()) {
    if(it.second->id() == cell_node_id) {
      it.second->set_min_parallel_num(value);
      return SUCCESS;
    }
  } 
  return CELL_NODE_NOT_FOUND;
}

namespace {
// gapp_id, msg_id, is
void ConfirmAddCell(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  uint64_t space_id;
  is >> space_id;
  CELLMGR.ConfirmAddCell(gapp_id, space_id);
}

void ReportLoad(uint64_t gapp_id, NetworkMessage msg_id, MemoryIStream& is) {
  ReportLoadMsg msg;
  msg.FromString(&is);
  auto cell_it = CELLMGR.game_apps().find(gapp_id);
  if(cell_it != CELLMGR.game_apps().end()) {
    CELLMGR.ReportLoad(gapp_id, msg);
  }

  auto base_it = BASEMGR.game_apps().find(gapp_id);
  if(base_it != BASEMGR.game_apps().end()) {
    BASEMGR.ReportLoad(gapp_id, msg);
  }
}
}

void CellMgrApi::DispatchMessage(uint64_t gapp_id, StringSlice ss) {
 NetworkIStream is(ss);
  switch (is.msg_id()) {
    DISPATCH_MESSAGE(NetworkMessage::ConfirmAddCell, ConfirmAddCell);
    DISPATCH_MESSAGE(NetworkMessage::ReportLoad, ReportLoad);
    default:
      GW_LOG_ERROR("dispatch fail, gapp_id:%lu, msg_id:%u", gapp_id, is.msg_id());
      break;
  }
}

GW_CELL_MGR_END