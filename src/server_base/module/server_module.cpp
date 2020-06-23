#include "module/server_module.h"
#include "string/string_ex.h"

BOOL CServerModuleContainer::init(void)
{
    m_nModuleCount = 0;
    memset(m_pModules, 0, sizeof(m_pModules));
    m_Name2Module.clear();

    return TRUE;
}
   
BOOL CServerModuleContainer::uninit(void)
{
    return TRUE;
}

BOOL CServerModuleContainer::add_module(CServerModule* pModule)
{
    LOG_PROCESS_ERROR(pModule);
    LOG_PROCESS_ERROR(m_nModuleCount < MAX_MODULE_COUNT);

    m_pModules[m_nModuleCount++] = pModule;
    m_Name2Module[pModule->get_name()] = pModule;

    INF("[module container]: add module %s", pModule->get_name());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CServerModuleContainer::del_module(CServerModule* pModule)
{
    int32_t nIndex = 0;

    LOG_PROCESS_ERROR(pModule);

    nIndex = pModule->get_cont_index();
    m_pModules[nIndex] = NULL;
    
    INF("[module container]: del module %s", pModule->get_name());

    return TRUE;
Exit0:
    return FALSE;
}

CServerModule* CServerModuleContainer::get_module_by_idx(int32_t nIndex)
{
    LOG_PROCESS_ERROR(nIndex >= 0);
    LOG_PROCESS_ERROR(nIndex < MAX_MODULE_COUNT);

    return m_pModules[nIndex];
Exit0:
    return NULL;
}

CServerModule* CServerModuleContainer::get_module_by_name(const char* pcszName)
{
    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(m_Name2Module.find(pcszName) != m_Name2Module.end());

    return m_Name2Module[pcszName];
Exit0:
    return NULL;
}