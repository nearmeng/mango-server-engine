#ifndef _CONN_MODULE_H_
#define _CONN_MODULE_H_

#include "module/server_module.h"
#include "shm/shm_pool.h"

struct CONN_SESSION
{
    uint32_t    dwInterIP;
    uint32_t    dwInterPort;
    uint32_t    dwOuterIP;
    uint32_t    dwOuterPort;
    int32_t     nConnIndex;         // tconnd conn
    int32_t     nConnAddr;          // tconnd addr
    int32_t     nBackendAddr;       // backend
    int32_t     nClientType;        // client type
    char        szUserStr[128];     // user string

    uint32_t    dwLastPingTime;     
};

class CConnModule : CServerModule
{
public:
    CConnModule() {};
    ~CConnModule() {};

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);
    
    inline static CConnModule* instance(const char* pcszModuleName, ...);

    inline CONN_SESSION* new_session(uint64_t qwSessionID);
    inline BOOL del_session(CONN_SESSION* pSession);
    inline CONN_SESSION* find_session(uint64_t qwSessionID);

    BOOL kick_conn_by_session(uint64_t qwSessionID, int32_t nReason);

private:
    BOOL _init_msg_handler(void);

private:
    CShmObjectPool<CONN_SESSION, uint64_t>  m_SessionMgr;
};

inline CConnModule* CConnModule::instance(const char* pcszModuleName, ...)
{
    CConnModule* pModule = new CConnModule();

    pModule->set_name(pcszModuleName);

    return pModule;
}

inline CONN_SESSION* CConnModule::new_session(uint64_t qwSessionID)
{
    return m_SessionMgr.new_object(qwSessionID);
}

inline BOOL CConnModule::del_session(CONN_SESSION* pSession)
{
    return m_SessionMgr.delete_object(pSession);
}

inline CONN_SESSION* CConnModule::find_session(uint64_t qwSessionID)
{
    return m_SessionMgr.find_object(qwSessionID);
}

#endif