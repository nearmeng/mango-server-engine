#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/range_trigger.h"

#include <cfloat>

#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/view_node.h"

namespace gw {
namespace game {

RangeNode::RangeNode(uint64_t guid, uint32_t flag, uint32_t order, RangeTrigger* trigger, float range) : ListNode(guid, flag, order), m_trigger(trigger), m_range(range) {
}

RangeNode::~RangeNode() {
}

void RangeNode::OnNodePassX(ListNode* node, const Vector3& old_pos, bool is_positive) {
  if (node == m_trigger->View() || 0 == (node->Flag() & NODE_FLAG_VIEW)) {
    return;
  }
  bool is_in_x = ((Flag() & NODE_FLAG_NEGATIVE) && is_positive) || ((Flag() & NODE_FLAG_POSITIVE) && (!is_positive));
  m_trigger->OnNodePassX(node, old_pos, is_in_x);
}

void RangeNode::OnNodePassZ(ListNode* node, const Vector3& old_pos, bool is_positive) {
  if (node == m_trigger->View() || 0 == (node->Flag() & NODE_FLAG_VIEW)) {
    return;
  }
  bool is_in_z = ((Flag() & NODE_FLAG_NEGATIVE) && is_positive) || ((Flag() & NODE_FLAG_POSITIVE) && (!is_positive));
  m_trigger->OnNodePassZ(node, old_pos, is_in_z);
}

RangeTrigger::RangeTrigger(uint32_t range, ViewNode* node, AOIFUNC on_enter, AOIFUNC on_leave) : m_is_install(false), m_range(range), m_view_node(node), m_positive_node(nullptr), m_negative_node(nullptr), on_enter_(on_enter), on_leave_(on_leave) {
}

RangeTrigger::~RangeTrigger() {
  if (nullptr != m_positive_node) {
    delete m_positive_node;
    m_positive_node = nullptr;
  }
  if (nullptr != m_negative_node) {
    delete m_negative_node;
    m_negative_node = nullptr;
  }
}

void RangeTrigger::Install() {
  uint64_t guid = m_view_node->Guid();
  const Vector3& pos = m_view_node->GetPos();
  m_positive_node = new RangeNode(guid, NODE_FLAG_POSITIVE, NODE_ORDER_POSITIVE, this, m_range);
  m_positive_node->SetPos(pos.x + m_range, pos.y + m_range, pos.z + m_range);
  m_negative_node = new RangeNode(guid, NODE_FLAG_NEGATIVE, NODE_ORDER_NEGATIVE, this, 0.0 - m_range);
  m_negative_node->SetPos(pos.x - m_range, pos.y - m_range, pos.z - m_range);

  m_view_node->InsertPreX(m_negative_node);
  m_view_node->InsertPreZ(m_negative_node);
  Vector3 max_pos(FLT_MAX, FLT_MAX, FLT_MAX);
  m_negative_node->Update(max_pos);

  m_view_node->InsertPreX(m_positive_node);
  m_view_node->InsertPreZ(m_positive_node);
  Vector3 min_pos(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  m_positive_node->Update(min_pos);
  m_is_install = true;
  m_old_pos = m_view_node->GetPos();
}

void RangeTrigger::UnInstall() {
  m_positive_node->SelfRemove();
  m_negative_node->SelfRemove();
  m_is_install = false;
}

void RangeTrigger::UpdateBefore(bool is_x_inc, bool is_z_inc, const Vector3& old_pos) {
  m_old_pos = old_pos;
  float neg_range = m_negative_node->Range();
  Vector3 neg_pos(old_pos.x + neg_range, old_pos.y + neg_range, old_pos.z + neg_range);
  float pos_range = m_positive_node->Range();
  Vector3 pos_pos(old_pos.x + pos_range, old_pos.y + pos_range, old_pos.z + pos_range);

  if (is_x_inc) {
    m_positive_node->MoveNodeX(pos_pos);
  } else {
    m_negative_node->MoveNodeX(neg_pos);
  }
  if (is_z_inc) {
    m_positive_node->MoveNodeZ(pos_pos);
  } else {
    m_negative_node->MoveNodeZ(neg_pos);
  }
}

void RangeTrigger::UpdateAfter(bool is_x_inc, bool is_z_inc, const Vector3& old_pos) {
  float neg_range = m_negative_node->Range();
  Vector3 neg_pos(old_pos.x + neg_range, old_pos.y + neg_range, old_pos.z + neg_range);
  float pos_range = m_positive_node->Range();
  Vector3 pos_pos(old_pos.x + pos_range, old_pos.y + pos_range, old_pos.z + pos_range);

  if (is_x_inc) {
    m_negative_node->MoveNodeX(pos_pos);
  } else {
    m_positive_node->MoveNodeX(neg_pos);
  }
  if (is_z_inc) {
    m_negative_node->MoveNodeZ(pos_pos);
  } else {
    m_positive_node->MoveNodeZ(neg_pos);
  }
  m_old_pos = m_view_node->GetPos();
}

void RangeTrigger::Update(const Vector3& pos) {
  // 原来位置保留判定WasInRange
  float neg_range = m_negative_node->Range();
  Vector3 neg_pos(pos.x + neg_range, pos.y + neg_range, pos.z + neg_range);
  m_negative_node->SetPos(neg_pos);

  float pos_range = m_positive_node->Range();
  Vector3 pos_pos(pos.x + pos_range, pos.y + pos_range, pos.z + pos_range);
  m_positive_node->SetPos(pos_pos);
}

void RangeTrigger::OnNodePassX(ListNode* node, const Vector3& old_pos, bool is_in_x) {
  bool was_in_z = this->WasInZRange(node, old_pos);
  if (!was_in_z) return;  // 不在z范围，交给passz判定后续
  // is_in_x并不是真的在x范围，只是通过移动node做到初步判定
  if (is_in_x) {
    // 还需要check 是否在x范围，防止穿透range
    bool is_in = this->IsInXRange(node) && this->IsInZRange(node);
    if (is_in) this->OnEnter(node);
  } else {
    bool was_in = this->WasInXRange(node, old_pos);
    if (was_in) this->OnLeave(node);
  }
}
void RangeTrigger::OnNodePassZ(ListNode* node, const Vector3& old_pos, bool is_in_z) {
  bool is_in_x = this->IsInXRange(node);
  if (!is_in_x) return;  // 当前不在x范围，则passX的时候已经触发离开逻辑

  // is_in_z并不是真的在z范围，只是通过移动node做到初步判定
  if (is_in_z) {
    // 还需要check 是否在z范围，防止穿透range
    bool is_in = this->IsInZRange(node);
    if (is_in) this->OnEnter(node);
  } else {
    bool was_in = this->WasInXRange(node, old_pos) && this->WasInZRange(node, old_pos);
    if (was_in) this->OnLeave(node);
  }
}

void RangeTrigger::OnEnter(ListNode* node) {
  on_enter_(m_view_node->Guid(), node->Guid(), node->m_pos);
}

void RangeTrigger::OnLeave(ListNode* node) {
  on_leave_(m_view_node->Guid(), node->Guid(), node->m_pos);
}

bool RangeTrigger::IsInXRange(ListNode* node) {
  float tx = node->GetX();
  return (tx >= m_negative_node->GetX()) && (tx <= m_positive_node->GetX());
}

bool RangeTrigger::WasInXRange(ListNode*, const Vector3& old_pos) {
  float node_x = old_pos.x;
  float neg_x = m_old_pos.x - m_range;
  float pos_x = m_old_pos.x + m_range;
  return m_is_install && (node_x >= neg_x) && (node_x <= pos_x);
  ;
}

bool RangeTrigger::IsInZRange(ListNode* node) {
  float tz = node->GetZ();
  return (tz >= m_negative_node->GetZ()) && (tz <= m_positive_node->GetZ());
}

bool RangeTrigger::WasInZRange(ListNode*, const Vector3& old_pos) {
  float node_z = old_pos.z;
  float neg_z = m_old_pos.z - m_range;
  float pos_z = m_old_pos.z + m_range;
  return m_is_install && (node_z >= neg_z) && (node_z <= pos_z);
  ;
}

}  // end namespace game
}  // end namespace gw
