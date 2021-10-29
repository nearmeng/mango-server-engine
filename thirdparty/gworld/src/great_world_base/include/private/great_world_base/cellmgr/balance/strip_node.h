#pragma once
#include "great_world_base/cellmgr/balance/balance_node.h"
#include "great_world_base/cellmgr/balance/region_node.h"

GW_CELL_MGR_BEGIN

class CellNode;

class StripNode : public BalanceNode {
  GWCLASS_DECL(StripNode)
 public:
  StripNode();
  StripNode(RegionNode* parent);
  ~StripNode();
  bool Balance(bool tracing) override;
  bool AssignGameApps(ExcludeGameAppFilter& filter) override;
  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;
  bool ReadFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void VisitNodes(RegionNodeVisitor& visitor);
  CellNode* SearchMinLoadNode(uint16_t& idx) const;
  CellNode* SearchMaxLoadNodeWithMin(uint16_t& idx, CellNode** found_min_load_node) const;
  bool SearchLowLoadAdjacentNodes(float load, uint16_t& idx, CellNode** found_node, CellNode** found_next_node) const;
  CellNode* SearchLowLoadNodeToRemove(float load, uint16_t& idx) const;
  CellNode* SearchMinMemsizeNode(uint16_t& idx) const;
  CellNode* SearchMaxMemsizeNodeWithMin(uint16_t& idx, CellNode** found_min_men_node) const;
  CellNode* SearchMinTileAreaNode(uint16_t& idx) const;

  CellNode* AppendCellNode(GameAppEntry* gapp);
  CellNode* InsertCellNode(GameAppEntry* gapp, uint16_t index);
  CellNode* first_node() const;
  CellNode* last_node() const;
  void ResizeCellRect();
  bool RecalcLoad();
  // 是否有cell正在移除中
  bool IsSomeCellRemoving();
  bool CanDelete() const;

  uint16_t num_nodes() const { return static_cast<uint16_t>(nodes_.size()); }
  bool vertical() const { return vertical_; }
  void set_vertical(bool vertical) { vertical_ = vertical; }
  const GWVector<CellNode*>& nodes() const { return nodes_; }
  GWVector<CellNode*>& mutable_nodes() { return nodes_; }
  float total_conv_load() const { return total_conv_load_; }
  uint32_t total_memsize() const { return total_memsize_; }

  // BalanceNode begin
  float curr_load() const override;
  float revised_load() const override;
  float conv_load() const override;
  uint32_t memsize() const override;
  const Rect& node_rect() const override { return rect_; }
  Rect& mutable_node_rect() override { return rect_; }
  const Rect& entity_rect() const override { return entity_rect_; }
  Rect& mutable_entity_rect() override { return entity_rect_; }
  const Rect& tile_rect() const override { return tile_rect_; }
  Rect& mutable_tile_rect() override { return tile_rect_; }

  uint32_t num_entities(bool use_parallel = false, uint64_t gapp_id = 0) const override { return num_entities_; }
  uint32_t num_avatars(bool use_parallel = false, uint64_t gapp_id = 0) const override { return 0; }
  uint32_t num_reals(bool use_parallel = false, uint64_t gapp_id = 0) const override { return 0; }
  // BalanceNode end

  bool removing() const override;
  void set_removing(bool removing);
  bool removed() const { return removed_; }
  void set_removed(bool removed) { removed_ = removed; }

 private:
  GWVector<CellNode*> nodes_;
  float curr_load_ = 0.0f;
  float revised_load_ = 0.0f;
  float conv_load_ = 0.0f;
  uint32_t memsize_ = 0.0f;
  float total_curr_load_ = 0.0f;
  float total_revised_load_ = 0.0f;
  float total_conv_load_ = 0.0f;
  uint32_t total_memsize_ = 0.0f;
  uint32_t num_entities_ = 0;
  Rect entity_rect_;
  Rect tile_rect_;
  bool vertical_ = false;      // 内部的cell节点是否垂直排布
  bool balance_first_ = true;  // 是否从第一个节点开始
  bool removing_ = false;      // 是否在移除中
  bool removed_ = false;       // 是否执行移除
};

GW_CELL_MGR_END