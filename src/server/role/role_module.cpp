#include "stdafx.h"
#include "module/role_module.h"
#include "module/gs_user_module.h"
#include "module/session_module.h"

#include "event/server_event.h"
#include "define/bt_def.h"

#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"

MG_REGISTER_MODULE(CRoleModule);

void CRoleModule::_on_event_role_sync_data(EVENT_INFO* pEvent, EVENT_PARAM& stEventParam)
{
    int32_t nRetCode = 0;
    CRole* pRole = NULL;

    LOG_PROCESS_ERROR(pEvent);
    LOG_PROCESS_ERROR(stEventParam.nOwnerType == otRole);

    pRole = (CRole*)stEventParam.pOwner;
    LOG_PROCESS_ERROR(pRole);

    INF("on event role sync data, roleid %lld param %lld %lld", stEventParam.qwOwnerID, stEventParam.llTriggerVar0, stEventParam.llTriggerVar1);

    //for (int32_t i = rsmtInvalid + 1; i < rsmtTotal; i++)
    //{
    //    CRoleSubModule* pSubModule  = (CRoleSubModule*)((char*)pRole + pRole->m_nSubModuleOffset[i]);
    //    LOG_PROCESS_ERROR(pSubModule);

    //    pSubModule->on_event_sync_data();
    //}

    pRole->on_event_sync_data();

Exit0:
    return;
}

BOOL CRoleModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;
    int32_t nEventID = 0;

    nRetCode = m_RolePool.init(stdRole, g_ServerConfig.GS.nRoleCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);
    
    nEventID = CEventMgr::instance().create_c_event(evtRoleSyncData, 0, 0, _on_event_role_sync_data, 0);
    LOG_PROCESS_ERROR(nEventID != 0);

    nRetCode = CGlobalEventListMgr::instance().register_global_event(nEventID);
    LOG_PROCESS_ERROR(nRetCode);

    if (bResume)
    {
        TRAVERSE_ROLE_RESUME TraverseRoleResume;
        m_RolePool.traverse(TraverseRoleResume);
    }

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
    TRAVERSE_ROLE TraverseRole;
    m_RolePool.traverse(TraverseRole);
}

BOOL CRoleModule::TRAVERSE_ROLE_RESUME::operator()(uint64_t qwRoleID, CRole * pRole)
{
    pRole->on_resume();
    return TRUE;
}

BOOL CRoleModule::TRAVERSE_ROLE::operator()(uint64_t qwRoleID, CRole* pRole)
{
    pRole->mainloop();
    return TRUE;
}
