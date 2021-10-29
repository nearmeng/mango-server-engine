#pragma once

#include <string>

#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/cellmgr/space.h"

GW_CELL_MGR_BEGIN

class RegionNodeVisitor;

class RegionNode {
 public:
  virtual void Dispose() {}
  RegionNode();
  RegionNode(const Rect& rect, RegionNode* parent = nullptr);
  RegionNode(RegionNode* parent);
  RegionNode(const RegionNode& rhs);
  virtual ~RegionNode();
  // 执行一次 node 的负载均衡，可能导致node内的cell边界变化，参数是对负载均衡的过程做记录，返回值是是否有边界移动
  virtual bool Balance(bool tracing) { return false; }
  // 执行一次 node 的负载均衡，可能导致 node/cell 的数量变化，参数是对负载均衡的过程做记录，返回值是是否有 cell/node 数量变化
  virtual bool GAppBalance(ExcludeGameAppFilter& filter, bool tracing) { return false; }
  // 给这个 node 分配 gameapp，返回值是是否分配成功
  virtual bool AssignGameApps(ExcludeGameAppFilter& filter) { return true; }
  // 把 node 信息转换成stream，后面会用 MSG_CMD_ON_ADD_CELL 传给所有 gameapp
  virtual bool WriteToStream(IMemoryOStream& os, bool update_only = false, bool detail_for_viewer = false) = 0;
  // 从 stream 创建 node
  virtual bool ReadFromStream(IMemoryIStream& in) = 0;
  // 更新 node
  virtual bool UpdateFromStream(IMemoryIStream& in) { return true; }
  // 查找对应的 cell，参数是任意坐标，返回包含这个坐标的 gameapp
  virtual GameAppEntry* FindCell(float x, float y) { return nullptr; }

  virtual void Accept(RegionNodeVisitor& visitor) = 0;

  static RegionNode* Parse(IMemoryIStream& in, const Rect* root_rect = nullptr);

  uint64_t id() const { return id_; }
  void set_id(uint64_t id) { id_ = id; }

  const Rect& rect() const { return rect_; }
  void set_rect(const Rect& rect) { rect_ = rect; }
  Rect& mutable_rect() { return rect_; }

  Space* space() const { return space_; }
  void set_space(Space* space) { space_ = space; }

  RegionNode* parent() const { return parent_; }
  void set_parent(RegionNode* parent) { parent_ = parent; }

  GWString to_string() {
    std::stringstream ss;
    ss << id_ << ":" << rect_.minx() << "," << rect_.miny() << "," << rect_.maxx() << "," << rect_.maxy();
    return GWString(ss.str().c_str(), ss.str().size());
  }

  RegionNodeType GetType() { return type; }

 protected:
  uint64_t id_ = 0;
  Rect rect_;
  Space* space_ = nullptr;
  RegionNode* parent_ = nullptr;
  RegionNodeType type = RegionNodeType::kNone;
};

GW_CELL_MGR_END