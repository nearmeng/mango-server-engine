#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/aoi_imp_list.h"

#include <algorithm>
#include <cfloat>

#include "great_world_base/common/common_private.h"

namespace gw {
namespace game {

AoiImpList::AoiImpList()
    : m_head(-1, NODE_FLAG_HEAD, NODE_ORDER_HEAD), m_tail(-1, NODE_FLAG_TAIL, NODE_ORDER_TAIL) {
  Vector3 pos_head(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  m_head.SetPos(pos_head);
  m_head.m_flag = NODE_FLAG_HEAD;

  Vector3 pos_tail(FLT_MAX, FLT_MAX, FLT_MAX);
  m_tail.SetPos(pos_tail);
  m_tail.m_flag = NODE_FLAG_TAIL;

  m_tail.InsertPreX(&m_head);
  m_tail.InsertPreZ(&m_head);
}

AoiImpList::~AoiImpList() {
  for (auto iter = m_views.begin(); iter != m_views.end(); ++iter) {
    ViewNode* node = iter->second;
    node->SelfRemove();
    delete node;
  }
  m_views.clear();
}

bool AoiImpList::Enter(uint64_t guid, const Vector3& pos) {
  auto iter = m_views.find(guid);
  if (iter != m_views.end()) {
    return false;
  }
  ViewNode* node = new ViewNode(this, guid);
  node->SetPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
  m_tail.InsertPreX(node);
  m_tail.InsertPreZ(node);
  m_views[guid] = node;
  node->Update(pos);
  //this->DebugOutput();
  return true;
}

void AoiImpList::Leave(uint64_t guid) {
  auto iter = m_views.find(guid);
  if (iter == m_views.end()) {
    return;
  }
  ViewNode* node = iter->second;
  node->Leave();
  m_views.erase(iter);
  delete node;
}

void AoiImpList::Move(uint64_t guid, const Vector3& pos) {
  ViewNode* node = FindView(guid);
  if (nullptr == node) {
    return;
  }
  node->Update(pos);
}

bool AoiImpList::AddTrigger(uint64_t guid, uint32_t range, AOIFUNC on_enter, AOIFUNC on_leave) {
  ViewNode* view = FindView(guid);
  if (nullptr == view) {
    return false;
  }
  return view->AddTrigger(range, on_enter, on_leave);
}

void AoiImpList::RemoveTrigger(uint64_t guid, uint32_t range) {
  ViewNode* view = FindView(guid);
  if (nullptr == view) {
    return;
  }
  view->RemoveTrigger(range);
}

ViewNode* AoiImpList::FindView(uint64_t guid) {
  auto iter = m_views.find(guid);
  if (iter == m_views.end()) {
    return nullptr;
  }
  return iter->second;
}

void AoiImpList::DebugOutput() {
  GW_LOG_DEBUG("X-List: ");
  for (ListNode* node = m_head.m_next_x; node != nullptr; node = node->m_next_x) {
    GW_LOG_DEBUG("%s", node->DebugString().c_str());
  }
  GW_LOG_DEBUG("Z-List:");
  for (ListNode* node = m_head.m_next_z; node != nullptr; node = node->m_next_z) {
    GW_LOG_DEBUG("%s", node->DebugString().c_str());
  }
}

}  // end namespace game
}  // end namespace gw
