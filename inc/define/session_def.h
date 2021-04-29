#ifndef _SESSION_DEF_H_
#define _SESSION_DEF_H_

#define MAX_OPENID_LEN      (64)

struct CLIENT_SESSION
{
    uint64_t    qwConnID;
    int32_t     nConnServerAddr;

    uint64_t    qwUserID;
    char        szOpenID[MAX_OPENID_LEN];
    uint32_t    dwLastPingTime;

    uint64_t    qwCurrRoleID;
};

#endif