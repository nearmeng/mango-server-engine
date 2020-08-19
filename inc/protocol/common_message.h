#ifndef _COMMON_MESSAGE_H_
#define _COMMON_MESSAGE_H_

#include "internal_message_header.h"
#include "define/str_def.h"

#pragma pack(push, 1)
//tolua_begin

enum COMMON_MESSAGE_ID
{
	a2a_register = common_message_begin,
	a2a_unregister,
	a2a_ping,
	a2a_stop,
	a2a_time_sync,
    a2a_control_req,
    a2a_control_ack,
};

struct A2A_REGISTER : INTERNAL_MESSAGE_HEADER
{
	int32_t		nSrcAddr;
    uint8_t		bInit;
    uint8_t		bResume;
};

struct A2A_UNREGISTER : INTERNAL_MESSAGE_HEADER
{
	int32_t		nSrcAddr;
};

struct A2A_PING : INTERNAL_MESSAGE_HEADER
{
	int32_t		nSrcAddr;
};

struct A2A_STOP : INTERNAL_MESSAGE_HEADER
{
	int32_t     nSrcAddr;
};

struct A2A_TIME_SYNC : INTERNAL_MESSAGE_HEADER
{
	int64_t   llTimeDiff;
};

struct A2A_CONTROL_REQ : INTERNAL_MESSAGE_HEADER 
{
	char szCommandType[MAX_CMD_TYPE_LEN];
    char szCommandContent[MAX_GM_COMMAND_LEN];
	uint64_t qwParam;
};

struct A2A_CONTROL_ACK : INTERNAL_MESSAGE_HEADER
{
    int32_t nResult;
    int32_t nDescLen;
    char szDesc[MAX_GM_COMMAND_LEN];
};

struct TEST_SEND_DATA : INTERNAL_MESSAGE_HEADER
{
    int32_t nData;  
};

// tolua_end
#pragma pack(pop)
#endif	// _COMMON_MESSAGE_H_
