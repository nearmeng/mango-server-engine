#pragma once
#include <memory>
#include <string>

#include "great_world_base/common/vector3.h"

namespace gw {
namespace game {

enum NodeFlag {
  NODE_FLAG_NONE = 0,
  NODE_FLAG_HEAD = 1,
  NODE_FLAG_TAIL = 2,
  NODE_FLAG_VIEW = 4,
  NODE_FLAG_NEGATIVE = 8,
  NODE_FLAG_POSITIVE = 16
};

// 用于节点的排序优先级，否则容易出现pass比较的不一致
enum NodeOrder {
  NODE_ORDER_HEAD = 0,
  NODE_ORDER_NEGATIVE = 1,
  NODE_ORDER_VIEW = 2,
  NODE_ORDER_POSITIVE = 3,
  NODE_ORDER_TAIL = 4
};

class ListNode {
 public:
  ListNode(uint64_t guid, uint32_t flag, uint32_t order);
  virtual ~ListNode();

  virtual void Update(const Vector3& pos);
  virtual void SelfRemove();
  virtual void OnNodePassX(ListNode*, const Vector3&, bool) {}
  virtual void OnNodePassZ(ListNode*, const Vector3&, bool) {}

  void MoveNodeXZ(const Vector3& old_pos);
  void MoveNodeX(const Vector3& old_pos);
  void MoveNodeZ(const Vector3& old_pos);

  void InsertPreX(ListNode* node);
  void InsertPreZ(ListNode* node);

  inline float GetX() const {
    return m_pos.x;
  }
  inline float GetZ() const {
    return m_pos.z;
  }
  inline void SetPos(const Vector3& pos) {
    m_pos = pos;
  }
  inline void SetPos(float x, float y, float z) {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
  }
  inline const Vector3& GetPos() const {
    return m_pos;
  }

  inline uint32_t Flag() const {
    return m_flag;
  }
  inline uint32_t Order() const {
    return m_order;
  }
  inline void Flag(uint32_t flag) {
    m_flag = m_flag | flag;
  }
  inline uint64_t Guid() const {
    return m_guid;
  }

  GWString DebugString();

 public:
  uint64_t m_guid;
  uint32_t m_flag;
  uint32_t m_order;
  uint64_t m_tag;
  ListNode* m_pre_x;
  ListNode* m_next_x;
  ListNode* m_pre_z;
  ListNode* m_next_z;
  Vector3 m_pos;
};

}  // end namespace game
}  // end namespace gw
