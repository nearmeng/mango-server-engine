#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN

class GameAppEntry;

class GameAppFilter {
 public:
  virtual ~GameAppFilter() {}
  virtual bool Check(const GameAppEntry* gapp) { return true; }
};

class Space;

class ExcludeGameAppFilter : public GameAppFilter {
 public:
  GWCLASS_DECL(ExcludeGameAppFilter)
 public:
  virtual ~ExcludeGameAppFilter() {}
  ExcludeGameAppFilter(Space* space = nullptr);
  bool Check(const GameAppEntry* gapp) override;
  void Cache(uint64_t id);
  void Clear();
  GameAppEntry* SearchSuitable(bool show_log = true, bool clear_if_not_found = false);
  GameAppEntry* SearchGood();
  GWHashSet<uint64_t>& gapp_ids() { return gapp_ids_; }

 private:
  GWHashSet<uint64_t> gapp_ids_;
  GameAppEntry* last_ = nullptr;  // 上一次的结果，但没有使用
  Space* space_;
};

class GameAppEntry {
  GWCLASS_DECL(GameAppEntry)
 public:
  GameAppEntry(uint64_t id);
  ~GameAppEntry();

  bool available() const;
  void AddConvLoad(float delta) { conv_load_ += delta; }
  void AddRevisedLoad(float delta) { revised_load_ += delta; }

  uint64_t id() const { return id_; }
  float curr_load() const { return curr_load_; }
  void set_curr_load(float load) { curr_load_ = load; }
  float conv_load() const { return conv_load_; }
  void set_conv_load(float load) { conv_load_ = load; }
  float revised_load() const { return revised_load_; }
  void set_revised_load(float load) { revised_load_ = load; }
  uint32_t memsize() const { return memsize_; }
  void set_memsize(uint32_t memsize) { memsize_ = memsize; }
  uint32_t num_entities() const { return num_entities_; }
  void set_num_entities(uint32_t num) { num_entities_ = num; }
  uint32_t num_avatars() const { return num_avatars_; }
  void set_num_avatars(uint32_t num) { num_avatars_ = num; }
  uint32_t num_reals() const { return num_reals_; }
  void set_num_reals(uint32_t num) { num_reals_ = num; }
  GameAppState state() const { return state_; }
  void set_state(GameAppState state) { state_ = state; }

 private:
  uint64_t id_;
  GWString machine_id_;
  float curr_load_ = 0;
  float conv_load_ = 0;
  float revised_load_ = 0;
  uint32_t memsize_ = 0;
  uint32_t num_entities_ = 0;
  uint32_t num_avatars_ = 0;
  uint32_t num_reals_ = 0;
  GameAppState state_;
};

typedef GWHashMap<uint64_t, GameAppEntry*> GameAppEntries;
typedef GWHashSet<GameAppEntry*> GameAppEntrySet;

GW_CELL_MGR_END