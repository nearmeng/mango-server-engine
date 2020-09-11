#ifndef  _GAME_ONLINE_MESSAGE_H_
#define  _GAME_ONLINE_MESSAGE_H_

#include "protocol/internal_message_header.h"

#pragma pack(push, 1)

enum GAME_ONLINE_MESSAGE_ID
{
	g2o_user_login = game_online_message_begin,
    o2g_user_login_ack,
    g2o_user_logout,
    o2g_user_logout_ack,
    o2g_user_kick,
};

struct G2O_USER_LOGIN : INTERNAL_MESSAGE_HEADER
{
    uint64_t        qwUserID;
    uint64_t        qwSessionID;
    int32_t         nServerAddr;
};

struct O2G_USER_LOGIN_ACK: INTERNAL_MESSAGE_HEADER
{
    int32_t         nErrorCode;
    uint64_t        qwUserID;
    uint64_t        qwSessionID;
};

struct G2O_USER_LOGOUT : INTERNAL_MESSAGE_HEADER
{
    uint64_t        qwUserID;
    uint64_t        qwSessionID;
    int32_t         nServerAddr;
};

struct O2G_USER_LOGOUT_ACK : INTERNAL_MESSAGE_HEADER
{
    int32_t         nErrorCode;
    uint64_t        qwUserID;
    uint64_t        qwSessionID;
};

struct O2G_USER_KICK : INTERNAL_MESSAGE_HEADER
{
    int32_t         nErrorCode;
    uint64_t        qwUserID;
    uint64_t        qwSessionID;
};

#pragma pack(pop)

#endif
