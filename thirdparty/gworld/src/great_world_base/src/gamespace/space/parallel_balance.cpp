#include <cmath>
#include "great_world_base/gamespace/space/parallel_balance.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_manager.h"

GW_GAME_BEGIN

GWCLASS_DECL_IMPLEMENT(ParallelBalance)

uint32_t ParallelBalance::max_migrate_num_one_time = 20;

void ParallelBalance::Sender_RequestMigrateAvatarNum(uint64_t gapp_id) {
  NetworkOStream os(NetworkMessage::RequestMigrateAvatarNum);
  os << cell_->space()->id();
  SEND_MSG_TO_CELL(gapp_id, os);
}

void ParallelBalance::Sender_MigrateTo(uint64_t gapp_id, uint32_t num) {
  if (num < 0) {
    return;
  }
  auto context_it = contexts_.find(gapp_id);
  if (context_it == contexts_.end()) {
    contexts_.emplace(gapp_id, ParallelBalanceContext());
    context_it = contexts_.find(gapp_id);
  }
  auto end_num = std::min(currect_migrate_num_ + num, static_cast<uint32_t>(real_eids.size()));
  auto final_migrate_num = end_num - currect_migrate_num_;
  // 开始迁移
  for (; currect_migrate_num_ < end_num; currect_migrate_num_++) {
    auto target_eid = real_eids.at(currect_migrate_num_);
    cell_->MigrateTo(target_eid, gapp_id);
  }
  // 预估一个结束时间，然后再继续询问
  uint32_t migrate_time =
      std::ceil(final_migrate_num * 1.0 / cell_->migration_record().predicte()) * CELL_CFG.migrate_check_interval();
  context_it->second.next_migrate_ts_ = GetWalltimeMs() + migrate_time;
}

void ParallelBalance::Receiver_RequestMigrateAvatarNum(uint64_t sender) {
  auto predicte = cell_->avatar_num_record().predicte();
  NetworkOStream os(NetworkMessage::RequestMigrateAvatarNum);
  MigrateAvatarNum data;
  data.set_space_id(cell_->space()->id());
  if (predicte > cell_->node()->upper_normal_avatar_num()) {
    data.set_num(0);
  } else {
    data.set_num(std::max(max_migrate_num_one_time, cell_->node()->upper_normal_avatar_num() - predicte));
  }
  data.ToString(&os);
  GW_LOG_DEBUG("receive %lu ");
  SEND_MSG_TO_CELL(sender, os);
}

void ParallelBalance::OnCellMigrateCheck() {
  auto now = GetWalltimeMs();
  if (InBalance()) {
    if (now > begin_time_ + config_.max_balance_time_ || currect_migrate_num_ >= real_eids.size()) EndBalance();
    return;
  }
  for (auto& it : contexts_) {
    if (now > it.second.next_migrate_ts_) {
      Sender_RequestMigrateAvatarNum(it.first);
    }
  }
}

void ParallelBalance::EndBalance() {
  begin_time_ = 0;
  GW_LOG_DEBUG("end balance:space_id %lu, migrate_num %u", cell_->space()->id(), currect_migrate_num_);
}

bool ParallelBalance::InBalance() { return begin_time_ > 0; }

bool ParallelBalance::BeginBalance(GWVector<uint64_t> real_eids) {
  if (InBalance() || real_eids.empty()) {
    return false;
  }
  auto space_id = cell_->space()->id();
  auto space = SPACE_MGR.GetSpace(space_id);
  if (!space) {
    GW_LOG_DEBUG("space nou found, space_id %lu", space_id);
    return false;
  }
  GWHashSet<uint64_t> target_ids = std::move(space->curr_cell_node()->AllGameIds());
  target_ids.erase(SPACE_MGR.gapp_id());
  for (auto id : target_ids) {
    Sender_RequestMigrateAvatarNum(id);
  }
  return true;
}

GW_GAME_END