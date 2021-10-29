#pragma once

#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class CellNode;

class StripNode : public RegionNode {
  GWCLASS_DECL(StripNode)
 public:
  StripNode();
  StripNode(RegionNode* parent);
  StripNode(const StripNode&) = delete;
  ~StripNode();
  bool ReadFromStream(IMemoryIStream& in) override;
  bool UpdateFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  virtual void Destroy() override;
  void VisitNodes(RegionNodeVisitor& visitor);

  uint16_t num_nodes() const { return static_cast<uint16_t>(nodes_.size()); }
  bool vertical() const { return vertical_; }
  void set_vertical(bool vertical) { vertical_ = vertical; }
  const GWVector<CellNode*> nodes() const { return nodes_; }

 private:
  GWVector<CellNode*> nodes_;
  bool vertical_ = false;  // 内部的cell节点是否垂直排布
};

GW_GAME_END