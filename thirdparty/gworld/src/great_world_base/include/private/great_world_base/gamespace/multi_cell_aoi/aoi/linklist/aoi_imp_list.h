#pragma once

#include <unordered_map>
#include <unordered_set>

#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_imp.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/list_node.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/linklist/view_node.h"

namespace gw {
namespace game {

class AoiImpList : public AoiImp {
 public:
  AoiImpList();
  virtual ~AoiImpList();

  bool Enter(uint64_t guid, const Vector3& pos) override;
  void Leave(uint64_t guid) override;
  void Move(uint64_t guid, const Vector3& pos) override;
  bool AddTrigger(uint64_t guid, uint32_t range, AOIFUNC on_enter, AOIFUNC on_leave) override;
  void RemoveTrigger(uint64_t guid, uint32_t range) override;
  void DebugOutput() override;
  
 protected:
  ViewNode* FindView(uint64_t guid);

 private:
  ListNode m_head;
  ListNode m_tail;
  std::unordered_map<uint64_t, ViewNode*> m_views;
};

}  // end namespace game
}  // end namespace gw
