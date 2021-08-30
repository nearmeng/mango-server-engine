#ifndef _CONN_MODULE_H_
#define _CONN_MODULE_H_

#include "module/server_module.h"
#include "shm/shm_pool.h"

#define MAX_TCONND_COUNT	(1)

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

class CConnModule : public CServerModule
{
public:
    CConnModule() {};
    ~CConnModule() {};

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);

    inline CONN_SESSION* new_session(uint64_t qwSessionID);
    inline BOOL del_session(CONN_SESSION* pSession);
    inline CONN_SESSION* find_session(uint64_t qwSessionID);

    BOOL kick_conn_by_session(uint64_t qwSessionID, int32_t nReason, uint64_t qwParam);

	inline int32_t get_tconnd_count(void);
	inline int32_t get_tconnd_by_index(int32_t nIndex);

private:
    BOOL _init_msg_handler(void);
	BOOL _init_tconnd_addr(void);

private:
    CShmObjectPool<CONN_SESSION, uint64_t>  m_SessionMgr;

	int32_t									m_nTconndAddrCount;
	int32_t									m_nTconndAddr[MAX_TCONND_COUNT];
};

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
	
inline int32_t CConnModule::get_tconnd_count(void)
{
	return m_nTconndAddrCount;
}

inline int32_t CConnModule::get_tconnd_by_index(int32_t nIndex)
{
	LOG_PROCESS_ERROR(nIndex < MAX_TCONND_COUNT && nIndex >= 0);
	return m_nTconndAddr[nIndex];
Exit0:
	return 0;
}

#endif