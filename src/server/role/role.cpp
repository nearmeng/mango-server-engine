#include "stdafx.h"
#include "object/role.h"

#include "db/role_db_data.pb.h"

BOOL CRole::init(uint64_t qwObjID)
{
    m_qwObjID = qwObjID;
    m_qwUserID = 0;
    m_qwSessionID = 0;

    m_dwCreateTime = 0;
    m_dwSaveTime = 0;
    m_dwLoginTime = 0;

    memset(m_szName, 0, sizeof(m_szName));
    m_nLevel = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRole::uninit(void)
{
    return TRUE;
}

void CRole::mainloop()
{
    return;
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

    return TRUE;
Exit0:
    return FALSE;
}