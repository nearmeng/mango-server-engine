#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/list_node.h"

#include <math.h>

#include <limits>
#include <sstream>

namespace gw {
namespace game {

static inline bool float_equal(float x, float y) {
  //return !(x > y || x < y);
  return fabs(x - y) <= std::numeric_limits<float>::epsilon();
}

ListNode::ListNode(uint64_t guid, uint32_t flag, uint32_t order) : m_guid(guid), m_flag(flag), m_order(order), m_pre_x(nullptr), m_next_x(nullptr), m_pre_z(nullptr), m_next_z(nullptr) {
}

ListNode::~ListNode() {
}

void ListNode::SelfRemove() {
  // remove from x
  if (nullptr != m_pre_x) m_pre_x->m_next_x = m_next_x;
  if (nullptr != m_next_x) m_next_x->m_pre_x = m_pre_x;
  // remove from z
  if (nullptr != m_pre_z) m_pre_z->m_next_z = m_next_z;
  if (nullptr != m_next_z) m_next_z->m_pre_z = m_pre_z;
  // set null
  m_pre_x = m_next_x = m_pre_z = m_next_z = nullptr;
}

void ListNode::Update(const Vector3& old_pos) {
  MoveNodeXZ(old_pos);
}

void ListNode::MoveNodeXZ(const Vector3& old_pos) {
  MoveNodeX(old_pos);
  MoveNodeZ(old_pos);
}

void ListNode::MoveNodeX(const Vector3& old_pos) {
  float x = GetX();
  uint32_t order = Order();
  ListNode* pre = m_pre_x;
  //try to move left/ negative
  while (nullptr != pre && (pre->GetX() > x || (float_equal(pre->GetX(), x) && pre->Order() >= order))) {
    this->OnNodePassX(pre, pre->GetPos(), true);
    pre->OnNodePassX(this, old_pos, false);

    if (nullptr != this->m_next_x) this->m_next_x->m_pre_x = pre;
    pre->m_next_x = this->m_next_x;

    this->m_next_x = pre;
    this->m_pre_x = pre->m_pre_x;

    pre->m_pre_x = this;
    pre = this->m_pre_x;
    if (nullptr != pre) pre->m_next_x = this;
  }
  // try to move right / positive
  ListNode* next = this->m_next_x;
  while (nullptr != next && (next->GetX() < x || (float_equal(next->GetX(), x) && next->Order() <= order))) {
    this->OnNodePassX(next, next->GetPos(), false);
    next->OnNodePassX(this, old_pos, true);

    if (nullptr != this->m_pre_x) this->m_pre_x->m_next_x = next;
    next->m_pre_x = this->m_pre_x;

    this->m_pre_x = next;
    this->m_next_x = next->m_next_x;

    next->m_next_x = this;
    next = this->m_next_x;
    if (nullptr != next) next->m_pre_x = this;
  }
}

void ListNode::MoveNodeZ(const Vector3& old_pos) {
  float z = GetZ();
  uint32_t order = Order();
  ListNode* pre = m_pre_z;
  //try to move left/ negative
  while (nullptr != pre && (pre->GetZ() > z || (float_equal(pre->GetZ(), z) && pre->Order() >= order))) {
    this->OnNodePassZ(pre, pre->GetPos(), true);
    pre->OnNodePassZ(this, old_pos, false);

    // move this
    if (nullptr != this->m_next_z) this->m_next_z->m_pre_z = pre;
    pre->m_next_z = this->m_next_z;

    this->m_next_z = pre;
    this->m_pre_z = pre->m_pre_z;

    pre->m_pre_z = this;
    pre = this->m_pre_z;
    if (nullptr != pre) pre->m_next_z = this;
  }
  // try to move right / positive
  ListNode* next = this->m_next_z;
  while (nullptr != next && (next->GetZ() < z || (float_equal(next->GetZ(), z) && next->Order() <= order))) {
    this->OnNodePassZ(next, next->GetPos(), false);
    next->OnNodePassZ(this, old_pos, true);

    if (nullptr != this->m_pre_z) this->m_pre_z->m_next_z = next;
    next->m_pre_z = this->m_pre_z;

    this->m_pre_z = next;
    this->m_next_z = next->m_next_z;

    next->m_next_z = this;
    next = this->m_next_z;
    if (nullptr != next) next->m_pre_z = this;
  }
}

void ListNode::InsertPreX(ListNode* node) {
  if (this == node) {
    return;
  }
  if (nullptr != m_pre_x) m_pre_x->m_next_x = node;
  node->m_pre_x = m_pre_x;
  node->m_next_x = this;
  this->m_pre_x = node;
}

void ListNode::InsertPreZ(ListNode* node) {
  if (this == node) {
    return;
  }
  if (nullptr != m_pre_z) m_pre_z->m_next_z = node;
  node->m_pre_z = m_pre_z;
  node->m_next_z = this;
  this->m_pre_z = node;
}

GWString ListNode::DebugString() {
  std::stringstream str;
  str << Guid() << "-> flag:" << Flag() << " order:" << Order() << " x:" << GetX() << " z:" << GetZ() << " y:" << m_pos.y;
  return GWString(str.str().c_str(), str.str().size());
}

}  // end namespace game
}  // end namespace gw
