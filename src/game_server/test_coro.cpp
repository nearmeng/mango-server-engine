#include "stdafx.h"
#include "test_coro.h"

#include "db_proxy_client/db_proxy_client.h"
#include "protocol/common_message.h"
#include "app/server_msg_handler.h"

void CTestCoro::set_start_arg(int32_t nTestValue)
{
    m_nTestValue = nTestValue;
}

CORO_STATE CTestCoro::coro_process()
{
    int32_t nRetCode = 0;
    redisReply* pReply = NULL;

    CORO_BEGIN()

    DBG("[CORO]: coro is start %d, server_addr %d mgr_index %d", get_coro_id(), GET_CORO_SERVER_ADDR(get_coro_id()), GET_CORO_MGR_INDEX(get_coro_id()));

    nRetCode = CDBProxyClient::instance().redis_command_coro("get test_key");
    LOG_PROCESS_ERROR(nRetCode);

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    INF("get result is %s", pReply->str);

    {
        TEST_SEND_DATA msg;
        msg.nData = 123;

        nRetCode = send_server_msg_by_routerid_coro(0, svrTest, 263, &msg, sizeof(msg));
        LOG_PROCESS_ERROR(nRetCode);
    }

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtMsg);

    {
        TEST_SEND_DATA* msg = (TEST_SEND_DATA*)(get_coro_reply().pReplyData);
        INF("get test data %d", msg->nData);
    }

    CORO_END()

Exit0:
    return crsFailed;
}
