#include "stdafx.h"

int main(int argc, char* argv[])
{
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();
    
    MG_CONFIG config;
    config.bUseRouter = TRUE;
    config.bUseConn = TRUE;
    config.bUseDBProxy = TRUE;

    pServer->set_config(config);

    nRetCode = pServer->init("game_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}

