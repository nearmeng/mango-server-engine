#pragma once
#include <queue>

#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN

class Space;
class ParallelBalance;

struct AvatarRecord {
  GWQueue<uint32_t> record_;
  uint32_t sum_ = 0;
  uint64_t record_ts_ = 0;
  static uint32_t max_record_num_;
  static uint32_t record_interval_;

  void Add(uint32_t num) {
    if(record_ts_ + record_interval_ > GetWalltimeMs()) {
      return;
    }
    record_ts_ = GetWalltimeMs();
    record_.push(num);
    sum_ += num;
    if (record_.size() > max_record_num_) {
      sum_ -= record_.front();
      record_.pop();
    }
  }

  uint32_t average() const {
    if (record_.empty()) {
      return 0;
    } else {
      return sum_ / record_.size();
    }
  }

  // 预测接下来一段时间的record数据
  // 主要用来做负载预测，要悲观一点
  uint32_t predicte() const {
    uint32_t r_average = average();
    if(r_average >= record_.front()) {
      return record_.front();
    }
    else {
      return record_.front() - average() + record_.front();
    }
  }
};

struct ParallelMigrateLock {
  uint32_t lock_num_;
  unsigned long end_ts_;
};

class Cell {
 public:
  GWCLASS_DECL(Cell)
  Cell() = default;
  typedef GWHashMap<uint64_t, RealEntry*> RealEntries;
  typedef GWHashMap<uint64_t, EntityEntry*> EntityEntries;
  Cell(Space* space, CellNode* node);
  ~Cell();
  void PrepareDestroy();
  void OnDestroy();
  CellNode* node() const;
  const Rect& entity_rect() const { return entity_rect_; }
  const Rect& tile_rect() const { return tile_rect_; }
  Rect& mutable_tile_rect() { return tile_rect_; }
  Space* space() { return space_; }
  void AddEntityEntry(uint64_t id, EntityEntry* entity);
  void DelEntityEntry(uint64_t id);
  uint32_t CheckMigration();

  uint32_t MigrationOldLayout();

  void CalcEntityRect();

  uint32_t RegisterGhostToGapp(uint64_t eid, uint64_t gapp_id);
  uint32_t UnRegisterGhostToGapp(uint64_t eid, uint64_t gapp_id);
  bool ForceUnRegisterGhostToGapp(uint64_t eid, uint64_t gapp_id);
  bool MigrateTo(uint64_t eid, uint64_t gapp_id);

  const AvatarRecord& migration_record() { return migration_record_; }
  const AvatarRecord& avatar_num_record() { return avatar_num_record_; }

  inline ParallelBalance* parallel_balance() { return balance_; }

 private:
  void Balance(GWVector<uint64_t>& inner_real_entry);

  Space* space_;
  CellNode* node_;
  Rect entity_rect_;
  Rect real_entity_rect_;
  Rect tile_rect_;
  RealEntries real_entries_;      // 管理cell上的所有real entity
  EntityEntries entity_entries_;  // 管理cell上的所有entity （包含了real、ghost）
  ParallelBalance* balance_ = nullptr;
  AvatarRecord migration_record_;
  AvatarRecord avatar_num_record_;
};

GW_GAME_END