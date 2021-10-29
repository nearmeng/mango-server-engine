#include "great_world_base/gamespace/space/space_ghost_visitor.h"

#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/gamespace/space/evenly_divided_node.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/gamespace/space/strip_group_node.h"

GW_GAME_BEGIN

// #define PRINT_REAL_GHOST
SpaceGhostVisitor::SpaceGhostVisitor(EntityEntry& entity_entry, const Rect& rect, Space* space,
                                     GhostEntries& ghost_entries, bool use_parallel)
    : entity_entry_(entity_entry),
      rect_(rect),
      space_(space),
      ghost_entries_(ghost_entries),
      use_parallel_(use_parallel) {}

void SpaceGhostVisitor::Visit(CellNode& target_node) {
  const Rect& target_cell_rect = target_node.rect();
  auto dst_gapp_id_ = entity_entry_.dst_gapp_id();
  auto src_gapp_id = entity_entry_.real_gapp_id();
  auto real_entry_ = entity_entry_.real_entry();
#ifdef PRINT_REAL_GHOST
  GW_LOG_TRACE("SpaceGhostVisitor: target_cell_rect %f %f %f %f start check", target_cell_rect.minx(),
               target_cell_rect.miny(), target_cell_rect.maxx(), target_cell_rect.maxy());
#endif
  if (dst_gapp_id_) {  // 正在传送中
    return;
  }
  auto src_cell_node = space_->FindCellNode(src_gapp_id);
  if (!src_cell_node) return;

  if (!target_cell_rect.HasArea()) {
#ifdef PRINT_REAL_GHOST
    GW_LOG_TRACE("SpaceGhostVisitor: target_cell_rect %f %f %f %f has no area", target_cell_rect.minx(),
                 target_cell_rect.miny(), target_cell_rect.maxx(), target_cell_rect.maxy());
#endif
    return;
  }

#ifdef PRINT_REAL_GHOST
  GW_LOG_TRACE("SpaceGhostVisitor: target_cell_rect %f %f %f %f", target_cell_rect.minx(), target_cell_rect.miny(),
               target_cell_rect.maxx(), target_cell_rect.maxy());
#endif

  std::unordered_set<uint64_t> need_create;
  for (auto& it : real_entry_->ghosts().registered_gapp_ids()) {
    need_create.insert(it.first);
  }
  if(entity_entry_.parallel_dst_gapp_id()) {
    need_create.insert(entity_entry_.parallel_dst_gapp_id());
  }

#ifdef PRINT_REAL_GHOST
    GW_LOG_TRACE("SpaceGhostVisitor: target_node %d, main_gapp %d, visible %d", target_node.id(),
                 target_node.main_gapp_id(), target_node.parallel_visible());
#endif

  if (target_node.id() == src_cell_node->id()) {  // 同cell_node，直接在其他分线创建
    if (target_node.main_gapp_id() != src_gapp_id && target_node.main_cell().Created()) {
      if (target_node.parallel_visible() || real_entry_->ghosts().GhostRegistered(target_node.main_gapp_id()) ||
          real_entry_->ParallelIsAlwaysVisible()) {
        need_create.insert(target_node.main_gapp_id());
      }
    }
    for (auto& it : target_node.parallel_cells()) {
      if (it.first != src_gapp_id && it.second.Created()) {
        if (target_node.parallel_visible() || real_entry_->ghosts().GhostRegistered(it.first) ||
            real_entry_->ParallelIsAlwaysVisible()) {
          need_create.insert(it.first);
        }
      }
    }
  } else if (rect_.Intersects(target_cell_rect)) {  // 不同cell_node，判断区域重叠
    // 都是互通的，全 cell 创建
    // src 隔离，dst 互通，全 cell 创建
    // 都是隔离的，选一个
    // src 互通，dst 隔离，选一个
    if (target_node.parallel_visible()) {
      need_create.insert(target_node.main_gapp_id());
      for (auto& it : target_node.parallel_cells()) {
        if (it.second.Created()) {
          need_create.insert(it.first);
        }
      }
    } else {
      auto gapp_id = SPACE_MGR.choose_migration_cell(real_entry_->cell_entity()->GetEid(), target_node.id());
      if (!gapp_id) {
        auto it = real_entry_->ghosts().choose_record().find(target_node.id());
        if (it != real_entry_->ghosts().choose_record().end()) {
          gapp_id = it->second;
        }
      }
      if (!gapp_id) {
        gapp_id = target_node.RandomCell();
        real_entry_->ghosts().RecordChoose(target_node.id(), gapp_id);
      }
      if (gapp_id == target_node.main_gapp_id()) {
        if (target_node.main_cell().Created()) {
          need_create.insert(gapp_id);
        }
      } else {
        auto it = target_node.parallel_cells().find(gapp_id);
        if (it != target_node.parallel_cells().end() && it->second.Created()) {
          need_create.insert(gapp_id);
        }
      }
    }
  }

  for (auto gapp_id : need_create) {
    GhostEntry* entry = ghost_entries_.Find(gapp_id);
    if (entry) {  // ghost仍在该区域中
      entry->set_keep(true);
    } else {  // 需要创建ghost
      entry = ghost_entries_.Add(SPACE_MGR.FindGameApp(gapp_id, true));
      entry->set_keep(true);
      entry->set_create_pending(true);
    }
  }
}

GW_GAME_END