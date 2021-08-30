#ifndef  _ROUTER_MESSAGE_H_
#define  _ROUTER_MESSAGE_H_

#include "define/session_def.h"
#include "protocol/internal_message_header.h"

#define MAX_MULTICAST_CONN_PER_TCONND		(128)

#pragma pack(push, 1)

enum CONN_MESSAGE_ID
{
    conn_ntf_event = conn_message_begin,
    conn_ntf_event_ack,
    conn_transfer_msg,
    conn_business_event,
	conn_broadcast_msg,
	conn_multicast_msg,
};

struct CONN_NTF_EVENT : INTERNAL_MESSAGE_HEADER
{
    uint64_t    qwConnID;
    int32_t     nEventType;
    char        szOpenID[MAX_OPENID_LEN];
};

struct CONN_NTF_EVENT_ACK : INTERNAL_MESSAGE_HEADER
{
    uint64_t    qwConnID;
    int32_t     nEventType;
};

struct CONN_TRANSFER_MSG : INTERNAL_MESSAGE_HEADER
{
    uint64_t    qwConnID;
    int32_t     nMsgSize;
    char        szMsg[0];
};

struct CONN_BUSINESS_EVENT : INTERNAL_MESSAGE_HEADER
{
    uint64_t    qwConnID;
    int32_t     nEventType;
    uint64_t    qwEventParam0;
    uint64_t    qwEventParam1;
};

struct CONN_BROADCAST_MSG : INTERNAL_MESSAGE_HEADER
{
	int32_t		nMsgSize;
	char		szMsg[0];
};

struct CONN_MULTICAST_MSG : INTERNAL_MESSAGE_HEADER
{
	int32_t		nConnIDCount;
	uint64_t	qwConnID[MAX_MULTICAST_CONN_PER_TCONND];
	int32_t		nMsgSize;
	char		szMsg[0];
};

#pragma pack(pop)

#endif