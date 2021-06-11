#ifndef  _DB_PROXY_MESSAGE_H_
#define  _DB_PROXY_MESSAGE_H_

#include "protocol/internal_message_header.h"

#include "define/redis_def.h"

#pragma pack(push, 1)

enum ROUTER_MESSAGE_ID
{
	db_proxy_client_redis_req = db_proxy_message_begin,
    db_proxy_client_redis_rsp,
};

struct DB_PROXY_CLIENT_REDIS_REQ : INTERNAL_MESSAGE_HEADER
{
    int32_t nCmdID;
    int32_t nUserDataSize;
    char    szUserData[MAX_REDIS_USER_DATA_LEN];
    int32_t nCommandSize;
    char    szCommandBuffer[0];
};

struct DB_PROXY_CLIENT_REDIS_RSP : INTERNAL_MESSAGE_HEADER
{
    int32_t nCmdID;
    int32_t nUserDataSize;
    char    szUserData[MAX_REDIS_USER_DATA_LEN];
    int32_t nReplySize;
    char    szReplyBuffer[0];
};

#pragma pack(pop)

#endif