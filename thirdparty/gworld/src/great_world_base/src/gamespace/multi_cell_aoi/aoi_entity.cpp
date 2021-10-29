#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/common_private.h"
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_fast_notify.h"
#include "great_world_base/gamespace/cell_entity/cell_entity.h"

GW_GAME_BEGIN

AoIEntity::AoIEntity(uint64_t eid, AoIManager* mgr, Vector3& pos) : eid_(eid), mgr_(mgr) {
  observed_ = ShmNew<MultiCellAoIObserved>(eid_);
  observed_timer_id_ = gw::TimerManager::Get()->CreateTimer(std::bind(&AoIEntity::DoObserverdOnTick, this), 100, true);
}

AoIEntity::~AoIEntity() {
  gw::TimerManager::Get()->DeleteTimer(observed_timer_id_);
  if (observed_)
    observed_->Dispose();
  if (observer_)
    observer_->Dispose();
  if(filter_) 
    filter_->Dispose();
}

void AoIEntity::Move(const Vector3& pos_tmp) {
  Vector3 pos = pos_tmp;
  pos.z = pos.y;
  mgr_->Move(eid_, pos);
  if(cell_entity_->IsGhost()) return;
  auto tmp_data = new DefaultFilterData(eid_, pos);
  filter_->Update(tmp_data);
  delete tmp_data;
}

void AoIEntity::Move(Vector3&& pos) {
  Move(pos);
}

bool AoIEntity::AddTrigger(uint32_t radius, AOIFUNC on_enter, AOIFUNC on_leave) {
  return mgr_->AddTrigger(eid_, radius, on_enter, on_leave);
}

void AoIEntity::RemoveTrigger(uint32_t radius) {
  mgr_->RemoveTrigger(eid_, radius);
}

void AoIEntity::AddLabels(uint32_t new_labels, uint64_t other_eid) {
  if(cell_entity_->IsGhost()) return;
  observer_->AddLabels(new_labels, other_eid);
}

void AoIEntity::RemoveLabels(uint32_t removed_labels, uint64_t other_eid) {
  if(cell_entity_->IsGhost()) return;
  observer_->RemoveLabels(removed_labels, other_eid);
}

uint32_t AoIEntity::AddNotifyData(uint32_t type, StringSlice s, uint32_t labels, uint64_t timestamp, uint32_t order_id) {
  AoIOrderlyNotifyProp* prop;
  if (!order_id) {
    order_id = observed_->next_id();
  }
  prop = gw::ShmNew<AoIOrderlyNotifyProp>(order_id, type, s.data(), s.size(), labels, timestamp);
  if(!observed_->AddOrderlyNotifyProp(prop)) {
    order_id = 0;
  }
  return order_id;
}

StringSlice AoIEntity::Notify() {
  return observer_->NotifyProp();
}

void AoIEntity::ForceObserve(GWVector<uint64_t>& end_eids, GWHashMap<uint64_t, PropLabel>& begin_eids) {
  if(cell_entity_->IsGhost()) return;
  filter_->ForceObserve(end_eids, begin_eids);
}

void AoIEntity::ForceChangeLabels(bool is_add, uint64_t other_eid, uint32_t labels) {
  if(cell_entity_->IsGhost()) return;
  if(is_add) {
    labels |= filter_->GetLabels(other_eid);
  }
  else {
    labels &= filter_->GetLabels(other_eid);
  }
  filter_->ForceUpdateLabels(other_eid, labels);
}

void AoIEntity::OnEnterBaseAoI(uint64_t self, uint64_t other, const Vector3& pos) {
  aoi_base_all_eids_.insert(other);
  if(cell_entity_->IsGhost()) return;
  GW_LOG_TRACE("OnEnterBaseAoI, self %lu, other %lu", self, other);
  DefaultFilterData* filter_data = new DefaultFilterData(other, pos);
  filter_->Enter(filter_data);
}

void AoIEntity::OnLeaveBaseAoI(uint64_t self, uint64_t other, const Vector3& pos) {
  aoi_base_all_eids_.erase(other);
  if(cell_entity_->IsGhost()) return;
  GW_LOG_TRACE("OnLeaveBaseAoI, self %lu, other %lu", self, other);
  filter_->Leave(other);
}

void AoIEntity::OnFilterBeginObserver(uint64_t other_eid, uint32_t labels) {
  if(cell_entity_->IsGhost()) return;
  auto other = reinterpret_cast<AoIEntity*>(mgr_->GetAoIEntity(other_eid));
  if(other) {
    other->PrepareObservedBy(eid_, labels);
  }
}

void AoIEntity::OnFilterEndObserver(uint64_t other_eid, uint32_t labels) {
  if(cell_entity_->IsGhost()) return;
  auto other = reinterpret_cast<AoIEntity*>(mgr_->GetAoIEntity(other_eid));
  if(other) {
    other->EndObservedBy(eid_);
  }
  observer_->EndObserve(other_eid);
  AOI_WORLD.OnEndObserver(eid_, other_eid);
}

void AoIEntity::PrepareObservedBy(uint64_t other_eid, uint32_t labels) {
  observed_padding_.emplace(other_eid, labels);
}

void AoIEntity::DoObserverdOnTick() {
  GWVector<uint64_t> final_observe;
  for(auto& other_it:observed_padding_) {
    auto other = reinterpret_cast<AoIEntity*>(mgr_->GetAoIEntity(other_it.first));
    if(!other || !other->observer()) {
      continue;
    }
    if(other->observer()->BeginObserve(observed_, other_it.second)) {
      final_observe.push_back(other_it.first);
    }
    else {
      GW_LOG_ERROR("%lu observer %lu fail", other_it.first, eid_);
    }
  }
  if(!final_observe.empty())
    AOI_WORLD.OnBeginObserver(final_observe, eid_);
  observed_padding_.clear();
}

void AoIEntity::EndObservedBy(uint64_t other_eid) {
  if(observed_padding_.erase(other_eid) > 0) {
    return;
  }
}

bool AoIEntity::WriteToMemoryStreamGhost(IMemoryOStream& os) {
  return observed_->WriteToMemoryStream(os);
}

bool AoIEntity::ReadFromMemoryStreamGhost(IMemoryIStream& is) {
  return observed_->ReadFromMemoryStream(is);
}

bool AoIEntity::WriteToMemoryStreamReal(IMemoryOStream& os) {
  return observer_->WriteToMemoryStream(os) && filter_->WriteToMemoryStream(os);
}

void AoIEntity::CheckAoIData() {
  // 以aoi_base的数据为基准检查 filter 的数据
  GWVector<uint64_t> filter_to_delete;
  for(auto& it:filter_->observe_entities()) {
    if(aoi_base_all_eids_.find(it.first) == aoi_base_all_eids_.end()) {
      filter_to_delete.push_back(it.first);
    }
  }
  GWHashMap<uint64_t, PropLabel> begin_eids;
  filter_->ForceObserve(filter_to_delete, begin_eids);

  // 以 filter 的数据为基准检查 observer 的数据
  auto& filter_observe = filter_->observe_entities();
  GWVector<uint64_t> observer_to_delete;
  for(auto& it:observer_->observed_actor_ids()) {
    if(filter_observe.find(it.first) == filter_observe.end()) {
      observer_to_delete.push_back(it.first);
    }
  }
  for(auto it:observer_to_delete) {
    observer_->EndObserve(it);
  }
}

bool AoIEntity::ReadFromMemoryStreamReal(IMemoryIStream& is) {
  return observer_->ReadFromMemoryStream(is) && filter_->ReadFromMemoryStream(is);
}

bool AoIEntity::ConvertToGhost() {
  if(filter_) {
    filter_->Dispose();
    filter_ = nullptr;
  }
  if(observer_) {
    observer_->Dispose();
    observer_ = nullptr;
  }
  return true;
}

bool AoIEntity::ConvertToReal() {
  if(filter_ || observer_) {
    return false;
  }
  filter_ = ShmNew<DefaultMultiCellAoIFilter>(eid_, cell_entity_->pos(), AOI_WORLD.AoIConf().observer_entity_max_num(), false);
  GW_LOG_DEBUG("observer_entity_max_num:%d", AOI_WORLD.AoIConf().observer_entity_max_num());
  filter_->set_begin_observer_cb(std::bind(&AoIEntity::OnFilterBeginObserver, this, std::placeholders::_1, std::placeholders::_2));
  filter_->set_end_observer_cb(std::bind(&AoIEntity::OnFilterEndObserver, this, std::placeholders::_1, std::placeholders::_2));
  observer_ = ShmNew<MultiCellAoiFastNotify>(this, AOI_WORLD.NotifyOS());
  return true;
}

GW_GAME_END