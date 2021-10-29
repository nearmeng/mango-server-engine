#pragma once
#include "great_world_base/cellmgr/balance/balance_node.h"
#include "great_world_base/cellmgr/balance/region_node.h"

GW_CELL_MGR_BEGIN

struct MgrCellInfo {
  GameAppEntry* gapp_ = nullptr;
  uint32_t num_entities_ = 0;  // 所有类型的entity的数量
  uint32_t num_reals_ = 0;     // real的数量
  uint32_t num_avatars_ = 0;   // 玩家对象的数量
  CellState state_ = CellState::kInit;
  MgrCellInfo(GameAppEntry* gapp) {
    gapp_ = gapp;
  }
  MgrCellInfo() {}
};

class CellNode : public BalanceNode {
  GWCLASS_DECL(CellNode)
 public:
  CellNode();
  CellNode(const Rect& rect, RegionNode* parent = nullptr);
  ~CellNode();

  bool AssignGameApps(ExcludeGameAppFilter& filter) override;
  bool WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) override;
  bool ReadFromStream(IMemoryIStream& in) override;
  void Accept(RegionNodeVisitor& visitor) override;
  GameAppEntry* FindCell(float x, float y) override;
  std::shared_ptr<std::unordered_map<uint64_t, GameAppEntry*>> FindAllCell(float x, float y);

  GameAppEntry* main_gapp() const { return main_cell_.gapp_; }
  void set_main_gapp(GameAppEntry* gapp) { main_cell_.gapp_ = gapp; }

  CellState main_state() const { return main_cell_.state_; }
  bool set_main_state(CellState state);
  CellState state(uint64_t gapp_id) const;
  bool set_state(CellState state, uint64_t gapp_id);

  uint32_t num_entities(bool use_parallel = false, uint64_t gapp_id = 0) const override;
  void set_num_entities(uint32_t num, uint64_t gapp_id);

  uint32_t num_reals(bool use_parallel = false, uint64_t gapp_id = 0) const override;
  void set_num_reals(uint32_t num, uint64_t gapp_id);

  uint32_t num_avatars(bool use_parallel = false, uint64_t gapp_id = 0) const override;
  void set_num_avatars(uint32_t num, uint64_t gapp_id);

  bool available() const;
  bool CanDelete() const;
  bool created() const;

  // BalanceNode begin
  float curr_load() const override;
  float conv_load() const override;
  uint32_t memsize() const override;
  float revised_load() const override;
  const Rect& node_rect() const override { return rect_; }
  Rect& mutable_node_rect() override { return rect_; }
  const Rect& entity_rect() const override { return entity_rect_; }
  Rect& mutable_entity_rect() override { return entity_rect_; }
  const Rect& tile_rect() const override { return tile_rect_; }
  Rect& mutable_tile_rect() override { return tile_rect_; }
  bool removing() const override;
  // BalanceNode end

  bool have_parallel_cell() { return !parallel_cells_.empty(); }
  const GWHashMap<uint64_t, MgrCellInfo>& parallel_gapps() const { return parallel_cells_; }
  GWHashMap<uint64_t, MgrCellInfo>& mutable_parallel_gapps() { return parallel_cells_; }
  bool AddCell(GameAppEntry* gapp);
  bool RemoveCell(uint64_t gapp_id);
  void CheckAddOrRemoveCell(bool& need_add, bool& need_remove, uint64_t& remove_gapp_id, bool& visible_changed);

  uint64_t ChooseFreeGame();

  std::vector<uint64_t> CreatedParallelCells();

  uint16_t min_parallel_num() { return min_parallel_num_; }
  void set_min_parallel_num(uint16_t num) { min_parallel_num_ = num; }

  uint32_t UpperAvatarNumPerCell();

  bool parallel_visible() { return parallel_visible_; }

 private:
  // 主世界
  MgrCellInfo main_cell_;
  // 分线 gapp_id -> MgrCellInfo
  GWHashMap<uint64_t, MgrCellInfo> parallel_cells_;
  // 分线之间是否是可见的（隔离还是互通）
  bool parallel_visible_ = true;

  unsigned long auto_change_cell_num_ts_ = 0;
  unsigned long force_change_cell_num_ts_ = 0;

  uint16_t min_parallel_num_ = 0;

  Rect entity_rect_;
  Rect tile_rect_;
};

GW_CELL_MGR_END