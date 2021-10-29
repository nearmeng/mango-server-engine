#include "great_world_base/cellmgr/space.h"

#include "great_world_base/cellmgr/balance/cell_node.h"
#include "great_world_base/cellmgr/balance/evenly_divided_node.h"
#include "great_world_base/cellmgr/balance/fixed_divided_node.h"
#include "great_world_base/cellmgr/balance/region_node_visitor.h"
#include "great_world_base/cellmgr/balance/strip_group_node.h"
#include "great_world_base/cellmgr/balance/strip_node.h"
#include "great_world_base/cellmgr/cellmgr.h"
// #include "great_world_base/proto/gamemgr.pb.h"
#include "great_world_base/cellmgr/proxy/mgr_service_proxy.h"

GW_CELL_MGR_BEGIN

class SpaceRegionNodeVisitor : public RegionNodeVisitor {
 public:
  SpaceRegionNodeVisitor(Space* space) : space_(space) {
  }

  void Visit(EvenlyDividedNode& node) override {
    node.set_space(space_);
    node.set_id(space_->Nextuint64_t());
    node.VisitNodes(*this);
  }

  void Visit(CellNode& node) override {
    node.set_space(space_);
    node.set_id(space_->Nextuint64_t());
    gapps_.emplace(node.main_gapp());
    for (auto& it : node.parallel_gapps()) {
      gapps_.emplace(it.second.gapp_);
    }
    space_->RegisterCell(&node);
  }

  void Visit(StripNode& node) override {
    node.set_space(space_);
    node.set_id(space_->Nextuint64_t());
    node.VisitNodes(*this);
  }

  void Visit(StripGroupNode& node) override {
    node.set_space(space_);
    node.set_id(space_->Nextuint64_t());
    node.VisitNodes(*this);
  }

  void Visit(FixedDividedNode& node) override {
    node.set_space(space_);
    node.set_id(space_->Nextuint64_t());
    node.VisitNodes(*this);
  }

  GameAppEntrySet& gapps() { return gapps_; }

 private:
  GameAppEntrySet gapps_;
  Space* space_;
};

// 遍历查找（x，y）所在的CellNode
class FindCellVisitor : public RegionNodeVisitor {
 public:
  FindCellVisitor(float x, float y) : x_(x), y_(y) {
  }

  void Visit(EvenlyDividedNode& node) override {
    if (found_) {
      return;
    }
    node.VisitNodes(*this);
  }

  void Visit(CellNode& node) override {
    if (found_) {
      return;
    }

    if (node.rect().Contains(x_, y_)) {
      found_ = &node;
    }
  }

  void Visit(StripNode& node) override {
    if (found_) {
      return;
    }
    node.VisitNodes(*this);
  }

  void Visit(StripGroupNode& node) override {
    if (found_) {
      return;
    }
    node.VisitNodes(*this);
  }

  void Visit(FixedDividedNode& node) override {
    if (found_) {
      return;
    }
    node.VisitNodes(*this);
  }

  CellNode* result() const {
    return found_;
  }

 private:
  CellNode* found_ = nullptr;
  float x_;
  float y_;
};

GWCLASS_DECL_IMPLEMENT(Space)

Space::Space(uint64_t id, const GWString& name, const GWString& tag,
             uint16_t min_cell_num, uint16_t max_cell_num, const Rect& rect, RegionNode* root) : id_(id), name_(name.data(), name.size()), tag_(tag.data(), tag.size()), min_cell_num_(min_cell_num), max_cell_num_(max_cell_num), rect_(rect), root_(root) {
  SpaceRegionNodeVisitor visitor(this);
  root_->Accept(visitor);
  // for(const auto& gapp : visitor.gapps()) {
  //     gapps_.emplace(gapp->id(), gapp);
  // }
}

Space::~Space() {
  if (root_) {
    delete root_;
  }
  // gapps_.clear();
}

bool Space::WriteLayoutToStream(IMemoryOStream& os, bool update_only, bool detail_for_viewer) {
  os << rect_;
  return root_->WriteToStream(os, update_only, detail_for_viewer);
}

// bool Space::AddGameApp(GameAppEntry* gapp) {
//     if (!gapp || gapps_.find(gapp->id()) != gapps_.end()) {
//         return false;
//     }
//     gapps_[gapp->id()] = gapp;
//     return true;
// }

// bool Space::RemoveGameApp(uint64_t gapp_id) {
//     if (gapps_.find(gapp_id) == gapps_.end()) {
//         return false;
//     }
//     gapps_.erase(gapp_id);
//     return true;
// }

bool Space::RegisterCell(CellNode* node) {
  GW_LOG_DEBUG("RegisterCell mainuint64_t<%lu>", node->main_gapp()->id());
  GW_RETURN_IF_ERROR(!node, false, "invalid node");
  GW_RETURN_IF_ERROR(!node->main_gapp(), false, "cell node has no game app");
  uint64_t main_gapp_id = node->main_gapp()->id();
  GW_RETURN_IF_ERROR(cells_.find(main_gapp_id) != cells_.end(), false, "cell already registered ： %d", main_gapp_id);
  cells_[main_gapp_id] = node;
  for (auto& it : node->parallel_gapps()) {
    GW_RETURN_IF_ERROR(cells_.find(it.first) != cells_.end(), false, "cell already registered ： %d", it.first);
    cells_[it.first] = node;
  }
  return true;
}

bool Space::RegisterCell(CellNode* node, GameAppEntry* gapp) {
  GW_LOG_DEBUG("RegisterCell uint64_t<%lu>", gapp->id());
  GW_RETURN_IF_ERROR(!node, false, "invalid node");
  GW_RETURN_IF_ERROR(!gapp, false, "game app is null");
  GW_RETURN_IF_ERROR(node->parallel_gapps().find(gapp->id()) == node->parallel_gapps().end() && node->main_gapp() != gapp, false, "node gapp not match");
  cells_[gapp->id()] = node;
  return true;
}

bool Space::UnregisterCell(uint64_t gapp_id) {
  GW_RETURN_IF_ERROR(cells_.find(gapp_id) == cells_.end(), false, "cell not found : %d", gapp_id);
  cells_.erase(gapp_id);
  return true;
}

CellNode* Space::FindCell(uint64_t gapp_id) const {
  auto it = cells_.find(gapp_id);
  return it != cells_.end() ? it->second : nullptr;
}

CellNode* Space::FindCell(float x, float y) const {
  GW_RETURN_IF_ERROR(!root_, nullptr, "layout root not exist");
  FindCellVisitor visitor(x, y);
  root_->Accept(visitor);
  return visitor.result();
}

bool Space::Balance(bool tracing) {
  G6_ASSERT(root_);
  return root_->Balance(tracing);
}

bool Space::GAppBalance(bool tracing) {
  G6_ASSERT(root_);
  ExcludeGameAppFilter filter(this);
  return root_->GAppBalance(filter, tracing);
}

bool Space::IsAllCellCreated() {
  for (const auto& iter : cells_) {
    if (!iter.second->created()) {
      return false;
    }
  }
  return true;
}

GW_CELL_MGR_END