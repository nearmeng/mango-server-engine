#pragma once
#include "great_world_base/cellmgr/balance/region_node.h"
#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

class BalanceNode : public RegionNode {
 public:
  BalanceNode();
  BalanceNode(const Rect& rect, RegionNode* parent = nullptr);
  BalanceNode(RegionNode* parent);
  BalanceNode(const RegionNode& rhs);
  virtual ~BalanceNode();
  virtual float curr_load() const = 0;          // 最近的负载
  virtual float revised_load() const = 0;       // 修正后的最近负载
  virtual float conv_load() const = 0;          // 近期平均负载
  virtual uint32_t memsize() const = 0;         // 内存占用，单位：MB
  virtual const Rect& node_rect() const = 0;    // 节点区域
  virtual Rect& mutable_node_rect() = 0;        // 节点区域（mutable）
  virtual const Rect& entity_rect() const = 0;  // entity 区域
  virtual Rect& mutable_entity_rect() = 0;      // entity 区域（mutable）
  virtual const Rect& tile_rect() const = 0;    // 已加载地图的区域
  virtual Rect& mutable_tile_rect() = 0;        // 已加载地图的区域（mutable）
  virtual bool removing() const = 0;            // 是否正在移除中

  virtual uint32_t num_entities(bool use_parallel = false, uint64_t gapp_id = 0) const = 0;  // entity数目
  virtual uint32_t num_avatars(bool use_parallel = false, uint64_t gapp_id = 0) const = 0;
  virtual uint32_t num_reals(bool use_parallel = false, uint64_t gapp_id = 0) const = 0;

  bool init_expanding() const { return init_expanding_; }
  void set_init_expanding(bool init_expanding) { init_expanding_ = init_expanding; }

 protected:
  bool init_expanding_;  // 是否在动态添加后的初次扩张中
};

GW_CELL_MGR_END