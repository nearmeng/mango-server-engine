#include "stdafx.h"
#include "role/role.h"

#include "db/role_db_data.pb.h"
#include "define/object_def.h"

#include "guid/guid.h"

BOOL CRole::init(uint64_t qwObjID)
{
    int32_t nRetCode = 0;
    std::list<CComponentModule*> ModuleList = CMGApp::instance().get_component_list(cotRole);

    m_qwObjID = qwObjID;
    m_qwUserID = 0;
    m_qwSessionID = 0;

    m_dwCreateTime = 0;
    m_dwSaveTime = 0;
    m_dwLoginTime = 0;

    memset(m_szName, 0, sizeof(m_szName));
    m_nLevel = 0;
    
    nRetCode = m_ComponentCont.init();
    LOG_PROCESS_ERROR(nRetCode);

    for (std::list<CComponentModule*>::iterator it = ModuleList.begin(); it != ModuleList.end(); it++)
    {
        CComponent* pComp = NULL;
        CComponentModule * pCompModule = *it;

        uint64_t qwComponentID = guid_alloc(gtdComponent);
        LOG_PROCESS_ERROR(qwComponentID > 0);

        pComp = pCompModule->new_component(qwComponentID);
        LOG_PROCESS_ERROR(pComp);

        nRetCode = pComp->init(this);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_ComponentCont.add_component(pCompModule->get_owner_type(), qwComponentID, pComp);
        LOG_PROCESS_ERROR(nRetCode);
    }

    DBG("role %lld is inited", m_qwObjID);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::uninit(void)
{
    int32_t nRetCode = 0;

    for (int32_t i = 0; i < MAX_COMPONENT_COUNT; i++)
    {
        CComponent* pCompnent = m_ComponentCont.get_component(i);
        if(pCompnent)
        {
            nRetCode = pCompnent->uninit();
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    nRetCode = m_ComponentCont.uninit();
    LOG_CHECK_ERROR(nRetCode);
    
    DBG("role %lld is uninited", m_qwObjID);

    return TRUE;
Exit0:
    return FALSE;
}

void CRole::mainloop()
{
    m_ComponentCont.mainloop();
}
    
void CRole::on_resume(void)
{
    int32_t nRetCode = 0;

    std::list<CComponentModule*> ModuleList = CMGApp::instance().get_component_list(cotRole);
    for (std::list<CComponentModule*>::iterator it = ModuleList.begin(); it != ModuleList.end(); it++)
    {
        CComponent* pComp = NULL;
        CComponentModule * pCompModule = *it;

        uint64_t qwComponentID = m_ComponentCont.get_component_id(pCompModule->get_owner_type());
        LOG_PROCESS_ERROR(qwComponentID > 0);

        pComp = pCompModule->find_component(qwComponentID);
        LOG_PROCESS_ERROR(pComp);
        
        nRetCode = m_ComponentCont.set_component(pCompModule->get_owner_type(), pComp);
        LOG_PROCESS_ERROR(nRetCode);

        pComp->on_resume();
    }

Exit0:
    return;
}
    
void CRole::on_event_sync_data(void)
{
    for (int32_t i = 0; i < MAX_COMPONENT_COUNT; i++)
    {
        CComponent* pCompnent = m_ComponentCont.get_component(i);
        if(pCompnent)
        {
            pCompnent->on_event_sync_data();
        }
    }
}
    
BOOL CRole::_save_base_data(ROLE_DB::ROLE_BASE_DATA& BaseData)
{
    BaseData.set_role_id(m_qwObjID);
    BaseData.set_user_id(m_qwUserID);
    BaseData.set_role_name(m_szName);
    BaseData.set_level(m_nLevel);
    BaseData.set_create_time(m_dwCreateTime);
    BaseData.set_last_login_time(m_dwLoginTime);
    BaseData.set_last_save_time(m_dwSaveTime);

    return TRUE;
}
   
BOOL CRole::_load_base_data(const ROLE_DB::ROLE_BASE_DATA& BaseData)
{
    m_qwObjID = BaseData.role_id();
    m_qwUserID = BaseData.user_id();
    strxcpy(m_szName, BaseData.role_name().c_str(), sizeof(m_szName));
    m_nLevel = BaseData.level();
    m_dwCreateTime = BaseData.create_time();
    m_dwLoginTime = BaseData.last_login_time();
    m_dwSaveTime = BaseData.last_save_time();

    return TRUE;
}

BOOL CRole::save(char* pData, uint32_t &dwSize, char* pBaseData, uint32_t &dwBaseDataSize)
{
    int32_t nRetCode = 0;
    ROLE_DB::ROLE_DATA RoleData;

    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(pBaseData);

    nRetCode = _save_base_data(*RoleData.mutable_base_data());
    LOG_PROCESS_ERROR(nRetCode);
    
    for (int32_t i = 0; i < MAX_COMPONENT_COUNT; i++)
    {
        CComponent* pCompnent = m_ComponentCont.get_component(i);
        if(pCompnent)
        {
            pCompnent->save_data(&RoleData);
        }
    }

    nRetCode = RoleData.base_data().SerializeToArray(pBaseData, dwBaseDataSize);
    LOG_PROCESS_ERROR(nRetCode);

    dwBaseDataSize = RoleData.base_data().GetCachedSize();

    nRetCode = RoleData.SerializeToArray(pData, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    dwSize = RoleData.GetCachedSize();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::load(const char* pData, uint32_t dwSize)
{
    int32_t nRetCode = 0;
    ROLE_DB::ROLE_DATA RoleData;

    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(dwSize > 0);

    nRetCode = RoleData.ParseFromArray(pData, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _load_base_data(RoleData.base_data());
    LOG_PROCESS_ERROR(nRetCode);
    
    for (int32_t i = 0; i < MAX_COMPONENT_COUNT; i++)
    {
        CComponent* pCompnent = m_ComponentCont.get_component(i);
        if(pCompnent)
        {
            pCompnent->load_data(&RoleData);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}
    