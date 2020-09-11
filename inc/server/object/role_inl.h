#include "object/role.h"

#ifndef _ROLE_INL_H_
#define _ROLE_INL_H_

inline void CRole::set_name(const char* pcszName)
{
    strxcpy(m_szName, pcszName, sizeof(m_szName));
}

inline const char* CRole::get_name(void)
{
    return m_szName;
}

inline void CRole::set_level(int32_t nLevel)
{
    m_nLevel = nLevel;
}

inline int32_t CRole::get_level(void)
{
    return m_nLevel;
}

inline void CRole::set_create_time(uint32_t dwCreateTime)
{
    m_dwCreateTime = dwCreateTime;
}

inline uint32_t CRole::get_create_time(void)
{
    return m_dwCreateTime;
}

inline void CRole::set_save_time(uint32_t dwSaveTime)
{
    m_dwSaveTime = dwSaveTime;
}

inline uint32_t CRole::get_save_time(void)
{
    return m_dwSaveTime;
}

inline void CRole::set_login_time(uint32_t dwLoginTime)
{
    m_dwLoginTime = dwLoginTime;
}

inline uint32_t CRole::get_login_time(void)
{
    return m_dwLoginTime;
}

inline void CRole::set_session_id(uint64_t qwSessionID)
{
    m_qwSessionID = qwSessionID;
}

inline uint64_t CRole::get_session_id(void)
{
    return m_qwSessionID;
}

inline void CRole::set_user_id(uint64_t qwUserID)
{
    m_qwUserID = qwUserID;
}

inline uint64_t CRole::get_user_id(void)
{
    return m_qwUserID;
}
    
inline uint64_t CRole::get_obj_id(void)
{
    return m_qwObjID;
}

#endif