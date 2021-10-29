// #pragma once
// #include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
// #include "log.h"

// GW_GAME_BEGIN

// class MockRealEntry : public RealEntry {
//   GWCLASS_DECL(MockRealEntry)
//  public:
//   void OnMigrate(const std::function<void()>& done) {
//     GW_LOG_DEBUG("OnMigrate");
//   }

//   void OnDeleteGhost(const GhostEntry& ghost_entry) {
//     GW_LOG_DEBUG("OnDeleteGhost");
//   }

//   void OnCreateGhost(const GhostEntry& ghost_entry) {
//     GW_LOG_DEBUG("OnCreateGhost");
//   }
// };

// class MockEntityEntry : public EntityEntry {
//   GWCLASS_DECL(MockEntityEntry)
//  public:
//   const Vector3& position() {
//     return v_;
//   }

//   void PrepareDestroy() {
//     GW_LOG_DEBUG("PrepareDestroy");
//   }

//   void OnCellDestroy() {
//     GW_LOG_DEBUG("OnCellDestroy");
//   }

//  private:
//   Vector3 v_;
// };

// GW_GAME_END