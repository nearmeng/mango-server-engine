#pragma once

#include <list>

#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/list_node.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/range_trigger.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"

namespace gw {
namespace game {

class AoiImpList;

class ViewNode : public ListNode {
 public:
  typedef std::list<RangeTrigger*> TriggerList;
  typedef TriggerList::iterator TriggerListIter;
  typedef TriggerList::reverse_iterator TriggerListReverseIter;

  ViewNode(AoiImpList* imp, uint64_t guid);
  virtual ~ViewNode();
  virtual void Update(const Vector3& pos);
  void Leave();

  bool AddTrigger(uint32_t range, AOIFUNC on_enter, AOIFUNC on_leave);
  void RemoveTrigger(uint32_t range);
  void ClearTriggers();

 protected:
  void UpdatePos(const Vector3& pos);

 private:
  AoiImpList* m_imp_list;
  TriggerList m_trigger_list;
};

}  // end namespace game
}  // end namespace gw