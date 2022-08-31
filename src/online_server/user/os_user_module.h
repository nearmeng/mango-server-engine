#ifndef _OS_USER_MODULE_H_
#define _OS_USER_MODULE_H_

#include "module/server_module.h"

struct OS_USER
{
    uint64_t    qwUserID;
    uint64_t    qwSessionID;
    int32_t     nServerAddr;
};

class CUserModule : public CServerModule
{
public:
    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);

    inline OS_USER* new_user(uint64_t qwUserID);
    inline BOOL del_user(OS_USER* pUser);
    inline OS_USER* find_user(uint64_t qwUserID);
    inline int32_t get_user_count(void);

private:
    static void on_user_login(SSMSG_CONTEXT* pCtx, const char* pBuffer, size_t dwSize);
    static void on_user_logout(SSMSG_CONTEXT* pCtx, const char* pBuffer, size_t dwSize);

private:
    BOOL _init_msg_handler(void);

private:
    CShmObjectPool<uint64_t, OS_USER>   m_UserPool;
};

inline OS_USER* CUserModule::new_user(uint64_t qwUserID)
{
    return m_UserPool.new_object(qwUserID);
}

inline BOOL CUserModule::del_user(OS_USER* pUser)
{
    return m_UserPool.delete_object(pUser);
}

inline OS_USER* CUserModule::find_user(uint64_t qwUserID)
{
    return m_UserPool.find_object(qwUserID);
}

inline int32_t CUserModule::get_user_count()
{
    return m_UserPool.get_used_count();
}

#endif