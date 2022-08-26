#ifndef _GS_USER_MODULE_H_
#define _GS_USER_MODULE_H_

#include "module/server_module.h"
#include "shm/shm_pool.h"

#include "define/str_def.h"

#define MAX_ROLE_COUNT          (4)
#define MAX_BAN_REASON_LEN      (128)
#define MAX_USER_DATA_SIZE      (1024)

enum USER_STATE
{
    usInvalid,

    usLogin,
    usFinishLogin,
    usCreateRole,
    usFinishCreateRole,
    usSelectRole,
    usPlaying,
    usKick,

    usTotal
};

struct ROLE_BASE
{
    uint64_t            qwRoleID;
    char                szRoleName[COMMON_NAME_LEN];
    uint16_t            wLevel;

    uint32_t            dwCreateTime;
    uint32_t            dwLastLoginTime;
    uint32_t            dwLastSaveTime;
};

struct USER
{
    uint64_t            qwUserID;
    uint64_t            qwSessionID;

    int32_t             nRoleCount;
    ROLE_BASE           stRoleBase[MAX_ROLE_COUNT];

    uint64_t            qwCurrPlayingRole;

    uint32_t            dwBanEndTime;
    char                szBanReason[MAX_BAN_REASON_LEN];

    int32_t             nState;
    int32_t             nSessionStopTime;

    BOOL is_role(uint64_t qwRoleID);

    BOOL save_user(char* pData, uint32_t &dwSize);
    BOOL load_user(const char* pData, uint32_t dwSize);

    BOOL save_role_base(uint64_t qwRoleID, char* pData, uint32_t &dwSize);
    BOOL load_role_base(const char* pData, uint32_t dwSize);
};

class CUserModule : public CServerModule
{
public:
    CUserModule() {};
    virtual ~CUserModule() {};

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);

    virtual void on_frame(void);

    inline int32_t get_user_count(void);

    inline USER* create_user(uint64_t qwUserID);
    inline BOOL destroy_user(USER* pUser);
    inline USER* find_user(uint64_t qwUserID);

    BOOL kick_user(uint64_t qwUserID, int32_t nReason, uint64_t qwReasonParam);

private:
    BOOL _init_msg_handler(void);

    struct TRAVERSE_USER_SESSION_CHECK
    {
        BOOL operator()(uint64_t qwUserID, USER* pUser);
    };

private:
    CShmObjectPool<USER, uint64_t>      m_UserPool;
};

inline USER* CUserModule::create_user(uint64_t qwUserID)
{
    USER* pUser = NULL;

    pUser = m_UserPool.new_object(qwUserID);
    LOG_PROCESS_ERROR(pUser);

    memset(pUser, 0, sizeof(USER));
    pUser->qwUserID = qwUserID;

    return pUser;
Exit0:
    return NULL;
}

inline BOOL CUserModule::destroy_user(USER* pUser)
{
    return m_UserPool.delete_object(pUser);
}

inline USER* CUserModule::find_user(uint64_t qwUserID)
{
    return m_UserPool.find_object(qwUserID);
}

inline int32_t CUserModule::get_user_count(void)
{
    return m_UserPool.get_used_count();
}

#endif