#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/game_app_entry.h"
#include "great_world_base/common/memory_stream.h"

GW_CELL_MGR_BEGIN

class GameAppEntry;
class RegionNode;
class CellNode;

class Space;
class Space {
  GWCLASS_DECL(Space)
 public:
  typedef GWHashMap<uint64_t, CellNode*> Cells;
  Space() = default;
  Space(uint64_t id, const GWString& name, const GWString& tag,
        uint16_t min_cell_num, uint16_t max_cell_num, const Rect& rect, RegionNode* root);
  ~Space();
  bool WriteLayoutToStream(IMemoryOStream& os, bool update_only = false, bool detail_for_viewer = false);
  bool AddGameApp(GameAppEntry* gapp);
  bool RemoveGameApp(uint64_t gapp_id);
  bool RegisterCell(CellNode* node);
  bool RegisterCell(CellNode* node, GameAppEntry* gapp);
  bool UnregisterCell(uint64_t gapp_id);
  CellNode* FindCell(uint64_t gapp_id) const;
  CellNode* FindCell(float x, float y) const;
  // Space内部Cell间的负载均衡
  bool Balance(bool tracing);
  // 动态添加或移除Cell
  bool GAppBalance(bool tracing);
  bool IsAllCellCreated();
  uint64_t Nextuint64_t() { return ++last_node_id_; }

  uint64_t id() const { return id_; }
  GWString name() const { return name_; }
  GWString tag() const { return tag_; }
  uint16_t min_cell_num() const { return min_cell_num_; }
  uint16_t max_cell_num() const { return max_cell_num_; }
  const Rect& rect() const { return rect_; }
  // const GameAppEntries& gapps() const { return gapps_; }
  const Cells& cells() const { return cells_; }
  Cells& mutable_cells() { return cells_; }
  const RegionNode* root() const { return root_; }
  RegionNode* mutable_root() { return root_; }
  unsigned long last_add_cell_time() const { return last_add_cell_time_; }
  void set_last_add_cell_time(unsigned long t) { last_add_cell_time_ = t; }
  unsigned long last_remove_cell_time() const { return last_remove_cell_time_; }
  void set_last_remove_cell_time(unsigned long t) { last_remove_cell_time_ = t; }
  unsigned long last_add_strip_time() const { return last_add_strip_time_; }
  void set_last_add_strip_time(unsigned long t) { last_add_strip_time_ = t; }
  unsigned long last_remove_strip_time() const { return last_remove_strip_time_; }
  void set_last_remove_strip_time(unsigned long t) { last_remove_strip_time_ = t; }
  unsigned long last_add_time() const { return last_add_time_; }
  void set_last_add_time(unsigned long t) { last_add_time_ = t; }
  unsigned long last_remove_time() const { return last_remove_time_; }
  void set_last_remove_time(unsigned long t) { last_remove_time_ = t; }
  bool is_notified() const { return is_notified_; }
  void set_is_notified(bool is_notified) { is_notified_ = is_notified; }
  bool is_static() const { return is_static_; }
  void set_is_static(bool is_static) { is_static_ = is_static; }
  GWString meta_data() const { return meta_data_; }
  void set_meta_data(GWString meta_data) { meta_data_ = meta_data; }

 private:
  uint64_t id_;
  GWString name_;
  GWString tag_;
  uint16_t min_cell_num_;
  uint16_t max_cell_num_;
  // GameAppEntries gapps_;
  Rect rect_;
  RegionNode* root_;
  Cells cells_;
  uint64_t last_node_id_ = 0;
  unsigned long last_add_cell_time_ = 0;
  unsigned long last_remove_cell_time_ = 0;
  unsigned long last_add_strip_time_ = 0;
  unsigned long last_remove_strip_time_ = 0;
  unsigned long last_add_time_ = 0;
  unsigned long last_remove_time_ = 0;
  bool is_notified_ = false;
  bool is_static_ = false;
  GWString meta_data_;
};

GW_CELL_MGR_END