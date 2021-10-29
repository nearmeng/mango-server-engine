#pragma once

#include <functional>
#include <unordered_map>

#include "great_world_base/cellmgr/balance/region_node.h"

GW_CELL_MGR_BEGIN

class FixedDividedNode : public RegionNode {
  GWCLASS_DECL(FixedDividedNode)
 protected:
  GWVector<RegionNode*> nodes_;
  // key是node的minx数值；value是一个vector，里面存的是minx值是key的node集合，集合内的数据按照y值从小到大排序
  GWHashMap<float, GWVector<RegionNode*>> minx_to_nodes_;

  void Clear();

 public:
  bool Balance(bool tracing) override;
  bool GAppBalance(ExcludeGameAppFilter& filter, bool tracing) override;
  bool AssignGameApps(ExcludeGameAppFilter& filter) override;
  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;
  bool ReadFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  GameAppEntry* FindCell(float x, float y) override;

  void VisitNodes(RegionNodeVisitor& visitor);

  bool CheckLayout(GWVector<RegionNode*> node);

  GWVector<RegionNode*> GetNodes() { return nodes_; }

  FixedDividedNode();
  ~FixedDividedNode();
};

GW_CELL_MGR_END