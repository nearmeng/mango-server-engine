#pragma once

#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/cellmgr/spacelayout/space_layout_reader.h"
#include "great_world_base/common/shm/shm_singleton_template.h"

GW_CELL_MGR_BEGIN

class SpaceLayoutMgr : public ShmSingleton<SpaceLayoutMgr> {
  GWCLASS_DECL(SpaceLayoutMgr)
 public:
  const std::shared_ptr<SpaceLayoutData> GetSpaceStreamData(const GWString& name) {
    if (name_to_layout_.find(name) != name_to_layout_.end()) {
      return name_to_layout_[name];
    } else {
      if (ReloadOne(name)) {
        return name_to_layout_[name];
      }
    }
    return nullptr;
  }

  bool ReloadOne(const GWString& name) {
    auto data = reader_->LoadSpaceLayout(path_ + name);
    name_to_layout_[name] = data;
    return true;
  }

  bool ReloadAll() {
    for (auto iter = name_to_layout_.begin(); iter != name_to_layout_.end(); iter++) {
      iter->second = reader_->LoadSpaceLayout(path_ + iter->first);
    }
    return true;
  }

  bool Load(std::unordered_set<GWString> map_names) {
    for (auto name : map_names) {
      if (!ReloadOne(name)) {
        return false;
      }
    }
    return true;
  }

  void SetReader(std::shared_ptr<ISpaceLayoutReader> reader);
  inline const std::shared_ptr<ISpaceLayoutReader> reader() { return reader_; }

  void SetPath(GWString path) { path_ = std::move(path); }

 private:
  std::shared_ptr<ISpaceLayoutReader> reader_ = nullptr;
  std::unordered_map<GWString, std::shared_ptr<SpaceLayoutData>> name_to_layout_;
  GWString path_;
};

#define SPACE_LAYOUT_MGR gw::cellmgr::SpaceLayoutMgr::GetInstance()

GW_CELL_MGR_END