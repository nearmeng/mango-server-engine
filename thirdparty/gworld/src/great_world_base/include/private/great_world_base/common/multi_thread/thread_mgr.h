#pragma once
#include <unordered_map>

#include "thread_cmd.h"
#include "group/thread_group.h"

SMT_NS_BEGIN

class ThreadMgr {
 public:
  virtual ~ThreadMgr() {
    for(auto& it:groups_) {
      delete it.second;
    }
  }

  // 初始化
  void Init() {}

  // 添加group
  void AddGroup(ThreadGroup* group) {
    if (!group) return;
    auto it = groups_.find(group->id());
    if (it != groups_.end())
      return;
    groups_.emplace(group->id(), group);
  }

  // 往指定线程推cmd
  bool Push(uint32_t group_id, ChildThreadCmd* cmd) {
    if (!cmd) return false;
    auto it = groups_.find(group_id);
    if (it == groups_.end())
      return false;
    it->second->Push(cmd);
    return true;
  }

  // 执行从子线程推过来的 cmd
  void ExecuteMainCmd() {
    for (auto it = groups_.begin(); it != groups_.end(); ++it) {
      while(MainThreadCmd* cmd = it->second->Pop()) {
        cmd->run();
        delete cmd;
      }
    }
  }

  void Start() {
    for (auto it = groups_.begin(); it != groups_.end(); ++it) {
      it->second->Start();
    }
  }

  void Stop() {
    for (auto it = groups_.begin(); it != groups_.end(); ++it) {
      it->second->Stop();
    }
    ExecuteMainCmd();
  }

 private:
  std::unordered_map<uint32_t, ThreadGroup*> groups_;
};

SMT_NS_END