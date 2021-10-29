#pragma once

#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/list_node.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

namespace gw {
namespace game {

class RangeTrigger;
class ViewNode;

class RangeNode : public ListNode {
 public:
  RangeNode(uint64_t guid, uint32_t flag, uint32_t order, RangeTrigger* trigger, float range);
  virtual ~RangeNode();

  inline float Range() const {
    return m_range;
  }
  virtual void OnNodePassX(ListNode* node, const Vector3& old_pos, bool is_positive);
  virtual void OnNodePassZ(ListNode* node, const Vector3& old_pos, bool is_positive);

 private:
  RangeTrigger* m_trigger;
  float m_range;
};

class RangeTrigger {
 public:
  RangeTrigger(uint32_t range, ViewNode* node, AOIFUNC on_enter, AOIFUNC on_leave);
  ~RangeTrigger();

  void Install();
  void UnInstall();
  void UpdateBefore(bool is_x_inc, bool is_z_inc, const Vector3& old_pos);
  void Update(const Vector3& pos);
  void UpdateAfter(bool is_x_inc, bool is_z_inc, const Vector3& old_pos);

  void OnNodePassX(ListNode* node, const Vector3& old_pos, bool is_positive);
  void OnNodePassZ(ListNode* node, const Vector3& old_pos, bool is_positive);

  void OnEnter(ListNode* node);
  void OnLeave(ListNode* node);
  inline const ViewNode* View() const {
    return m_view_node;
  }
  inline uint32_t Range() const {
    return m_range;
  }

  bool IsInXRange(ListNode* node);
  bool WasInXRange(ListNode* node, const Vector3& old_pos);
  bool IsInZRange(ListNode* node);
  bool WasInZRange(ListNode* node, const Vector3& old_pos);

  inline bool operator<(const RangeTrigger& other) const {
    return m_range < other.Range();
  }

 private:
  bool m_is_install;
  uint32_t m_range;
  ViewNode* m_view_node;
  RangeNode* m_positive_node;
  RangeNode* m_negative_node;
  Vector3 m_old_pos;

  AOIFUNC on_enter_ = nullptr;
  AOIFUNC on_leave_ = nullptr;

};

}  // end namespace game
}  // end namespace gw
