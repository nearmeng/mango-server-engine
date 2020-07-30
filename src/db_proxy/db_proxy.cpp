#include "stdafx.h"
#include "app/server_app.h"
#include "redis_module/redis_module.h"

int main(int argc, char* argv[])
{ 
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();

    pServer->set_use_router(TRUE);
    MG_REGISTER_MODULE(pServer, CRedisModule);

    nRetCode = pServer->init("redis_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
	return 0;
}
