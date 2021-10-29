#pragma once

#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class EvenlyDividedNode : public RegionNode {
 public:
  GWCLASS_DECL(EvenlyDividedNode)

  EvenlyDividedNode();
  EvenlyDividedNode(const EvenlyDividedNode&) = delete;
  ~EvenlyDividedNode();
  bool ReadFromStream(IMemoryIStream& in) override;
  bool UpdateFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void Destroy() override;
  void VisitNodes(RegionNodeVisitor& visitor);

  uint16_t rows() const { return rows_; }
  uint16_t cols() const { return cols_; }

 private:
  uint16_t rows_ = 0;
  uint16_t cols_ = 0;
  GWVector<GWVector<RegionNode*>> nodes_;
};

GW_GAME_END