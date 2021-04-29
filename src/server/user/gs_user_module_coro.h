#ifndef _GS_USER_MODULE_CORO_H_
#define _GS_USER_MODULE_CORO_H_

#include "coroutine/coro_stackless.h"

#include "define/session_def.h"

struct USER;
class CRole;
class CUserModule;
class CRoleModule;
class CSessionModule;

class CLoginCoro : public CCoroStackless
{
public:
    CLoginCoro() {};
    virtual ~CLoginCoro() {};

    BOOL on_resume();
    void set_start_arg(const char* pcszOpenID, uint64_t qwSessionID);
    virtual CORO_STATE coro_process();

private:
    char        m_szOpenID[MAX_OPENID_LEN];
    uint64_t    m_qwUserID;
    uint64_t    m_qwSessionID;

    CUserModule* m_pUserModule;
    CSessionModule* m_pSessionModule;
};

class CCreateRoleCoro : public CCoroStackless
{
public:
    CCreateRoleCoro() {};
    virtual ~CCreateRoleCoro() {};

    BOOL on_resume();
    void set_start_arg(uint64_t qwSessionID, uint64_t qwUserID, const char* pcszRoleName);
    virtual CORO_STATE coro_process();

private:
    char        m_szRoleName[COMMON_NAME_LEN];
    uint64_t    m_qwRoleID;
    uint64_t    m_qwUserID;
    uint64_t    m_qwSessionID;
    
    uint32_t    m_dwRoleSize;
    uint32_t    m_dwRoleBaseSize;

    CRole*      m_pRole;

    CUserModule* m_pUserModule;
    CRoleModule* m_pRoleModule;
};

class CSelectRoleCoro : public CCoroStackless
{
public:
    CSelectRoleCoro() {};
    ~CSelectRoleCoro() {};
    
    BOOL on_resume();
    void set_start_arg(uint64_t qwSessionID, uint64_t qwUserID, uint64_t qwRoleID);
    virtual CORO_STATE coro_process();

private:
    uint64_t    m_qwRoleID;
    uint64_t    m_qwUserID;
    uint64_t    m_qwSessionID;

    CUserModule* m_pUserModule;
    CRoleModule* m_pRoleModule;
    CSessionModule* m_pSessionModule;
};

class CKickUserCoro : public CCoroStackless
{
public:
    CKickUserCoro() {};
    ~CKickUserCoro() {};

    BOOL on_resume();
    void set_start_arg(uint64_t qwUserID, uint64_t qwSessionID, int32_t nKickReason, uint64_t qwKickParam);
    virtual CORO_STATE coro_process();

private:
    uint64_t        m_qwUserID;
    uint64_t        m_qwSessionID;
    int32_t         m_nKickReason;
    uint64_t        m_qwKickParam;

    USER*           m_pUser;
    CRole*          m_pRole;

    CUserModule*    m_pUserModule;
    CRoleModule*    m_pRoleModule;
    CSessionModule* m_pSessionModule;
};

#endif
