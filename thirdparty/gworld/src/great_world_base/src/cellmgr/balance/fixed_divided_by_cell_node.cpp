#include "great_world_base/cellmgr/balance/fixed_divided_by_cell_node.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/cellmgr_config.h"

GW_CELL_MGR_BEGIN

class SpaceRegionNodeVisitor;
GWCLASS_DECL_IMPLEMENT(FixedDividedByCellNode)

FixedDividedByCellNode::FixedDividedByCellNode() {
  type = RegionNodeType::kFixedDividedByCell;
}

bool FixedDividedByCellNode::WriteToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << static_cast<uint8_t>(RegionNodeType::kFixedDividedByCell);
  os << id_;
  for (auto node : nodes_) {
    os << node->rect();
    if (!node->WriteToStream(os, update_only, detail_for_viewer))
      return false;
  }
  return true;
}

bool FixedDividedByCellNode::ReadFromStream(IMemoryIStream& in) {
  if (!FixedDividedNode::ReadFromStream(in))
    return false;
  for (auto node : nodes_) {
    if (node->GetType() != RegionNodeType::kCell) {
      GW_LOG_DEBUG("FixedDividedByCellNode_ReadFromStream subnode is not cell:%d", node->GetType());
      return false;
    }
  }
  return true;
}

namespace {
struct CellNodeCompareInfo {
  RegionNode* new_node_;
  RegionNode* old_node_;
  float cover_area_;

  CellNodeCompareInfo(RegionNode* new_node, RegionNode* old_node, float cover_area) {
    new_node_ = new_node;
    old_node_ = old_node;
    cover_area_ = cover_area;
  }
};
}  // namespace

// 给ngr的老方案，不用了
bool FixedDividedByCellNode::UpdateFromStream(IMemoryIStream& in) {
  return true;
}

bool FixedDividedByCellNode::GAppBalance(ExcludeGameAppFilter& filter, bool tracing) {
  // 检查 新增/移除/内部迁移
  bool changed = false;
  for (auto& it : space_->cells()) {
    filter.Cache(it.first);
  }
  std::shared_ptr<std::vector<uint64_t>> removed_ids = nullptr;
  for (auto& it : nodes_) {
    bool need_add = false;
    bool need_remove = false;
    bool visible_changed = false;
    uint64_t removed_id = 100;
    CellNode* cellNode = static_cast<CellNode*>(it);
    cellNode->CheckAddOrRemoveCell(need_add, need_remove, removed_id, visible_changed);
    if (visible_changed) {
      changed = true;
    }
    if (need_add) {
      auto gapp = CELLMGR.SearchGood(filter);
      if (gapp != nullptr) {
        filter.Cache(gapp->id());
        cellNode->AddCell(gapp);
        if (!space_->RegisterCell(cellNode, gapp)) {
          cellNode->RemoveCell(gapp->id());
        } else {
          changed = true;
          GW_LOG_INFO("space %d cellnode %d add cell to game %d, parallel_gapps_size %d", space_->id(), cellNode->id(), gapp->id(), cellNode->parallel_gapps().size());
        }
      } else {
        GW_LOG_INFO("space %d cellnode %d add cell game not found", space_->id(), cellNode->id());
      }
    }
    if (need_remove) {
      cellNode->RemoveCell(removed_id);
      space_->UnregisterCell(removed_id);
      if (removed_ids == nullptr) {
        removed_ids = std::make_shared<std::vector<uint64_t>>();
      }
      removed_ids->push_back(removed_id);
      GW_LOG_INFO("space %d cellnode %d remove cell game %d", space_->id(), cellNode->id(), removed_id);
      changed = true;
      CELLMGR.UpdateLayoutToGame(space_->id(), removed_id);
    }
  }
  return changed;
}

GW_CELL_MGR_END