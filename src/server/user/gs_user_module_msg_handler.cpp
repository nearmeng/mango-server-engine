#include "stdafx.h"
#include "module/gs_user_module.h"
#include "module/server_default_session_module.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/game_online_msg.h"

#include "gs_user_module_coro.h"

#include "object/role.h"

BOOL do_g2c_sync_role_list(uint64_t qwSessionID, USER* pUser)
{
    int32_t nRetCode = 0;
    SC_SYNC_ROLE_LIST msg;
    CServerDefaultSessionModule* pModule = NULL;

    LOG_PROCESS_ERROR(pUser);

    pModule = MG_GET_MODULE(CServerDefaultSessionModule);
    LOG_PROCESS_ERROR(pModule);

    for (int32_t i = 0; i < pUser->nRoleCount; i++)
    {
        ROLE_INFO* pRoleInfo = msg.add_role_list();
        LOG_PROCESS_ERROR(pRoleInfo);

        pRoleInfo->set_roleid(pUser->stRoleBase[i].qwRoleID);
        pRoleInfo->set_role_name(pUser->stRoleBase[i].szRoleName);
        pRoleInfo->set_level(pUser->stRoleBase[i].wLevel);
        pRoleInfo->set_create_time(pUser->stRoleBase[i].dwCreateTime);
    }

    nRetCode = SEND_TO_CLIENT_BY_SESSION(pModule->find_session(qwSessionID), sc_sync_role_list, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL do_g2c_sync_role_data(uint64_t qwSessionID, CRole* pRole)
{
    int32_t nRetCode = 0;
    CServerDefaultSessionModule* pModule = NULL;
    SC_SYNC_ROLE_DATA msg;

    LOG_PROCESS_ERROR(pRole);

    pModule = MG_GET_MODULE(CServerDefaultSessionModule);
    LOG_PROCESS_ERROR(pModule);

    msg.set_roleid(pRole->get_obj_id());
    msg.set_rolename(pRole->get_name());
    msg.set_level(pRole->get_level());
    
    nRetCode = SEND_TO_CLIENT_BY_SESSION(pModule->find_session(qwSessionID), sc_sync_role_data, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void on_conn_start_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    SC_ALLOW_LOGIN msg;

    nRetCode = SEND_TO_CLIENT_BY_SESSION(pSession, sc_allow_login, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    INF("recv conn %lld ntf start event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);

Exit0:
    return;
}

void on_conn_stop_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    USER* pUser = NULL;
    CUserModule* pModule = NULL;
    
    INF("recv conn %lld ntf stop event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);

    pModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pModule);
    
    pUser =  pModule->find_user(pSession->qwUserID);
    LOG_PROCESS_ERROR(pUser);

    //nRetCode = pModule->destroy_user(pUser);
    //LOG_PROCESS_ERROR(nRetCode);

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

void on_os_user_kick(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
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
    int32_t nRetCode = 0;

    REG_SESSION_CONN_EVENT_HANDLER(cetStart, on_conn_start_event);
    REG_SESSION_CONN_EVENT_HANDLER(cetStop, on_conn_stop_event);
    
    REG_SESSION_CLI_MSG_HANDLER(cs_login, on_login);
    REG_SESSION_CLI_MSG_HANDLER(cs_logout, on_logout);
    REG_SESSION_CLI_MSG_HANDLER(cs_create_role, on_create_role);
    REG_SESSION_CLI_MSG_HANDLER(cs_select_role, on_select_role);
    //REG_SESSION_CLI_MSG_HANDLER(cs_message_relay, on_relay);

    register_server_msg_handler(o2g_user_kick, on_os_user_kick);

    return TRUE;
}