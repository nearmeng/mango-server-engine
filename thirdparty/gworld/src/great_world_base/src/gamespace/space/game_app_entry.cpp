#include "great_world_base/gamespace/space/game_app_entry.h"

GW_GAME_BEGIN

GWCLASS_DECL_IMPLEMENT(GameAppEntry);

GameAppEntry::GameAppEntry(uint64_t id) : id_(id) {
}

GameAppEntry::~GameAppEntry() {
}

GW_GAME_END