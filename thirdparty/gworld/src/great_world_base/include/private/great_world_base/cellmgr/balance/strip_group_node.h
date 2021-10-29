#pragma once
#include "great_world_base/cellmgr/balance/region_node.h"

GW_CELL_MGR_BEGIN

class CellNode;
class StripNode;

//
// vertical为true的时候的布局
// ---------------
// |   |     |   | // StripNode
// ---------------
// |   |     |   | // StripNode
// ---------------
class StripGroupNode : public RegionNode {
  GWCLASS_DECL(StripGroupNode)
 public:
  typedef GWVector<StripNode*> StripNodes;
  StripGroupNode();
  ~StripGroupNode();
  bool Balance(bool tracing) override;
  bool CellNumBalance(ExcludeGameAppFilter& filter, bool tracing);
  bool StripNumBalance(ExcludeGameAppFilter& filter, bool tracing);
  bool GAppBalance(ExcludeGameAppFilter& filter, bool tracing) override;
  bool AssignGameApps(ExcludeGameAppFilter& filter) override;
  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;
  bool ReadFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  void VisitNodes(RegionNodeVisitor& visitor);
  void RecalcLoad();
  uint16_t GetNumCells() const;
  StripNode* SearchMinLoadNode(uint16_t& idx) const;
  StripNode* SearchMaxLoadNodeWithMin(uint16_t& idx, StripNode** found_min_load_node) const;
  StripNode* SearchMinMemsizeNode(uint16_t& idx) const;
  StripNode* SearchMaxMemsizeNodeWithMin(uint16_t& idx, StripNode** found_min_men_node) const;
  StripNode* SearchMinTileAreaNode(uint16_t& idx) const;
  StripNode* SearchLowLoadNodeToRemove(float load, uint16_t& idx) const;
  bool IsSomeStripRemoving() const;

  uint16_t num_strips() const { return static_cast<uint16_t>(nodes_.size()); }
  uint16_t min_strips() const { return min_strips_; }
  void set_min_strips(uint16_t min_strips) { min_strips_ = min_strips; }
  uint16_t max_strips() const { return max_strips_; }
  void set_max_strips(uint16_t max_strips) { max_strips_ = max_strips; }
  uint16_t min_cells_per_strip() const { return min_cells_per_strip_; }
  void set_min_cells_per_strip(uint16_t min_cells_per_strip) {
    min_cells_per_strip_ = min_cells_per_strip;
  }
  uint16_t max_cells_per_strip() const { return max_cells_per_strip_; }
  void set_max_cells_per_strip(uint16_t max_cells_per_strip) {
    max_cells_per_strip_ = max_cells_per_strip;
  }
  const StripNodes& nodes() const { return nodes_; }
  StripNodes& mutable_nodes() { return nodes_; }
  bool dynamic() const { return dynamic_; }
  void set_dynamic(bool dynamic) { dynamic_ = dynamic; }
  bool vertical() const { return vertical_; }
  void set_vertical(bool vertical);

 protected:
  StripNode* AppendStripNode(GameAppEntry* gapp);
  StripNode* InsertStripNode(GameAppEntry* gapp, uint16_t index);
  bool CheckToAddCell(ExcludeGameAppFilter& filter, bool tracing);
  bool CheckToRemoveCell();
  bool CheckToAddStrip(ExcludeGameAppFilter& filter, bool tracing);
  bool CheckToRemoveStrip();

 private:
  StripNodes nodes_;
  bool dynamic_ = false;
  uint16_t min_strips_ = 1;           // 带状区域数下限
  uint16_t max_strips_ = 1;           // 带状区域数上限
  uint16_t min_cells_per_strip_ = 1;  // 每个带状区域内cell数目下限
  uint16_t max_cells_per_strip_ = 1;  // 每个带状区域内cell数目上限
  bool vertical_ = true;                  // StripNode是否按纵向排列
  bool balance_first_ = true;             // 是否从第一个节点开始

  float cell_conv_load_ = 0.0f;   // cell平均的负载
  uint32_t cell_memsize_ = 0;     // cell平均的内存占用
  float strip_conv_load_ = 0.0f;  // strip平均的负载
  uint32_t strip_memsize_ = 0;    // strip平均的内存占用
  float total_conv_load_ = 0.0f;  // 总负载
  uint32_t total_memsize_ = 0;    // 总内存
};

GW_CELL_MGR_END