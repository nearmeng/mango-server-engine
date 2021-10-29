#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/view_node.h"

#include <cfloat>

#include "great_world_base/common/common_private.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/aoi_imp_list.h"

namespace gw {
namespace game {

ViewNode::ViewNode(AoiImpList* imp, uint64_t guid) : ListNode(guid, NODE_FLAG_VIEW, NODE_ORDER_VIEW), m_imp_list(imp) {
}

ViewNode::~ViewNode() {
  ClearTriggers();
}

void ViewNode::UpdatePos(const Vector3& pos) {
  this->SetPos(pos);
  for (TriggerListIter iter = m_trigger_list.begin(); iter != m_trigger_list.end(); ++iter) {
    (*iter)->Update(pos);
  }
}

void ViewNode::Update(const Vector3& pos) {
  Vector3 old_pos = GetPos();
  this->UpdatePos(pos);
  //SetPos(pos);
  bool is_x_inc = pos.x > old_pos.x;
  bool is_z_inc = pos.z > old_pos.z;
  for (TriggerListReverseIter iter = m_trigger_list.rbegin(); iter != m_trigger_list.rend(); ++iter) {
    (*iter)->UpdateBefore(is_x_inc, is_z_inc, old_pos);
  }
  ListNode::Update(old_pos);
  for (TriggerListReverseIter iter = m_trigger_list.rbegin(); iter != m_trigger_list.rend(); ++iter) {
    (*iter)->UpdateAfter(is_x_inc, is_z_inc, old_pos);
  }
}

bool ViewNode::AddTrigger(uint32_t range, AOIFUNC on_enter, AOIFUNC on_leave) {
  TriggerListIter insert_iter = m_trigger_list.end();
  for (TriggerListIter iter = m_trigger_list.begin(); iter != m_trigger_list.end(); ++iter) {
    RangeTrigger* iter_trigger = *iter;
    if (iter_trigger->Range() <= range) {
      insert_iter = iter;
    }
  }
  RangeTrigger* trigger = new RangeTrigger(range, this, on_enter, on_leave);
  if (nullptr == trigger) {
    GW_LOG_ERROR("View id: %lu, add range %lu failed", m_guid, range);
    return false;
  }
  m_trigger_list.insert(insert_iter, trigger);
  trigger->Install();
  return true;
}

void ViewNode::RemoveTrigger(uint32_t range) {
  for (TriggerListIter iter = m_trigger_list.begin(); iter != m_trigger_list.end(); ++iter) {
    RangeTrigger* trigger = *iter;
    if (trigger->Range() == range) {
      trigger->UnInstall();
      delete trigger;
      m_trigger_list.erase(iter);
      break;
    }
  }
}

void ViewNode::ClearTriggers() {
  for (TriggerListIter iter = m_trigger_list.begin(); iter != m_trigger_list.end(); ++iter) {
    RangeTrigger* trigger = *iter;
    trigger->UnInstall();
    delete trigger;
  }
  m_trigger_list.clear();
}

void ViewNode::Leave() {
  // TODO: 确认Leave时Leave对象也进行回调?
#if 0
//    Vector3 old_pos = GetPos();
//    Vector3 leave_pos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
//    SetPos(leave_pos);
//    ListNode::Update(old_pos);
#else
  Vector3 leave_pos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
  Update(leave_pos);
#endif

  ClearTriggers();
  SelfRemove();
}

}  // end namespace game
}  // end namespace gw
