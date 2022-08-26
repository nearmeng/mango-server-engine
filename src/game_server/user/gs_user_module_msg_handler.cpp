#include "stdafx.h"

#include "user/gs_user_module.h"
#include "session/session_module.h"
#include "msg_handler/msg_handler_module.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/game_online_msg.h"

#include "gs_user_module_coro.h"

#include "role/role.h"

BOOL do_g2c_sync_role_list(uint64_t qwSessionID, USER* pUser)
{
    int32_t nRetCode = 0;
    SC_SYNC_ROLE_LIST msg;

    LOG_PROCESS_ERROR(pUser);

    for (int32_t i = 0; i < pUser->nRoleCount; i++)
    {
        ROLE_INFO* pRoleInfo = msg.add_role_list();
        LOG_PROCESS_ERROR(pRoleInfo);

        pRoleInfo->set_roleid(pUser->stRoleBase[i].qwRoleID);
        pRoleInfo->set_role_name(pUser->stRoleBase[i].szRoleName);
        pRoleInfo->set_level(pUser->stRoleBase[i].wLevel);
        pRoleInfo->set_create_time(pUser->stRoleBase[i].dwCreateTime);
    }

    nRetCode = g_MsgHandlerModule->send_to_client(qwSessionID, sc_sync_role_list, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL do_g2c_sync_role_data(uint64_t qwSessionID, CRole* pRole)
{
    int32_t nRetCode = 0;
    SC_SYNC_ROLE_DATA msg;

    LOG_PROCESS_ERROR(pRole);

    msg.set_roleid(pRole->get_obj_id());
    msg.set_rolename(pRole->get_name());
    msg.set_level(pRole->get_level());
    
    nRetCode = g_MsgHandlerModule->send_to_client(qwSessionID, sc_sync_role_data, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void on_conn_start_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    SC_ALLOW_LOGIN msg;

    nRetCode = g_MsgHandlerModule->send_to_client(pSession, sc_allow_login, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    INF("recv conn %lld ntf start event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);

Exit0:
    return;
}

void on_conn_stop_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    USER* pUser = NULL;
    CUserModule* pUserModule = NULL;
    CSessionModule* pSessionModule = NULL;
    
    INF("recv conn %lld ntf stop event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);

    pUserModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pUserModule);

    pSessionModule = MG_GET_MODULE(CSessionModule);
    LOG_PROCESS_ERROR(pSessionModule);
    
    if (pSession->qwUserID > 0)
    {
        pUser = pUserModule->find_user(pSession->qwUserID);
        LOG_PROCESS_ERROR(pUser);
    
        pUser->qwSessionID = 0;
        pUser->nSessionStopTime = CTimeMgr::instance().get_time_sec();
    }

    nRetCode = pSessionModule->destroy_session(pSession);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void on_conn_timeout_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    USER* pUser = NULL;
    CUserModule* pUserModule = NULL;
    CSessionModule* pSessionModule = NULL;
    
    INF("recv conn %lld timeout, last ping %d", pSession->qwConnID, pSession->dwLastPingTime);

    pUserModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pUserModule);

    pSessionModule = MG_GET_MODULE(CSessionModule);
    LOG_PROCESS_ERROR(pSessionModule);

    nRetCode = pSessionModule->kick_session(pSession, errConnTimeout, 0);
    LOG_PROCESS_ERROR(nRetCode);
    
    pUser =  pUserModule->find_user(pSession->qwUserID);
    LOG_PROCESS_ERROR(pUser);
    
    pUser->qwSessionID = 0;
    pUser->nSessionStopTime = CTimeMgr::instance().get_time_sec();

Exit0:
    return;
}

void on_login(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{
	int32_t nRetCode = 0;
	CS_LOGIN* msg = (CS_LOGIN*)pMsg;
    CLoginCoro* pCoro = NULL;

	INF("login msg, openid is %s", pSession->szOpenID);
        
    pCoro = CCoroStacklessMgr<CLoginCoro>::instance().new_coro();
    LOG_PROCESS_ERROR(pCoro);

    pCoro->set_start_arg(pSession->szOpenID, pSession->qwConnID);
    nRetCode = CCoroStacklessMgr<CLoginCoro>::instance().start_coro(pCoro);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void on_logout(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{
    int32_t nRetCode = 0;
    CUserModule* pModule = NULL;

    pModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pModule);

    nRetCode = pModule->kick_user(pSession->qwUserID, errLogoutNormal, 0);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void on_create_role(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{
    int32_t nRetCode = 0;
    CS_CREATE_ROLE* msg = (CS_CREATE_ROLE*)pMsg;
    CCreateRoleCoro* pCoro = NULL;

    INF("create role msg, rolename is %s", msg->role_name().c_str());

    pCoro = CCoroStacklessMgr<CCreateRoleCoro>::instance().new_coro();
    LOG_PROCESS_ERROR(pCoro);

    pCoro->set_start_arg(pSession->qwConnID, pSession->qwUserID, msg->role_name().c_str());
    nRetCode = CCoroStacklessMgr<CCreateRoleCoro>::instance().start_coro(pCoro);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void on_select_role(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{
    int32_t nRetCode = 0;
    CS_SELECT_ROLE* msg = (CS_SELECT_ROLE*)pMsg;
    CSelectRoleCoro* pCoro = NULL;

    INF("select role msg, roleid is %llu", msg->roleid());

    pCoro = CCoroStacklessMgr<CSelectRoleCoro>::instance().new_coro();
    LOG_PROCESS_ERROR(pCoro);

    pCoro->set_start_arg(pSession->qwConnID, pSession->qwUserID, msg->roleid());
    nRetCode = CCoroStacklessMgr<CSelectRoleCoro>::instance().start_coro(pCoro);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void on_relay(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{

}

void on_os_user_kick(SSMSG_CONTEXT* pCtx, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
    CUserModule* pModule = NULL;
    O2G_USER_KICK* msg = (O2G_USER_KICK*)pBuffer;

    pModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pModule);

    nRetCode = pModule->kick_user(msg->qwUserID, msg->nErrorCode, 0);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

BOOL CUserModule::_init_msg_handler()
{
    g_MsgHandlerModule->register_conn_event_handler(cetStart, on_conn_start_event);
    g_MsgHandlerModule->register_conn_event_handler(cetStop, on_conn_stop_event);
    g_MsgHandlerModule->register_conn_event_handler(cetTimeout, on_conn_timeout_event);
    
    g_MsgHandlerModule->register_client_msg_handler(cs_login, on_login);
    g_MsgHandlerModule->register_client_msg_handler(cs_logout, on_logout);
    g_MsgHandlerModule->register_client_msg_handler(cs_create_role, on_create_role);
    g_MsgHandlerModule->register_client_msg_handler(cs_select_role, on_select_role);

    g_MsgHandlerModule->register_server_msg_handler(o2g_user_kick, on_os_user_kick);

    return TRUE;
}