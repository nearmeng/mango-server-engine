#pragma once
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/game_app_entry.h"

GW_GAME_BEGIN

class GhostEntry {
  GWCLASS_DECL(GhostEntry)
 public:
  GhostEntry() = default;
  GhostEntry(GameAppEntry* gapp);
  ~GhostEntry();
  bool keep() const { return keep_; }
  void set_keep(bool keep) { keep_ = keep; }
  bool create_pending() const { return create_pending_; }
  void set_create_pending(bool create_pending) { create_pending_ = create_pending; }
  bool is_created() { return keep_ && !create_pending_; }
  GameAppEntry* gapp() const { return gapp_; }

 private:
  GameAppEntry* gapp_;
  bool keep_;            // 是否需要保留（仍在ghost边界范围内）
  bool create_pending_;  // 是否需要创建
};

class GhostEntries {
 public:
  typedef GWVector<GhostEntry*> Entries;  // ghost比较少，使用vector够了
  GhostEntries();
  ~GhostEntries();
  GhostEntries(const GhostEntries&) = delete;
  GhostEntries(GhostEntries&&) = delete;
  GhostEntries& operator=(const GhostEntries&) = delete;
  GhostEntries& operator=(GhostEntries&&) = delete;

  GhostEntry* Add(GameAppEntry* gapp);
  GhostEntry* Find(uint64_t gapp_id);
  bool Remove(uint64_t gapp_id);
  // 请求在 gapp_id 创建 ghost
  uint32_t RegisterGhost(uint64_t gapp_id);
  uint32_t UnRegisterGhost(uint64_t gapp_id);
  bool ForceUnRegisterGhost(uint64_t gapp_id);
  bool GhostRegistered(uint64_t gapp_id);
  const GWHashMap<uint64_t, uint32_t>& registered_gapp_ids() { return registered_gapp_ids_; }

  const Entries& entries() { return entries_; }
  Entries& mut_entries() { return entries_; }

  void RecordChoose(uint64_t id, uint64_t gapp_id) { choose_record_[id] = gapp_id; }
  const GWHashMap<uint64_t, uint64_t> choose_record() { return choose_record_; }

 private:
  Entries entries_;
  GWHashMap<uint64_t, uint32_t> registered_gapp_ids_;
  GWHashMap<uint64_t, uint64_t> choose_record_;
};

GW_GAME_END