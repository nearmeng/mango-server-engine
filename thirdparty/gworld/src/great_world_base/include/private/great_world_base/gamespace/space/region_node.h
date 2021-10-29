#pragma once

#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class Space;
class RegionNode;
class RegionNodeVisitor;

class RegionNode {
 public:
  RegionNode();
  RegionNode(const Rect& rect, RegionNode* parent);
  RegionNode(RegionNode* parent);

  virtual ~RegionNode();
  virtual bool ReadFromStream(IMemoryIStream& in) = 0;
  virtual bool UpdateFromStream(IMemoryIStream& in) = 0;
  virtual void Accept(RegionNodeVisitor& visitor) = 0;
  virtual void Destroy() = 0;
  static RegionNode* Parse(IMemoryIStream& in, Space* space, const Rect* rect = nullptr);

  uint64_t id() const { return id_; }
  void set_id(uint64_t id) { id_ = id; }

  const Rect& rect() const { return rect_; }
  Rect& mutable_rect() { return rect_; }
  void set_rect(const Rect& rect) { rect_ = rect; }

  Space* space() const;
  void set_space(Space* space);

  RegionNode* parent() const { return parent_; }
  void set_parent(RegionNode* parent) { parent_ = parent; }

  bool keep() const { return keep_; }
  void set_keep(bool keep) { keep_ = keep; }

  virtual bool use_parallel() { return false; }

  RegionNodeType region_node_type() const { return type_; }
  void set_region_node_type(RegionNodeType t) { type_ = t; }

 protected:
  uint64_t id_;
  Rect rect_;
  Space* space_;
  RegionNode* parent_;
  bool keep_ = true;
  RegionNodeType type_;
};

GW_GAME_END