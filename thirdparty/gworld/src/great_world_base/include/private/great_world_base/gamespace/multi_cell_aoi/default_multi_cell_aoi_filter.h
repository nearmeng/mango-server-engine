#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_filter.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_common.h"
// 分2段进行过滤
// 数据上：
// 1. 服务器用所有的aoi内数据维护一个订阅的优先队列
// 2. 客户端维护一个aoi内玩家的id列表
//
// 过滤流程上：
// 1. 客户端做最终的订阅操作
// 2. 服务器定期推送优先队列的头部数据，辅助客户端做决定
//
// 另外，增加一个选项，客户端的行为可以交给服务器来做
//

GW_GAME_BEGIN

typedef std::function<void(uint64_t)> AoIFilterClientCallBack;

struct DefaultFilterData : public IFilterData {
  DefaultFilterData() : IFilterData(0) {}
  DefaultFilterData(uint64_t eid, const Vector3& position) : IFilterData(eid), position_(position) {}

  Vector3 position_;
  PropLabel labels_ = 0;
  bool is_force_labels_ = false;

  bool WriteToMemoryStream(IMemoryOStream& os);
  bool ReadFromMemoryStream(IMemoryIStream& is);
};

class DefaultMultiCellAoIFilter : public IMultiCellAoIFilter {
  GWCLASS_DECL(DefaultMultiCellAoIFilter)
 public:
  DefaultMultiCellAoIFilter() = default;
  virtual ~DefaultMultiCellAoIFilter();
  DefaultMultiCellAoIFilter(uint64_t eid, const Vector3 position, uint32_t max_observe_num, bool use_client_filter) : self_filter_data_(eid, position), max_observe_num_(max_observe_num), use_client_filter_(use_client_filter) {}

  // 进aoi
  void Enter(IFilterData* data) override;
  void Enter(std::vector<IFilterData*>& data) override;

  // 出aoi
  void Leave(uint64_t eid) override;
  void Leave(std::vector<uint64_t>& eid) override;

  // 更新数据
  void Update(const IFilterData* data) override;
  void Update(std::vector<const IFilterData*>& data) override;

  void ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, PropLabel>& begin_eids) override;
  void ForceUpdateLabels(uint64_t eid, PropLabel labels) override;

  PropLabel GetLabels(uint64_t eid) override;

  bool WriteToMemoryStream(IMemoryOStream& os) override;
  bool ReadFromMemoryStream(IMemoryIStream& is) override;

  void set_use_client_filter(bool b) { use_client_filter_ = b; }

  void set_enter_client_cb(AoIFilterClientCallBack enter_client_cb) { enter_client_cb_ = enter_client_cb; }
  void set_leave_client_cb(AoIFilterClientCallBack leave_client_cb) { leave_client_cb_ = leave_client_cb; }
  void set_recommend_entity_cb(std::function<GWVector<DefaultFilterData*>(uint32_t size)> recommend_entity_cb) {
    recommend_entity_cb_ = recommend_entity_cb;
  }

  inline const GWHashMap<uint64_t, DefaultFilterData*>& observe_entities() { return observe_entities_; }


 private:
  PropLabel calc_prop_label(uint64_t eid) { return 0; }
  void update_labels(uint64_t eid, PropLabel labels, bool force = false);

 private:
  bool begin_observe_container(DefaultFilterData* data);
  void end_observe_container(uint64_t eid);

  DefaultFilterData self_filter_data_;
  uint32_t max_observe_num_ = 0;
  bool use_client_filter_ = false;  // 如果客户端决定，服务器的都绕过去，等客户端ForceObserve。服务器决定就直接设置
  GWHashMap<uint64_t, DefaultFilterData*> observe_entities_;

  AoIFilterClientCallBack enter_client_cb_;
  AoIFilterClientCallBack leave_client_cb_;
  std::function<GWVector<DefaultFilterData*>(uint32_t size)> recommend_entity_cb_;
};

GW_GAME_END