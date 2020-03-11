#ifndef __RAFT_SERVICE_H__
#define __RAFT_SERVICE_H__

#include "define/raft_def.h"

struct RAFT_MGR;

RAFT_MGR* raft_service_new(const char* pInstTag, BOOL bResume);
void raft_service_free(RAFT_MGR* pRaftMgr);
void raft_service_mainloop(RAFT_MGR* pRaftMgr, int32_t nIntervalMS);

BOOL raft_service_set_callback(RAFT_MGR* pRaftMgr, RAFT_CALLBACK_MGR& stCallBackMgr);	
BOOL raft_service_set_timeout_config(RAFT_MGR* pRaftMgr, int32_t nRequestTimeOut, int32_t nElectionTimeout);

RAFT_NODE* raft_service_add_node(RAFT_MGR* pRaftMgr, int32_t nNodeID, BOOL bSelf);
BOOL raft_service_del_node(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode);
RAFT_NODE* raft_service_get_node(RAFT_MGR* pRaftMgr, int32_t nNodeID);

BOOL raft_service_set_data(RAFT_MGR* pRaftMgr, int32_t nType, int64_t llParam, void* pData, int32_t nDataSize);
BOOL raft_service_recv_msg(RAFT_MGR* pRaftMgr, int32_t nNodeID, int32_t nMsgType, const char* pMsgData);

int32_t raft_service_get_status(RAFT_MGR* pRaftMgr);
int32_t raft_service_get_curr_term(RAFT_MGR* pRaftMgr);
int32_t raft_service_get_curr_index(RAFT_MGR* pRaftMgr);
int32_t raft_service_get_commit_index(RAFT_MGR* pRaftMgr);
int32_t raft_service_get_last_applied_index(RAFT_MGR* pRaftMgr);
int32_t raft_service_get_curr_leader_nodeid(RAFT_MGR* pRaftMgr);

#endif