#include "great_world_base/cellmgr/balance/balance_policy.h"

GW_CELL_MGR_BEGIN

// 下面2个模块同时工作

// 策略1：
// 负载均衡分为client/server，client决定把负载添加到哪里，防止造成性能问题（client 可以支持多线程？）
// 或者负载的同步传给多个端(一个cell传给相邻的区域、分线和mgr)，各个客户端自己做负载均衡
// 预测 => 给一个节点增加的时候也要假设其他的几个分线或者邻居也往这个地方加了人，

// 策略2：
// 负载自均衡，如果真的出现某些进程负载很高，可以主动发起问询，去把自己的负载迁移到其他进程

class DefaultBalancePolicy : public BalancePolicy {
  GWCLASS_DECL(DefaultBalancePolicy)
 public:
  DefaultBalancePolicy() = default;
  DefaultBalancePolicy(bool vertical, bool tracing);
  bool Balance(BalanceNode* curr_node, BalanceNode* next_node, BalanceResult& result) override;

 protected:
  bool BalanceRemoving(BalanceNode* curr_node, BalanceNode* next_node, BalanceResult& result);

 private:
  bool vertical_;
  bool tracing_;
};

GW_CELL_MGR_END