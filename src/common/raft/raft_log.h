#ifndef __RAFT_LOG_H__
#define __RAFT_LOG_H__

#include "define/raft_def.h"

struct RAFT_ENTRY;
struct RAFT_LOG;

RAFT_LOG* raft_log_new(int32_t nInitSize);
void raft_log_free(RAFT_LOG* pRaftLog);
void raft_log_clear(RAFT_LOG* pRaftLog);
void raft_log_clear_entry(RAFT_LOG* pRaftLog);
void raft_log_set_callback(RAFT_LOG* pRaftLog, RAFT_CALLBACK_MGR* pCallBackMgr, RAFT_MGR* pRaftMgr);

void raft_log_append_entry(RAFT_LOG* pRaftLog, RAFT_ENTRY* pEntry);
BOOL raft_log_delete_entry_from_index(RAFT_LOG* pRaftLog, int32_t nIndex);
void raft_log_empety_queue(RAFT_LOG* pRaftLog);

RAFT_ENTRY* raft_log_get_entry(RAFT_LOG* pRaftLog, int32_t nIndex);
RAFT_ENTRY* raft_log_get_from_entry(RAFT_LOG* pRaftLog, int32_t nIndex, int32_t& nCount);
RAFT_ENTRY* raft_log_get_entry_tail(RAFT_LOG* pRaftLog);
int32_t raft_log_get_entry_count(RAFT_LOG* pRaftLog);
int32_t raft_log_get_curr_index(RAFT_LOG* pRaftLog);
int32_t raft_log_get_base_index(RAFT_LOG* pRaftLog);

#endif

