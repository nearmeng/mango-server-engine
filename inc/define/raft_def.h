#ifndef _RAFT_DEF_H_
#define _RAFT_DEF_H_

#define		INIT_RAFT_LOG_SIZE			(64)
#define		DEFAULT_REQUEST_TIMEOUT		(200)
#define		DEFAULT_ELECTION_TIMEOUT	(1000)
#define		DEFAULT_DB_SIZE_MB			(100)

struct RAFT_MGR;
struct RAFT_ENTRY;
struct RAFT_NODE;

typedef BOOL(*RAFT_CALLBACK_LOG_EVENT)(RAFT_MGR* pRaftMgr, RAFT_ENTRY* pEntry, int32_t nEntryIndex);
typedef BOOL(*RAFT_CALLBACK_PERSIST_VOTE)(RAFT_MGR* pRaftMgr, int32_t nNodeID);
typedef BOOL(*RAFT_CALLBACK_PERSIST_TERM)(RAFT_MGR* pRaftMgr, int32_t nTerm);
typedef BOOL(*RAFT_CALLBACK_APPLY_ENTRY)(RAFT_MGR* pRaftMgr, RAFT_ENTRY* pEntry, int32_t nIndex);
typedef BOOL(*RAFT_CALLBACK_SEND_REQUEST)(int32_t nNodeID, int32_t nMsgType, const char* pBuff, int32_t nSize);
typedef BOOL(*RAFT_CALLBACK_BECOME_LEADER)(int32_t nTerm);

struct RAFT_CALLBACK_MGR
{
	RAFT_CALLBACK_MGR()
	{
		pLogAppend = NULL;
		pLogDeleteLast = NULL;
		pLogClear = NULL;				
		pPersistVote = NULL;
		pPersistTerm = NULL;
		pApplyEntry = NULL;
		pSendRequest = NULL;
	}

	RAFT_CALLBACK_LOG_EVENT					pLogAppend;
	RAFT_CALLBACK_LOG_EVENT					pLogDeleteLast;
	RAFT_CALLBACK_LOG_EVENT					pLogClear;				// not use for now
	RAFT_CALLBACK_PERSIST_VOTE  			pPersistVote;
	RAFT_CALLBACK_PERSIST_TERM				pPersistTerm;
	RAFT_CALLBACK_APPLY_ENTRY				pApplyEntry;
	RAFT_CALLBACK_SEND_REQUEST				pSendRequest;
	RAFT_CALLBACK_BECOME_LEADER				pBecomeLeader;
};

struct RAFT_ENTRY_DATA
{
	void*		pBuff;
	uint32_t	dwSize;
};

struct RAFT_ENTRY
{
	int32_t nEntryID;
	int32_t nTerm;
	int32_t nType;
	int64_t llParam;
	RAFT_ENTRY_DATA stData;
};

enum RAFT_DATA_TYPE
{
	radtInvalid, 

	radtInitServiceInfo,
	radtEventExcute,

	radtTotal
};

enum RAFT_STATUS
{
	rsInvalid,	

	rsLeader,
	rsCandidate,
	rsFollower,

	rsTotal
};

enum RAFT_NODE_STATE
{
	rnsInvalid = 0,

	rnsVotedForMe	= 1 << 0,
	rnsVoting		= 1 << 1,

	rnsTotal
};

enum RAFT_MSG_TYPE
{
	rmtInvalid,

	rmtVoteReq,
	rmtVoteRsp,
	rmtAppendReq,
	rmtAppenRsp,

	rmtTotal
};

//interact msg
struct VOTE_REQUEST
{
	int32_t nTerm;
	int32_t nLastLogIndex;
	int32_t nLastLogTerm;
	int32_t nCandidateID;
};

struct VOTE_RESPONSE
{
	BOOL		bGranted;
	int32_t		nTerm;
};

struct APPEND_ENTRY_REQUEST
{
	int32_t		nTerm;
	int32_t		nLeaderID;
	int32_t		nLeaderCommit;
	int32_t		nPreLogIndex;
	int32_t		nPreLogTerm;
	int32_t		nEntryCount;
	RAFT_ENTRY  stEntry[0];
};

struct APPEND_ENTRY_RESPONSE
{
	int32_t		nTerm;
	BOOL		bSuccess;
	int32_t		nCurrIndex;
};

//tolua_begin
//tolua_end

#endif