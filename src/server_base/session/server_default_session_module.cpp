#include "stdafx.h"
#include "module/server_default_session_module.h"

#include "google/protobuf/message.h"
#include "app/server_msg_handler.h"
#include "app/server_app.h"

#include "time/time_mgr.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"

BOOL CServerDefaultSessionModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    memset(m_ClientMsgHandler, 0, sizeof(m_ClientMsgHandler));
    memset(m_ConnEventHandler, 0, sizeof(m_ConnEventHandler));

    nRetCode = m_SessionMgr.init(bResume);
    LOG_PROCESS_ERROR(nRetCode);

    register_conn_event_handler(cetStart, _on_conn_start_event);
    register_conn_event_handler(cetStop, _on_conn_stop_event);
    
    register_client_msg_handler(cs_ping, _on_recv_client_ping);

    INF("default session module inited");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CServerDefaultSessionModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_SessionMgr.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}
    
BOOL CServerDefaultSessionModule::register_conn_event_handler_for_session(int32_t nEventType, CONN_EVENT_HANDLER_FOR_SESSION pEventHandler)
{
    int32_t nRetCode = 0;
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    LOG_PROCESS_ERROR(nEventType > cetInvalid && nEventType < cetTotal);
    LOG_PROCESS_ERROR(pEventHandler);
    LOG_PROCESS_ERROR(pModule->m_ConnEventHandler[nEventType] == NULL);

    pModule->m_ConnEventHandler[nEventType] = pEventHandler;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CServerDefaultSessionModule::register_client_msg_handler_for_session(int32_t nCSMsgID, CLIENT_MSG_HANDLER_FOR_SESSION pMsgHandler)
{
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    LOG_PROCESS_ERROR(pMsgHandler);
    LOG_PROCESS_ERROR(nCSMsgID > 0 && nCSMsgID < MAX_MESSAGE_ID);
    LOG_PROCESS_ERROR(pModule->m_ClientMsgHandler[nCSMsgID] == NULL);

    pModule->m_ClientMsgHandler[nCSMsgID] = pMsgHandler;

    register_client_msg_handler(nCSMsgID, _on_recv_client_msg);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CServerDefaultSessionModule::send_error_code_to_client(CLIENT_SESSION* pSession, int32_t nErrorCode)
{
    int32_t nRetCode = 0;
    SC_ERROR_CODE msg;

    msg.set_error_code(nErrorCode);

    nRetCode = send_to_client(pSession, sc_error_code, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CServerDefaultSessionModule::send_error_code_to_client_by_sessionid(uint64_t qwSessionID, int32_t nErrorCode)
{
    int32_t nRetCode = 0;
    CLIENT_SESSION* pSession = NULL;
    
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    pSession = pModule->m_SessionMgr.find_session(qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    nRetCode = send_error_code_to_client(pSession, nErrorCode);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CServerDefaultSessionModule::send_to_client(CLIENT_SESSION * pSession, int32_t nMsgID, const google::protobuf::Message * pMsg)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pSession);
    LOG_PROCESS_ERROR(pMsg);

    nRetCode = ::send_to_client(pSession->nConnServerAddr, pSession->qwConnID, nMsgID, pMsg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CServerDefaultSessionModule::_on_recv_client_ping(uint64_t qwConnID, const CS_HEAD * pHead, const google::protobuf::Message * pMsg)
{
    CLIENT_SESSION* pSession = NULL;
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    pSession = pModule->m_SessionMgr.find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);

    pSession->dwLastPingTime = CTimeMgr::instance().get_time_sec();

Exit0:
    return;
}

void CServerDefaultSessionModule::_on_recv_client_msg(uint64_t qwConnID, const CS_HEAD * pHead, const google::protobuf::Message * pMsg)
{
    CLIENT_SESSION* pSession = NULL;
    int32_t nCSMsgID = pHead->msgid();
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);
    
    LOG_PROCESS_ERROR(pModule->m_ClientMsgHandler[nCSMsgID]);
    
    pSession = pModule->m_SessionMgr.find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);

    pModule->m_ClientMsgHandler[nCSMsgID](pSession, pHead, pMsg);

Exit0:
    return;
}

void CServerDefaultSessionModule::_on_conn_start_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr)
{
    CLIENT_SESSION* pSession = NULL;
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    LOG_PROCESS_ERROR(pcszOpenID);

    pSession = pModule->m_SessionMgr.find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession == NULL);

    pSession = pModule->m_SessionMgr.new_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);

    pSession->qwConnID = qwConnID;
    pSession->nConnServerAddr = nConnServerAddr;
    strxcpy(pSession->szOpenID, pcszOpenID, sizeof(pSession->szOpenID));
    
    pSession->qwUserID = 0;
    pSession->dwLastPingTime = CTimeMgr::instance().get_time_sec();

    INF("on conn start, connid %llu openid %s server_addr %s", qwConnID, pcszOpenID, tbus_get_str(nConnServerAddr));

    if (pModule->m_ConnEventHandler[cetStart])
    {
        pModule->m_ConnEventHandler[cetStart](pSession);
    }

Exit0:
    return;
}

void CServerDefaultSessionModule::_on_conn_stop_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr)
{
    int32_t nRetCode = 0;
    CLIENT_SESSION* pSession = NULL;
    CServerDefaultSessionModule* pModule = MG_GET_MODULE(CServerDefaultSessionModule);

    pSession = pModule->m_SessionMgr.find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);
    
    INF("on conn stop, connid %llu openid %s server_addr %s", qwConnID, pSession->szOpenID, tbus_get_str(nConnServerAddr));
    
    if (pModule->m_ConnEventHandler[cetStop])
    {
        pModule->m_ConnEventHandler[cetStop](pSession);
    }

    nRetCode = pModule->m_SessionMgr.destroy_session(pSession);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CServerDefaultSessionModule::on_frame()
{
    //session timeout check
    m_SessionMgr.mainloop();
}

CLIENT_SESSION* CServerDefaultSessionModule::find_session(uint64_t qwSessionID)
{
    return m_SessionMgr.find_session(qwSessionID);
}

BOOL CServerDefaultSessionModule::kick_session(CLIENT_SESSION * pSession)
{
    int32_t nRetCode = 0;

    //send stop to tconnd

    //clear session

    return TRUE;
Exit0:
    return FALSE;
}
