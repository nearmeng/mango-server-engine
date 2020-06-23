#include "stdafx.h"

#include "game_data/global_config.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "control_module/control_message_handler.h"
#include "control_module/control_module.h"

#include "router_client/router_client_api.h"

BOOL control_init(TAPPCTX *pCtx, void* pArg)
{
    INF("center controller init");

	setbuf(stdout, NULL);
	setvbuf(stdout, 0, _IONBF, 0);
    return TRUE;
}

BOOL control_fini(TAPPCTX *pCtx, void* pArg)
{
    INF("center controller fini");
    return TRUE;
}

BOOL control_pre_proc_cmdline(unsigned short argc, const char** argv)
{
    return TRUE;
}

BOOL control_proc_cmdline(TAPPCTX* pCtx, void* pArg, unsigned short argc, const char** argv)
{
    CControlModule* pModule = (CControlModule*)CMGApp::instance().get_module("CControlModule");

    return pModule->proc_cmdline(argc, argv);
}

const char* control_help(void)
{
    CControlModule* pModule = (CControlModule*)CMGApp::instance().get_module("CControlModule");

    return pModule->help_info();
}

int main(int argc, char* argv[])
{ 
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();

    pServer->set_use_router(TRUE);
    pServer->set_control_func(control_init, control_fini, control_pre_proc_cmdline, control_proc_cmdline, control_help);
    MG_REGISTER_MODULE(pServer, CControlModule);

    nRetCode = pServer->init("control_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}
