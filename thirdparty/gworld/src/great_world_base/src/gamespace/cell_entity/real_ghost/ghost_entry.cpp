#include "great_world_base/gamespace/cell_entity/real_ghost/ghost_entry.h"
#include "great_world_base/gamespace/space/game_app_entry.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(GhostEntry)

GhostEntry::GhostEntry(GameAppEntry* gapp) : gapp_(gapp), keep_(true), create_pending_(false) {
}

GhostEntry::~GhostEntry() {
}

GhostEntries::GhostEntries() {
}

GhostEntries::~GhostEntries() {
  for (auto it : entries_) {
    it->Dispose();
  }
  entries_.clear();
}

GhostEntry* GhostEntries::Add(GameAppEntry* gapp) {
  if (!gapp) {
    return nullptr;
  }
  GhostEntry* entry = Find(gapp->id());
  if (entry) {
    return entry;
  }
  entry = ShmNew<GhostEntry>(gapp);
  entries_.emplace_back(entry);
  return entry;
}

GhostEntry* GhostEntries::Find(uint64_t gapp_id) {
  for (auto it : entries_) {
    if (it->gapp()->id() == gapp_id) {
      return it;
    }
  }
  return nullptr;
}

bool GhostEntries::Remove(uint64_t gapp_id) {
  for (auto it = entries_.begin(); it != entries_.end(); ++it) {
    if ((*it)->gapp()->id() == gapp_id) {
      (*it)->Dispose();
      entries_.erase(it);
      return true;
    }
  }
  return false;
}

uint32_t GhostEntries::RegisterGhost(uint64_t gapp_id) {
  auto it = registered_gapp_ids_.find(gapp_id);
  if (it == registered_gapp_ids_.end()) {
    registered_gapp_ids_[gapp_id] = 1;
    it = registered_gapp_ids_.find(gapp_id);
  }
  return it->second;
}

uint32_t GhostEntries::UnRegisterGhost(uint64_t gapp_id) {
  auto it = registered_gapp_ids_.find(gapp_id);
  if (it == registered_gapp_ids_.end()) {
    return 0;
  }
  if (it->second == 1) {
    registered_gapp_ids_.erase(gapp_id);
    return 0;
  }
  it->second--;  // 注册的引用计数
  return it->second;
}

bool GhostEntries::ForceUnRegisterGhost(uint64_t gapp_id) {
  registered_gapp_ids_.erase(gapp_id);
  return true;
}

bool GhostEntries::GhostRegistered(uint64_t gapp_id) {
  return registered_gapp_ids_.find(gapp_id) != registered_gapp_ids_.end();
}

GW_GAME_END
