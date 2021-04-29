#include "stdafx.h"
#include "module/server_component.h"

BOOL CComponentContainer::init()
{
    m_nComponentCount = 0;
    memset(m_pComponent, 0, sizeof(m_pComponent));

    return TRUE;
};

BOOL CComponentContainer::uninit()
{
    return TRUE;
}

void CComponentContainer::mainloop()
{
    for (int32_t i = 0; i < m_nComponentCount; i++)
    {
        if (m_pComponent[i])
            m_pComponent[i]->mainloop();
    }
}

BOOL CComponentContainer::add_component(int32_t nComponentType, uint64_t qwComponentID, CComponent* pComponent)
{
    LOG_PROCESS_ERROR(pComponent);
    LOG_PROCESS_ERROR(m_nComponentCount < MAX_COMPONENT_COUNT);
    LOG_PROCESS_ERROR(nComponentType >= 0 && nComponentType < MAX_COMPONENT_COUNT);
    LOG_PROCESS_ERROR(m_pComponent[nComponentType] == NULL);

    m_pComponent[nComponentType] = pComponent;
    m_qwComponentID[nComponentType] = qwComponentID;
    m_nComponentCount++;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CComponentContainer::del_component(CComponent* pComponent)
{
    LOG_PROCESS_ERROR(pComponent);

    for (int32_t i = 0; i < MAX_COMPONENT_COUNT; i++)
    {
        if (m_pComponent[i] == pComponent)
        {
            m_pComponent[i] = NULL;
            m_nComponentCount--;
            return TRUE;
        }
    }
Exit0:
    return FALSE;
}
    
BOOL CComponentContainer::set_component(int32_t nComponentType, CComponent* pComponent)
{
    LOG_PROCESS_ERROR(pComponent);
    LOG_PROCESS_ERROR(nComponentType >= 0 && nComponentType < MAX_COMPONENT_COUNT);

    m_pComponent[nComponentType] = pComponent;

    return TRUE;
Exit0:
    return FALSE;
}

CComponent* CComponentContainer::get_component(int32_t nComponentType)
{
    LOG_PROCESS_ERROR(nComponentType >= 0 && nComponentType < MAX_COMPONENT_COUNT);

    return m_pComponent[nComponentType];
Exit0:
    return NULL;
}
    
uint64_t CComponentContainer::get_component_id(int32_t nComponentType)
{
    LOG_PROCESS_ERROR(nComponentType >= 0 && nComponentType < MAX_COMPONENT_COUNT);

    return m_qwComponentID[nComponentType];
Exit0:
    return 0;
}

int32_t CComponentContainer::get_component_count(void)
{
    return m_nComponentCount;
}