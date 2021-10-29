#include "great_world_base/cellmgr/game_app_entry.h"

#include "great_world_base/cellmgr/cellmgr.h"
#include "great_world_base/cellmgr/space.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/cellmgr/cellmgr_common.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(ExcludeGameAppFilter)

ExcludeGameAppFilter::ExcludeGameAppFilter(Space* space) : space_(space) {
}

bool ExcludeGameAppFilter::Check(const GameAppEntry* gapp) {
  if (space_ && space_->cells().find(gapp->id()) != space_->cells().end()) {
    return false;
  }
  return gapp_ids_.find(gapp->id()) == gapp_ids_.end();
}

void ExcludeGameAppFilter::Cache(uint64_t id) {
  gapp_ids_.emplace(id);
  last_ = nullptr;
}

void ExcludeGameAppFilter::Clear() {
  gapp_ids_.clear();
}

GameAppEntry* ExcludeGameAppFilter::SearchSuitable(bool show_log, bool clear_if_not_found) {
  if (last_) {
    return last_;
  }
  GameAppEntry* gapp = CELLMGR.SearchGood(*this);
  do {
    if (gapp) {
      break;
    }
    if (show_log) {
      GW_LOG_INFO("no good game app found");
    }
    gapp = CELLMGR.SearchLeastLoaded(*this);
    if (gapp) {
      break;
    }
    if (show_log) {
      GW_LOG_INFO("no game app found");
    }
    if (clear_if_not_found) {
      Clear();
      gapp = CELLMGR.SearchGood(*this);
      if (gapp) {
        break;
      }
      if (show_log) {
        GW_LOG_INFO("still no good game app found after clear cache");
      }
      gapp = CELLMGR.SearchLeastLoaded(*this);
      if (gapp) {
        break;
      }
      if (show_log) {
        GW_LOG_INFO("still no game app found after clear cache");
      }
    }
  } while (false);
  last_ = gapp;
  return gapp;
}

GameAppEntry* ExcludeGameAppFilter::SearchGood() {
  if (last_) {
    return last_;
  }
  last_ = CELLMGR.SearchGood(*this);
  return last_;
}

GameAppEntry::GameAppEntry(uint64_t id) : id_(id), state_(GameAppState::kNormal) {
}

GameAppEntry::~GameAppEntry() {
}

bool GameAppEntry::available() const {
  return state_ == GameAppState::kNormal;
}

GW_CELL_MGR_END