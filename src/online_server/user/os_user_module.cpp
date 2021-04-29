#include "stdafx.h"
#include "os_user_module.h"
    
MG_REGISTER_MODULE(CUserModule);

BOOL CUserModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = m_UserPool.init(stdOnlineUser, g_ServerConfig.OS.nUserObjCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _init_msg_handler();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CUserModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_UserPool.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}
    