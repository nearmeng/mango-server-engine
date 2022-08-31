#ifndef _SESSION_MGR_H_
#define _SESSION_MGR_H_

#include "shm/shm_pool.h"
#include "define/session_def.h"

#include "module/server_module.h"

struct CLIENT_SESSION;

class CSessionModule : public CServerModule
{
public:
    CSessionModule() {};
    ~CSessionModule() {};

    BOOL init(BOOL bResume);
    BOOL uninit(void);

    void mainloop(void);

    inline CLIENT_SESSION* new_session(uint64_t qwSessionID);
    inline BOOL destroy_session(CLIENT_SESSION* pSession);
    inline CLIENT_SESSION* find_session(uint64_t qwSessionID);

    BOOL kick_session(CLIENT_SESSION* pSession, int32_t nReason, uint64_t qwParam);

private:
    static void _on_recv_client_ping(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

private:
    struct TRAVERSE_SESSION
	{
		BOOL operator()(uint64_t qwSessionID, CLIENT_SESSION* pSession);
	};
	TRAVERSE_SESSION	m_TraverseSession;

    CShmObjectPool<uint64_t, CLIENT_SESSION>    m_ClientSessionPool;
};

inline CLIENT_SESSION* CSessionModule::new_session(uint64_t qwSessionID)
{
    return m_ClientSessionPool.new_object(qwSessionID);
}

inline BOOL CSessionModule::destroy_session(CLIENT_SESSION* pSession)
{
    return m_ClientSessionPool.delete_object(pSession);
}

inline CLIENT_SESSION* CSessionModule::find_session(uint64_t qwSessionID)
{
    return m_ClientSessionPool.find_object(qwSessionID);
}

#endif