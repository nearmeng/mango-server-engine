#include "stdafx.h"

#include "game_data/global_config.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "message/co_message_handler.h"
#include "control/control_handler.h"

#include "router_client/router_client_api.h"

BOOL server_init(TAPPCTX* pCtx, BOOL bResume)
{
    int nRetCode = 0;

	nRetCode = co_msg_init();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CControlHandler::instance().init();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL server_fini(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = co_msg_uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CControlHandler::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL server_reload(TAPPCTX* pCtx, BOOL bResume)
{
    return TRUE;
}

BOOL control_init(TAPPCTX *pCtx, BOOL bResume)
{
    INF("center controller init");

	setbuf(stdout, NULL);
	setvbuf(stdout, 0, _IONBF, 0);
    return TRUE;
}

BOOL control_fini(TAPPCTX *pCtx, BOOL bResume)
{
    INF("center controller fini");
    return TRUE;
}

BOOL control_pre_proc_cmdline(unsigned short argc, const char** argv)
{
    return TRUE;
}

BOOL control_proc_cmdline(unsigned short argc, const char** argv)
{
    return CControlHandler::instance().proc_cmdline(argc, argv);
}

const char* control_help(void)
{
    return CControlHandler::instance().help_info();
}

int main(int argc, char* argv[])
{ 
	CONTROL_FUNCS stControlFuncs;
	
	stControlFuncs.pInit = &control_init;
	stControlFuncs.pFini = &control_fini;
	stControlFuncs.pHelpInfo = &control_help;
	stControlFuncs.pPreProcCmdLine = &control_pre_proc_cmdline;
	stControlFuncs.pProcCmdLine = &control_proc_cmdline;

	mg_app_main(argc, argv, server_init, server_fini, NULL, server_reload, NULL, NULL, &stControlFuncs);

Exit0:
	return 0;
}
