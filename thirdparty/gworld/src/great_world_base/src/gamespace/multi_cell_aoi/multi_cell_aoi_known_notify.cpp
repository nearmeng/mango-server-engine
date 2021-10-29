#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_known_notify.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(MultiCellAoiKnownNotify)

StringSlice MultiCellAoiKnownNotify::NotifyProp() {
  const uint8_t mask_have_data = 0;
  os().reset();
  uint32_t visited_num = 0;
  for (auto& eid : buffer_added_observed_eids_) {
    auto it = observed_actor_ids_.find(eid);
    if (it == observed_actor_ids_.end()) {
      continue;
    }
    MultiCellAoIObservedActor* observed_node = it->second;
    auto multi_cell_observed = get_observed_by_eid(observed_node->eid());
    if (!multi_cell_observed) {
      EndObserve(observed_node->eid());
      GW_LOG_DEBUG("%d aoi_observed_component not found", observed_node->eid());
      continue;
    }
    visited_num++;
    uint32_t notify_bytes_this_time = 0;
    bool inited = false;
    while (notify_bytes_this_time < AOI_WORLD.AoIConf().observer_notify_bytes_one_time()) {
      auto notify_prop = multi_cell_observed->GetNotifyPropById(observed_node->next_id());
      if (!notify_prop) {
        break;
      } else {
        // 根据时间戳判断下是否可以发 diff_label_prop
        auto diff_prop = observed_node->TryPopDiffProp(notify_prop->timestamp_);
        if (diff_prop) {
          notify_prop = diff_prop;
          GW_LOG_DEBUG("NotifyProp:observed_node diff_prop %lu, notify_prop_labels %d, ts %lu", owner_id(), notify_prop->labels_, notify_prop->timestamp_);
        } else {
          observed_node->next_add();
        }
      }
      // 根据 label 判断一下这个属性需不需要发送
      if (observed_node->labels() == 0 || ((observed_node->labels() & notify_prop->labels_) > 0)) {
        // {{eid, {{have_mask, eid, type, data}, ~have_mask}, {eid, {{have_mask, eid, type, data}, ~have_mask} ... }
        if (!inited) {
          os() << observed_node->eid();
          inited = true;
        }
        os() << static_cast<uint8_t>(mask_have_data);
        os() << StringSlice(notify_prop->data_.c_str(), notify_prop->data_.size());
        notify_bytes_this_time += notify_prop->data_.size();
      }
    }
    if (inited) {
      os() << static_cast<uint8_t>(~mask_have_data);
    }
    if (visited_num >= AOI_WORLD.AoIConf().observer_notify_entity_num_one_time()) {
      break;
    }
  }
  buffer_added_observed_eids_.clear();
  return os().view();
}

bool MultiCellAoiKnownNotify::WriteToMemoryStream(IMemoryOStream& os) {
  bool ret = MultiCellAoIObserver::WriteToMemoryStream(os);
  if (ret) {
    os << buffer_added_observed_eids_.size();
    for (auto& eid : buffer_added_observed_eids_) {
      os << eid;
    }
  }
  return ret;
}

bool MultiCellAoiKnownNotify::ReadFromMemoryStream(IMemoryIStream& is) {
  bool ret = MultiCellAoIObserver::ReadFromMemoryStream(is);
  if (ret) {
    std::size_t tmp_size;
    is >> tmp_size;
    for (size_t i = 0; i < tmp_size; i++) {
      uint64_t eid;
      is >> eid;
      buffer_added_observed_eids_.insert(eid);
    }
  }
  return ret;
}

GW_GAME_END