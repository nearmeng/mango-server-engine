#include "stdafx.h"
#include <time.h>

#include "game_data/global_config.h"
#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"

#include "protocol/common_message.h"

void test_func(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
	//TEST_SEND_DATA* msg = (TEST_SEND_DATA*)pBuffer;

	//INF("recv data is %d", msg->nData);
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

	nRetCode = do_send_control_ack(0, "success", nSrcAddr);
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

BOOL server_frame(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

BOOL server_reload(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

int main(int argc, char* argv[])
{ 
	mg_app_main(argc, argv, server_init, server_fini, server_frame, server_reload);

Exit0:
	return 0;
}

