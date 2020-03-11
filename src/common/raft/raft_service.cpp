#include "stdafx.h"

#include "raft/raft_service.h"
#include "raft/raft_node.h"
#include "raft/raft_log.h"

#include "lmdb/lmdb_helper.h"
#include "tbus/tbus_wrapper.h"

struct RAFT_DB
{
	MDB_env		*pDBEnv;
	MDB_dbi		StateDB;
	MDB_dbi		LogDB;
};

struct RAFT_MGR
{
	int32_t		nCurrTerm;
	int32_t		nVoteFor;
	int32_t		nCommitIndex;
	int32_t		nLastAppliedIndex;

	int32_t		nStaus;
	int32_t		nTimeoutElapsed;

	int32_t		nRaftNodeCount;
	void**		pRaftNodes;
	
	RAFT_NODE*	pCurrLeader;
	RAFT_NODE*	pMyNode;

	int32_t		nElectionTimeout;				// 选举超时
	int32_t		nRandElectionTimeout;			// 开始选举随机时间
	int32_t		nRequestTimeout;				// 发送请求超时

	RAFT_LOG*	pLog;

	RAFT_CALLBACK_MGR stCallBackMgr;
	void*		pUserData;

	RAFT_DB     stRaftDB;
};

// normal set
static BOOL _raft_service_apply_all(RAFT_MGR* pRaftMgr);
static void _raft_service_set_current_term(RAFT_MGR* pRaftMgr, int32_t nTerm);
static void _raft_service_set_commit_index(RAFT_MGR* pRaftMgr, int32_t nCommitIndex);
static BOOL _raft_service_append_entry(RAFT_MGR* pRaftMgr, RAFT_ENTRY* pEntry);

// default callback
static BOOL _raft_default_log_append_callback(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex);
static BOOL _raft_default_log_delete_last_callback(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex);
static BOOL _raft_default_persist_vote_callback(RAFT_MGR* pMgr, int32_t nNodeID);
static BOOL _raft_default_persist_term_callback(RAFT_MGR* pMgr, int32_t nTerm);
static BOOL _raft_default_apply_entry(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex);

//recv msg logic 
static BOOL _raft_service_recv_request_vote(RAFT_MGR* pRaftMgr, VOTE_REQUEST* pVoteReq);
static BOOL _raft_service_recv_request_vote_rsp(RAFT_MGR* pRaftMgr, int32_t nNodeID, VOTE_RESPONSE* pVoteRsp);
static BOOL _raft_service_recv_append_entry(RAFT_MGR* pRaftMgr, APPEND_ENTRY_REQUEST* pAppendReq);
static BOOL _raft_service_recv_append_entry_rsp(RAFT_MGR* pRaftMgr, int32_t nNodeID, APPEND_ENTRY_RESPONSE* pAppendRsp);

static void _raft_rand_election_timeout(RAFT_MGR* pRaftMgr)
{
	int32_t nRandTime = 0;
	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftMgr->nRandElectionTimeout = pRaftMgr->nElectionTimeout + rand() % (pRaftMgr->nElectionTimeout);

Exit0:
	return;
}

static BOOL _raft_set_default_callback(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;
	RAFT_CALLBACK_MGR stCallback;

	stCallback.pLogAppend = _raft_default_log_append_callback;
	stCallback.pLogDeleteLast = _raft_default_log_delete_last_callback;
	stCallback.pPersistVote = _raft_default_persist_vote_callback;
	stCallback.pPersistTerm = _raft_default_persist_term_callback;
	stCallback.pApplyEntry = _raft_default_apply_entry;
	stCallback.pLogClear = NULL;				
	stCallback.pSendRequest = NULL;
	stCallback.pBecomeLeader = NULL;

	nRetCode = raft_service_set_callback(pRaftMgr, stCallback);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _load_log_entry_from_db(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;
	MDB_cursor* pCursor = NULL;
	MDB_txn* pTxn = NULL;
	MDB_val k, v;
	RAFT_ENTRY stEntry = { 0 };
	int32_t nEntryCount = 0;
	int32_t nCommitIndex = 0;

	LOG_PROCESS_ERROR(pRaftMgr);

	nRetCode = mdb_txn_begin(pRaftMgr->stRaftDB.pDBEnv, NULL, MDB_RDONLY, &pTxn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = mdb_cursor_open(pTxn, pRaftMgr->stRaftDB.LogDB, &pCursor);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = mdb_cursor_get(pCursor, &k, &v, MDB_FIRST);
	if (nRetCode == MDB_NOTFOUND)
	{
		INF("finish to load entry from db");
		return TRUE;
	}
	else
		LOG_PROCESS_ERROR(nRetCode == 0);

	do
	{
		if (*(int*)k.mv_data & 1)
		{
			//data
			stEntry.stData.dwSize = v.mv_size;
			stEntry.stData.pBuff = malloc(v.mv_size);
			memcpy(stEntry.stData.pBuff, v.mv_data, v.mv_size);

			nEntryCount++;

			nRetCode = _raft_service_append_entry(pRaftMgr, &stEntry);
			LOG_CHECK_ERROR(nRetCode);
		}
		else
		{
			//meta
			RAFT_ENTRY* pCurEntry = (RAFT_ENTRY*)v.mv_data;
			stEntry = *pCurEntry;
		}

		nRetCode = mdb_cursor_get(pCursor, &k, &v, MDB_NEXT);
	} while (nRetCode == 0);

	mdb_cursor_close(pCursor);

	nRetCode = mdb_txn_commit(pTxn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = lmdb_gets_int(pRaftMgr->stRaftDB.pDBEnv, pRaftMgr->stRaftDB.StateDB, "commit_index", &nCommitIndex);
	LOG_PROCESS_ERROR(nRetCode);

	_raft_service_set_commit_index(pRaftMgr, nCommitIndex);

	nRetCode = _raft_service_apply_all(pRaftMgr);
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _load_persist_state_from_db(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;
	int32_t nTerm = 0;
	int32_t nVoteFor = 0;

	LOG_PROCESS_ERROR(pRaftMgr);

	nRetCode = lmdb_gets_int(pRaftMgr->stRaftDB.pDBEnv, pRaftMgr->stRaftDB.StateDB, "vote", &nVoteFor);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = lmdb_gets_int(pRaftMgr->stRaftDB.pDBEnv, pRaftMgr->stRaftDB.StateDB, "term", &nTerm);
	LOG_PROCESS_ERROR(nRetCode);

	pRaftMgr->nVoteFor = nVoteFor;
	pRaftMgr->nCurrTerm = nTerm;

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_db_init(RAFT_MGR* pRaftMgr, const char* pInstTag, BOOL bResume)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pInstTag);

	nRetCode = lmdb_env_create(&pRaftMgr->stRaftDB.pDBEnv, 0, pInstTag, DEFAULT_DB_SIZE_MB);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = lmdb_db_create(&(pRaftMgr->stRaftDB.LogDB), pRaftMgr->stRaftDB.pDBEnv, "log_entry");
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = lmdb_db_create(&(pRaftMgr->stRaftDB.StateDB), pRaftMgr->stRaftDB.pDBEnv, "raft_state");
	LOG_PROCESS_ERROR(nRetCode);

	if (!bResume)
	{
		//clear database
		MDB_dbi dbs[] = { pRaftMgr->stRaftDB.LogDB, pRaftMgr->stRaftDB.StateDB };
		nRetCode = lmdb_drop_dbs(pRaftMgr->stRaftDB.pDBEnv, dbs, sizeof(dbs) / sizeof(dbs[0]));
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = lmdb_db_create(&(pRaftMgr->stRaftDB.LogDB), pRaftMgr->stRaftDB.pDBEnv, "log_entry");
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = lmdb_db_create(&(pRaftMgr->stRaftDB.StateDB), pRaftMgr->stRaftDB.pDBEnv, "raft_state");
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = _load_log_entry_from_db(pRaftMgr);
		LOG_PROCESS_ERROR(nRetCode);
		
		nRetCode = _load_persist_state_from_db(pRaftMgr);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

RAFT_MGR* raft_service_new(const char* pInstTag, BOOL bResume)
{
	int32_t nRetCode = 0;
	RAFT_MGR* pRaftMgr = NULL;

	LOG_PROCESS_ERROR(pInstTag);

	pRaftMgr = (RAFT_MGR*)malloc(sizeof(RAFT_MGR));
	LOG_PROCESS_ERROR(pRaftMgr);

	memset(pRaftMgr, 0, sizeof(RAFT_MGR));

	pRaftMgr->nCurrTerm = 0;
	pRaftMgr->nVoteFor = -1;
	pRaftMgr->nTimeoutElapsed = 0;
	pRaftMgr->nRequestTimeout = DEFAULT_REQUEST_TIMEOUT;
	pRaftMgr->nElectionTimeout = DEFAULT_ELECTION_TIMEOUT;
	pRaftMgr->nStaus = rsFollower;
	pRaftMgr->pLog = raft_log_new(INIT_RAFT_LOG_SIZE);
	LOG_PROCESS_ERROR(pRaftMgr->pLog);

	_raft_rand_election_timeout(pRaftMgr);

	nRetCode = _raft_db_init(pRaftMgr, pInstTag, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _raft_set_default_callback(pRaftMgr);
	LOG_PROCESS_ERROR(nRetCode);

	return pRaftMgr;
Exit0:
	raft_service_free(pRaftMgr);
	return NULL;
}

void raft_service_free(RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	if (pRaftMgr->stRaftDB.pDBEnv)
	{
		lmdb_env_destroy(pRaftMgr->stRaftDB.pDBEnv);
	}
	
	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
		raft_node_free(pNode);
	}

	SAFE_FREE(pRaftMgr->pRaftNodes);
	raft_log_free(pRaftMgr->pLog);
	SAFE_FREE(pRaftMgr);

	INF("[raft]: raft service is freed!");
Exit0:
	return;
}

static BOOL _raft_default_log_append_callback(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex)
{
	int32_t nRetCode = 0;
	MDB_val key;
	MDB_val value;
	MDB_txn* txn;

	LOG_PROCESS_ERROR(pMgr);
	LOG_PROCESS_ERROR(pEntry);

	nRetCode = mdb_txn_begin(pMgr->stRaftDB.pDBEnv, NULL, 0, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nEntryIndex <<= 1;
	key.mv_size = sizeof(nEntryIndex);
	key.mv_data = (void*)&nEntryIndex;

	value.mv_size = sizeof(RAFT_ENTRY);
	value.mv_data = (void*)pEntry;

	nRetCode = mdb_put(txn, pMgr->stRaftDB.LogDB, &key, &value, 0);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nEntryIndex |= 1;
	key.mv_size = sizeof(nEntryIndex);
	key.mv_data = (void*)&nEntryIndex;

	value.mv_size = pEntry->stData.dwSize;
	value.mv_data = pEntry->stData.pBuff;

	nRetCode = mdb_put(txn, pMgr->stRaftDB.LogDB, &key, &value, 0);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_default_log_delete_last_callback(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex)
{
	int32_t nRetCode = 0;
	MDB_val k, v;

	nRetCode = lmdb_delete_last(pMgr->stRaftDB.pDBEnv, pMgr->stRaftDB.LogDB, &k, &v);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_default_persist_vote_callback(RAFT_MGR* pMgr, int32_t nNodeID)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pMgr);

	nRetCode = lmdb_puts_int(pMgr->stRaftDB.pDBEnv, pMgr->stRaftDB.StateDB, "vote", nNodeID);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_default_persist_term_callback(RAFT_MGR* pMgr, int32_t nTerm)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pMgr);

	nRetCode = lmdb_puts_int(pMgr->stRaftDB.pDBEnv, pMgr->stRaftDB.StateDB, "term", nTerm);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_default_apply_entry(RAFT_MGR* pMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pMgr);
	LOG_PROCESS_ERROR(pEntry);
	LOG_PROCESS_ERROR(nEntryIndex > 0);

	INF("enter default entry apply, we suggest that user take over this callback");

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_vote(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pRaftNode);

	if (pRaftMgr->stCallBackMgr.pPersistVote)
	{
		nRetCode = pRaftMgr->stCallBackMgr.pPersistVote(pRaftMgr, raft_node_get_nodeid(pRaftNode));
		LOG_CHECK_ERROR(nRetCode);
	}

	pRaftMgr->nVoteFor = raft_node_get_nodeid(pRaftNode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_send_append_entry(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode)
{
	int32_t nRetCode = 0;
	int32_t nNextIndex = 0;
	int32_t nEntryCount = 0;
	int32_t nSendSize = 0;
	RAFT_ENTRY* pRaftEntry = NULL;
	APPEND_ENTRY_REQUEST* msg = NULL;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(pRaftNode != pRaftMgr->pMyNode);
	LOG_PROCESS_ERROR(pRaftMgr->nStaus == rsLeader);
	
	nNextIndex = raft_node_get_next_index(pRaftNode);
	pRaftEntry = raft_log_get_from_entry(pRaftMgr->pLog, nNextIndex, nEntryCount);

	nSendSize = sizeof(APPEND_ENTRY_REQUEST) + sizeof(RAFT_ENTRY) * nEntryCount;
	for (int32_t i = 0; i < nEntryCount; i++)
	{
		nSendSize += pRaftEntry[i].stData.dwSize;
	}

	msg = (APPEND_ENTRY_REQUEST*)alloca(nSendSize);
	LOG_PROCESS_ERROR(msg);

	msg->nTerm = pRaftMgr->nCurrTerm;
	msg->nLeaderID = raft_node_get_nodeid(pRaftMgr->pMyNode);
	msg->nLeaderCommit = pRaftMgr->nCommitIndex;

	if (nNextIndex > 1)
	{
		RAFT_ENTRY* pPreEntry = raft_log_get_entry(pRaftMgr->pLog, nNextIndex - 1);
		LOG_PROCESS_ERROR(pPreEntry);

		msg->nPreLogIndex = nNextIndex - 1;
		msg->nPreLogTerm = pPreEntry->nTerm;
	}
	else
	{
		msg->nPreLogIndex = 0;
		msg->nPreLogTerm = 0;
	}

	msg->nEntryCount = nEntryCount;
	if (pRaftEntry)
	{
		int32_t nDataOffset = 0;
		char* pBaseAddr = (char*)(msg->stEntry);
		memcpy(msg->stEntry, pRaftEntry, sizeof(RAFT_ENTRY) * nEntryCount);
		for (int32_t i = 0; i < nEntryCount; i++)
		{
			memcpy(pBaseAddr + sizeof(RAFT_ENTRY) * nEntryCount + nDataOffset, pRaftEntry[i].stData.pBuff, pRaftEntry[i].stData.dwSize);
			nDataOffset += pRaftEntry[i].stData.dwSize;
		}
	}

	LOG_PROCESS_ERROR(pRaftMgr->stCallBackMgr.pSendRequest);
	nRetCode = pRaftMgr->stCallBackMgr.pSendRequest(raft_node_get_nodeid(pRaftNode), rmtAppendReq, (char*)msg, nSendSize);
	LOG_PROCESS_ERROR(nRetCode);

	//INF("[raft]: do send append entry");

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_send_request_vote(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode)
{
	int32_t nRetCode = 0;
	VOTE_REQUEST msg;

	LOG_PROCESS_ERROR(pRaftMgr);

	msg.nTerm = pRaftMgr->nCurrTerm;
	msg.nLastLogIndex = raft_log_get_curr_index(pRaftMgr->pLog);
	if (msg.nLastLogIndex > 0)
	{
		RAFT_ENTRY* pLastEntry = raft_log_get_entry_tail(pRaftMgr->pLog);
		LOG_PROCESS_ERROR(pLastEntry);
		msg.nLastLogTerm = pLastEntry->nTerm;
	}
	else
		msg.nLastLogTerm = 0;
	msg.nCandidateID = raft_node_get_nodeid(pRaftMgr->pMyNode);

	LOG_PROCESS_ERROR(pRaftMgr->stCallBackMgr.pSendRequest);
	nRetCode = pRaftMgr->stCallBackMgr.pSendRequest(raft_node_get_nodeid(pRaftNode), rmtVoteReq, (char*)&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);
	
	INF("[raft]: do send request vote to node %s", tbus_get_str(raft_node_get_nodeid(pRaftNode)));

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_send_request_vote_rsp(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode, VOTE_RESPONSE* pVoteRsp)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(pVoteRsp);
	LOG_PROCESS_ERROR(pRaftMgr->stCallBackMgr.pSendRequest);

	nRetCode = pRaftMgr->stCallBackMgr.pSendRequest(raft_node_get_nodeid(pRaftNode), rmtVoteRsp, (char*)pVoteRsp, sizeof(VOTE_RESPONSE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_send_append_entry_rsp(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode, APPEND_ENTRY_RESPONSE* pAppendRsp)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(pAppendRsp);

	LOG_PROCESS_ERROR(pRaftMgr->stCallBackMgr.pSendRequest);
	nRetCode = pRaftMgr->stCallBackMgr.pSendRequest(raft_node_get_nodeid(pRaftNode), rmtAppenRsp, (char*)pAppendRsp, sizeof(APPEND_ENTRY_RESPONSE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}


static BOOL _raft_become_follower(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftMgr->nStaus = rsFollower;
	pRaftMgr->nTimeoutElapsed = 0;
	_raft_rand_election_timeout(pRaftMgr);
	
	INF("[raft]: node become follower");

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_become_candidate(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;

	INF("[raft]: node become candinate");

	pRaftMgr->nCurrTerm++;

	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		raft_node_vote_for_me((RAFT_NODE*)(pRaftMgr->pRaftNodes[i]), FALSE);
	}
	_raft_service_vote(pRaftMgr, pRaftMgr->pMyNode);
	pRaftMgr->pCurrLeader = NULL;
	pRaftMgr->nStaus = rsCandidate;

	_raft_rand_election_timeout(pRaftMgr);
	pRaftMgr->nTimeoutElapsed = 0;

	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pRaftNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];

		if (pRaftNode == pRaftMgr->pMyNode)
			continue;

		if (!raft_node_is_voting(pRaftNode))
			continue;

		nRetCode = _raft_service_send_request_vote(pRaftMgr, pRaftNode);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_become_leader(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	
	pRaftMgr->nStaus = rsLeader;
	pRaftMgr->nTimeoutElapsed = 0;
	pRaftMgr->pCurrLeader = pRaftMgr->pMyNode;

	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pRaftNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];

		if (pRaftNode == pRaftMgr->pMyNode)
			continue;

		raft_node_set_next_index(pRaftNode, raft_log_get_curr_index(pRaftMgr->pLog) + 1);
		raft_node_set_match_index(pRaftNode, 0);
		nRetCode = _raft_service_send_append_entry(pRaftMgr, pRaftNode);
		LOG_CHECK_ERROR(nRetCode);
	}

	INF("[raft]: node become leader");

	if (pRaftMgr->stCallBackMgr.pBecomeLeader)
	{
		pRaftMgr->stCallBackMgr.pBecomeLeader(pRaftMgr->nCurrTerm);
	}

	return TRUE;
Exit0:
	return FALSE;
}


BOOL _raft_service_election_start(RAFT_MGR* pRaftMgr)
{
	INF("[raft]: raft election start: timeout %d curr_term %d curr index %d", pRaftMgr->nRandElectionTimeout,
			pRaftMgr->nCurrTerm, pRaftMgr->nCommitIndex);

	if (pRaftMgr->nRaftNodeCount <= 1)
		return _raft_become_leader(pRaftMgr);
	else
		return _raft_become_candidate(pRaftMgr);
}

static BOOL _raft_need_vote(RAFT_MGR* pRaftMgr, VOTE_REQUEST* pVoteReq)
{
	int32_t nCurrIndex = 0;
	RAFT_ENTRY* pRaftEntry = NULL;

	LOG_PROCESS_ERROR(pVoteReq);
	LOG_PROCESS_ERROR(raft_node_is_voting(pRaftMgr->pMyNode));

	if (pRaftMgr->nCurrTerm > pVoteReq->nTerm)
		return FALSE;
	
	if (pRaftMgr->nVoteFor != -1)
		return FALSE;

	nCurrIndex = raft_log_get_curr_index(pRaftMgr->pLog);
	if (nCurrIndex == 0)
		return TRUE;

	pRaftEntry = raft_log_get_entry(pRaftMgr->pLog, nCurrIndex);
	LOG_PROCESS_ERROR(pRaftEntry);

	if (pRaftEntry->nTerm < pVoteReq->nLastLogTerm)
		return TRUE;
	else if (pRaftEntry->nTerm == pVoteReq->nLastLogTerm && nCurrIndex <= pVoteReq->nLastLogIndex)
		return TRUE;

Exit0:
	return FALSE;
}

void _raft_service_set_current_term(RAFT_MGR* pRaftMgr, int32_t nTerm)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	pRaftMgr->nCurrTerm = nTerm;

	if (pRaftMgr->stCallBackMgr.pPersistTerm)
	{
		nRetCode = pRaftMgr->stCallBackMgr.pPersistTerm(pRaftMgr, nTerm);
		LOG_CHECK_ERROR(nRetCode);
	}
Exit0:
	return;
}

void _raft_service_set_commit_index(RAFT_MGR* pRaftMgr, int32_t nCommitIndex)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftMgr->nCommitIndex = nCommitIndex;
Exit0:
	return;
}

BOOL _raft_service_recv_request_vote(RAFT_MGR* pRaftMgr, VOTE_REQUEST* pVoteReq)
{
	int32_t nRetCode = 0;
	RAFT_NODE* pRaftNode = NULL;
	VOTE_RESPONSE rVoteRsp;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pVoteReq);

	INF("[raft]: recv request vote, term %d candidate %s", pVoteReq->nTerm, tbus_get_str(pVoteReq->nCandidateID));
	
	pRaftNode = raft_service_get_node(pRaftMgr, pVoteReq->nCandidateID);
	LOG_PROCESS_ERROR(pRaftNode);

	if (pRaftMgr->pCurrLeader && pRaftMgr->pCurrLeader != pRaftNode && pRaftMgr->nTimeoutElapsed < pRaftMgr->nElectionTimeout)
	{
		rVoteRsp.bGranted = FALSE;
		PROCESS_SUCCESS(TRUE);
	}

	if (pRaftMgr->nCurrTerm < pVoteReq->nTerm)
	{
		pRaftMgr->nVoteFor = -1;

		_raft_service_set_current_term(pRaftMgr, pVoteReq->nTerm);
		_raft_become_follower(pRaftMgr);
		pRaftMgr->pCurrLeader = NULL;
	}

		
	if (_raft_need_vote(pRaftMgr, pVoteReq))
	{
		pRaftMgr->nVoteFor = pVoteReq->nCandidateID;
		pRaftMgr->pCurrLeader = NULL;
		pRaftMgr->nTimeoutElapsed = 0;

		if (pRaftMgr->stCallBackMgr.pPersistVote)
		{
			nRetCode = pRaftMgr->stCallBackMgr.pPersistVote(pRaftMgr, pVoteReq->nCandidateID);
			LOG_CHECK_ERROR(nRetCode);
		}

		rVoteRsp.bGranted = TRUE;
	}
	else
	{
		rVoteRsp.bGranted = FALSE;
	}

Exit1:
	rVoteRsp.nTerm = pRaftMgr->nCurrTerm;

	nRetCode = _raft_service_send_request_vote_rsp(pRaftMgr, pRaftNode, &rVoteRsp);
	LOG_CHECK_ERROR(nRetCode);

	INF("[raft]: send request vote response, term %d granted %d", rVoteRsp.nTerm, rVoteRsp.bGranted);

	return TRUE;

Exit0:
	return FALSE;
}

static int32_t _raft_get_voting_count(RAFT_MGR* pRaftMgr)
{
	int32_t nVotingCount = 0;

	LOG_PROCESS_ERROR(pRaftMgr);

	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pRaftNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
		if (raft_node_is_voting(pRaftNode))
		{
			nVotingCount++;
		}
	}

Exit0:
	return nVotingCount;
}

static int32_t _raft_get_vote_count_for_me(RAFT_MGR* pRaftMgr)
{
	int32_t nVoteCount = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pRaftNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
		if (pRaftMgr->pMyNode != pRaftNode && raft_node_is_voting(pRaftNode) && raft_node_has_vote_for_me(pRaftNode))
		{
			nVoteCount++;
		}
	}

	if (pRaftMgr->nVoteFor == raft_node_get_nodeid(pRaftMgr->pMyNode))
		nVoteCount++;

Exit0:
	return nVoteCount;
}

BOOL _raft_service_recv_request_vote_rsp(RAFT_MGR* pRaftMgr, int32_t nNodeID, VOTE_RESPONSE* pVoteRsp)
{
	int32_t nRetCode = 0;
	RAFT_NODE* pRaftNode = NULL;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pVoteRsp);

	INF("[raft]: recv vote rsp, nodeid %s granted %d term %d", tbus_get_str(nNodeID), pVoteRsp->bGranted, pVoteRsp->nTerm);

	pRaftNode = raft_service_get_node(pRaftMgr, nNodeID);
	LOG_PROCESS_ERROR(pRaftNode);

	if (pRaftMgr->nStaus == rsLeader)
		return TRUE;

	LOG_PROCESS_ERROR(pRaftMgr->nStaus == rsCandidate);

	if (pRaftMgr->nCurrTerm < pVoteRsp->nTerm)
	{
		_raft_service_set_current_term(pRaftMgr, pVoteRsp->nTerm);
		_raft_become_follower(pRaftMgr);
		pRaftMgr->pCurrLeader = NULL;
		
		return TRUE;
	}
	else
		LOG_PROCESS_ERROR(pRaftMgr->nCurrTerm == pVoteRsp->nTerm);

	if (pVoteRsp->bGranted)
	{
		raft_node_vote_for_me(pRaftNode, TRUE);

		if (_raft_get_vote_count_for_me(pRaftMgr) > _raft_get_voting_count(pRaftMgr) / 2)
		{
			_raft_become_leader(pRaftMgr);
		}

	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _raft_service_recv_append_entry(RAFT_MGR* pRaftMgr, APPEND_ENTRY_REQUEST* pAppendReq)
{
	int32_t nRetCode = 0;
	int32_t nScanID = 0;
	RAFT_NODE* pLeaderNode = NULL;
	APPEND_ENTRY_RESPONSE rAppendRsp = { 0 };

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pAppendReq);
	
	//INF("[raft]: recv append entry from leader %s, term %d", get_tbus_str(pAppendReq->nLeaderID), pAppendReq->nTerm);

	LOG_PROCESS_ERROR(pRaftMgr->nCurrTerm <= pAppendReq->nTerm);

	if (pRaftMgr->nStaus == rsCandidate && pRaftMgr->nCurrTerm == pAppendReq->nTerm)
	{
		_raft_become_follower(pRaftMgr);
	}
	
	if (pRaftMgr->nCurrTerm < pAppendReq->nTerm)
	{
		_raft_service_set_current_term(pRaftMgr, pAppendReq->nTerm);
		_raft_become_follower(pRaftMgr);
	}

	pLeaderNode = raft_service_get_node(pRaftMgr, pAppendReq->nLeaderID);
	LOG_PROCESS_ERROR(pLeaderNode);

	pRaftMgr->pCurrLeader = pLeaderNode;
	pRaftMgr->nTimeoutElapsed = 0;

	if (pAppendReq->nPreLogIndex > 0)
	{
		RAFT_ENTRY* pEntry = raft_log_get_entry(pRaftMgr->pLog, pAppendReq->nPreLogIndex);
		if (pEntry == NULL)
		{
			rAppendRsp.bSuccess = FALSE;
			PROCESS_SUCCESS(TRUE);
		}
		else if (pEntry->nTerm != pAppendReq->nPreLogTerm)
		{
			LOG_PROCESS_ERROR(pAppendReq->nPreLogIndex > raft_service_get_commit_index(pRaftMgr));

			raft_log_delete_entry_from_index(pRaftMgr->pLog, pAppendReq->nPreLogIndex);
			PROCESS_SUCCESS(TRUE);
		}
	}

	rAppendRsp.bSuccess = TRUE;
	rAppendRsp.nCurrIndex = pAppendReq->nPreLogIndex;

	//1 clean the unmatched entry
	for (nScanID = 0; nScanID < pAppendReq->nEntryCount; nScanID++)
	{
		RAFT_ENTRY& rRecvEntry = pAppendReq->stEntry[nScanID];
		int32_t nCurrIndex = pAppendReq->nPreLogIndex + nScanID + 1;
		RAFT_ENTRY* pCurrEnty = raft_log_get_entry(pRaftMgr->pLog, nCurrIndex);
		if (pCurrEnty == NULL)
			break;

		if (pCurrEnty->nTerm != rRecvEntry.nTerm)
		{
			raft_log_delete_entry_from_index(pRaftMgr->pLog, nCurrIndex);
			break;
		}

		rAppendRsp.nCurrIndex = nCurrIndex;
	}

	//2 append new entry
	for (; nScanID < pAppendReq->nEntryCount; nScanID++)
	{
		//fix the pointer
		char* pBaseAddr = (char*)pAppendReq->stEntry;
		int32_t nOffset = pAppendReq->nEntryCount * sizeof(RAFT_ENTRY);
		for (int32_t j = 0; j < nScanID; j++)
		{
			nOffset += pAppendReq->stEntry[j].stData.dwSize;
		}
		pAppendReq->stEntry[nScanID].stData.pBuff = pBaseAddr + nOffset;

		raft_log_append_entry(pRaftMgr->pLog, &pAppendReq->stEntry[nScanID]);
		rAppendRsp.nCurrIndex = pAppendReq->nPreLogIndex + nScanID + 1;
	}

	//3 modify commit index by leader commit
	if (raft_service_get_commit_index(pRaftMgr) < pAppendReq->nLeaderCommit)
	{
		int32_t nLastLogIndex = std::max<int32_t>(raft_log_get_curr_index(pRaftMgr->pLog), 1);
		pRaftMgr->nCommitIndex = std::min<int32_t>(nLastLogIndex, pAppendReq->nLeaderCommit);
	}

Exit1:
	rAppendRsp.nTerm = pRaftMgr->nCurrTerm;
	if (rAppendRsp.bSuccess)
		rAppendRsp.nCurrIndex = raft_log_get_curr_index(pRaftMgr->pLog);

	nRetCode = _raft_service_send_append_entry_rsp(pRaftMgr, pLeaderNode, &rAppendRsp);
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;

Exit0:
	return FALSE;
}

BOOL _raft_service_recv_append_entry_rsp(RAFT_MGR* pRaftMgr, int32_t nNodeID, APPEND_ENTRY_RESPONSE* pAppendRsp)
{
	int32_t nRetCode = 0;
	int32_t nMatchIndex = 0;
	RAFT_NODE* pRaftNode = NULL;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pAppendRsp);
	
	//INF("[raft]: recv append entry rsp, nodeid %s", get_tbus_str(nNodeID));
	
	pRaftNode = raft_service_get_node(pRaftMgr, nNodeID);
	LOG_PROCESS_ERROR(pRaftNode);

	LOG_PROCESS_ERROR(pRaftMgr->nStaus == rsLeader);

	if (pRaftMgr->nCurrTerm < pAppendRsp->nTerm)
	{
		_raft_service_set_current_term(pRaftMgr, pAppendRsp->nTerm);
		_raft_become_follower(pRaftMgr);
		pRaftMgr->pCurrLeader = NULL;
		return TRUE;
	}

	LOG_PROCESS_ERROR(pRaftMgr->nCurrTerm == pAppendRsp->nTerm);

	nMatchIndex = raft_node_get_match_index(pRaftNode);
	if (!pAppendRsp->bSuccess)
	{
		int32_t nNextIndex = raft_node_get_next_index(pRaftNode);
		LOG_PROCESS_ERROR(nNextIndex > 0);

		if (pAppendRsp->nCurrIndex < nMatchIndex)
			return TRUE;

		if (pAppendRsp->nCurrIndex < nNextIndex - 1)
			raft_node_set_next_index(pRaftNode, std::min<int32_t>(pAppendRsp->nCurrIndex + 1, raft_log_get_curr_index(pRaftMgr->pLog)));
		else
			raft_node_set_next_index(pRaftNode, nNextIndex - 1);

		_raft_service_send_append_entry(pRaftMgr, pRaftNode);
		return TRUE;
	}

	if (pAppendRsp->nCurrIndex < nMatchIndex)
		return TRUE;

	raft_node_set_next_index(pRaftNode, pAppendRsp->nCurrIndex + 1);
	raft_node_set_match_index(pRaftNode, pAppendRsp->nCurrIndex);

	//set commit by curr index
	if (pAppendRsp->nCurrIndex > 0)
	{
		RAFT_ENTRY* pEntry = raft_log_get_entry(pRaftMgr->pLog, pAppendRsp->nCurrIndex);
		LOG_CHECK_ERROR(pEntry);

		if (pEntry && pEntry->nTerm == pRaftMgr->nCurrTerm && raft_service_get_commit_index(pRaftMgr) < pAppendRsp->nCurrIndex)
		{
			int32_t nVoteCount = 0;
			for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
			{
				RAFT_NODE* pCurrNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
				if (pCurrNode != pRaftMgr->pMyNode && raft_node_is_voting(pRaftNode) && pAppendRsp->nCurrIndex <= raft_node_get_match_index(pRaftNode))
				{
					nVoteCount++;
				}
			}

			if (nVoteCount > _raft_get_voting_count(pRaftMgr) / 2)
			{
				pRaftMgr->nCommitIndex = pAppendRsp->nCurrIndex;
			}
		}
	}

	if (raft_log_get_entry(pRaftMgr->pLog, raft_node_get_next_index(pRaftNode)))
		_raft_service_send_append_entry(pRaftMgr, pRaftNode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _raft_service_send_append_entry_to_all(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftMgr->nTimeoutElapsed = 0;
	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pCurrNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
		if(pRaftMgr->pMyNode == pCurrNode)
			continue;

		nRetCode = _raft_service_send_append_entry(pRaftMgr, pCurrNode);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_apply_entry(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;
	int32_t nApplyIndex = 0;
	RAFT_ENTRY* pEntry = NULL;
	LOG_PROCESS_ERROR(pRaftMgr);

	LOG_PROCESS_ERROR(pRaftMgr->nLastAppliedIndex < raft_service_get_commit_index(pRaftMgr));

	nApplyIndex = pRaftMgr->nLastAppliedIndex + 1;
	pEntry = raft_log_get_entry(pRaftMgr->pLog, nApplyIndex);
	LOG_PROCESS_ERROR(pEntry);

	pRaftMgr->nLastAppliedIndex++;
	if (pRaftMgr->stCallBackMgr.pApplyEntry)
	{
		nRetCode = pRaftMgr->stCallBackMgr.pApplyEntry(pRaftMgr, pEntry, pRaftMgr->nLastAppliedIndex);
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = lmdb_puts_int(pRaftMgr->stRaftDB.pDBEnv, pRaftMgr->stRaftDB.StateDB, "commit_index", pRaftMgr->nLastAppliedIndex);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _raft_service_apply_all(RAFT_MGR* pRaftMgr)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);

	while (pRaftMgr->nLastAppliedIndex < pRaftMgr->nCommitIndex)
	{
		nRetCode = _raft_service_apply_entry(pRaftMgr);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

int32_t raft_service_get_last_applied_index(RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	return pRaftMgr->nLastAppliedIndex;
Exit0:
	return 0;
}

int32_t raft_service_get_curr_leader_nodeid(RAFT_MGR * pRaftMgr)
{
	RAFT_NODE* pLeader = NULL;
	LOG_PROCESS_ERROR(pRaftMgr);
	
	pLeader = pRaftMgr->pCurrLeader;
	LOG_PROCESS_ERROR(pLeader);

	return raft_node_get_nodeid(pLeader);
Exit0:
	return 0;
}

int32_t raft_service_get_status(RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	return pRaftMgr->nStaus;
Exit0:
	return 0;
}

int32_t raft_service_get_curr_term(RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	return pRaftMgr->nCurrTerm;
Exit0:
	return 0;
}

int32_t raft_service_get_curr_index(RAFT_MGR * pRaftMgr)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pRaftMgr);

	return raft_log_get_curr_index(pRaftMgr->pLog);
Exit0:
	return 0;
}

int32_t raft_service_get_commit_index(RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	return pRaftMgr->nCommitIndex;
Exit0:
	return 0;
}

RAFT_NODE* raft_service_add_node(RAFT_MGR* pRaftMgr, int32_t nNodeID, BOOL bSelf)
{
	int32_t nRetCode = 0;
	RAFT_NODE* pRaftNode = NULL;

	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftNode = raft_service_get_node(pRaftMgr, nNodeID);
	if (pRaftNode)
	{
		if (!raft_node_is_voting(pRaftNode))
			raft_node_set_is_voting(pRaftNode, TRUE);
	}
	else
	{
		void* pNewData = NULL;

		pRaftNode = raft_node_new(nNodeID, NULL);
		LOG_PROCESS_ERROR(pRaftNode);

		pNewData = realloc(pRaftMgr->pRaftNodes, sizeof(void*) * (pRaftMgr->nRaftNodeCount + 1));
		if (pNewData == NULL)
		{
			raft_node_free(pRaftNode);
			return NULL;
		}

		pRaftMgr->pRaftNodes = (void**)pNewData;
		pRaftMgr->pRaftNodes[pRaftMgr->nRaftNodeCount] = pRaftNode;
		pRaftMgr->nRaftNodeCount++;

		if (bSelf)
			pRaftMgr->pMyNode = pRaftNode;
			
		if(!raft_node_is_voting(pRaftNode))
			raft_node_set_is_voting(pRaftNode, TRUE);
	}

	INF("[raft]: add node, nodeid %s is_self %d", tbus_get_str(nNodeID), bSelf);
		
	return pRaftNode;
Exit0:
	return NULL;
}

BOOL raft_service_del_node(RAFT_MGR* pRaftMgr, RAFT_NODE* pRaftNode)
{
	//we don't support del node for now
	return FALSE;
}

RAFT_NODE* raft_service_get_node(RAFT_MGR* pRaftMgr, int32_t nNodeID)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pRaftMgr);

	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		if (raft_node_get_nodeid((RAFT_NODE*)pRaftMgr->pRaftNodes[i]) == nNodeID)
			return (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
	}

Exit0:
	return NULL;
}

BOOL raft_service_set_callback(RAFT_MGR* pRaftMgr, RAFT_CALLBACK_MGR& stCallBackMgr)
{
	LOG_PROCESS_ERROR(pRaftMgr);

	if (stCallBackMgr.pLogAppend)
		pRaftMgr->stCallBackMgr.pLogAppend = stCallBackMgr.pLogAppend;
	if (stCallBackMgr.pLogDeleteLast)
		pRaftMgr->stCallBackMgr.pLogDeleteLast = stCallBackMgr.pLogDeleteLast;
	if (stCallBackMgr.pLogClear)
		pRaftMgr->stCallBackMgr.pLogClear = stCallBackMgr.pLogClear;
	if (stCallBackMgr.pPersistTerm)
		pRaftMgr->stCallBackMgr.pPersistTerm = stCallBackMgr.pPersistTerm;
	if (stCallBackMgr.pPersistVote)
		pRaftMgr->stCallBackMgr.pPersistVote = stCallBackMgr.pPersistVote;
	if (stCallBackMgr.pApplyEntry)
		pRaftMgr->stCallBackMgr.pApplyEntry = stCallBackMgr.pApplyEntry;
	if(stCallBackMgr.pSendRequest)
		pRaftMgr->stCallBackMgr.pSendRequest = stCallBackMgr.pSendRequest;
	if (stCallBackMgr.pBecomeLeader)
		pRaftMgr->stCallBackMgr.pBecomeLeader = stCallBackMgr.pBecomeLeader;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL raft_service_set_timeout_config(RAFT_MGR* pRaftMgr, int32_t nRequestTimeOut, int32_t nElectionTimeout)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(nRequestTimeOut > 0);
	LOG_PROCESS_ERROR(nElectionTimeout > 0);

	pRaftMgr->nRequestTimeout = nRequestTimeOut;
	pRaftMgr->nElectionTimeout = nElectionTimeout;

	return TRUE;
Exit0:
	return FALSE;
}

void raft_service_mainloop(RAFT_MGR* pRaftMgr, int32_t nIntervalMS)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftMgr->nTimeoutElapsed += nIntervalMS;

	if (_raft_get_voting_count(pRaftMgr) == 1 && raft_node_is_voting(pRaftMgr->pMyNode) && 
			pRaftMgr->nStaus != rsLeader)
	{
		_raft_become_leader(pRaftMgr);
	}

	if (pRaftMgr->nStaus == rsLeader)
	{
		if (pRaftMgr->nTimeoutElapsed > pRaftMgr->nRequestTimeout)
		{
			nRetCode = _raft_service_send_append_entry_to_all(pRaftMgr);
			LOG_CHECK_ERROR(nRetCode);
		}
	}
	else
	{
		if (pRaftMgr->nTimeoutElapsed > pRaftMgr->nRandElectionTimeout)
		{
			nRetCode = _raft_service_election_start(pRaftMgr);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	if (pRaftMgr->nLastAppliedIndex < pRaftMgr->nCommitIndex)
	{
		nRetCode = _raft_service_apply_all(pRaftMgr);
		LOG_CHECK_ERROR(nRetCode);
	}

Exit0:
	return;
}

BOOL raft_service_set_data(RAFT_MGR* pRaftMgr, int32_t nType, int64_t llParam, void* pData, int32_t nDataSize)
{
	int32_t nRetCode = 0;
	RAFT_ENTRY stEntry = { 0 };

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pData);
	LOG_PROCESS_ERROR(nDataSize > 0);
	LOG_PROCESS_ERROR(nType > radtInvalid && nType < radtTotal);
	LOG_PROCESS_ERROR(pRaftMgr->nStaus == rsLeader);

	// append local
	stEntry.nEntryID = 0;
	stEntry.nTerm = pRaftMgr->nCurrTerm;
	stEntry.nType = nType;
	stEntry.llParam = llParam;
	stEntry.stData.dwSize = nDataSize;
	stEntry.stData.pBuff = pData;
	raft_log_append_entry(pRaftMgr->pLog, &stEntry);

	// send to peer
	for (int32_t i = 0; i < pRaftMgr->nRaftNodeCount; i++)
	{
		RAFT_NODE* pNode = (RAFT_NODE*)pRaftMgr->pRaftNodes[i];
		if (pNode == pRaftMgr->pMyNode)
			continue;

		if (!raft_node_is_voting(pNode))
			continue;

		if (raft_node_get_next_index(pNode) == raft_service_get_curr_index(pRaftMgr))
		{
			_raft_service_send_append_entry(pRaftMgr, pNode);
		}
	}

	if (_raft_get_voting_count(pRaftMgr) == 1)
		_raft_service_set_commit_index(pRaftMgr, raft_service_get_curr_index(pRaftMgr));

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL _raft_service_append_entry(RAFT_MGR* pRaftMgr, RAFT_ENTRY* pEntry)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr); 
	LOG_PROCESS_ERROR(pEntry);

	raft_log_append_entry(pRaftMgr->pLog, pEntry);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL raft_service_recv_msg(RAFT_MGR* pRaftMgr, int32_t nNodeID, int32_t nMsgType, const char* pMsgData)
{
	int32_t nRetCode = 0;

	switch (nMsgType)
	{
	case rmtVoteReq:
	{
		nRetCode = _raft_service_recv_request_vote(pRaftMgr, (VOTE_REQUEST*)pMsgData);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case rmtVoteRsp:
	{
		nRetCode = _raft_service_recv_request_vote_rsp(pRaftMgr, nNodeID, (VOTE_RESPONSE*)pMsgData);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case rmtAppendReq:
	{
		nRetCode = _raft_service_recv_append_entry(pRaftMgr, (APPEND_ENTRY_REQUEST*)pMsgData);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case rmtAppenRsp:
	{
		nRetCode = _raft_service_recv_append_entry_rsp(pRaftMgr, nNodeID, (APPEND_ENTRY_RESPONSE*)pMsgData);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}