#pragma once

#include <functional>
#include <unordered_map>

#include "great_world_base/cellmgr/balance/region_node.h"
#include "fixed_divided_node.h"

GW_CELL_MGR_BEGIN

class FixedDividedByCellNode : public FixedDividedNode {
  GWCLASS_DECL(FixedDividedByCellNode)
 private:
 public:
  FixedDividedByCellNode();

  bool ReadFromStream(IMemoryIStream& in) override;

  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;

  bool UpdateFromStream(IMemoryIStream& in) override;

  bool GAppBalance(ExcludeGameAppFilter& filter, bool tracing) override;
};

GW_CELL_MGR_END