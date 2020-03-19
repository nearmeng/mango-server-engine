#include "stdafx.h"
#include <time.h>

#include "game_data/global_config.h"
#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"

#include "protocol/common_message.h"

void test_func(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
	TEST_SEND_DATA* msg = (TEST_SEND_DATA*)pBuffer;

	INF("recv data is %d", msg->nData);
}

BOOL server_init(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;

	INF("server is init");

	register_server_msg_handler(263, test_func);

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

