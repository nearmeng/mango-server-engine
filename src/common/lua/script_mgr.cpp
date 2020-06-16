#include "stdafx.h"
#include "lua/script_mgr.h"

#include "lstate.h"
#include "lmem.h"
#include <stdio.h>

CScriptMgr CScriptMgr::ms_Instance;

CScriptMgr::CScriptMgr(void)
{
	m_nIncludePathCount = 0;
	m_pDependFunc = NULL;
	m_dwCRC = 0;
	m_szCachePrefix[0] = 0;
}

CScriptMgr::~CScriptMgr(void)
{
    uninit();
}

BOOL CScriptMgr::init(const char* pcszCachePrefix)
{
	m_nIncludePathCount = 0;
	m_pDependFunc = NULL;

	strxcpy(m_szCachePrefix, pcszCachePrefix, SCRIPT_NAME_LEN);

    return TRUE;
}

BOOL CScriptMgr::uninit(void)
{
    for (MAP_NAME_2_PACKAGE::iterator it = m_mapPackage.begin();
        it != m_mapPackage.end(); ++it)
    {
        const char* pcszPackageName = it->first;
        SAFE_DELETE_ARRAY(pcszPackageName);
    }

    for (MAP_NAME_2_SCRIPT::iterator it = m_mapScript.begin();
        it != m_mapScript.end(); ++it)
    {
        it->second->uninit();

        SAFE_DELETE(it->second);
    }

    m_mapPackage.clear();
    m_mapScript.clear();
    m_lstLuaFunc.clear();
	m_nIncludePathCount = 0;

    return TRUE;
}

CScriptMgr& CScriptMgr::instance(void)
{
    return ms_Instance;
}

BOOL CScriptMgr::reload(int32_t& rnErrorCount, BOOL bForce)
{
    int32_t nRetCode = 0;

    rnErrorCount = 0;
	m_dwCRC = 0;

    for (MAP_NAME_2_SCRIPT::iterator it = m_mapScript.begin();
        it != m_mapScript.end(); ++it)
    {
        CLuaScript* poScript = it->second;

        nRetCode = poScript->load_from_file(it->first, bForce);
        LOG_CHECK_ERROR(nRetCode);
		if (nRetCode)
		{
			m_dwCRC = crc32_combine(m_dwCRC, poScript->get_crc(), poScript->get_file_size());
		}
		else
            rnErrorCount++;
    }
	INF("reloaded all script crc %08X", m_dwCRC);

    return TRUE;
}

BOOL CScriptMgr::reload(const char *pcszScriptName, BOOL bForce)
{
    int32_t       nRetCode = 0;
    CLuaScript* poScript = NULL;
    MAP_NAME_2_SCRIPT::iterator     iter;
    LOG_PROCESS_ERROR(pcszScriptName);

    iter = m_mapScript.find(pcszScriptName);
    LOG_PROCESS_ERROR(iter != m_mapScript.end());

    poScript = iter->second;

    nRetCode = poScript->load_from_file(iter->first, bForce);
    LOG_PROCESS_ERROR(nRetCode );

    return TRUE;
Exit0:
    return FALSE;
}

int32_t CScriptMgr::_load_package(lua_State* L)
{
	int32_t nResult = 0;
    const char * pcszPackageName = NULL;
    lua_CFunction pLoadPackage = NULL;
	char szPackageName[SCRIPT_NAME_LEN];
	char* pszPointer = NULL;

    LOG_PROCESS_ERROR(lua_gettop(L) > 0);

    pcszPackageName = lua_tostring(L, 1);
    LOG_PROCESS_ERROR(pcszPackageName);

    pLoadPackage = CScriptMgr::instance().find_package(pcszPackageName);
    LOG_PROCESS_ERROR(pLoadPackage);

	snprintf(szPackageName, SCRIPT_NAME_LEN, "__%s", pcszPackageName);
	pszPointer = szPackageName;
	while (*pszPointer)
	{
		if (!isalnum(*pszPointer))
			*pszPointer = '_';
		pszPointer++;
	}

	lua_getglobal(L, szPackageName);

	if (lua_isnil(L, -1))
	{
		nResult = pLoadPackage(L);
		lua_pushstring(L, szPackageName);
		lua_setglobal(L, szPackageName);
	}

	lua_pop(L, 1);

	return nResult;
Exit0:
    if (pcszPackageName)
        CRI("failed to load package %s", pcszPackageName);

    return 0;
}

CLuaScript* CScriptMgr::new_script(const char* pcszScriptFile)
{
	int32_t nRetCode = FALSE;
    CLuaScript* poScript = NULL;
    std::pair<MAP_NAME_2_SCRIPT::iterator, bool> InsRet;
	std::pair<MAP_HASH_2_SCRIPT::iterator, bool> InsRet2;

    LOG_PROCESS_ERROR(pcszScriptFile);

	poScript = new CLuaScript();
    LOG_PROCESS_ERROR(poScript);

	nRetCode = poScript->init();
	LOG_PROCESS_ERROR(nRetCode);

	poScript->set_depend_func(m_pDependFunc);
    lua_register(poScript->get_lua_state(), "LoadPackage", _load_package);

    for (LST_LUA_FUNC::iterator it = m_lstLuaFunc.begin();
        it != m_lstLuaFunc.end(); ++it)
    {
        nRetCode = poScript->register_func(*it, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

	nRetCode = poScript->load_from_file(pcszScriptFile, FALSE);
	LOG_PROCESS_ERROR(nRetCode);

    InsRet = m_mapScript.insert(std::make_pair(poScript->get_script_name(), poScript));
    LOG_PROCESS_ERROR(InsRet.second);

	InsRet2 = m_hashScript.insert(std::make_pair(poScript->get_name_hash(), poScript));
	LOG_PROCESS_ERROR(InsRet2.second);

	m_dwCRC = crc32_combine(m_dwCRC, poScript->get_crc(), poScript->get_file_size());

	return poScript;
Exit0:
    if (poScript)
    {
        poScript->uninit();
        SAFE_DELETE(poScript);
    }

	return NULL;
}

BOOL CScriptMgr::del_script(CLuaScript* poScript)
{
    MAP_NAME_2_SCRIPT::iterator it;
	MAP_HASH_2_SCRIPT::iterator itHash;

	PROCESS_ERROR(poScript);

    it = m_mapScript.find(poScript->get_script_name());
    LOG_PROCESS_ERROR(it != m_mapScript.end());

	itHash = m_hashScript.find(poScript->get_name_hash());
	LOG_PROCESS_ERROR(itHash != m_hashScript.end());

	SAFE_DELETE(it->second);

	m_mapScript.erase(it);
	m_hashScript.erase(itHash);

    return TRUE;
Exit0:

    return FALSE;
}

CLuaScript* CScriptMgr::find_script(const char* pcszScriptName)
{
    MAP_NAME_2_SCRIPT::iterator it = m_mapScript.find(pcszScriptName);
    PROCESS_ERROR(it != m_mapScript.end());

	return it->second;
Exit0:
	return NULL;
}

CLuaScript * CScriptMgr::find_script(uint64_t qwNameHash)
{
	MAP_HASH_2_SCRIPT::iterator it = m_hashScript.find(qwNameHash);
	PROCESS_ERROR(it != m_hashScript.end());

	return it->second;
Exit0:
	return NULL;
}

BOOL CScriptMgr::register_package(const char* pcszPackageName, lua_CFunction pLoadPackage)
{
    char* pszPackageName = NULL;
    std::pair<MAP_NAME_2_PACKAGE::iterator, bool> InsRet;

    LOG_PROCESS_ERROR(pcszPackageName);
    LOG_PROCESS_ERROR(pLoadPackage);

    pszPackageName = new char[SCRIPT_NAME_LEN];
    LOG_PROCESS_ERROR(pszPackageName);

    strxcpy(pszPackageName, pcszPackageName, SCRIPT_NAME_LEN);

    InsRet = m_mapPackage.insert(std::make_pair(pszPackageName, pLoadPackage));
    LOG_PROCESS_ERROR(InsRet.second);

    return TRUE;
Exit0:
    SAFE_DELETE_ARRAY(pszPackageName);

    return FALSE;
}

lua_CFunction CScriptMgr::find_package(const char* pcszPackageName)
{
    MAP_NAME_2_PACKAGE::iterator it;

    it = m_mapPackage.find(pcszPackageName);
    LOG_PROCESS_ERROR(it != m_mapPackage.end());

    return it->second;
Exit0:
	if (pcszPackageName)
		CRI("failed to find package %s", pcszPackageName);
    return NULL;
}

BOOL CScriptMgr::register_func_list(LUA_FUNC* pFuncList)
{
    LOG_PROCESS_ERROR(pFuncList);

    m_lstLuaFunc.push_back(pFuncList);

    return TRUE;
Exit0:
    return FALSE;
}

void CScriptMgr::add_include_path(const char* pcszRootPath)
{
	LOG_PROCESS_ERROR(m_nIncludePathCount < MAX_INCLUDE_PATH);
	strxcpy(m_szIncludePathList[m_nIncludePathCount++], pcszRootPath, SCRIPT_NAME_LEN);

	INF("add include path %s", pcszRootPath);
Exit0:
	return;
}

const char* CScriptMgr::get_include_path(int32_t nIndex)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_INCLUDE_PATH);
    return m_szIncludePathList[nIndex];
Exit0:
	return NULL;
}

void CScriptMgr::set_depend_func(DEPEND_FUNC pDependFunc)
{
	m_pDependFunc = pDependFunc;
}

int64_t CScriptMgr::print_memory_stat(void)
{
	int64_t llTotalSize = 0;

	for (MAP_NAME_2_SCRIPT::iterator it = m_mapScript.begin();
		it != m_mapScript.end(); ++it)
	{
		lua_State* L = it->second->get_lua_state();
		INF("script %s allocated %.2fMB used %.2f", it->first, L->l_G->totalbytes / (1024 * 1024.0f),
			L->l_G->GCestimate / (1024 * 1024.0f));
		llTotalSize += (L->l_G->totalbytes + sizeof(CLuaScript));
	}

	INF("all scripts allocated %.2fMB", llTotalSize / (1024 * 1024.0f));

	return llTotalSize;
}
