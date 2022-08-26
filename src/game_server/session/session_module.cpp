#include "stdafx.h"

#include "config/global_config.h"
#include "time/time_mgr.h"

#include "session/session_module.h"
#include "msg_handler/msg_handler_module.h"

#include "protocol/proto_msgid.pb.h"

BOOL CSessionModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;
    
    nRetCode = m_ClientSessionPool.init(stdServerSessionDefault, g_ServerConfig.Common.nInitDefaultClientSessionCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    LOG_PROCESS_ERROR(g_MsgHandlerModule);
    g_MsgHandlerModule->register_client_msg_handler(cs_ping, _on_recv_client_ping);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSessionModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_ClientSessionPool.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

void CSessionModule::mainloop(void)
{
    TRAVERSE_SESSION TraverseSession;

    m_ClientSessionPool.traverse(TraverseSession);
}

BOOL CSessionModule::TRAVERSE_SESSION::operator()(uint64_t qwSessionID, CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    uint32_t dwCurrTime = CTimeMgr::instance().get_time_sec();

    if (dwCurrTime > pSession->dwLastPingTime + g_ServerConfig.Common.nClientPingTimeout / 1000)
    {
        CRI("session %llu is timeout, do kick", qwSessionID);

        g_MsgHandlerModule->on_conn_timeout_event(pSession);

        //delete it anyway
        CSessionModule* pSessionModule = MG_GET_MODULE(CSessionModule);
        LOG_PROCESS_ERROR(pSessionModule);

        pSession = pSessionModule->find_session(qwSessionID);
        if (pSession)
        {
            WRN("upper layer did't delete session %llu, so we delete it", qwSessionID);
            LOG_CHECK_ERROR(pSessionModule->destroy_session(pSession));
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSessionModule::kick_session(CLIENT_SESSION * pSession, int32_t nReason, uint64_t qwReasonParam)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pSession);

    //send stop to tconnd
    nRetCode = send_to_conn_server(pSession->nConnServerAddr, pSession->qwConnID, cetStop, nReason, qwReasonParam);
    LOG_PROCESS_ERROR(nRetCode);

    //clear session
    nRetCode = destroy_session(pSession);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CSessionModule::_on_recv_client_ping(CLIENT_SESSION* pSession, const CS_HEAD * pHead, const google::protobuf::Message * pMsg)
{
    pSession->dwLastPingTime = CTimeMgr::instance().get_time_sec();
}

MG_REGISTER_MODULE(CSessionModule);