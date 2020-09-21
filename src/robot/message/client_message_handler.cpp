#include "stdafx.h"
#include "define/error_def.h"
#include "define/server_def.h"

#include "config/global_config.h"

#include "robot_conn.h"
#include "robot_user.h"

#include "client_message_handler.h"

#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/proto_msgid.pb.h"

#include "time/time_mgr.h"

CClientMessageHandler CClientMessageHandler::ms_Instance;

CClientMessageHandler::CClientMessageHandler()
{

}

CClientMessageHandler::~CClientMessageHandler()
{

}

BOOL CClientMessageHandler::init(void)
{
	s_MessageFuncList[sc_allow_login] = &CClientMessageHandler::on_allow_login;
	s_MessageFuncList[sc_login] = &CClientMessageHandler::on_login;
    s_MessageFuncList[sc_sync_role_list] = &CClientMessageHandler::on_sync_role_list;
    s_MessageFuncList[sc_sync_role_data] = &CClientMessageHandler::on_sync_role_data;
    s_MessageFuncList[sc_error_code] = &CClientMessageHandler::on_recv_error_code;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CClientMessageHandler::uninit(void)
{
	return TRUE;
}

void CClientMessageHandler::recv(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser *pUser)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pSCHead);
	LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(s_MessageFuncList[pSCHead->msgid()]);

	(this->*s_MessageFuncList[pSCHead->msgid()])(pSCHead, pMsg, pUser);

Exit0:
	return;
}


BOOL CClientMessageHandler::send(CRobotUser* pUser, int32_t nMsgID, google::protobuf::Message& Msg)
{
	int32_t nRetCode = 0;
	uint8_t* pBuffer = NULL;
	int32_t nSize = 0;
	ROBOT_CONNECTION* pConn = NULL;

	pConn = pUser->get_conn();
	LOG_PROCESS_ERROR(pConn);

	nRetCode = CRobotConnMgr::instance().send(pConn, nMsgID, Msg);
	LOG_PROCESS_ERROR(nRetCode);
	
	return TRUE;
Exit0:
	if (pConn)
	{
		INF("fail to send msg to server, user %s:%d", pConn->sUserName.c_str(), pConn->nUserID);
	}
	return FALSE;
}
	
void CClientMessageHandler::on_conn_start(ROBOT_CONNECTION* pConn)
{
Exit0:
	return;
}

void CClientMessageHandler::on_conn_stop(ROBOT_CONNECTION* pConn)
{

}
	
void CClientMessageHandler::on_allow_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser)
{
	int32_t nRetCode = 0;
	CS_LOGIN msg;

	//msg.set_userid(11111);
	//msg.set_password("this is password");
    msg.set_server_group(1);

	nRetCode = send(pUser, cs_login, msg);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}
	
void CClientMessageHandler::on_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser)
{
	SC_LOGIN* msg = (SC_LOGIN*)pMsg;

	INF("get login rsp, %s", msg->answer().c_str());
}
    
BOOL CClientMessageHandler::do_create_role(CRobotUser* pUser, const char* pcszRoleName)
{
    int32_t nRetCode = 0;
    CS_CREATE_ROLE msg;

    LOG_PROCESS_ERROR(pcszRoleName);

    msg.set_role_name(pcszRoleName);

    nRetCode = send(pUser, cs_create_role, msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CClientMessageHandler::do_select_role(CRobotUser* pUser, uint64_t qwRoleID)
{
    int32_t nRetCode = 0;
    CS_SELECT_ROLE msg;

    msg.set_roleid(qwRoleID);

    nRetCode = send(pUser, cs_select_role, msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CClientMessageHandler::do_logout(CRobotUser* pUser)
{
    int32_t nRetCode = 0;
    CS_LOGOUT msg;

    nRetCode = send(pUser, cs_logout, msg);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CClientMessageHandler::on_sync_role_data(SC_HEAD * pSCHead, google::protobuf::Message * pMsg, CRobotUser * pUser)
{
    int32_t nRetCode = 0;
    SC_SYNC_ROLE_DATA* msg = (SC_SYNC_ROLE_DATA*)pMsg;

    INF("recv sync role data, roleid %llu role_name %s level %d", msg->roleid(), msg->rolename().c_str(), msg->level());

    //nRetCode = do_logout(pUser);
    //LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CClientMessageHandler::on_sync_role_list(SC_HEAD * pSCHead, google::protobuf::Message * pMsg, CRobotUser * pUser)
{
    int32_t nRetCode = 0;
    SC_SYNC_ROLE_LIST* msg = (SC_SYNC_ROLE_LIST*)pMsg;

    INF("recv sync role list from server, role_list_count %d", msg->role_list_size());

    for (int32_t i = 0; i < msg->role_list_size(); i++)
    {
        const ROLE_INFO* pRoleInfo = &msg->role_list(i);
        
        INF("get role list index %d, role_name %s level %d", i, pRoleInfo->role_name().c_str(), pRoleInfo->level());
    }

    if (msg->role_list_size() == 0)
    {
        //create role
        char szName[256];
        snprintf(szName, sizeof(szName), "robot_%s", CTimeMgr::get_time_str(time(NULL)));

        nRetCode = do_create_role(pUser, szName);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        //select role
        nRetCode = do_select_role(pUser, msg->role_list(0).roleid());
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void CClientMessageHandler::on_recv_error_code(SC_HEAD * pSCHead, google::protobuf::Message * pMsg, CRobotUser * pUser)
{
    SC_ERROR_CODE* msg = (SC_ERROR_CODE*)pMsg;

    INF("on recv error code %d", msg->error_code());
}
	
BOOL CClientMessageHandler::do_gm_command(const char* pGMCommand, CRobotUser* pUser)
{
	int32_t nRetCode = 0;
	

	return TRUE;
Exit0:
	return FALSE;
}
//------------------------------------------------------------------