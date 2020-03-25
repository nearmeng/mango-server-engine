#include "stdafx.h"
#include "define/error_def.h"
#include "define/server_def.h"

#include "game_data/global_config.h"

#include "robot_conn.h"
#include "robot_user.h"

#include "client_message_handler.h"

#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"

CClientMessageHandler CClientMessageHandler::ms_Instance;

CClientMessageHandler::CClientMessageHandler()
{

}

CClientMessageHandler::~CClientMessageHandler()
{

}

BOOL CClientMessageHandler::init(void)
{
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CClientMessageHandler::uninit(void)
{
	return TRUE;
}

void CClientMessageHandler::recv(const char* pBuffer, int32_t nSize, CRobotUser *pUser)
{
	int32_t nRetCode = 0;
	int32_t nHeadLen = 0;
	SC_HEAD Head;

	nHeadLen = pBuffer[0];

	nRetCode = Head.ParseFromArray(pBuffer + 1, nHeadLen);
	LOG_PROCESS_ERROR(nRetCode);

	

	//(this->*s_MessageFuncList[pHeader->wMsg])(pBuffer + sizeof(*pHeader), nSize - sizeof(*pHeader), pUser);

Exit0:
	return;
}


BOOL CClientMessageHandler::send(int32_t nConnID, int32_t nMsgID, google::protobuf::Message& Msg)
{
	int32_t nRetCode = 0;
	uint8_t* pBuffer = NULL;
	int32_t nSize = 0;
	//EXTERNAL_MESSAGE_HEADER* pHeader = NULL;
	ROBOT_CONNECTION* pConn = NULL;

	//nSize = Msg.ByteSize() + sizeof(*pHeader);
	
	pBuffer = new uint8_t[nSize];
	LOG_PROCESS_ERROR(pBuffer);

	//pHeader = (EXTERNAL_MESSAGE_HEADER*)pBuffer;
	//pHeader->wMsg = (uint16_t)nMsgID;

	//nRetCode = Msg.SerializeToArray(pBuffer + sizeof(*pHeader), nSize - sizeof(*pHeader));
	//LOG_PROCESS_ERROR(nRetCode);

	pConn = CRobotConnMgr::instance().get_conn(nConnID);
	LOG_PROCESS_ERROR(pConn);

	nRetCode = CRobotConnMgr::instance().send(pConn, (const char*)pBuffer, nSize);
	LOG_PROCESS_ERROR(nRetCode);
	
    SAFE_DELETE_ARRAY(pBuffer);
	return TRUE;
Exit0:
	if (pConn)
	{
		INF("fail to send msg to server, user %s:%d", pConn->sUserName.c_str(), pConn->nUserID);
	}

    SAFE_DELETE_ARRAY(pBuffer);
	return FALSE;
}
//------------------------------------------------------------------