#pragma once

#include "great_world_base/gamespace/space/game_app_entry.h"
#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class CellNode;
struct GameCellInfo {
  uint64_t gapp_id_ = 0;
  CellState state_;
  uint32_t num_avatars_ = 0;
  bool keep_ = true;

  GameCellInfo(CellState state) : state_(state) {
  }

  GameCellInfo(uint64_t gapp_id, CellState state) : gapp_id_(gapp_id), state_(state) {
  }

  bool Created() const {
    return state_ == CellState::kCreated;
  }
};

class CellNode : public RegionNode {
 public:
  GWCLASS_DECL(CellNode)

  CellNode(const Rect& rect, RegionNode* parent = nullptr);
  CellNode(RegionNode* parent = nullptr);
  ~CellNode();

  bool ReadFromStream(IMemoryIStream& in) override;
  bool UpdateFromStream(IMemoryIStream& in) override;
  void Destroy() override;

  void Accept(RegionNodeVisitor& visitor) override;
  bool fixed() const { return is_fixed; }
  void set_fixed(bool fixed) { is_fixed = fixed; }

  uint64_t main_gapp_id() const { return main_cell_.gapp_id_; }
  void set_main_gapp_id(uint64_t gapp_id) { main_cell_.gapp_id_ = gapp_id; }

  uint32_t num_avatars(uint64_t gapp_id) const;
  void set_num_avatars(uint32_t load, uint64_t gapp_id);
  CellState state(uint64_t gapp_id) const;
  void set_state(CellState state, uint64_t gapp_id);
  bool cell_keep(uint64_t gapp_id);
  void set_cell_keep(bool keep, uint64_t gapp_id);
  const GameCellInfo& main_cell() { return main_cell_; }

  GWHashMap<uint64_t, GameCellInfo>& parallel_cells() { return parallel_cells_; }
  GameCellInfo* FindCell(uint64_t);

  void OnMigrateIn(uint64_t entity_id);
  void OnMigrateDone(uint64_t entity_id);

  void OnAllCellCreated();

  uint64_t ChooseGameForMigrate();
  uint32_t upper_normal_avatar_num() { return upper_normal_avatar_num_; }
  bool parallel_visible() { return parallel_visible_; }
  uint64_t RandomCell();

  GWHashSet<uint64_t> AllGameIds();

 private:
  GameCellInfo main_cell_;
  GWHashSet<uint64_t> migratings_;  // 记录本game上往此Cell迁移的uint64_t。
  GWHashMap<uint64_t, GameCellInfo> parallel_cells_;
  uint32_t upper_normal_avatar_num_;
  bool is_fixed = true;
  bool parallel_visible_ = true;
};

GW_GAME_END