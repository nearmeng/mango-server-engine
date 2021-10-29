#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(LabelDiffNotifyProp)

LabelDiffNotifyProp::LabelDiffNotifyProp(PropLabel new_label, GWVector<AoIOrderlyNotifyProp*> all_props) {
  new_label_ = new_label;
  all_props_ = std::move(all_props);
  ts_ = GetWalltimeMs();
}

LabelDiffNotifyProp::~LabelDiffNotifyProp() {
  for (auto& it : all_props_) {
    it->Dispose();
  }
}

LabelDiffNotifyProp::LabelDiffNotifyProp(IMemoryIStream& is) {
  is >> ts_ >> new_label_;
  std::size_t size;
  is >> size;
  for (size_t i = 0; i < size; i++) {
    all_props_.push_back(ShmNew<AoIOrderlyNotifyProp>(is));
  }
}

bool LabelDiffNotifyProp::WriteToMemoryStreamReal(IMemoryOStream& os) {
  os << ts_ << new_label_;
  os << all_props_.size();
  for (auto& it : all_props_) {
    if (!it->WriteToMemoryStreamReal(os)) {
      return false;
    }
  }
  return true;
}

// MultiCellAoIObservedActor

MultiCellAoIObservedActor::~MultiCellAoIObservedActor() {
  // 维护一个这个的队列
  // GWQueue<LabelDiffNotifyProp*> label_diff_notify_queue_;
  while (!label_diff_notify_queue_.empty()) {
    auto data = label_diff_notify_queue_.front();
    data->Dispose();
    label_diff_notify_queue_.pop();
  }
}
GWCLASS_DECL_IMPLEMENT(MultiCellAoIObservedActor)

MultiCellAoIObservedActor::MultiCellAoIObservedActor(IMemoryIStream& is) {
  is >> eid_ >> next_id_ >> labels_;
  std::size_t label_last_diff_notify_ts_size;
  is >> label_last_diff_notify_ts_size;
  for (size_t i = 0; i < label_last_diff_notify_ts_size; i++) {
    PropLabel label;
    uint64_t ts;
    is >> label >> ts;
    label_last_diff_notify_ts_[label] = ts;
  }
  std::size_t label_diff_notify_queue_size;
  is >> label_diff_notify_queue_size;
  std::queue<LabelDiffNotifyProp*> temp_queue;
  for (size_t i = 0; i < label_diff_notify_queue_size; i++) {
    temp_queue.push(ShmNew<LabelDiffNotifyProp>(is));
  }
  while (!temp_queue.empty()) {
    label_diff_notify_queue_.push(temp_queue.front());
    temp_queue.pop();
  }
}

bool MultiCellAoIObservedActor::WriteToMemoryStreamReal(IMemoryOStream& os) {
  os << eid_ << next_id_ << labels_;
  os << label_last_diff_notify_ts_.size();
  for (auto& it : label_last_diff_notify_ts_) {
    os << it.first << it.second;
  }
  os << label_diff_notify_queue_.size();
  while (!label_diff_notify_queue_.empty()) {
    auto top = label_diff_notify_queue_.front();
    label_diff_notify_queue_.pop();
    if (!top->WriteToMemoryStreamReal(os)) {
      return false;
    }
  }
  return true;
}

AoIOrderlyNotifyProp* MultiCellAoIObservedActor::TryPopDiffProp(unsigned long ts) {
  if (label_diff_notify_queue_.empty()) {
    return nullptr;
  }
  auto top = label_diff_notify_queue_.front();
  if (ts == 0 || top->ts_ < ts) {
    labels_ |= top->new_label_;
    AoIOrderlyNotifyProp* ret = nullptr;
    if (!top->all_props_.empty()) {
      ret = top->all_props_.back();
      top->all_props_.pop_back();
    }
    if (top->all_props_.empty()) {
      label_diff_notify_queue_.pop();
    }
    return ret;
  }
  return nullptr;
}

// MultiCellAoIObserver
void MultiCellAoIObserver::AddLabels(PropLabel new_labels, uint64_t eid) {
  auto iter = observed_actor_ids_.find(eid); 
  if (iter == observed_actor_ids_.end()) {
    GW_LOG_INFO("AddLabels fail:%lu observe %lu", owner_id(), eid);
    return;
  }
  auto observed = iter->second;
  auto diff_labels = ((~observed->labels_) & new_labels);
  // 检查 cd，有些可以从diff里面刨出去
  auto now = GetWalltimeMs();
  for (auto iter = observed->label_last_diff_notify_ts_.begin(); iter != observed->label_last_diff_notify_ts_.end(); iter++) {
    if ((iter->first & diff_labels) && (iter->second + AOI_WORLD.AoIConf().observer_label_check_ms_cd() < now)) {
      diff_labels &= (~iter->first);
    }
  }
  // 找出属性的label 仅 在 new_labels 中出现的
  // 遍历所有属性，找出符合的属性去同步。复杂度 label数量*属性数量
  // PropLabel diff_labels, PropLabel new_labels, uint64_t eid
  GWVector<AoIDiffProp>&& diff_props = AOI_WORLD.GetDiffProps(eid, diff_labels);
  GWVector<AoIOrderlyNotifyProp*> ret;
  for(auto& it:diff_props) {
    ret.push_back(ShmNew<AoIOrderlyNotifyProp>(0, it.type_, it.data_.data(), it.data_.size(), it.labels_, 0));
  }
  LabelDiffNotifyProp* new_label_diff_prop = ShmNew<LabelDiffNotifyProp>(new_labels, ret);
  observed->label_diff_notify_queue_.push(new_label_diff_prop);
}

void MultiCellAoIObserver::RemoveLabels(PropLabel removed_labels, uint64_t eid) {
  auto iter = observed_actor_ids_.find(eid); 
  if (iter == observed_actor_ids_.end()) {
    GW_LOG_DEBUG("RemoveLabels fail:%lu observe %lu", owner_id(), eid);
    return;
  }
  auto observed = iter->second;
  observed->labels_ &= (~removed_labels);
}

bool MultiCellAoIObserver::BeginObserve(MultiCellAoIObserved* observed, PropLabel labels) {
  if (observed_actor_ids_.find(observed->owner_id()) != observed_actor_ids_.end()) {
    GW_LOG_ERROR("%lu BeginObserveActor %lu repeat,%p", owner_id(), observed->owner_id(), this);
    return false;
  }
  if (observed_actor_ids_.size() >= AOI_WORLD.AoIConf().observer_entity_max_num()) {
    GW_LOG_ERROR("BeginObserveActor_ERROR:%ld,%lu,%d,%d", owner_id(),  observed->owner_id(), observed_actor_ids_.size(), AOI_WORLD.AoIConf().observer_entity_max_num());
    return false;
  }
  MultiCellAoIObservedActor* new_observed_actor = ShmNew<MultiCellAoIObservedActor>(observed->next_id(), observed->owner_id(), this, labels);
  new_observed_actor->next_id_ = observed->next_id();
  auto it = observed_actor_ids_.find(observed->owner_id());
  if(it != observed_actor_ids_.end()) {
    it->second->Dispose();
    it->second = new_observed_actor;
  }
  else {
    observed_actor_ids_.emplace(observed->owner_id(), new_observed_actor);
  }
  observed->BeginObserveBy(owner_id());
  GW_LOG_DEBUG("%lu BeginObserveActor %lu, size %d", owner_id(), observed->owner_id(), observed_actor_ids_.size());
  return true;
}

void MultiCellAoIObserver::EndObserve(uint64_t eid) {
  auto iter = observed_actor_ids_.find(eid);
  if (observed_actor_ids_.find(eid) == observed_actor_ids_.end()) {
    return;
  }
  iter->second->Dispose();
  observed_actor_ids_.erase(eid);
  auto observed = get_observed_by_eid(eid);
  if(observed)
    observed->EndObserveBy(owner_id());
  GW_LOG_DEBUG("EndObserveActor:%lu remove %lu, size %d", owner_id(), eid, observed_actor_ids_.size());
}

MultiCellAoIObserver::~MultiCellAoIObserver() {
  for (auto& it : observed_actor_ids_) {
    delete it.second;
  }
}

bool MultiCellAoIObserver::WriteToMemoryStream(IMemoryOStream& os) {
  os << observed_actor_ids_.size();
  for (auto& it : observed_actor_ids_) {
    it.second->WriteToMemoryStreamReal(os);
  }
  return true;
}

bool MultiCellAoIObserver::ReadFromMemoryStream(IMemoryIStream& is) {
  std::size_t tmp_size;
  is >> tmp_size;
  for (size_t i = 0; i < tmp_size; i++) {
    auto ptr = ShmNew<MultiCellAoIObservedActor>(is);
    auto it = observed_actor_ids_.find(ptr->eid_);
    if(it != observed_actor_ids_.end()) {
      it->second->Dispose();
      it->second = ptr;
    }
    else {
      observed_actor_ids_.emplace(ptr->eid_, ptr);
    }
  }
  return true;
}

uint64_t MultiCellAoIObserver::owner_id() { 
  return owner_->eid();
}

MultiCellAoIObserved* MultiCellAoIObserver::get_observed_by_eid(uint64_t eid) {
  auto other = reinterpret_cast<AoIEntity*>(owner_->mgr()->GetAoIEntity(eid));
  if(other) {
    return other->observed();
  }
  else {
    return nullptr;
  }
}

GW_GAME_END