#include "great_world_base/cellmgr/spacelayout/space_layout_mgr.h"

GW_CELL_MGR_BEGIN
GWCLASS_DECL_IMPLEMENT(SpaceLayoutMgr)

void SpaceLayoutMgr::SetReader(std::shared_ptr<ISpaceLayoutReader> reader) {
  reader_ = reader;
  if (reader_) {
    ReloadAll();
  }
}

GW_CELL_MGR_END