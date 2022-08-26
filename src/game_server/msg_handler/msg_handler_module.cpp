#include "stdafx.h"

#include "google/protobuf/message.h"
#include "app/server_msg_handler.h"
#include "app/server_app.h"

#include "time/time_mgr.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"

#include "msg_handler/msg_handler_module.h"
#include "session/session_module.h"
#include "role/role_module.h"

MG_REGISTER_MODULE_PRE_INIT(CMsgHandlerModule);

CMsgHandlerModule*  g_MsgHandlerModule;

BOOL CMsgHandlerModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    memset(m_RoleMsgHandler, 0, sizeof(m_RoleMsgHandler));
    memset(m_ClientMsgHandler, 0, sizeof(m_ClientMsgHandler));
    memset(m_ConnEventHandler, 0, sizeof(m_ConnEventHandler));

    ::register_conn_event_handler(cetStart, _on_conn_start_event);
    ::register_conn_event_handler(cetStop, _on_conn_stop_event);

    g_MsgHandlerModule = MG_GET_MODULE(CMsgHandlerModule);
    LOG_PROCESS_ERROR(g_MsgHandlerModule);

    m_pSessionModule = MG_GET_MODULE(CSessionModule);
    LOG_PROCESS_ERROR(m_pSessionModule);

    INF("default session module inited");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsgHandlerModule::uninit(void)
{
    return TRUE;
}
    
BOOL CMsgHandlerModule::register_conn_event_handler(int32_t nEventType, UPPER_CONN_EVENT_HANDLER pEventHandler)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(nEventType > cetInvalid && nEventType < cetTotal);
    LOG_PROCESS_ERROR(pEventHandler);
    LOG_PROCESS_ERROR(m_ConnEventHandler[nEventType] == NULL);

    m_ConnEventHandler[nEventType] = pEventHandler;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsgHandlerModule::register_client_msg_handler(int32_t nCSMsgID, UPPER_CLIENT_MSG_HANDLER pMsgHandler)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pMsgHandler);
    LOG_PROCESS_ERROR(nCSMsgID > 0 && nCSMsgID < MAX_MESSAGE_ID);
    LOG_PROCESS_ERROR(m_ClientMsgHandler[nCSMsgID] == NULL);

    nRetCode = ::register_client_msg_handler(nCSMsgID, _on_recv_client_msg);
    LOG_PROCESS_ERROR(nRetCode);

    m_ClientMsgHandler[nCSMsgID] = pMsgHandler;

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CMsgHandlerModule::register_role_msg_handler(int32_t nCSMsgID, UPPER_ROLE_MSG_HANDLER pMsgHandler)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pMsgHandler);
    LOG_PROCESS_ERROR(nCSMsgID > 0 && nCSMsgID < MAX_MESSAGE_ID);
    LOG_PROCESS_ERROR(m_RoleMsgHandler[nCSMsgID] == NULL);
    
    nRetCode = ::register_client_msg_handler(nCSMsgID, _on_recv_client_msg);
    LOG_PROCESS_ERROR(nRetCode);

    m_RoleMsgHandler[nCSMsgID] = pMsgHandler;

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CMsgHandlerModule::register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler)
{
    int32_t nRetCode = 0;
    LOG_PROCESS_ERROR(pMsgHandler);

    nRetCode = ::register_server_msg_handler(nSSMsgID, pMsgHandler);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CMsgHandlerModule::send_error_code_to_client(CLIENT_SESSION* pSession, int32_t nErrorCode, uint64_t qwParam)
{
    int32_t nRetCode = 0;
    SC_ERROR_CODE msg;

    msg.set_error_code(nErrorCode);
    msg.set_param(qwParam);

    nRetCode = send_to_client(pSession, sc_error_code, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsgHandlerModule::send_error_code_to_client_by_sessionid(uint64_t qwSessionID, int32_t nErrorCode, uint64_t qwParam)
{
    int32_t nRetCode = 0;
    CLIENT_SESSION* pSession = NULL;

    pSession = m_pSessionModule->find_session(qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    nRetCode = send_error_code_to_client(pSession, nErrorCode, qwParam);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
	
BOOL CMsgHandlerModule::multicast_to_client(std::vector<CLIENT_SESSION*> vSession, int32_t nMsgID, const google::protobuf::Message* pMsg)
{
	int32_t nRetCode = 0;
	std::map<int32_t, std::vector<uint64_t> > Target;	// conn_server --> conn_id list

	LOG_PROCESS_ERROR(pMsg);

	for (int32_t i = 0; i < vSession.size(); i++)
	{
		Target[vSession[i]->nConnServerAddr].push_back(vSession[i]->qwConnID);
	}

	LOG_PROCESS_ERROR(Target.size() > 0);

	nRetCode = ::multicast_to_client(Target, nMsgID, pMsg);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMsgHandlerModule::broadcast_to_client(int32_t nMsgID, const google::protobuf::Message* pMsg)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pMsg);

	nRetCode = ::broadcast_to_client(nMsgID, pMsg);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMsgHandlerModule::send_to_client(uint64_t qwSessionID, int32_t nMsgID, const google::protobuf::Message* pMsg)
{
    CLIENT_SESSION* pSession = m_pSessionModule->find_session(qwSessionID);
    return send_to_client(pSession, nMsgID, pMsg);
}

BOOL CMsgHandlerModule::send_to_client(CLIENT_SESSION * pSession, int32_t nMsgID, const google::protobuf::Message * pMsg)
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

void CMsgHandlerModule::_on_recv_client_msg(uint64_t qwConnID, const CS_HEAD * pHead, const google::protobuf::Message * pMsg)
{
    CLIENT_SESSION* pSession = NULL;
    int32_t nCSMsgID = pHead->msgid();
    
    LOG_PROCESS_ERROR(g_MsgHandlerModule->m_ClientMsgHandler[nCSMsgID]);
    
    pSession = g_MsgHandlerModule->m_pSessionModule->find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);

    if (pSession->qwCurrRoleID > 0)
    {
        CRole* pRole = g_MsgHandlerModule->m_pRoleModule->find_role(pSession->qwConnID);
        LOG_PROCESS_ERROR(pRole);

        g_MsgHandlerModule->m_RoleMsgHandler[nCSMsgID](pRole, pHead, pMsg);
    }
    else
        g_MsgHandlerModule->m_ClientMsgHandler[nCSMsgID](pSession, pHead, pMsg);

Exit0:
    return;
}
    
void CMsgHandlerModule::on_conn_timeout_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pSession);

    if (g_MsgHandlerModule->m_ConnEventHandler[cetTimeout])
    {
        g_MsgHandlerModule->m_ConnEventHandler[cetTimeout](pSession);
    }

Exit0:
    return;
}

void CMsgHandlerModule::_on_conn_start_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr)
{
    CLIENT_SESSION* pSession = NULL;

    LOG_PROCESS_ERROR(pcszOpenID);

    pSession = g_MsgHandlerModule->m_pSessionModule->find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession == NULL);

    pSession = g_MsgHandlerModule->m_pSessionModule->new_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);

    pSession->qwConnID = qwConnID;
    pSession->nConnServerAddr = nConnServerAddr;
    strxcpy(pSession->szOpenID, pcszOpenID, sizeof(pSession->szOpenID));
    
    pSession->qwUserID = 0;
    pSession->qwCurrRoleID = 0;
    pSession->dwLastPingTime = CTimeMgr::instance().get_time_sec();

    INF("on conn start, connid %llu openid %s server_addr %s", qwConnID, pcszOpenID, tbus_get_str(nConnServerAddr));

    if (g_MsgHandlerModule->m_ConnEventHandler[cetStart])
    {
        g_MsgHandlerModule->m_ConnEventHandler[cetStart](pSession);
    }

Exit0:
    return;
}

void CMsgHandlerModule::_on_conn_stop_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr)
{
    int32_t nRetCode = 0;
    CLIENT_SESSION* pSession = NULL;

    pSession = g_MsgHandlerModule->m_pSessionModule->find_session(qwConnID);
    LOG_PROCESS_ERROR(pSession);
    
    INF("on conn stop, connid %llu openid %s server_addr %s", qwConnID, pSession->szOpenID, tbus_get_str(nConnServerAddr));
    
    if (g_MsgHandlerModule->m_ConnEventHandler[cetStop])
    {
        g_MsgHandlerModule->m_ConnEventHandler[cetStop](pSession);
    }

    nRetCode = g_MsgHandlerModule->m_pSessionModule->destroy_session(pSession);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMsgHandlerModule::on_frame()
{
}