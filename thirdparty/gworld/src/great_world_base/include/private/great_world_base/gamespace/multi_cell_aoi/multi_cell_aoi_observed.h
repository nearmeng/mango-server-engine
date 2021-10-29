#pragma once

#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_common.h"
#include "great_world_base/gamespace/multi_cell_aoi/notify_ring_buffer.h"

GW_GAME_BEGIN

class MultiCellAoIObserver;

class MultiCellAoIObserved {
 public:
  GWCLASS_DECL(MultiCellAoIObserved)
  MultiCellAoIObserved() = default;

  MultiCellAoIObserved(uint64_t owner_id);
  virtual ~MultiCellAoIObserved();
  bool WriteToMemoryStream(IMemoryOStream& os);
  bool ReadFromMemoryStream(IMemoryIStream& is);

  bool AddOrderlyNotifyProp(AoIOrderlyNotifyProp* orderly_notify);
  void GetNotifyPropById(AoINotifyId beginId, uint32_t size, GWVector<const AoIOrderlyNotifyProp*>* props);
  const AoIOrderlyNotifyProp* GetNotifyPropById(AoINotifyId id);

  inline AoINotifyId next_id() {
    return notify_ring_buffer_.next_id();
  }

  inline AoINotifyId max_id() {
    return notify_ring_buffer_.max_id();
  }

  inline uint64_t owner_id() { return owner_id_; }
  inline const GWHashSet<uint64_t>& get_observer_ids() { return observer_ids_; }
  inline void BeginObserveBy(uint64_t eid) { observer_ids_.insert(eid); }
  inline void EndObserveBy(uint64_t eid) { observer_ids_.erase(eid); }
  bool NotifyBufferAdded();

 private:
  GWHashSet<uint64_t> observer_ids_;  // 被谁关注
  bool buffer_added_ = false;          // 距离上一次检查是否有数据增加

  uint64_t owner_id_;
  NotifyRingBuffer notify_ring_buffer_;
  GWHashMap<AoINotifyId, AoIOrderlyNotifyProp*> wait_add_map_;  // 乱序的节点们
};

GW_GAME_END