#pragma once
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

class GameAppEntry {
  GWCLASS_DECL(GameAppEntry)
 public:
  GameAppEntry() {}
  GameAppEntry(uint64_t id);
  ~GameAppEntry();
  uint64_t id() const { return id_; }
  void set_id(uint64_t id) { id_ = id; }

 private:
  uint64_t id_;
};

GW_GAME_END