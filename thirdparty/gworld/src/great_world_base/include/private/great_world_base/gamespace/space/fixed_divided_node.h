#pragma once

#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class FixedDividedNode : public RegionNode {
 public:
  GWCLASS_DECL(FixedDividedNode)
  FixedDividedNode() = default;
  bool ReadFromStream(IMemoryIStream& in) override;
  bool UpdateFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void Destroy() override;

  void VisitNodes(RegionNodeVisitor& visitor);

 protected:
  GWVector<RegionNode*> nodes_;
};

GW_GAME_END
