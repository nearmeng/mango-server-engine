#include "stdafx.h"

#include "config/global_config.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "control_module/control_message_handler.h"
#include "control_module/control_module.h"

#include "router_client/router_client_api.h"

int control_init(TAPPCTX *pCtx, void* pArg)
{
    INF("center controller init");

	setbuf(stdout, NULL);
	setvbuf(stdout, 0, _IONBF, 0);
    return 0;
}

int control_fini(TAPPCTX *pCtx, void* pArg)
{
    INF("center controller fini");
    return 0;
}

int control_pre_proc_cmdline(unsigned short argc, const char** argv)
{
    return 0;
}

int control_proc_cmdline(TAPPCTX* pCtx, void* pArg, unsigned short argc, const char** argv)
{
    int32_t nRetCode = 0;
    CControlModule* pModule = (CControlModule*)CMGApp::instance().get_module("CControlModule");

    nRetCode = pModule->proc_cmdline(argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    return 0;
Exit0:
    return -1;
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

    MG_CONFIG config;
    config.bUseRouter = TRUE;
    config.pControlInit = control_init;
    config.pControlFini = control_fini;
    config.pControlPreProc = control_pre_proc_cmdline;
    config.pControlProc = control_proc_cmdline;
    config.pControlHelp = control_help;

    pServer->set_config(config);

    nRetCode = pServer->init("control_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}
