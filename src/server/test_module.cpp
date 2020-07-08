#include "stdafx.h"
#include "test_module.h"

#include "router_client/router_client_api.h"
#include "tconnd/inc/tconnapi/tframehead.h"

#include "protocol/common_message.h"
#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/proto_head.pb.h"
#include "protocol/conn_message.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"
#include "string/string_ex.h"
#include "define/server_def.h"
#include "define/conn_def.h"

#include "game_data/res_def/achieve_res.h"

std::map<uint64_t, int32_t> ms_AddrMap;

void on_conn_start_event(uint64_t qwConnID, int32_t nConnServerAddr)
{
    int32_t nRetCode = 0;
    SC_MESSAGE_ALLOW_LOGIN msg;

    ms_AddrMap[qwConnID] = nConnServerAddr;

    nRetCode = send_to_client(nConnServerAddr, qwConnID, sc_message_allow_login, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    INF("recv conn %lld ntf start event", qwConnID);

Exit0:
    return;
}

void on_conn_stop_event(uint64_t qwConnID, int32_t nConnServerAddr)
{
    INF("recv conn %lld ntf stop event", qwConnID);
}

void on_login(uint64_t qwConnID, const CS_HEAD* pHead, const Message* pMsg)
{
	int32_t nRetCode = 0;
	CS_MESSAGE_LOGIN* msg = (CS_MESSAGE_LOGIN*)pMsg;

	INF("login msg, id %d password %s", msg->userid(), msg->password().c_str());

	SC_MESSAGE_LOGIN rsp;
	rsp.set_answer("this is rsp");

    nRetCode = send_to_client(ms_AddrMap[qwConnID], qwConnID, sc_message_login, &rsp);
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
    int32_t nCount = 0;
    const ACHIEVE_RES* pRes = CResMgr<ACHIEVE_RES>::instance().get_first_res();

	INF("test on control command type %s command content %s param %lld", msg->szCommandType, msg->szCommandContent, msg->qwParam);

	if (strcmp(msg->szCommandType, "reload") == 0)
	{
        CMGApp::instance().reload();
	}
    
    while (pRes)
    {
        nCount++;
        INF("all read count %d", nCount);
        INF("res_data: id %d name %s cond_type %d achieve_type %d cond_param %d reward_id %d score %d", pRes->nID, pRes->szName, pRes->nCondType, pRes->nAchieveType, pRes->nCondParam[0], pRes->nRewardID, pRes->nScore);
        pRes = CResMgr<ACHIEVE_RES>::instance().get_next_res();
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

	register_client_msg_handler(cs_message_login, on_login);

	register_server_msg_handler(a2a_control_req, on_control);
    register_conn_event_handler(cetStart, on_conn_start_event);
    register_conn_event_handler(cetStop, on_conn_stop_event);

    int32_t nCount = 0;
    const ACHIEVE_RES* pRes = CResMgr<ACHIEVE_RES>::instance().get_first_res();
    while (pRes)
    {
        nCount++;
        INF("all read count %d", nCount);
        INF("res_data: id %d name %s cond_type %d achieve_type %d cond_param %d reward_id %d score %d", pRes->nID, pRes->szName, pRes->nCondType, pRes->nAchieveType, pRes->nCondParam[0], pRes->nRewardID, pRes->nScore);
        pRes = CResMgr<ACHIEVE_RES>::instance().get_next_res();
    }

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