#include "stdafx.h"

#include "config/global_config.h"
#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"

#include "protocol/common_message.h"
#include "monitor/monitor_module.h"

void test_func(SSMSG_CONTEXT* pCtx, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
	TEST_SEND_DATA* msg = (TEST_SEND_DATA*)pBuffer;

	INF("recv data is %d", msg->nData);

    nRetCode = pCtx->send_ack(msg->wMsg, pBuffer, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

BOOL do_send_control_ack(SSMSG_CONTEXT* pCtx, int32_t nResult, const char* pDesc)
{
	int32_t nRetCode = 0;
	A2A_CONTROL_ACK msg;

	msg.nResult = nResult;
	strxcpy(msg.szDesc, pDesc, sizeof(msg.szDesc));
	msg.nDescLen = strlen(pDesc);

    nRetCode = pCtx->send_ack(a2a_control_ack, &msg, sizeof(msg));
    LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

void on_control(SSMSG_CONTEXT* pCtx, const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	A2A_CONTROL_REQ* msg = (A2A_CONTROL_REQ*)pBuffer;

	INF("test on control command type %s command content %s param %lld", msg->szCommandType, msg->szCommandContent, msg->qwParam);

	nRetCode = do_send_control_ack(pCtx, 0, "success");
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

BOOL server_init(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;

	INF("server is init");

	register_server_msg_handler(263, test_func);
	register_server_msg_handler(a2a_control_req, on_control);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL server_fini(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;

	INF("server is fini");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL server_frame(void)
{
	return TRUE;
}

BOOL server_reload(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

int main(int argc, char* argv[])
{ 
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();
    
    MG_CONFIG config;
    config.pAppInit = server_init;
    config.pAppFini = server_fini;
    config.pAppFrame = server_frame;
    config.pAppReload = server_reload;
    config.bUseRouter = TRUE;

    pServer->set_config(config);

    nRetCode = pServer->init("func_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}

