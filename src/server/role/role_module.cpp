#include "stdafx.h"
#include "module/role_module.h"

BOOL CRoleModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = m_RolePool.init(stdRole, g_ServerConfig.GS.nRoleCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_RolePool.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

void CRoleModule::mainloop()
{

}