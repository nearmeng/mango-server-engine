#ifndef _SESSION_MGR_H_
#define _SESSION_MGR_H_

#include "shm/shm_pool.h"
#include "define/session_def.h"

class CSessionMgr
{
public:
    CSessionMgr() {};
    ~CSessionMgr() {};

    BOOL init(BOOL bResume);
    BOOL uninit(void);

    void mainloop(void);

    inline CLIENT_SESSION* new_session(uint64_t qwSessionID);
    inline BOOL destroy_session(CLIENT_SESSION* pSession);
    inline CLIENT_SESSION* find_session(uint64_t qwSessionID);

private:
    struct TRAVERSE_SESSION
	{
		BOOL operator()(uint64_t qwSessionID, CLIENT_SESSION* pSession);
	};
	TRAVERSE_SESSION	m_TraverseSession;

    CShmObjectPool<CLIENT_SESSION, uint64_t>    m_ClientSessionPool;
};

inline CLIENT_SESSION* CSessionMgr::new_session(uint64_t qwSessionID)
{
    return m_ClientSessionPool.new_object(qwSessionID);
}

inline BOOL CSessionMgr::destroy_session(CLIENT_SESSION* pSession)
{
    return m_ClientSessionPool.delete_object(pSession);
}

inline CLIENT_SESSION* CSessionMgr::find_session(uint64_t qwSessionID)
{
    return m_ClientSessionPool.find_object(qwSessionID);
}

#endif