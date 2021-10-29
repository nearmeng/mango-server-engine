#pragma once

#include "great_world_base/gamespace/space/fixed_divided_node.h"
#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class FixedDividedByCellNode : public FixedDividedNode {
 public:
  GWCLASS_DECL(FixedDividedByCellNode)

  FixedDividedByCellNode() = default;
  bool use_parallel() override { return true; }
  bool UpdateFromStream(IMemoryIStream& in) override;
};

GW_GAME_END
