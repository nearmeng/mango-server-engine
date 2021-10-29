#pragma once
#include <functional>
#include "great_world_base/gamespace/space/space_common.h"

GW_GAME_BEGIN

// 整个分布式aoi分三层
// 1. aoi 算法，触发enter、leave、onmove事件，维护被谁感兴趣的列表
// 2. filter 更新数据，触发observe的begin和end，维护观察列表
// 3. observer/observed，做数据同步

typedef std::function<void(uint64_t eid, uint32_t labels)> MultiCellAoIFilterCallBack;

struct IFilterData {
  virtual ~IFilterData() {}
  IFilterData(uint64_t eid) : eid_(eid) {}

  uint64_t eid_;
};

class IMultiCellAoIFilter {
 public:
  virtual ~IMultiCellAoIFilter() {}

  // 进aoi
  virtual void Enter(IFilterData* data) = 0;
  virtual void Enter(std::vector<IFilterData*>& data) = 0;

  // 出aoi
  virtual void Leave(uint64_t eid) = 0;
  virtual void Leave(std::vector<uint64_t>& eid) = 0;

  // 更新数据
  virtual void Update(const IFilterData* data) = 0;
  virtual void Update(std::vector<const IFilterData*>& data) = 0;

  // 外部修改 observe 数据的接口
  virtual void ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, uint32_t>& begin_eids) = 0;
  virtual void ForceUpdateLabels(uint64_t eid, uint32_t labels) = 0;

  virtual uint32_t GetLabels(uint64_t eid) = 0;

  // 序列化
  virtual bool WriteToMemoryStream(IMemoryOStream& os) = 0;
  virtual bool ReadFromMemoryStream(IMemoryIStream& is) = 0;

  // 设置 callback 数据
  void set_begin_observer_cb(MultiCellAoIFilterCallBack begin_observer_cb) { begin_observer_cb_ = begin_observer_cb; }
  void set_end_observer_cb(MultiCellAoIFilterCallBack end_observer_cb) { end_observer_cb_ = end_observer_cb; }

  void set_add_labels_cb(MultiCellAoIFilterCallBack add_labels_cb) { add_labels_cb_ = add_labels_cb; }
  void set_remove_labels_cb(MultiCellAoIFilterCallBack remove_labels_cb) { remove_labels_cb_ = remove_labels_cb; }

  void Dispose() { delete this; }

 protected:
  MultiCellAoIFilterCallBack begin_observer_cb_;
  MultiCellAoIFilterCallBack end_observer_cb_;
  MultiCellAoIFilterCallBack add_labels_cb_;
  MultiCellAoIFilterCallBack remove_labels_cb_;
};

GW_GAME_END