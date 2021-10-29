#pragma once

#include "great_world_base/gamespace/cell_entity/cell_entity_interface.h"
#include "great_world_base/gamespace/cell_entity/real_ghost/entity_entry.h"
#include "great_world_base/common/simple_simd_hash_map.hpp"
#include "great_world_base/common/common_private.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/gamespace/cell_entity/mail_box.h"

GW_GAME_BEGIN

class Space;

class CellEntity final : public ICellEntity  {
  GWCLASS_DECL(CellEntity)
 public:
  CellEntity() : entity_entry_(this), ICellEntity(nullptr) {}
  CellEntity(uint64_t eid, Space* space, ICellEntityAdapter* adapter, IMemoryIStream& is);
  CellEntity(uint64_t eid, uint64_t base_eid, uint64_t base_gapp_id, Vector3 pos, Space* space, ICellEntityAdapter* adapter)
      : ICellEntity(adapter), eid_(eid), base_eid_(base_eid), base_gapp_id_(base_gapp_id), entity_entry_(this), pos_(pos), space_(space) {}

  uint64_t GetEid() const override { return eid_; }
  uint64_t GetBaseEid() const override { return base_eid_; }
  uint64_t GetBaseGappId() const override { return base_gapp_id_; }
  uint64_t GetSpaceId() const override;

  bool IsGhost() const { return entity_entry_.is_ghost(); }
  void Move(const Vector3& pos) override;

  EntityEntry* entity_entry() { return &entity_entry_; }
  const Vector3& pos() const { return pos_; }
  void set_pos(const Vector3& new_pos) { pos_ = new_pos; }

  Space* space() { return space_; }
  void set_space(Space* space) { space_ = space; }

  void GetRealEntityMailBox(EntityMailBox& mailbox) override;
  void GetGhostEntityMailBoxes(std::vector<EntityMailBox>& mailboxes) override;
  
  void NotifyRealGappId() override;

  StringSlice Notify() override {
    return aoi_entity_->Notify();
  }

  inline void ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, uint32_t>& begin_eids) override {
    aoi_entity_->ForceObserve(end_eids, begin_eids);
  }

  inline void ForceChangeLabels(bool is_add, uint64_t eid, uint32_t labels) override {
    aoi_entity_->ForceChangeLabels(is_add, eid, labels);
  }

  RetCode NotifyData(uint8_t notify_flag, uint32_t data_type, StringSlice raw_data, uint32_t label) override;
  void OnNotifyData(MemoryIStream& is);

  inline bool AddTrigger(uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave) override {
    return aoi_entity_->AddTrigger(radius, on_enter, on_leave);
  }

  inline void RemoveTrigger(uint32_t radius) override {
    aoi_entity_->RemoveTrigger(radius);
  }

  inline void AddLabels(uint32_t new_labels, uint64_t eid) override {
    return aoi_entity_->AddLabels(new_labels, eid);
  }

  inline void RemoveLabels(uint32_t removed_labels, uint64_t eid) override {
    return aoi_entity_->AddLabels(removed_labels, eid);
  }

  inline void CheckAoIData() { aoi_entity_->CheckAoIData(); }

  inline void set_aoi_entity(AoIEntity* aoi_entity) {
    aoi_entity_ = aoi_entity;
  }

  bool Teleport(const uint64_t gapp_id, const uint64_t space_id, float x, float y, float z) override;

  void SetParallelVisible(bool always_visible) { entity_entry_.real_entry()->SetParallelVisible(always_visible); }

  bool InnerWriteToMemoryStreamGhost(IMemoryOStream& os);
  bool InnerReadFromMemoryStreamGhost(IMemoryIStream& is);
  bool InnerWriteToMemoryStreamReal(IMemoryOStream& os);
  bool InnerReadFromMemoryStreamReal(IMemoryIStream& is);

  bool ConvertToGhost();
  bool ConvertToReal();

  void ConfirmMigrateOut(uint64_t gapp_id) override;
  void ConfirmMigrateIn() override;

  void MoveMigrateStream(MemoryIStream& is) {
    is.MoveLeftTo(migrate_tmp_data_); 
  }

 private:
  uint64_t eid_;
  uint64_t base_eid_;
  uint64_t base_gapp_id_;
  EntityEntry entity_entry_;
  Vector3 pos_;
  Space* space_;
  AoIEntity* aoi_entity_;
  GWVector<char> migrate_tmp_data_;
};

class CellEntities {
 public:
  RetCode AddEntity(CellEntity* cell_entity);
  void DelEntity(uint64_t eid);
  CellEntity* GetCellEntity(uint64_t eid);

 private:
  QUICK_HASH_MAP<uint64_t, CellEntity*> cell_entities_;  
};

GW_GAME_END