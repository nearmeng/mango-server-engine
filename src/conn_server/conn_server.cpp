#include "stdafx.h"

#include "app/server_app.h"
#include "conn/conn_module.h"

int main(int argc, char* argv[])
{ 
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();

    MG_CONFIG config;
    config.bUseTconnd = TRUE;
    config.bUseRouter = TRUE;
    pServer->set_config(config);
    
    MG_REGISTER_MODULE(pServer, CConnModule);

    nRetCode = pServer->init("conn_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}
