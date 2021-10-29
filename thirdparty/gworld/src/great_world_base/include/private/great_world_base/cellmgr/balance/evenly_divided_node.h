#pragma once

#include "great_world_base/cellmgr/balance/region_node.h"

GW_CELL_MGR_BEGIN

class EvenlyDividedNode : public RegionNode {
  GWCLASS_DECL(EvenlyDividedNode)
 public:
  EvenlyDividedNode();
  EvenlyDividedNode(const Rect& rect, uint16_t rows, uint16_t cols, RegionNode* parent);
  ~EvenlyDividedNode();
  bool Balance(bool tracing) override;
  bool AssignGameApps(ExcludeGameAppFilter& filter) override;
  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;
  bool ReadFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void VisitNodes(RegionNodeVisitor& visitor);

  uint16_t rows() const { return rows_; }
  uint16_t cols() const { return cols_; }

 private:
  uint16_t rows_;
  uint16_t cols_;
  GWVector<GWVector<RegionNode*>> nodes_;
};

GW_CELL_MGR_END