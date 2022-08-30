#include "stdafx.h"
#include "redis_module.h"

#include "redis_mgr.h"
#include "config/global_config.h"

MG_REGISTER_MODULE(CRedisModule);

BOOL CRedisModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;
    pRedisCli = NULL;

    nRetCode = CRedisCliMgr::instance().init(bResume);
    LOG_PROCESS_ERROR(nRetCode);
    
    pRedisCli = CRedisCliMgr::instance().new_client(g_ServerConfig.DP.szClientName, g_ServerConfig.DP.szRedisServerAddr, 
			g_ServerConfig.DP.nRedisPort, g_ServerConfig.DP.szRedisPassword);
    LOG_PROCESS_ERROR(pRedisCli);

    nRetCode = pRedisCli->sync_connect();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _init_msg_handler();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = pRedisCli->sync_disconnect();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CRedisCliMgr::instance().del_client(pRedisCli);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CRedisModule::on_proc()
{
    CRedisCliMgr::instance().on_proc();
}

void CRedisModule::on_frame()
{
    CRedisCliMgr::instance().on_frame();
}
