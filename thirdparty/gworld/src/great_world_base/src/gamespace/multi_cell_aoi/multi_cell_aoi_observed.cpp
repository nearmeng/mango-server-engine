#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observed.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(MultiCellAoIObserved)

MultiCellAoIObserved::MultiCellAoIObserved(uint64_t owner_id) {
  owner_id_ = owner_id;
  notify_ring_buffer_.Resize(AOI_WORLD.AoIConf().observed_prop_buffer_max_num());
}

MultiCellAoIObserved::~MultiCellAoIObserved() {
  for (auto& it : wait_add_map_) {
    it.second->Dispose();
  }
}

bool MultiCellAoIObserved::WriteToMemoryStream(IMemoryOStream& os) {
  // wait_add_map_

  // bool buffer_added_
  os << buffer_added_;
  // GWHashSet<uint64_t> observer_ids_
  os << observer_ids_.size();
  for (auto& v : observer_ids_) {
    os << v;
  }
  // notify_ring_buffer_
  return notify_ring_buffer_.WriteToMemoryStreamReal(os);
}

bool MultiCellAoIObserved::ReadFromMemoryStream(IMemoryIStream& is) {
  // wait_add_map_

  // bool buffer_added_
  bool tmp_buffer_added;
  is >> tmp_buffer_added;
  buffer_added_ = (buffer_added_ | tmp_buffer_added);
  // GWHashSet<uint64_t> observer_ids_
  observer_ids_.clear();
  std::size_t observer_ids_size;
  is >> observer_ids_size;
  for (size_t i = 0; i < observer_ids_size; i++) {
    uint64_t tmp_eid;
    is >> tmp_eid;
    observer_ids_.insert(tmp_eid);
  }

  // notify_ring_buffer_
  return notify_ring_buffer_.ReadFromMemoryStreamReal(is);
}

bool MultiCellAoIObserved::AddOrderlyNotifyProp(AoIOrderlyNotifyProp* orderly_notify_prop) {
  if (next_id() != orderly_notify_prop->id_) {
    GW_LOG_INFO("AddOrderlyNotifyProp_ERROR_INDEX: eid %lu, next_id %d, id %d, prop_addr %p", owner_id_, next_id(), orderly_notify_prop->id_, orderly_notify_prop);
    if (wait_add_map_.size() >= AOI_WORLD.AoIConf().observed_prop_max_wait_num()) {
      GW_LOG_ERROR("AddOrderlyNotifyProp_ERROR_INDEX:%d,%d,%d", owner_id_, next_id(), orderly_notify_prop->id_);
      return false;
    }
    wait_add_map_[orderly_notify_prop->id_] = orderly_notify_prop;
    return true;
  }
  if (!notify_ring_buffer_.Push(orderly_notify_prop)) {
    return false;
  }
  while (wait_add_map_.find(next_id()) != wait_add_map_.end()) {
    auto the_next_id = next_id();
    notify_ring_buffer_.Push(wait_add_map_[the_next_id]);
    wait_add_map_.erase(the_next_id);
  }
  buffer_added_ = true;
  return true;
}

void MultiCellAoIObserved::GetNotifyPropById(AoINotifyId beginId, uint32_t size, GWVector<const AoIOrderlyNotifyProp*>* props) {
  for (size_t i = 0; i < size; i++) {
    auto ret = notify_ring_buffer_.GetById(beginId + i);
    if (ret) {
      props->push_back(ret);
    } else {
      break;
    }
  }
}

const AoIOrderlyNotifyProp* MultiCellAoIObserved::GetNotifyPropById(AoINotifyId id) {
  return notify_ring_buffer_.GetById(id);
}

bool MultiCellAoIObserved::NotifyBufferAdded() {
  if (buffer_added_) {
    buffer_added_ = false;
    return true;
  } else {
    return false;
  }
}

GW_GAME_END