#include "stdafx.h"
#include "raft_node.h"

struct RAFT_NODE
{
	int32_t nNodeID;
	void* pUserData;
	int32_t nNextIndex;
	int32_t nMatchIndex;
	int32_t nState;
};

RAFT_NODE* raft_node_new(int32_t nNodeID, void* pUserData)
{
	RAFT_NODE* pRaftNode = NULL;

	pRaftNode = (RAFT_NODE*)malloc(sizeof(RAFT_NODE));
	LOG_PROCESS_ERROR(pRaftNode);

	pRaftNode->nNodeID = nNodeID;
	pRaftNode->pUserData = pUserData;
	pRaftNode->nNextIndex = 1;
	pRaftNode->nMatchIndex = 0;
	pRaftNode->nState = rnsVoting;

	return pRaftNode;
Exit0:
	return NULL;
}

void raft_node_free(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	SAFE_FREE(pRaftNode);
Exit0:
	return;
}

void raft_node_set_next_index(RAFT_NODE* pRaftNode, int32_t nNextIndex)
{
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(nNextIndex > 0);

	pRaftNode->nNextIndex = nNextIndex;
Exit0:
	return;
}

void raft_node_set_match_index(RAFT_NODE* pRaftNode, int32_t nMatchIndex)
{
	LOG_PROCESS_ERROR(pRaftNode);

	pRaftNode->nMatchIndex = nMatchIndex;
Exit0:
	return;
}

int32_t raft_node_get_match_index(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	return pRaftNode->nMatchIndex;
Exit0:
	return 0;
}

void* raft_node_get_user_data(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	return pRaftNode->pUserData;
Exit0:
	return NULL;
}

void raft_node_set_user_data(RAFT_NODE* pRaftNode, void* pUserData)
{
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(pUserData);

	pRaftNode->pUserData = pUserData;

Exit0:
	return;
}

void raft_node_vote_for_me(RAFT_NODE* pRaftNode, BOOL bVote)
{
	LOG_PROCESS_ERROR(pRaftNode);

	if (bVote)
		pRaftNode->nState |= rnsVotedForMe;
	else
		pRaftNode->nState &= ~rnsVotedForMe;

Exit0:
	return;
}

BOOL raft_node_has_vote_for_me(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	return (pRaftNode->nState & rnsVotedForMe) > 0;
Exit0:
	return FALSE;
}

void raft_node_set_is_voting(RAFT_NODE* pRaftNode, BOOL bVoting)
{
	LOG_PROCESS_ERROR(pRaftNode);
	LOG_PROCESS_ERROR(!raft_node_is_voting(pRaftNode));

	if (bVoting)
		pRaftNode->nState |= rnsVoting;
	else
		pRaftNode->nState &= ~rnsVoting;

Exit0:
	return;
}

BOOL raft_node_is_voting(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	return (pRaftNode->nState & rnsVoting) > 0;
Exit0:
	return FALSE;
}

int32_t raft_node_get_nodeid(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);
	return pRaftNode->nNodeID;
Exit0:
	return 0;
}

int32_t raft_node_get_next_index(RAFT_NODE* pRaftNode)
{
	LOG_PROCESS_ERROR(pRaftNode);

	return pRaftNode->nNextIndex;
Exit0:
	return 0;
}