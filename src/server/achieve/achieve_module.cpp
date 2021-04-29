#include "module/achieve_module.h"
#include "config/global_config.h"

#include "define/shm_def.h"
#include "define/object_def.h"

MG_REGISTER_COMPONENT_MODULE(CAchieveModule, cotRole, ctAchieve);

BOOL CAchieveModule::init(BOOL bResume)
{
    int nRetCode = 0;

    nRetCode = m_AchievePool.init(stdAchieve, g_ServerConfig.GS.nRoleCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAchieveModule::uninit(void)
{
    return TRUE;
}

CComponent* CAchieveModule::new_component(uint64_t qwComponentID)
{
    return (CComponent*)m_AchievePool.new_object(qwComponentID);
}

CComponent* CAchieveModule::find_component(uint64_t qwComponentID)
{
    return (CComponent*)m_AchievePool.find_object(qwComponentID);
}

BOOL CAchieveModule::del_component(CComponent* pComponent)
{
    CAchieve* pAchieve = dynamic_cast<CAchieve*>(pComponent);
    LOG_PROCESS_ERROR(pAchieve);

    return m_AchievePool.delete_object(pAchieve);
Exit0:
    return FALSE;
}