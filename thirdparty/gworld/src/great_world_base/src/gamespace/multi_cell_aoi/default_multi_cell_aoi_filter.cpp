#include "great_world_base/gamespace/multi_cell_aoi/default_multi_cell_aoi_filter.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN

bool DefaultFilterData::WriteToMemoryStream(IMemoryOStream& os) {
  os << eid_ << position_.x << position_.y << position_.z << labels_ << is_force_labels_;
  return true;
}
bool DefaultFilterData::ReadFromMemoryStream(IMemoryIStream& is) {
  is >> eid_ >> position_.x >> position_.y >> position_.z >> labels_ >> is_force_labels_;
  return true;
}

/////////////////////
GWCLASS_DECL_IMPLEMENT(DefaultMultiCellAoIFilter)

bool DefaultMultiCellAoIFilter::begin_observe_container(DefaultFilterData* data) {
  if(!data) return false;
  auto it = observe_entities_.find(data->eid_);
  if(it != observe_entities_.end()) {
    delete data;
    GW_LOG_DEBUG("begin_observe_container repeat:self %lu, other %lu", self_filter_data_.eid_, data->eid_);
    return false;
  }
  if (begin_observer_cb_) {
    begin_observer_cb_(data->eid_, data->labels_);
  }
  observe_entities_.emplace(data->eid_, data);
  GW_LOG_DEBUG("begin_observe_container self %lu, other %lu, size %d", self_filter_data_.eid_, data->eid_, observe_entities_.size());
  return true;
}

void DefaultMultiCellAoIFilter::end_observe_container(uint64_t eid) {
  auto it = observe_entities_.find(eid);
  if(it == observe_entities_.end()) return;
  auto data = it->second;
  if (end_observer_cb_) {
    end_observer_cb_(data->eid_, data->labels_);
  }
  observe_entities_.erase(data->eid_);
  delete data;
  GW_LOG_DEBUG("end_observe_container self %lu, other %lu, size %d", self_filter_data_.eid_, data->eid_, observe_entities_.size());
}

DefaultMultiCellAoIFilter::~DefaultMultiCellAoIFilter() {
  for (auto& it : observe_entities_) {
    delete it.second;
  }
  observe_entities_.clear();
}

void DefaultMultiCellAoIFilter::Enter(IFilterData* data) {
  if (!data) {
    return;
  }
  GW_LOG_DEBUG("%llu filter enter %llu", self_filter_data_.eid_, data->eid_);
  DefaultFilterData* real_data = dynamic_cast<DefaultFilterData*>(data);
  if (!real_data) {
    delete data;
    return;
  }
  real_data->labels_ = calc_prop_label(data->eid_);

  // 计算 observe_list
  if (use_client_filter_) {
    if (enter_client_cb_)
      enter_client_cb_(data->eid_);
    delete data;
  } else {
    if (observe_entities_.size() < max_observe_num_) {
      begin_observe_container(real_data);
    } else {
      delete data;
    }
  }
}

void DefaultMultiCellAoIFilter::Enter(std::vector<IFilterData*>& data) {
  for (auto& it : data) {
    Enter(it);
  }
}

void DefaultMultiCellAoIFilter::Leave(uint64_t eid) {
  GW_LOG_DEBUG("%llu filter leave %llu", eid, self_filter_data_.eid_);
  end_observe_container(eid);

  // 计算 observe_list
  if (use_client_filter_) {
    if (leave_client_cb_)
      leave_client_cb_(eid);
  } else {
    if (begin_observer_cb_ && observe_entities_.size() < max_observe_num_) {
      if (recommend_entity_cb_) {
        auto recommend_datas = recommend_entity_cb_(max_observe_num_ - observe_entities_.size());
        for(auto filter_data:recommend_datas) {
          // 获取应用层推荐
          if (filter_data) {
            begin_observe_container(filter_data);
          }
        }
      }
    }
  }
}

void DefaultMultiCellAoIFilter::Leave(std::vector<uint64_t>& data) {
  for (auto& it : data) {
    Leave(it);
  }
}

void DefaultMultiCellAoIFilter::Update(const IFilterData* data) {
  const DefaultFilterData* real_data = dynamic_cast<const DefaultFilterData*>(data);
  if (!real_data) {
    return;
  }
  auto it = observe_entities_.find(data->eid_);
  if (it == observe_entities_.end()) {
    return;
  }
  auto new_labels = calc_prop_label(real_data->eid_);
  update_labels(real_data->eid_, new_labels);
}

void DefaultMultiCellAoIFilter::Update(std::vector<const IFilterData*>& data) {
  for (auto& it : data) {
    Update(it);
  }
}

void DefaultMultiCellAoIFilter::update_labels(uint64_t eid, PropLabel new_labels, bool force) {
  // 如果有 force labels 就按照这个走，否则在去计算服务器 label
  auto it = observe_entities_.find(eid);
  if (it == observe_entities_.end()) {
    return;
  }
  auto real_data = it->second;
  if (!force && real_data->is_force_labels_) {
    return;
  }

  auto add_labels = (new_labels | real_data->labels_) - real_data->labels_;
  auto remove_labels = real_data->labels_ - (new_labels & real_data->labels_);
  if (add_labels) {
    add_labels_cb_(eid, add_labels);
  }
  if (remove_labels) {
    remove_labels_cb_(eid, remove_labels);
  }

  real_data->is_force_labels_ = force;
  real_data->labels_ = new_labels;
}

void DefaultMultiCellAoIFilter::ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, PropLabel>& begin_eids) {
  // 先end
  for (auto& eid : end_eids) {
    end_observe_container(eid);
  }

  // 根据begin的数量end一些
  int32_t del_num = static_cast<int32_t>(observe_entities_.size()) + static_cast<int32_t>(begin_eids.size()) - static_cast<int32_t>(max_observe_num_);
  while (del_num > 0 && observe_entities_.size() > 0) {
    del_num--;
    auto find_observe_it = observe_entities_.begin();
    if (find_observe_it == observe_entities_.end()) {
      continue;
    }
    end_observe_container(find_observe_it->first);
  }

  // 再begin
  for (auto& eid2labels : begin_eids) {
    if (observe_entities_.size() > max_observe_num_) break;
    auto filter_data = new DefaultFilterData();
    filter_data->eid_ = eid2labels.first;
    if(begin_observe_container(filter_data))
      update_labels(eid2labels.first, eid2labels.second);
  }

  // 如果还有空就补进去一些
  if (recommend_entity_cb_) {
    auto recommend_datas = recommend_entity_cb_(max_observe_num_ - observe_entities_.size());
    for (auto filter_data : recommend_datas) {
      // 获取应用层推荐
      if (filter_data) {
        begin_observe_container(filter_data);
      }
    }
  }
}

void DefaultMultiCellAoIFilter::ForceUpdateLabels(uint64_t eid, PropLabel labels) {
  update_labels(eid, labels);
}

PropLabel DefaultMultiCellAoIFilter::GetLabels(uint64_t eid) {
  auto it = observe_entities_.find(eid);
  if (it == observe_entities_.end())
    return 0;
  return it->second->labels_;
}

bool DefaultMultiCellAoIFilter::WriteToMemoryStream(IMemoryOStream& os) {
  // self_filter_data_
  self_filter_data_.WriteToMemoryStream(os);
  // observe_entities_
  os << observe_entities_.size();
  for (auto& it : observe_entities_) {
    it.second->WriteToMemoryStream(os);
  }
  // max_observe_num_
  os << max_observe_num_;
  // use_client_filter_
  os << use_client_filter_;
  GW_LOG_DEBUG("write use_client_filter_ %d", use_client_filter_);
  return true;
}

bool DefaultMultiCellAoIFilter::ReadFromMemoryStream(IMemoryIStream& is) {
  std::size_t temp_size;
  uint64_t temp_eid;

  // self_filter_data_
  self_filter_data_.ReadFromMemoryStream(is);
  // observe_entities_
  is >> temp_size;
  for (std::size_t i = 0; i != temp_size; i++) {
    DefaultFilterData* temp_data = new DefaultFilterData();
    temp_data->ReadFromMemoryStream(is);
    observe_entities_.emplace(temp_data->eid_, temp_data);
  }
  // max_observe_num_
  is >> max_observe_num_;
  // use_client_filter_
  is >> use_client_filter_;
  GW_LOG_DEBUG("read use_client_filter_ %d", use_client_filter_);
  return true;
}

GW_GAME_END