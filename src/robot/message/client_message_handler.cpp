#include "stdafx.h"
#include "define/error_def.h"
#include "define/server_def.h"

#include "game_data/global_config.h"

#include "robot_conn.h"
#include "robot_user.h"

#include "client_message_handler.h"

#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/proto_msgid.pb.h"

CClientMessageHandler CClientMessageHandler::ms_Instance;

CClientMessageHandler::CClientMessageHandler()
{

}

CClientMessageHandler::~CClientMessageHandler()
{

}

BOOL CClientMessageHandler::init(void)
{
	s_MessageFuncList[sc_message_login] = &CClientMessageHandler::on_login;

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
	int32_t nRetCode = 0;
	CRobotUser* pUser = NULL;
	CS_MESSAGE_LOGIN msg;

	pUser = CRobotUserMgr::instance().find_user(pConn->nUserID);
	LOG_PROCESS_ERROR(pUser);

	msg.set_userid(11111);
	msg.set_password("this is password");

	nRetCode = send(pUser, cs_message_login, msg);
	LOG_PROCESS_ERROR(nRetCode);
Exit0:
	return;
}

void CClientMessageHandler::on_conn_stop(ROBOT_CONNECTION* pConn)
{

}
	
void CClientMessageHandler::on_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser)
{
	SC_MESSAGE_LOGIN* msg = (SC_MESSAGE_LOGIN*)pMsg;

	INF("get login rsp, %s", msg->answer().c_str());
}
//------------------------------------------------------------------