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

#include "res_def/achieve_res.h"

#include "db_proxy_client/db_proxy_client.h"
#include "config/global_config.h"

#include "test_coro.h"
#include "module/server_default_session_module.h"

std::map<uint64_t, int32_t> ms_AddrMap;

void on_conn_start_event(CLIENT_SESSION* pSession)
{
    int32_t nRetCode = 0;
    SC_MESSAGE_ALLOW_LOGIN msg;

    nRetCode = SEND_TO_CLIENT_BY_SESSION(pSession, sc_message_allow_login, &msg);
    LOG_PROCESS_ERROR(nRetCode);

    INF("recv conn %lld ntf start event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);

Exit0:
    return;
}

void on_conn_stop_event(CLIENT_SESSION* pSession)
{
    INF("recv conn %lld ntf stop event from addr %d", pSession->qwConnID, pSession->nConnServerAddr);
}

void on_login(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const Message* pMsg)
{
	int32_t nRetCode = 0;
	CS_MESSAGE_LOGIN* msg = (CS_MESSAGE_LOGIN*)pMsg;

	INF("login msg, id %d password %s", msg->userid(), msg->password().c_str());

	SC_MESSAGE_LOGIN rsp;
	rsp.set_answer("this is rsp");

    nRetCode = SEND_TO_CLIENT_BY_SESSION(pSession, sc_message_login, &rsp);
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

struct TEST_USER_DATA
{
    int32_t nTestValue;
    char    szString[20];
};

void _test_redis_callback(redisReply* pReply, const char* pUserData, size_t dwDataSize)
{
    INF("got reply");
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

        /*
        TEST_USER_DATA stTestData;
        stTestData.nTestValue = 12345;
        strxcpy(stTestData.szString, "hello world", sizeof(stTestData.szString));

        CDBProxyClient::instance().reg_redis_callback(recTest, _test_redis_callback);

        //nRetCode = CDBProxyClient::instance().redis_command(1, (const char*)&stTestData, sizeof(stTestData), msg->szCommandContent);
        //LOG_PROCESS_ERROR(nRetCode);
        nRetCode = CDBProxyClient::instance().redis_eval(recTest, (const char*)&stTestData, sizeof(stTestData), g_RedisScriptConfig.scripts[recTest].szScript, "1 %s %d", "hello", 20);
        LOG_PROCESS_ERROR(nRetCode);
        */

        CTestCoro* pCoro = CCoroStacklessMgr<CTestCoro>::instance().new_coro();
        LOG_PROCESS_ERROR(pCoro);

        pCoro->set_start_arg(123456);
        nRetCode = CCoroStacklessMgr<CTestCoro>::instance().start_coro(pCoro);
        LOG_PROCESS_ERROR(nRetCode);
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

    REG_SESSION_CONN_EVENT_HANDLER(cetStart, on_conn_start_event);
    REG_SESSION_CONN_EVENT_HANDLER(cetStop, on_conn_stop_event);
    REG_SESSION_CLI_MSG_HANDLER(cs_message_login, on_login);
	
    register_server_msg_handler(a2a_control_req, on_control);

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