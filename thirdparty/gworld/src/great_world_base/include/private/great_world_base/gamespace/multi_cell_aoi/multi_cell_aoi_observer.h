#pragma once
#include "great_world_base/common/memory_stream.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_common.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observed.h"
#include "great_world_base/common/simple_simd_hash_map.hpp"

GW_GAME_BEGIN
struct LabelDiffNotifyProp {
  friend class MultiCellAoIObserver;
  friend class MultiCellAoIObservedActor;

 public:
  GWCLASS_DECL(LabelDiffNotifyProp)
  LabelDiffNotifyProp() = default;
  ~LabelDiffNotifyProp();
  LabelDiffNotifyProp(PropLabel new_label, GWVector<AoIOrderlyNotifyProp*> all_props);
  LabelDiffNotifyProp(IMemoryIStream& is);
  bool WriteToMemoryStreamReal(IMemoryOStream& os);

 private:
  GWVector<AoIOrderlyNotifyProp*> all_props_;
  uint64_t ts_;
  PropLabel new_label_;
};

class MultiCellAoIObservedActor {
  friend class MultiCellAoIObserver;

 public:
  GWCLASS_DECL(MultiCellAoIObservedActor)
  MultiCellAoIObservedActor() = default;
  MultiCellAoIObservedActor(AoINotifyId next_id, uint64_t eid, MultiCellAoIObserver* parent, PropLabel labels) : eid_(eid), next_id_(next_id), labels_(labels) {
  }
  MultiCellAoIObservedActor(IMemoryIStream& is);
  virtual ~MultiCellAoIObservedActor();
  bool WriteToMemoryStreamReal(IMemoryOStream& os);
  AoIOrderlyNotifyProp* TryPopDiffProp(unsigned long ts);

  inline uint64_t eid() const { return eid_; }
  inline uint64_t next_id() const { return next_id_; }
  inline PropLabel labels() const { return labels_; }
  inline void next_add() { next_id_++; }

 private:
  uint64_t eid_;
  AoINotifyId next_id_;
  PropLabel labels_;  // 上次做同步的时候观察者的 label
  // todo evanyu 这个实现是为了给label增删加个cd，防止频繁的改label影响性能，
  // 但是的有点问题，要改掉，可能会重复同步数据
  // 而且label的同步id可以用哪个 order_id 表示，别用时间
  GWHashMap<PropLabel, uint64_t> label_last_diff_notify_ts_;
  GWQueue<LabelDiffNotifyProp*> label_diff_notify_queue_;
};

class AoIEntity;
class MultiCellAoIObserver {
 public:
  MultiCellAoIObserver() {}
  MultiCellAoIObserver(AoIEntity* owner, IMemoryOStream* os) : owner_(owner), notify_stream_(os) {}

  virtual void Dispose() {}

  virtual ~MultiCellAoIObserver();

  // 同步
  virtual StringSlice NotifyProp() = 0;
  virtual void OnObservedDataAdded(uint64_t observed_eid) {}

  // observed 管理
  virtual bool BeginObserve(MultiCellAoIObserved* observed, PropLabel labels);
  virtual void EndObserve(uint64_t eid);

  // labels 管理
  virtual void AddLabels(PropLabel new_labels, uint64_t eid);
  virtual void RemoveLabels(PropLabel removed_labels, uint64_t eid);

  // 迁移
  virtual bool WriteToMemoryStream(IMemoryOStream& os);
  virtual bool ReadFromMemoryStream(IMemoryIStream& is);

  inline const QUICK_HASH_MAP<uint64_t, MultiCellAoIObservedActor*>& observed_actor_ids() { return observed_actor_ids_; }
  uint64_t owner_id();

  inline IMemoryOStream& os() { return *notify_stream_; }
  void SetNotifyStream(IMemoryOStream* stream) { notify_stream_ = stream; }

  MultiCellAoIObserved* get_observed_by_eid(uint64_t);

 protected:
  QUICK_HASH_MAP<uint64_t, MultiCellAoIObservedActor*> observed_actor_ids_;
  AoIEntity* owner_;
  IMemoryOStream* notify_stream_;
};

GW_GAME_END