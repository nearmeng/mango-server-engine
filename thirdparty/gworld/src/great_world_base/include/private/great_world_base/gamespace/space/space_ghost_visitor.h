#pragma once

#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/gamespace/space/region_node_visitor.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class GhostEntries;

//用于real entity的ghost边界与space中的cell区域的覆盖检测
class SpaceGhostVisitor : public RegionNodeVisitor {
 public:
  SpaceGhostVisitor(EntityEntry& entity_entry, const Rect& rect, Space* space, GhostEntries& ghost_entries,
                    bool use_parallel);
  void Visit(CellNode& node) override;

 private:
  EntityEntry& entity_entry_;
  const Rect& rect_; // 用于比较的entity的扩展区域
  Space* space_;
  GhostEntries& ghost_entries_;// real entity关联的全部ghost信息
  bool use_parallel_;
};

GW_GAME_END