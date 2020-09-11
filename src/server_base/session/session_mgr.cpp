#include "stdafx.h"

#include "module/session_mgr.h"
#include "config/global_config.h"
#include "time/time_mgr.h"

BOOL CSessionMgr::init(BOOL bResume)
{
    int32_t nRetCode = 0;
    
    nRetCode = m_ClientSessionPool.init(stdServerSessionDefault, g_ServerConfig.Common.nInitDefaultClientSessionCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSessionMgr::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_ClientSessionPool.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

void CSessionMgr::mainloop(void)
{
    TRAVERSE_SESSION TraverseSession;

    m_ClientSessionPool.traverse(TraverseSession);
}

BOOL CSessionMgr::TRAVERSE_SESSION::operator()(uint64_t qwSessionID, CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    uint32_t dwCurrTime = CTimeMgr::instance().get_time_sec();

    if (dwCurrTime > pSession->dwLastPingTime + g_ServerConfig.Common.nClientPingTimeout)
    {
        CRI("session %llu is timeout", qwSessionID);
    }
    return TRUE;
}
