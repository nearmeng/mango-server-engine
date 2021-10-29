#pragma once

#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class StripNode;

class StripGroupNode : public RegionNode {
  GWCLASS_DECL(StripGroupNode)
 public:
  StripGroupNode();
  StripGroupNode(const StripGroupNode&) = delete;
  ~StripGroupNode();
  bool ReadFromStream(IMemoryIStream& in) override;
  bool UpdateFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void Destroy() override;

  void VisitNodes(RegionNodeVisitor& visitor);

  uint16_t num_strips() const { return static_cast<uint16_t>(nodes_.size()); }
  bool vertical() const { return vertical_; }
  void set_vertical(bool vertical) { vertical_ = vertical; }
  const GWVector<StripNode*>& nodes() const { return nodes_; }

 private:
  GWVector<StripNode*> nodes_;
  bool vertical_ = true;  // StripNode是否按纵向排列
};

GW_GAME_END