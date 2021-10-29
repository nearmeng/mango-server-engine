#include <cfloat>
#include "great_world_base/gamespace/space/cell.h"

#include "great_world_base/gamespace/space/cell_config.h"
#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/game_app_entry.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_ghost_visitor.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/gamespace/space/parallel_balance.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(Cell)

uint32_t AvatarRecord::record_interval_ = 1000;
uint32_t AvatarRecord::max_record_num_ = 20;

Cell::Cell(Space* space, CellNode* node) : space_(space), node_(node) {
  balance_ = ShmNew<ParallelBalance>(this);
}

Cell::~Cell() {
  if(balance_)
    ShmDelete<ParallelBalance>(balance_);
  balance_ = nullptr;
}

void Cell::PrepareDestroy() {
  for (auto iter : entity_entries_) {
    iter.second->PrepareDestroy();
  }
}

void Cell::OnDestroy() {
  std::vector<uint64_t> v;
  v.reserve(entity_entries_.size());
  for (auto& it : entity_entries_) {
    v.push_back(it.first);
  }
  for (auto id : v) {
    auto entity = entity_entries_[id];
    DelEntityEntry(id);
    entity->OnCellDestroy();
  }
}

void Cell::CalcEntityRect() {
  Rect temp_real_rect(FLT_MAX, FLT_MAX, -1, -1);
  Rect temp_rect(FLT_MAX, FLT_MAX, -1, -1);
  for (auto it = entity_entries_.begin(); it != entity_entries_.end(); it++) {
    EntityEntry* entry = it->second;
    if (entry->is_real()) {
      temp_real_rect.minx_ = std::min(temp_real_rect.minx_, entry->cell_entity()->pos().x);
      temp_real_rect.miny_ = std::min(temp_real_rect.miny_, entry->cell_entity()->pos().y);
      temp_real_rect.maxx_ = std::max(temp_real_rect.maxx_, entry->cell_entity()->pos().x);
      temp_real_rect.maxy_ = std::max(temp_real_rect.maxy_,entry->cell_entity()->pos().y);
    }
    temp_rect.minx_ = std::min(temp_rect.minx_, entry->cell_entity()->pos().x);
    temp_rect.miny_ = std::min(temp_rect.miny_, entry->cell_entity()->pos().y);
    temp_rect.maxx_ = std::max(temp_rect.maxx_, entry->cell_entity()->pos().x);
    temp_rect.maxy_ = std::max(temp_rect.maxy_, entry->cell_entity()->pos().y);
  }
  entity_rect_.reset(&temp_rect);
  real_entity_rect_.reset(&temp_real_rect);
}

// 最初是给ngr做的策略，那边改成ds了，这个策略不要了。类似功能用分线支持
/** 目标：每次调用的时候迁走一部分，另外让一个点和周围的点尽可能一起迁走
 * 写2个算法：
 * 1）根据entity_rect去做：每帧计算好entity_rect，然后缩固定的大小(数值可以设计的小一点)。如果单次需要迁移的数量过多，则迁移部分entity，old_rect不进行压缩，等下次entity都迁走了再压缩
 * 2) 根据十字链表去做：每次从十字链表的中间(2边)的位置开始迁移，从一个点开始迁移周围的点，每一帧迁移固定数量
 * 算法2更贴近目标，但是要涉及到项目使用的aoi算法。优先实现算法1，算法2可以用在使用十字链表的项目中
 */
uint32_t Cell::MigrationOldLayout() {
  return 0;
}

CellNode* Cell::node() const { return node_; }

void Cell::AddEntityEntry(uint64_t id, EntityEntry* entity) {
  if (entity_entries_.find(id) != entity_entries_.end()) {
    GW_LOG_ERROR("Cell already has EntityEntry %llu", id);
    return;
  }
  entity_entries_[id] = entity;

  if (entity->is_real()) {
    if (real_entries_.find(id) != real_entries_.end()) {
      GW_LOG_ERROR("Cell already has RealEntry %llu", id);
    }
    real_entries_[id] = entity->real_entry();
  }
}

void Cell::DelEntityEntry(uint64_t id) {
  if (entity_entries_.find(id) == entity_entries_.end()) {
    GW_LOG_INFO("Cell can not find EntityEntry %llu", id);
    return;
  }
  auto entity = entity_entries_[id];
  if (entity->real_entry()) {
    entity->real_entry()->DeleteGhostsOnLeave();
  }
  entity_entries_.erase(id);
  real_entries_.erase(id);
}

// #define PRINT_REAL_GHOST

static inline float CalcGhostRange(EntityEntry* entry, Space* space) {
  float ghost_range = CELL_CFG.ghost_dist();
  // space配置覆盖全局配置
  float space_range = space->ghost_range();
  if (space_range != 0 && space_range < CELL_CFG.ghost_dist()) {
    ghost_range = space_range;
  }
  float entity_range = entry->ghost_range();
  if (entity_range != 0 && entity_range < CELL_CFG.ghost_dist() && (space_range == 0 || entity_range < space_range)) {
    ghost_range = entity_range;
  }
  return ghost_range;
}

uint32_t Cell::CheckMigration() {
  auto& cell_rect = node_->rect();
#ifdef PRINT_REAL_GHOST
  GW_LOG_TRACE("current cell rect: %f %f %f %f", cell_rect.minx(), cell_rect.miny(), cell_rect.maxx(), cell_rect.maxy());
#endif
  uint32_t migrate_count = 0;
  uint32_t create_count = 0;
  uint32_t real_count = 0;
  GWVector<uint64_t> inner_real_eids;
  for (auto it = entity_entries_.begin(); it != entity_entries_.end(); it++) {
    EntityEntry* entry = it->second;
    Rect entry_rect;
    const Vector3& pos = entry->cell_entity()->pos();

    if (!entry->is_real()) {
      continue;
    } else {
      real_count++;
    }
#ifdef PRINT_REAL_GHOST
    GW_LOG_TRACE("entity eid: %ld pos: %f %f %f", it->first, pos.x, pos.y, pos.z);
#endif
    GhostEntries& ghosts = entry->real_entry()->ghosts();
    GhostEntries::Entries& entries = ghosts.mut_entries();

    if (entry->dst_gapp_id() == 0) {
      if (migrate_count < CELL_CFG.max_migrate_num()) {
        // 检测migrate
        float migrate_dist = CELL_CFG.migrate_dist();
        entry_rect = Rect(pos.x - migrate_dist, pos.y - migrate_dist, pos.x + migrate_dist, pos.y + migrate_dist);

        if (!cell_rect.Intersects(entry_rect) || space_->wait_remove() || entry->parallel_dst_gapp_id() != 0) {
          CellNode* cell_node = space_->FindCellNode(pos.x, pos.y);
          if (!cell_node) {
            GW_LOG_TRACE("bad pos %f %f, not cell node found", pos.x, pos.y);
            continue;
          }
          uint64_t dst_app_id = 0;
          if (cell_node == node_) {
            dst_app_id = entry->parallel_dst_gapp_id();
            GW_LOG_INFO("%lu choose game %lu", entry->cell_entity()->GetEid(), dst_app_id);
          } else {
            dst_app_id = cell_node->ChooseGameForMigrate();
            GW_LOG_INFO("%lu choose game %d, entry_rect %s, cell_rect %s", entry->cell_entity()->GetEid(), dst_app_id, entry_rect.DebugString().c_str(), cell_rect.DebugString().c_str());
          }
          GhostEntry* ghost = ghosts.Find(dst_app_id);
          if (ghost != nullptr && ghost->is_created() && cell_node->state(dst_app_id) == CellState::kCreated) {
            entry->set_dst_gapp_id(dst_app_id);
            uint64_t entity_id = entry->cell_entity()->GetEid();
            cell_node->OnMigrateIn(entity_id);
            entry->real_entry()->Migrate();
            G6_ASSERT(entry->is_ghost());
            migrate_count++;
            continue;  // 迁移后real会变成ghost，跳过下面流程。
          }
        } else {
          inner_real_eids.push_back(entry->cell_entity()->GetEid());
        }
      }
    }
#ifdef PRINT_REAL_GHOST
    GW_LOG_TRACE("entity eid: %ld ghost check", it->first);
#endif

    // 检测ghost delete
    for (auto& it : entries) {
      it->set_keep(false);
    }

    float ghost_dist_extend = CELL_CFG.ghost_dist_extend();
    entry_rect = Rect(pos.x - ghost_dist_extend, pos.y - ghost_dist_extend, pos.x + ghost_dist_extend,
                      pos.y + ghost_dist_extend);

    GhostEntries temp;
    SpaceGhostVisitor delete_ghost_visitor(*entry, entry_rect, space_, temp, false);
    space_->root_region_node()->Accept(delete_ghost_visitor);

    std::vector<uint64_t> deleted;
    for (auto& it : entries) {
      if (temp.Find(it->gapp()->id())) {
        it->set_keep(true);
      } else {
        deleted.push_back(it->gapp()->id());
      }
    }

    for (auto& gapp_id : deleted) {
      GhostEntry* ghost_entry = ghosts.Find(gapp_id);
      if (!ghost_entry->create_pending()) {
        entry->real_entry()->DeleteGhost(*ghost_entry);
      }
      ghosts.Remove(gapp_id);
    }

    // 检测ghost create
    float ghost_range = CalcGhostRange(entry, space_);
#ifdef PRINT_REAL_GHOST
    GW_LOG_TRACE("entity eid: %ld ghost check range %f, position (%f, %f)", it->first, ghost_range, pos.x, pos.y);
#endif
    entry_rect = Rect(pos.x - ghost_range, pos.y - ghost_range, pos.x + ghost_range, pos.y + ghost_range);
    SpaceGhostVisitor create_ghost_visitor(*entry, entry_rect, space_, ghosts, true);
    space_->root_region_node()->Accept(create_ghost_visitor);
    for (auto& it : entries) {
      if (create_count < CELL_CFG.max_create_ghost_num() && it->create_pending()) {
        entry->real_entry()->CreateGhost(*it);
        it->set_create_pending(false);
        create_count++;
      }
    }
  }
  migration_record_.Add(migrate_count);
  avatar_num_record_.Add(inner_real_eids.size());
  Balance(inner_real_eids);
  return real_count;
}

uint32_t Cell::RegisterGhostToGapp(uint64_t eid, uint64_t gapp_id) {
  auto it = entity_entries_.find(eid);
  if (it == entity_entries_.end()) {
    return 0;
  }
  return it->second->real_entry()->ghosts().RegisterGhost(gapp_id);
}

uint32_t Cell::UnRegisterGhostToGapp(uint64_t eid, uint64_t gapp_id) {
  auto it = entity_entries_.find(eid);
  if (it == entity_entries_.end()) {
    return 0;
  }
  return it->second->real_entry()->ghosts().UnRegisterGhost(gapp_id);
}

bool Cell::ForceUnRegisterGhostToGapp(uint64_t eid, uint64_t gapp_id) {
  auto it = entity_entries_.find(eid);
  if (it == entity_entries_.end()) {
    return true;
  }
  return it->second->real_entry()->ghosts().ForceUnRegisterGhost(gapp_id);
}

void Cell::Balance(GWVector<uint64_t>& inner_real_eids) {
  if (!space_->root_region_node()->use_parallel() || !balance_) {
    return;
  }
  balance_->OnCellMigrateCheck();
  if(inner_real_eids.empty())
    return;

  if (CELL_CFG.max_migrate_num() - migration_record_.average() <= 0) {
    return;
  }
  auto migrate_num = static_cast<int32_t>(inner_real_eids.size()) - static_cast<int32_t>(node_->upper_normal_avatar_num());
  if (migrate_num < 0) {
    return;
  }
  // todo 迁移的时候考虑下entity之间的关系，有视野关系的尽量一起迁走，并且迁移到同个 cell
  GWVector<uint64_t> balance_entry(inner_real_eids.begin(), inner_real_eids.begin() + migrate_num);
  GW_LOG_DEBUG("space %d, avatar num %d, migrate num %d", space_->id(), balance_entry.size(), migrate_num);
  balance_->BeginBalance(balance_entry);
}

bool Cell::MigrateTo(uint64_t eid, uint64_t gapp_id) {
  auto it = entity_entries_.find(eid);
  if(it == entity_entries_.end()) {
    return true;
  }
  it->second->set_parallel_dst_gapp_id(gapp_id);
  return true;
}

GW_GAME_END