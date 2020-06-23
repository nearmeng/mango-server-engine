#include "stdafx.h"
#include "test_module.h"

#include "router_client/router_client_api.h"
#include "tconnd/inc/tconnapi/tframehead.h"

#include "protocol/common_message.h"
#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/proto_head.pb.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"
#include "string/string_ex.h"
#include "define/server_def.h"

static int conn = 0;

void on_conn_start(uint64_t qwConnID, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize)
{
	INF("recv start conn");

	pFrameHead->iID = pFrameHead->iConnIdx;
	conn = pFrameHead->iID;
	send_conn_msg(GET_TCONND_ADDR(qwConnID), pFrameHead, NULL, NULL);
}

void on_login(int32_t nSrcAddr, const CS_HEAD* pHead, const Message* pMsg)
{
	int32_t nRetCode = 0;
	CS_MESSAGE_LOGIN* msg = (CS_MESSAGE_LOGIN*)pMsg;

	INF("login msg, id %d password %s", msg->userid(), msg->password().c_str());

	SC_HEAD Head;
	TFRAMEHEAD FrameHead;

	FrameHead.chCmd = TFRAMEHEAD_CMD_INPROC;
	FrameHead.stCmdData.stInProc.chValid = 0;
	FrameHead.iConnIdx = conn;
	FrameHead.iID = conn;

	Head.set_msgid(sc_message_login);
	Head.set_seqid(0);

	SC_MESSAGE_LOGIN rsp;
	rsp.set_answer("this is rsp");

	nRetCode = send_conn_msg(nSrcAddr, &FrameHead, &Head, &rsp);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;

}

BOOL do_send_control_ack(int32_t nResult, const char* pDesc, int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	A2A_CONTROL_ACK msg;

	msg.nResult = nResult;
	strxcpy(msg.szDesc, pDesc, sizeof(msg.szDesc));
	msg.nDescLen = strlen(pDesc);

	nRetCode = send_server_msg_by_addr(nDstAddr, a2a_control_ack, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

void on_control(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	A2A_CONTROL_REQ* msg = (A2A_CONTROL_REQ*)pBuffer;

	INF("test on control command type %s command content %s param %lld", msg->szCommandType, msg->szCommandContent, msg->qwParam);

	if (strcmp(msg->szCommandType, "reload") == 0)
	{
        CMGApp::instance().reload();
	}

	nRetCode = do_send_control_ack(0, "success", nSrcAddr);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

BOOL CTestModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

	INF("test module is init");

	register_conn_msg_handler(TFRAMEHEAD_CMD_START, on_conn_start);
	register_client_msg_handler(cs_message_login, on_login);
	register_server_msg_handler(a2a_control_req, on_control);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CTestModule::uninit(void)
{
    return TRUE;
Exit0:
    return FALSE;
}
    
CTestModule* CTestModule::instance(const char* pcszModuleName, ...)
{
    CTestModule* pModule = new CTestModule();
    
    pModule->set_name(pcszModuleName);

    return pModule;
}