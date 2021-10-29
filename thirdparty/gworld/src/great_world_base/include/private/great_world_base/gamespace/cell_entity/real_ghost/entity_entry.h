#pragma once
#include <cassert>
#include <functional>

#include "great_world_base/common/vector3.h"
#include "great_world_base/gamespace/cell_entity/real_ghost/ghost_entry.h"

GW_GAME_BEGIN

class CellEntity;

// RealEntry只是entity的一部分（组合），包含了real所需要维护的信息。
class RealEntry {
  GWCLASS_DECL(RealEntry)
 public:
  RealEntry(CellEntity* entity) : cell_entity_(entity) {}
  virtual ~RealEntry() = default;

  GhostEntries& ghosts() { return ghosts_; }

  void Migrate();
  void DeleteGhost(const GhostEntry& ghost_entry);
  void CreateGhost(const GhostEntry& ghost_entry);
  void DeleteGhostsOnLeave();

  bool ParallelIsAlwaysVisible() { return parallel_is_always_visible_; }
  void SetParallelVisible(bool always_visible) { parallel_is_always_visible_ = always_visible; }

  const CellEntity* cell_entity() { return cell_entity_; }

 protected:
  CellEntity* cell_entity_;
  bool parallel_is_always_visible_ = false;  // 分线和邻近cell都必定创建ghost
  GhostEntries ghosts_;
};

// EntityEntry只是entity的一部分，包含了ghost所需要维护的信息。
// 其生命周期由具体的entity管理
class EntityEntry {
  GWCLASS_DECL(EntityEntry)
 public:
  EntityEntry(CellEntity* cell_entity) : cell_entity_(cell_entity), real_entry_(nullptr), ghost_range_(0.0) {}

  virtual ~EntityEntry();
  bool is_real() const { return real_entry_ != nullptr; }
  bool is_ghost() const { return !real_entry_; }

  uint64_t real_gapp_id() const { return real_gapp_id_; }
  void set_real_gapp_id(uint64_t gapp_id) { real_gapp_id_ = gapp_id; }
  uint64_t dst_gapp_id() const { return dst_gapp_id_; }
  void set_dst_gapp_id(uint64_t gapp_id) { dst_gapp_id_ = gapp_id; }
  uint64_t parallel_dst_gapp_id() const { return parallel_dst_gapp_id_; }
  void set_parallel_dst_gapp_id(uint64_t gapp_id) { parallel_dst_gapp_id_ = gapp_id; }

  RealEntry* real_entry() { return real_entry_; }
  void set_real_entry(RealEntry* real_entry) { real_entry_ = real_entry; }

  float ghost_range() const { return ghost_range_; }
  void set_ghost_range(float range) { ghost_range_ = range; }

  void PrepareDestroy();
  void OnCellDestroy();

  void AddReal();
  void RemoveReal();

  bool ConvertToGhost();
  bool ConvertToReal();

  bool UpdateGhostEntries(const std::vector<uint64_t>& entries);

  const CellEntity* cell_entity() { return cell_entity_; }

 protected:
  CellEntity* cell_entity_;
  RealEntry* real_entry_ = nullptr;     // 如果这个entity是real，那么real_entry_就有值，否则为空（即ghost entity）
  uint64_t real_gapp_id_ = 0;          // 当前real所在的game app id
  uint64_t dst_gapp_id_ = 0;           // 目标game app id（迁移中才有这个值
  uint64_t parallel_dst_gapp_id_ = 0;  // 准备迁往的平行 cell
  float ghost_range_;
};

GW_GAME_END