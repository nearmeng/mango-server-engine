#include "great_world_base/gamespace/space/region_node.h"

#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/evenly_divided_node.h"
#include "great_world_base/gamespace/space/fixed_divided_by_cell_node.h"
#include "great_world_base/gamespace/space/fixed_divided_node.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/strip_group_node.h"

GW_GAME_BEGIN

RegionNode::RegionNode() : space_(nullptr) {
}

RegionNode::RegionNode(const Rect& rect, RegionNode* parent) : rect_(rect), space_(nullptr), parent_(parent) {
}

RegionNode::RegionNode(RegionNode* parent) : space_(nullptr), parent_(parent) {
}

RegionNode::~RegionNode() {
}

Space* RegionNode::space() const { return space_; }
void RegionNode::set_space(Space* space) { space_ = space; }

RegionNode* RegionNode::Parse(IMemoryIStream& in, Space* space, const Rect* rect) {
  if (in.empty() || !space) {
    GW_LOG_TRACE("Parse region node fail,space=%p", space);
    return nullptr;
  }
  uint8_t node_type_num = 0;
  uint64_t node_id = 0;
  in >> node_type_num;
  RegionNodeType node_type = static_cast<RegionNodeType>(node_type_num);
  RegionNode* node;
  if (node_type == RegionNodeType::kEvenlyDivided || node_type == RegionNodeType::kStripGroup || node_type == RegionNodeType::kFixedDivided || node_type == RegionNodeType::kFixedDividedByCell) {
    in >> node_id;
    node = space->FindNonCellNode(node_id);
    if (!node) {
      if (node_type == RegionNodeType::kEvenlyDivided) {
        node = ShmNew<EvenlyDividedNode>();
      } else if (node_type == RegionNodeType::kStripGroup) {
        node = ShmNew<StripGroupNode>();
      } else if (node_type == RegionNodeType::kFixedDivided) {
        node = ShmNew<FixedDividedNode>();
      } else if (node_type == RegionNodeType::kFixedDividedByCell) {
        node = ShmNew<FixedDividedByCellNode>();
      } else {
        G6_ASSERT(false);  // not reachable
      }
      if (rect) {
        node->set_rect(*rect);
      }
      node->set_id(node_id);
      node->set_space(space);
      node->ReadFromStream(in);
      space->AddNonCellNode(node);
    } else {  // 更新已存在的节点
      if (!node->UpdateFromStream(in)) {
        return nullptr;
      }
    }
  } else if (node_type == RegionNodeType::kCell) {
    uint64_t gapp_id = 0;
    in >> gapp_id;
    CellNode* cell_node = space->FindCellNode(gapp_id);
    if (!cell_node) {
      cell_node = ShmNew<CellNode>();
      cell_node->set_main_gapp_id(gapp_id);
      cell_node->set_space(space);
      cell_node->ReadFromStream(in);
      space->AddCellNode(gapp_id, cell_node);
      for (auto& it : cell_node->parallel_cells()) {
        space->AddCellNode(it.first, cell_node);
      }
      if(rect)
        GW_LOG_DEBUG("space %lu, add_cell_node:main_gapp_id %lu, rect(%f,%f,%f,%f)", space->id(), 
            gapp_id, rect->minx(), rect->maxx(), rect->miny(), rect->maxy());
    } else {
      if (!cell_node->UpdateFromStream(in)) {
        return nullptr;
      }
    }
    cell_node->set_keep(true);
    node = cell_node;
  } else {
    GW_LOG_ERROR("invalid node type : %d", node_type_num);
    return nullptr;
  }
  if (node) {
    node->set_space(space);
    node->set_region_node_type(node_type);
  }
  return node;
}

GW_GAME_END