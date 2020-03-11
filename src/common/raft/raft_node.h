#ifndef _RAFT_NODE_H_
#define _RAFT_NODE_H_

#include "define/raft_def.h"

struct RAFT_NODE;

RAFT_NODE* raft_node_new(int32_t nNodeID, void* pUserData);
void raft_node_free(RAFT_NODE* pRaftNode);

void raft_node_set_next_index(RAFT_NODE* pRaftNode, int32_t nNextIndex);
void raft_node_set_match_index(RAFT_NODE* pRaftNode, int32_t nMatchIndex);
int32_t raft_node_get_match_index(RAFT_NODE* pRaftNode);

void* raft_node_get_user_data(RAFT_NODE* pRaftNode);
void raft_node_set_user_data(RAFT_NODE* pRaftNode, void* pUserData);

void raft_node_vote_for_me(RAFT_NODE* pRaftNode, BOOL bVote);
BOOL raft_node_has_vote_for_me(RAFT_NODE* pRaftNode);

void raft_node_set_is_voting(RAFT_NODE* pRaftNode, BOOL bVoting);
BOOL raft_node_is_voting(RAFT_NODE* pRaftNode);

int32_t raft_node_get_nodeid(RAFT_NODE* pRaftNode);
int32_t raft_node_get_next_index(RAFT_NODE* pRaftNode);

#endif