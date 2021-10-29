#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_filter.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observed.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"
#include "great_world_base/gamespace/multi_cell_aoi/default_multi_cell_aoi_filter.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_common.h"
#include "great_world_base/common/shm/shm_stl.h"

GW_GAME_BEGIN
class AoIManager;
class CellEntity;
class AoIEntity final {
 public:
  AoIEntity(uint64_t eid, AoIManager* mgr, Vector3& pos);
  ~AoIEntity();

  void Move(const Vector3& pos);
  void Move(Vector3&& pos);

  bool AddTrigger(uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave);
  void RemoveTrigger(uint32_t radius);

  void AddLabels(uint32_t new_labels, uint64_t eid);
  void RemoveLabels(uint32_t removed_labels, uint64_t eid);

  uint32_t AddNotifyData(uint32_t type, StringSlice s, uint32_t labels, uint64_t timestamp, uint32_t order_id = 0);

  StringSlice Notify();

  void ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, PropLabel>& begin_eids);
  void ForceChangeLabels(bool is_add, uint64_t eid, uint32_t labels);

  void CheckAoIData();

  bool WriteToMemoryStreamGhost(IMemoryOStream& os);
  bool ReadFromMemoryStreamGhost(IMemoryIStream& is);
  bool WriteToMemoryStreamReal(IMemoryOStream& os);
  bool ReadFromMemoryStreamReal(IMemoryIStream& is);

  // 基础aoi算法的结果
  void OnEnterBaseAoI(uint64_t self, uint64_t other, const Vector3& pos);
  void OnLeaveBaseAoI(uint64_t self, uint64_t other, const Vector3& pos);

  // 过滤器过滤的结果
  void OnFilterBeginObserver(uint64_t eid, uint32_t labels);
  void OnFilterEndObserver(uint64_t eid, uint32_t labels);

  // 
  void PrepareObservedBy(uint64_t eid, uint32_t labels);
  void DoObserverdOnTick();
  void EndObservedBy(uint64_t eid);

  bool ConvertToGhost();
  bool ConvertToReal();

 public:
  inline MultiCellAoIObserved* observed() { return observed_; }
  inline MultiCellAoIObserver* observer() { return observer_; }
  inline DefaultMultiCellAoIFilter* filter() { return filter_; }
  inline uint64_t eid() const { return eid_; }
  inline AoIManager* mgr() { return mgr_; } 
  inline CellEntity* cell_entity() { return cell_entity_; }
  inline void set_cell_entity(CellEntity* cell_entity) { cell_entity_ = cell_entity; }
  inline const GWHashSet<uint64_t>& aoi_base_all_eids() { return aoi_base_all_eids_; }
  
 private:
  uint64_t eid_;
  AoIManager* mgr_ = nullptr;
  MultiCellAoIObserved* observed_ = nullptr;
  MultiCellAoIObserver* observer_ = nullptr;
  DefaultMultiCellAoIFilter* filter_ = nullptr;

  GWHashMap<uint64_t, uint32_t> observed_padding_;
  uint64_t observed_timer_id_ = gw::TimerManager::kInvalidTimerId;
  CellEntity* cell_entity_;
  GWHashSet<uint64_t> aoi_base_all_eids_;
};

GW_GAME_END