#pragma once
#include "great_world_base/gamespace/space/cell.h"
#include "great_world_base/gamespace/space/parallel_balance.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/space/balance_message.h"

GW_GAME_BEGIN

struct ParallelBalanceConfig {
  uint64_t max_balance_time_ = 10000; // ms 
};

struct ParallelBalanceContext {
  uint64_t next_migrate_ts_ = 0;
};

class ParallelBalance {
  GWCLASS_DECL(ParallelBalance)
 public:
  ParallelBalance() = default;
  ParallelBalance(Cell* cell) : cell_(cell) {}
  bool InBalance();
  bool BeginBalance(GWVector<uint64_t> ids);
  void OnCellMigrateCheck();

  // 负载均衡的转出方
  void Sender_RequestMigrateAvatarNum(uint64_t gapp_id);
  void Sender_MigrateTo(uint64_t gapp_id, uint32_t num);
  
  // 负载均衡的转入方
  void Receiver_RequestMigrateAvatarNum(uint64_t sender);

 private:
  void EndBalance();

 private:
  Cell* cell_ = nullptr;
  uint64_t begin_time_ = 0;
  ParallelBalanceConfig config_;
  GWVector<uint64_t> real_eids;
  uint32_t currect_migrate_num_ = 0;
  GWHashMap<uint64_t, ParallelBalanceContext> contexts_;

 private:
  static uint32_t max_migrate_num_one_time;
};

GW_GAME_END