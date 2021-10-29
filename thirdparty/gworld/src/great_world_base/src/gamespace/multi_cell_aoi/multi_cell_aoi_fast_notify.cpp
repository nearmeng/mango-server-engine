#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_fast_notify.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_world.h"
#include "great_world_base/common/common_private.h"

GW_GAME_BEGIN
GWCLASS_DECL_IMPLEMENT(MultiCellAoiFastNotify)

namespace {
const uint8_t mask_have_data = 0;
}

MultiCellAoiFastNotify::MultiCellAoiFastNotify() : observed_actors_(AOI_WORLD.AoIConf().observer_entity_max_num()) {}
MultiCellAoiFastNotify::MultiCellAoiFastNotify(AoIEntity* owner, IMemoryOStream* os)
      : MultiCellAoIObserver(owner, os), observed_actors_(AOI_WORLD.AoIConf().observer_entity_max_num()) {}

uint32_t MultiCellAoiFastNotify::PushOneObservedData(MultiCellAoIObservedActor* observed_node,
                                                     MultiCellAoIObserved* multi_cell_observed) {
  // 计算label改变导致的需要直接同步的数据量，并且做同步操作
  // 这里有个问题，就是直接同步的属性可能队列里面也有，直接同步操作发生之前应该把之前收到的队列中的同步数据都发出去
  bool inited = false;
  uint32_t notify_bytes = 0;
  uint32_t notify_bytes_this_time = 0;
  while (notify_bytes_this_time < AOI_WORLD.AoIConf().observer_notify_bytes_one_time()) {
    auto notify_prop = multi_cell_observed->GetNotifyPropById(observed_node->next_id());
    if (!notify_prop) {
      break;
    } else {
      // 根据时间戳判断下是否可以发 diff_label_prop
      auto diff_prop = observed_node->TryPopDiffProp(notify_prop->timestamp_);
      if (diff_prop) {
        notify_prop = diff_prop;
        GW_LOG_DEBUG("NotifyProp:observed_node diff_prop %lu, notify_prop_labels %d, ts %lu", owner_id(),
                     notify_prop->labels_, notify_prop->timestamp_);
      } else {
        observed_node->next_add();
      }
    }
    // 根据 label 判断一下这个属性需不需要发送
    if (observed_node->labels() == 0 || ((observed_node->labels() & notify_prop->labels_) > 0)) {
      // {observed1 {have_mask type data ... } ~have_mask}  {observed2 {have_mask type data ... } ~have_mask}
      if (!inited) {
        os() << observed_node->eid();
        inited = true;
      }
      os() << static_cast<uint8_t>(mask_have_data);
      os() << notify_prop->type_;
      os() << StringSlice(notify_prop->data_.c_str(), notify_prop->data_.size());
      notify_bytes += notify_prop->data_.size();
      notify_bytes_this_time += notify_prop->data_.size();
    }
  }
  if (inited) {
    os() << static_cast<uint8_t>(~mask_have_data);
  }
  return notify_bytes;
}

StringSlice MultiCellAoiFastNotify::NotifyProp() {
  uint32_t notify_bytes = 0;
  uint32_t visited_num = 0;
  os().reset();
  auto max_notyfy_num = AOI_WORLD.AoIConf().observer_notify_entity_num_one_time();
  while (notify_bytes < AOI_WORLD.AoIConf().observer_notify_bytes_num_per_frame()) {
    if (observed_actors_.size() <= 0) break;
    uint64_t observed_eid = observed_actors_.top();
    observed_actors_.pop();
    auto iter = observed_actor_ids_.find(observed_eid);
    if(iter == observed_actor_ids_.end()) {
      continue;
    }
    auto multi_cell_observed = get_observed_by_eid(observed_eid);
    if (!multi_cell_observed) {
      EndObserve(observed_eid);
      GW_LOG_DEBUG("%d aoi_observed_component not found", observed_eid);
      continue;
    }
    MultiCellAoIObservedActor* observed_node = iter->second;
    visited_num++;
    notify_bytes += PushOneObservedData(observed_node, multi_cell_observed);
    observed_actors_.push(observed_eid);
    if (visited_num >= max_notyfy_num || visited_num >= observed_actors_.size()) {
      break;
    }
  }
  return os().view();
}

bool MultiCellAoiFastNotify::BeginObserve(MultiCellAoIObserved* observed, PropLabel labels) {
  auto ret = MultiCellAoIObserver::BeginObserve(observed, labels);
  if (ret) {
    observed_actors_.push(observed->owner_id());
  }
  return ret;
}

bool MultiCellAoiFastNotify::ReadFromMemoryStream(IMemoryIStream& is) {
  bool ret = MultiCellAoIObserver::ReadFromMemoryStream(is);
  if (ret) {
    for (auto& it : observed_actor_ids_) {
      observed_actors_.push(it.first);
    }
  }
  return ret;
}

GW_GAME_END