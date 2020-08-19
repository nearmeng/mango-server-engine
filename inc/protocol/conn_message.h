#ifndef  _ROUTER_MESSAGE_H_
#define  _ROUTER_MESSAGE_H_

#include "protocol/internal_message_header.h"

#pragma pack(push, 1)

enum CONN_MESSAGE_ID
{
    conn_ntf_event = conn_message_begin,
    conn_ntf_event_ack,
    conn_transfer_msg,
};

struct CONN_NTF_EVENT : INTERNAL_MESSAGE_HEADER
{
    uint64_t    qwConnID;
    int32_t     nEventType;
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

#pragma pack(pop)

#endif