#include "stdafx.h"
#include "lua/lua_script.h"

#include "lstate.h"
#include "lundump.h"

#ifndef _WIN32
#include <ifaddrs.h>
#endif

#define LUA_SCRIPT_THIS			"__LuaScriptThis"
#define MAX_PARAM_LEN			(64)
#define MAX_PRINT_LOCAL_VAR		(32)
#define LBC_MAGIC_TAG			(0x1BC0BEEF)
#define LBC_VERSION				(1)
#define MAX_SCRIPT_CACHE_SIZE	(1024 * 1024)
#define CACHE_PATH               "./cache"

static char s_szScriptCache[MAX_SCRIPT_CACHE_SIZE];
static int32_t s_nScriptCacheSize;

struct LBC_HEADER
{
	uint32_t dwMagicTag;
	uint32_t dwVersion;
	uint32_t dwFileCRC;
};

CLuaScript::CLuaScript(void)
{
	m_pLuaState = NULL;
	m_szScriptName[0] = 0;
	m_szFixedScriptName[0] = 0;
	m_qwNameHash = 0;
	m_dwCRC = 0;
	m_dwFileSize = 0;
	m_qwLoadTick = 0;
	m_pDependFunc = NULL;
	m_bExecuting = FALSE;
}

CLuaScript::~CLuaScript(void)
{
	uninit();
}

BOOL CLuaScript::init(void)
{
	LOG_PROCESS_ERROR(m_pLuaState == NULL);

	m_pLuaState = luaL_newstate();
	LOG_PROCESS_ERROR(m_pLuaState);

    luaL_openlibs(m_pLuaState);

    lua_register(m_pLuaState, "print", _print);
	lua_register(m_pLuaState, "warn", _warn);
	lua_register(m_pLuaState, "Include", _include);
	lua_register(m_pLuaState, "Depend", _depend);
	lua_register(m_pLuaState, "get_host_name", _get_host_name);
	lua_register(m_pLuaState, "get_host_ip", _get_host_ip);
	lua_register(m_pLuaState, "is_file_exist", _is_file_exist);
	lua_register(m_pLuaState, "get_user_type_mem", _get_user_type_mem);
	lua_register(m_pLuaState, "set_user_type_mem", _set_user_type_mem);

	lua_pushlightuserdata(m_pLuaState, this);
	lua_setglobal(m_pLuaState, LUA_SCRIPT_THIS);

	m_pDependFunc = NULL;
	m_bExecuting = FALSE;

	return TRUE;
Exit0:
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	return FALSE;
}

BOOL CLuaScript::uninit(void)
{
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	//m_szScriptName[0] = 0;

	return TRUE;
}

BOOL CLuaScript::load_from_file(const char* pcszFileName, BOOL bForce)
{
	int32_t nResult = FALSE;
	int32_t nRetCode = FALSE;
	uint32_t dwOldCRC = m_dwCRC;

    LOG_PROCESS_ERROR(pcszFileName);

	m_dwCRC = 0;
	m_dwFileSize = 0;
	strxcpy(m_szScriptName, pcszFileName, SCRIPT_NAME_LEN);
	_fix_script_name(m_szScriptName, m_szFixedScriptName);
	m_qwNameHash = hash_file_name(m_szScriptName);
	_get_crc(pcszFileName, 0);

	PROCESS_SUCCESS((m_dwCRC == dwOldCRC) && !bForce);

	if (m_dwCRC != dwOldCRC || bForce)
	{
		nRetCode = _load_from_file(pcszFileName, bForce);
		LOG_PROCESS_ERROR(nRetCode);

		//m_qwLoadTick = get_tick_count();

		INF("loaded lua script %s successfully", pcszFileName);
	}

Exit1:
	nResult = TRUE;
Exit0:
	return nResult;
}

uint64_t CLuaScript::get_name_hash(void)
{
	if(m_qwNameHash == 0)
		m_qwNameHash = hash_file_name(m_szScriptName);

	return m_qwNameHash;
}

BOOL CLuaScript::load_from_buffer(const char* pcszBufferName, const char* pcszBuffer, uint32_t dwSize)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pcszBuffer);

	nRetCode = luaL_loadbuffer(m_pLuaState, pcszBuffer, dwSize, pcszBufferName);
	LOG_PROCESS_ERROR(nRetCode == 0);

    lua_setglobal(m_pLuaState, pcszBufferName);
    lua_getglobal(m_pLuaState, pcszBufferName);

	nRetCode = lua_pcall(m_pLuaState, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
	const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
	if (pszErrorMsg)
	{
		CRI("[LUA] LoadFromBuffer %s : %s", pcszBufferName, pszErrorMsg);
	}
    else
    {
        CRI("[LUA] LoadFromBuffer %s", pcszBufferName);
    }

	return FALSE;
}

BOOL CLuaScript::_load_from_cache(const char* pcszFileName, uint32_t dwFileCRC)
{
	int32_t nResult = FALSE;
	int32_t nRetCode = 0;
	IFile* pFile = NULL;
	char* pContent = NULL;
	LBC_HEADER* pHeader = NULL;
	char szCacheFileName[SCRIPT_NAME_LEN];
	char szFixedFileName[SCRIPT_NAME_LEN];

	LOG_PROCESS_ERROR(pcszFileName);

	_fix_script_name(pcszFileName, szFixedFileName);

	snprintf(szCacheFileName, SCRIPT_NAME_LEN, CACHE_PATH"/%s_%s.lbc", 
		CScriptMgr::instance().get_cache_prefix(), szFixedFileName);

	pFile = open_file(szCacheFileName, "rb");
	PROCESS_ERROR(pFile);

	pContent = new char[pFile->size()];
	LOG_PROCESS_ERROR(pContent);

	nRetCode = pFile->read(pContent, pFile->size());
	LOG_PROCESS_ERROR((uint32_t)nRetCode == pFile->size());

	pHeader = (LBC_HEADER*)pContent;

	LOG_PROCESS_ERROR(pHeader->dwMagicTag == LBC_MAGIC_TAG);
	LOG_PROCESS_ERROR(pHeader->dwVersion == LBC_VERSION);
	PROCESS_ERROR(pHeader->dwFileCRC == dwFileCRC);

	nRetCode = load_from_buffer(szFixedFileName, pContent + sizeof(LBC_HEADER), pFile->size() - sizeof(LBC_HEADER));
	LOG_PROCESS_ERROR(nRetCode);

//	INF("loaded from script cache: %s", pcszFileName);

	nResult = TRUE;
Exit0:
	if (pFile)
		close_file(pFile);
	if (pContent)
		delete[] pContent;

	return nResult;
}

BOOL CLuaScript::_load_from_file(const char* pcszFileName, BOOL bForce)
{
	int32_t nResult = FALSE;
	int32_t nRetCode = FALSE;
	char* pszBuffer = NULL;
	char* pszCacheBuffer = NULL;
	uint32_t dwSize = 0;
	IFile* pFile = NULL;
	IFile* pCacheFile = NULL;
	char szFileName[SCRIPT_NAME_LEN];
	char szFixedFileName[SCRIPT_NAME_LEN];
	uint32_t dwFileCRC = 0;

	LOG_PROCESS_ERROR(pcszFileName);

	pFile = open_file(pcszFileName, "rb");
	for (int32_t i = 0; i < CScriptMgr::instance().get_include_path_count() && pFile == NULL; i++)
	{
		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s",
			CScriptMgr::instance().get_include_path(i), pcszFileName);
		pFile = open_file(szFileName, "rb");
	}
	if (pFile == NULL)
	{
		char szCurrentPath[SCRIPT_NAME_LEN];

		szCurrentPath[0] = 0;
		get_file_path(m_szScriptName, SCRIPT_NAME_LEN, szCurrentPath, SCRIPT_NAME_LEN);

		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s", szCurrentPath, pcszFileName);
		pFile = open_file(szFileName, "rb");
	}
	LOG_PROCESS_ERROR(pFile);

	dwSize = pFile->size();

	pszBuffer = new char[dwSize];
	LOG_PROCESS_ERROR(pszBuffer);

	nRetCode = pFile->read(pszBuffer, dwSize);
	LOG_PROCESS_ERROR((uint32_t)nRetCode == dwSize);

	_fix_script_name(pcszFileName, szFixedFileName);
	dwFileCRC = crc32c_append(0, (const uint8_t*)pszBuffer, dwSize);

	if (!bForce)
	{
		nRetCode = _load_from_cache(pcszFileName, dwFileCRC);
		PROCESS_SUCCESS(nRetCode);
	}

	if ((*(uint32_t*)pszBuffer & 0x00FFFFFF) == UTF8_BOM)
	{
		nRetCode = load_from_buffer(szFixedFileName, pszBuffer + 3, dwSize - 3);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = load_from_buffer(szFixedFileName, pszBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = _save_into_cache(szFixedFileName, dwFileCRC);
	LOG_CHECK_ERROR(nRetCode);

Exit1:
	nResult = TRUE;
Exit0:

	if (!nResult && pcszFileName)
		WRN("failed to load lua script %s", pcszFileName);

	SAFE_DELETE_ARRAY(pszBuffer);
	SAFE_DELETE_ARRAY(pszCacheBuffer);
	if (pFile)
		close_file(pFile);
	if (pCacheFile)
		close_file(pCacheFile);

	return nResult;
}

int dump_lua_script(lua_State* L, const void* p, size_t sz, void* ud)
{
	LOG_PROCESS_ERROR(s_nScriptCacheSize + sz < MAX_SCRIPT_CACHE_SIZE);
	memcpy(s_szScriptCache + s_nScriptCacheSize, p, sz);
	s_nScriptCacheSize += sz;
	return 0;
Exit0:
	return -1;
}

void CLuaScript::_fix_script_name(const char* pcszScriptName, char* pcszFixedName)
{
	int32_t i = 0;

	LOG_PROCESS_ERROR(pcszScriptName);
	LOG_PROCESS_ERROR(pcszFixedName);

	for (; i < (SCRIPT_NAME_LEN - 1) && pcszScriptName[i]; i++)
	{
		if (isalpha(pcszScriptName[i]) || isdigit(pcszScriptName[i]))
			pcszFixedName[i] = pcszScriptName[i];
		else
			pcszFixedName[i] = '_';
	}
	pcszFixedName[i] = 0;

Exit0:
	return;
}

BOOL CLuaScript::_save_into_cache(const char* pcszCacheName, uint32_t dwFileCRC)
{
	int32_t nRetCode = 0;
	LBC_HEADER* pHeader = (LBC_HEADER*)s_szScriptCache;
	char szCacheFileName[SCRIPT_NAME_LEN];
	FILE* pFile = NULL;
	
	LOG_PROCESS_ERROR(pcszCacheName);
	LOG_PROCESS_ERROR(m_pLuaState);

	snprintf(szCacheFileName, SCRIPT_NAME_LEN, CACHE_PATH"/%s_%s.lbc", 
		CScriptMgr::instance().get_cache_prefix(), pcszCacheName);

	lua_getglobal(m_pLuaState, pcszCacheName);

	s_nScriptCacheSize = sizeof(*pHeader);
	nRetCode = lua_dump(m_pLuaState, dump_lua_script, NULL, 1);
	LOG_PROCESS_ERROR(nRetCode == 0);

	lua_pop(m_pLuaState, 1);

	pFile = fopen(szCacheFileName, "wb");
	LOG_PROCESS_ERROR(pFile);

	pHeader->dwMagicTag = LBC_MAGIC_TAG;
	pHeader->dwVersion = LBC_VERSION;
	pHeader->dwFileCRC = dwFileCRC;

	nRetCode = fwrite(s_szScriptCache, 1, s_nScriptCacheSize, pFile);
	LOG_PROCESS_ERROR(nRetCode == s_nScriptCacheSize);

	fclose(pFile);

	return TRUE;
Exit0:
	if (pFile)
		fclose(pFile);
	return FALSE;
}

BOOL CLuaScript::register_func(LUA_FUNC FuncList[], int32_t nFuncCount)
{
	LOG_PROCESS_ERROR(m_pLuaState);

	if (nFuncCount == 0)
	{
		for (int nIndex = 0;  FuncList[nIndex].pcszFuncName != NULL; nIndex ++)
			lua_register(m_pLuaState, FuncList[nIndex].pcszFuncName, FuncList[nIndex].pFunc);
	}
	else
	{
		for (int nIndex = 0; nIndex < nFuncCount; nIndex ++)
			lua_register(m_pLuaState, FuncList[nIndex].pcszFuncName, FuncList[nIndex].pFunc);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CLuaScript::_call_function(const char* pcszFuncName, const char* pcszFormat, va_list valist )
{
	int32_t nRetCode = 0;
    int32_t nStackTop = -1;
	int32_t nIndex = 0;
	int32_t nArgnum = 0;
	int32_t nParam = 0;
	int32_t nErrFunc = 0;
    int32_t nResult = 0;
    int32_t nStackIndex = 0;

	LOG_PROCESS_ERROR(m_pLuaState);
	nStackTop = nParam = lua_gettop(m_pLuaState);

	lua_pushcfunction(m_pLuaState, _lua_err_func);
	nErrFunc = lua_gettop(m_pLuaState);

	lua_getglobal(m_pLuaState, pcszFuncName);
	LOG_PROCESS_ERROR(lua_isfunction(m_pLuaState, -1));


    while (pcszFormat[nIndex] != '\0' && pcszFormat[nIndex] != ':')
	{
		switch(pcszFormat[nIndex])
		{
		case 'f':
			{
				double fNumber = va_arg(valist, double);
				lua_pushnumber(m_pLuaState, fNumber);
				nArgnum++;
			}
			break;
		case 'd':
			{
				int32_t nNumber = (int32_t)(va_arg(valist, int32_t));
				lua_pushinteger(m_pLuaState, nNumber);
				nArgnum++;
			}
			break;
		case 'l':
			{
				int64_t llNumber = (int64_t)(va_arg(valist, int64_t));
				lua_pushinteger(m_pLuaState, llNumber);
				nArgnum++;
			}
			break;
		case 's':
			{
				char* pszString = va_arg(valist, char*);
				lua_pushstring(m_pLuaState, pszString);
				nArgnum++;
			}
			break;
		case 'N':
			{
				lua_pushnil(m_pLuaState);
				nArgnum++;
			}
			break;
		case 'F':
			{
				lua_CFunction pLuaCFunc = va_arg(valist, lua_CFunction);
				lua_pushcfunction(m_pLuaState, pLuaCFunc) ;
				nArgnum++;
			}
			break;
		case 't':
			{
				lua_pushvalue(m_pLuaState, nParam);
				lua_remove(m_pLuaState, nParam);
				nArgnum++;
				nParam--;
			}
			break;
		case 'u':
			{
				lua_pushvalue(m_pLuaState, nParam);
				lua_remove(m_pLuaState, nParam);
				nArgnum++;
				nParam--;
			}
			break;
		case 'b':
			{
				BOOL bValue = va_arg(valist, BOOL);
				lua_pushboolean(m_pLuaState, bValue) ;
				nArgnum++;
			}
			break;
		case 'o':
			{
				void* pObj = va_arg(valist, void*);
				char* pszClassName = va_arg(valist, char*);
				tolua_pushusertype(m_pLuaState, pObj, pszClassName);
				nArgnum++;
			}
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
		}

		nIndex++;
	}

    if (pcszFormat[nIndex] == ':')
        nIndex++;

    nResult = (int32_t)strxlen(pcszFormat + nIndex, MAX_PARAM_LEN);

	nRetCode = lua_pcall(m_pLuaState, nArgnum, nResult, nErrFunc);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nStackIndex = lua_gettop(m_pLuaState) - nResult + 1;
    LOG_PROCESS_ERROR(nStackIndex > 0);

    while (pcszFormat[nIndex] != '\0')
    {
        switch(pcszFormat[nIndex])
        {
        case 'f':
            {
                float* pfNumber = va_arg(valist, float*);
                LOG_PROCESS_ERROR(pfNumber);
                LOG_PROCESS_ERROR(lua_isnumber(m_pLuaState, nStackIndex));

                *pfNumber = (float)lua_tonumber(m_pLuaState, nStackIndex++);
            }
            break;
        case 'D':
            {
                double* pfNumber = va_arg(valist, double*);
                LOG_PROCESS_ERROR(pfNumber);
                LOG_PROCESS_ERROR(lua_isnumber(m_pLuaState, nStackIndex));

                *pfNumber = lua_tonumber(m_pLuaState, nStackIndex++);
            }
            break;
        case 'd':
            {
                int32_t* pnInteger = va_arg(valist, int32_t*);
                LOG_PROCESS_ERROR(pnInteger);
				if (lua_isinteger(m_pLuaState, nStackIndex))
					*pnInteger = (int32_t)lua_tointeger(m_pLuaState, nStackIndex);
				else if (lua_isnumber(m_pLuaState, nStackIndex))
					*pnInteger = (int32_t)lua_tonumber(m_pLuaState, nStackIndex);
				else if (lua_isnil(m_pLuaState, nStackIndex))
					*pnInteger = 0;
				else
					LOG_CHECK_ERROR(FALSE);
				nStackIndex++;
            }
            break;
        case 'l':
            {
                int64_t* pnInteger = va_arg(valist, int64_t*);
                LOG_PROCESS_ERROR(pnInteger);
				if (lua_isinteger(m_pLuaState, nStackIndex))
					*pnInteger = (int64_t)lua_tointeger(m_pLuaState, nStackIndex);
				else if (lua_isnumber(m_pLuaState, nStackIndex))
					*pnInteger = (int64_t)lua_tonumber(m_pLuaState, nStackIndex);
				else if (lua_isnil(m_pLuaState, nStackIndex))
					*pnInteger = 0;
				else
					LOG_CHECK_ERROR(FALSE);
				nStackIndex++;
            }
            break;
        case 's':
            {
                char* pszString = va_arg(valist, char*);
                LOG_PROCESS_ERROR(pszString);
                LOG_PROCESS_ERROR(lua_isstring(m_pLuaState, nStackIndex));
                int32_t nStrLen = va_arg(valist, int32_t);

                strxcpy(pszString, lua_tostring(m_pLuaState, nStackIndex++), nStrLen);
            }
            break;
        case 'b':
            {
                BOOL* pbBoolean = va_arg(valist, BOOL*);
                LOG_PROCESS_ERROR(pbBoolean);
                LOG_PROCESS_ERROR(lua_isboolean(m_pLuaState, nStackIndex));

                *pbBoolean = lua_toboolean(m_pLuaState, nStackIndex++);
            }
			break;
        case 'o':
            {
                void** ppObj = va_arg(valist, void**);
                LOG_PROCESS_ERROR(ppObj);
                LOG_PROCESS_ERROR(lua_isuserdata(m_pLuaState, nStackIndex) || lua_isnil(m_pLuaState, nStackIndex));

                *ppObj = lua_touserdata(m_pLuaState, nStackIndex++);
            }
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
        }

        nIndex++;
    }

    lua_settop(m_pLuaState, nStackTop);

	return TRUE;

Exit0:
	const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
	if (pszErrorMsg)
	{
		CRI("failed to call lua function %s(%s), %s", pcszFuncName, pcszFormat, pszErrorMsg);
	}
	else
	{
		CRI("failed to call lua function %s(%s)", pcszFuncName, pcszFormat);
	}

    if (nStackTop != -1)
        lua_settop(m_pLuaState, nStackTop);

	return FALSE;
}

BOOL CLuaScript::call_function(const char* pcszFuncName, const char* pcszFormat, ...)
{
	BOOL nRet = FALSE;

	va_list valist;
	va_start(valist, pcszFormat);
	nRet = _call_function(pcszFuncName, pcszFormat, valist );
	va_end(valist);

	return nRet;
}

BOOL CLuaScript::execute(void)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(m_pLuaState);

	m_bExecuting = TRUE;

    lua_getglobal(m_pLuaState, m_szFixedScriptName);

    nRetCode = lua_pcall(m_pLuaState, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode == 0);

	m_bExecuting = FALSE;

    return TRUE;
Exit0:
	m_bExecuting = FALSE;
	
	if (m_pLuaState)
    {
        const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
        if (pszErrorMsg)
        {
            CRI("[LUA] failed to execute script %s : %s", m_szScriptName, pszErrorMsg);
        }
        else
        {
            CRI("[LUA] failed to execute script %s", m_szScriptName);
        }
    }

    return FALSE;
}

BOOL CLuaScript::is_func_exist(const char* pcszFuncName)
{
	BOOL bResult = FALSE;

	LOG_PROCESS_ERROR(pcszFuncName);

	lua_getglobal(m_pLuaState, pcszFuncName);
	bResult = lua_isfunction(m_pLuaState, lua_gettop(m_pLuaState));
    lua_pop(m_pLuaState, 1);

Exit0:

	return bResult;
}

BOOL CLuaScript::is_table_exist(const char* pcszTableName)
{
    BOOL bResult = FALSE;

    LOG_PROCESS_ERROR(pcszTableName);

    lua_getglobal(m_pLuaState, pcszTableName);
    bResult = lua_istable(m_pLuaState, lua_gettop(m_pLuaState));
    lua_pop(m_pLuaState, 1);

Exit0:

    return bResult;
}

BOOL CLuaScript::is_value_exist(const char* pcszValueName)
{
	BOOL bResult = FALSE;

	LOG_PROCESS_ERROR(pcszValueName);

	lua_getglobal(m_pLuaState, pcszValueName);
	bResult = lua_isnil(m_pLuaState, lua_gettop(m_pLuaState));
	lua_pop(m_pLuaState, 1);

Exit0:

	return !bResult;
}

const char* CLuaScript::get_script_name(void)
{
	return m_szScriptName;
}

lua_State* CLuaScript::get_lua_state(void)
{
	return m_pLuaState;
}

CLuaScript* CLuaScript::get_script_from_state(lua_State* L)
{
	CLuaScript* pResult = NULL;
	int32_t nTop = 0;

	LOG_PROCESS_ERROR(L);

	nTop = lua_gettop(L);
	lua_getglobal(L, LUA_SCRIPT_THIS);

	pResult = (CLuaScript*)lua_touserdata(L, -1);
	LOG_PROCESS_ERROR(pResult);

Exit0:
	if (L)
		lua_settop(L, nTop);

	return pResult;
}

BOOL CLuaScript::collect_garbage(int32_t nGCSize)
{
    int32_t nMemoryAmount = lua_gc(m_pLuaState, LUA_GCCOUNT, 0);

    if (nMemoryAmount > nGCSize)
    {
        lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);
        CRI("[LUA] collect garbage in script %s (%d KB)", m_szScriptName, nMemoryAmount);
    }
    return TRUE;
}

uint32_t CLuaScript::get_crc(void)
{
	if (m_dwCRC == 0)
		_get_crc(m_szScriptName, 0);

	return m_dwCRC;
}

uint32_t CLuaScript::get_file_size(void)
{
	return m_dwFileSize;
}

uint64_t CLuaScript::get_load_tick(void)
{
	return m_qwLoadTick;
}

void CLuaScript::set_depend_func(DEPEND_FUNC pFunc)
{
	m_pDependFunc = pFunc;
}

void CLuaScript::_get_crc(const char* pszFileName, int32_t nLevel)
{
	int32_t nRetCode = 0;
	IFile* pFile = NULL;
	uint32_t dwSize = 0;
	char* pszFileData = NULL;
	uint32_t dwPointer = 0;
	BOOL bInString = FALSE;
	char chStartChar = 0;
	char szFileName[SCRIPT_NAME_LEN];
	char* pszIncludeFileName = NULL;

//	memset(szTab, 0x09, nLevel);
//	DBG("%sinclude file %s", szTab, pszFileName);

	pFile = open_file(pszFileName, "rb");
	for (int32_t i = 0; i < CScriptMgr::instance().get_include_path_count() && pFile == NULL; i++)
	{
		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s",
			CScriptMgr::instance().get_include_path(i), pszFileName);
		pFile = open_file(szFileName, "rb");
	}
	PROCESS_ERROR(pFile);

	dwSize = pFile->size();
	PROCESS_ERROR(dwSize > 0);

//	INF("	<size:%u>(%s)", dwSize, szFileName);

	pszFileData = new char[dwSize + 1];
	LOG_PROCESS_ERROR(pszFileData);

	nRetCode = pFile->read(pszFileData, dwSize);
	LOG_PROCESS_ERROR((uint32_t)nRetCode == dwSize);

	close_file(pFile);
	pFile = NULL;

	m_dwCRC = crc32c_append(m_dwCRC, (const unsigned char*)pszFileData, dwSize);
	m_dwFileSize += dwSize;

	while (dwPointer < dwSize)
	{
		switch (pszFileData[dwPointer])
		{
		case '\'':
			if (!bInString)
			{
				bInString = TRUE;
				chStartChar = '\'';
			}
			else if (bInString && chStartChar == '\'')
			{
				bInString = FALSE;
			}
			break;
		case '\"':
			if (!bInString)
			{
				bInString = TRUE;
				chStartChar = '\"';
			}
			else if (bInString && chStartChar == '\"')
			{
				bInString = FALSE;
			}
			break;
		case '[':
			if (!bInString && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '['))
			{
				bInString = TRUE;
				chStartChar = '[';
				dwPointer++;
			}
			break;
		case ']':
			if (bInString && chStartChar == '[' && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == ']'))
			{
				bInString = FALSE;
				dwPointer++;
			}
			break;
		case '\\':
			if (bInString)
			{
				dwPointer++;
			}
			break;
		case '-':
			if (!bInString && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '-'))
			{
				dwPointer += 2;
				if ((dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '['))
				{
					dwPointer += 3;
					while (dwPointer < dwSize && pszFileData[dwPointer - 2] != ']' && pszFileData[dwPointer - 1] != '-' && pszFileData[dwPointer] != '-')
					{
						dwPointer++;
					}
				}
				else
				{
					while (dwPointer < dwSize && pszFileData[dwPointer] != '\n')
						dwPointer++;
				}
			}
			break;
		default:

			if (!bInString && (dwPointer + 8 < dwSize))
			{
				if (*(uint32_t*)(pszFileData + dwPointer) == 'lcnI' &&
					*(uint32_t*)(pszFileData + dwPointer + 4) == '(edu')
				{
					dwPointer += 8;
					while (dwPointer < dwSize && pszFileData[dwPointer] != '\"' && pszFileData[dwPointer] != '\'')
						dwPointer++;
					dwPointer++;

					pszIncludeFileName = pszFileData + dwPointer;

					while (dwPointer < dwSize && pszFileData[dwPointer] != '\"' && pszFileData[dwPointer] != '\'')
						dwPointer++;
					pszFileData[dwPointer] = 0;

					if (strstr(pszIncludeFileName, "patch") == NULL)
						_get_crc(pszIncludeFileName, nLevel + 1);
				}
				else if (*(uint32_t*)(pszFileData + dwPointer) == 'epeD' &&
					(*(uint32_t*)(pszFileData + dwPointer + 4) == '\'(dn' || *(uint32_t*)(pszFileData + dwPointer + 4) == '\"(dn'))
				{
					dwPointer += 7;
					while (dwPointer < dwSize && pszFileData[dwPointer] != '\"' && pszFileData[dwPointer] != '\'')
						dwPointer++;
					dwPointer++;

					pszIncludeFileName = pszFileData + dwPointer;

					while (dwPointer < dwSize && pszFileData[dwPointer] != '\"' && pszFileData[dwPointer] != '\'')
						dwPointer++;
					pszFileData[dwPointer] = 0;

					if (m_pDependFunc)
						m_pDependFunc(pszIncludeFileName, m_dwCRC);
				}
			}
			break;
		}

		dwPointer++;
	}

	SAFE_DELETE_ARRAY(pszFileData);

	return;
Exit0:
	if (pFile)
		close_file(pFile);
	SAFE_DELETE_ARRAY(pszFileData);

	if (pszFileName)
		CRI("failed to get crc from %s", pszFileName);
	else
		CRI("failed to get crc from null pcszFileName");

	return;
}

int32_t CLuaScript::_include(lua_State* L)
{
	int32_t nRetCode = 0;
	const char* pcszFileName = NULL;
    int32_t nErrFunc = 0;
	char szIncludeName[SCRIPT_NAME_LEN] = { 0 };
	CLuaScript* pThis = NULL;

	pcszFileName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszFileName);

	_fix_script_name(pcszFileName, szIncludeName);

	pThis = get_script_from_state(L);
	LOG_PROCESS_ERROR(pThis);

	if (pThis->m_bExecuting)
	{
		lua_pushcfunction(L, _lua_err_func);
		nErrFunc = lua_gettop(L);

		lua_getglobal(L, szIncludeName);

		nRetCode = lua_pcall(L, 0, 0, nErrFunc);
		LOG_PROCESS_ERROR(nRetCode == 0);
	}
	else
	{
		nRetCode = pThis->_load_from_file(pcszFileName, FALSE);
		LOG_PROCESS_ERROR(nRetCode);
	}

	lua_pushboolean(L, TRUE);

	return 1;
Exit0:
	if (pcszFileName)
    {
        if (lua_isstring(L, -1))
            CRI("[LUA] Include() : %s, %s", pcszFileName, lua_tostring(L, -1));
        else
            CRI("[LUA] Include() : %s", pcszFileName);
    }
	else
		CRI("[LUA] Include() : nil file name");

	return 0;
}

int32_t CLuaScript::_depend(lua_State * L)
{
	return 0;
}

int32_t CLuaScript::_is_file_exist(lua_State * L)
{
	int32_t nRetCode = 0;
	const char* pcszFileName = NULL;
	char szFileName[SCRIPT_NAME_LEN] = { 0 };

	pcszFileName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszFileName);

	snprintf(szFileName, SCRIPT_NAME_LEN, "%s", pcszFileName);
	nRetCode = is_file_exist(szFileName);
	PROCESS_SUCCESS(nRetCode);

	for (int32_t i = 0; i < CScriptMgr::instance().get_include_path_count(); i++)
	{
		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s",
			CScriptMgr::instance().get_include_path(i), pcszFileName);

		nRetCode = is_file_exist(szFileName);
		PROCESS_SUCCESS(nRetCode);
	}

	{
		char szCurrentPath[SCRIPT_NAME_LEN];
		CLuaScript* pScript = get_script_from_state(L);

		szCurrentPath[0] = 0;
		get_file_path(pScript->get_script_name(), SCRIPT_NAME_LEN, szCurrentPath, SCRIPT_NAME_LEN);

		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s", szCurrentPath, pcszFileName);

		nRetCode = is_file_exist(szFileName);
		PROCESS_SUCCESS(nRetCode);
	}

Exit0:
	return 0;
Exit1:
	lua_pushboolean(L, TRUE);

	return 1;
}


void CLuaScript::_enum_table(lua_State* L, int nTableIndex, int nTableDepth, int32_t nMaxDepth)
{
	int32_t nFieldCount = 0;
    int nTop = lua_gettop(L);
    nTableIndex--;

    lua_pushnil(L);
    while (lua_next(L, nTableIndex) != 0)
    {
        char szMessage[SCRIPT_NAME_LEN] = {0};
        char szTab[SCRIPT_NAME_LEN] = {0};
        const char* pszKeyName = NULL;
		const char* pszValueName = NULL;

		pszKeyName = luaL_tolstring(L, -2, NULL);
        if  (pszKeyName == NULL)
        {
            WRN("NULL key name!");
			lua_pop(L, 1);
			continue;
        }

		if (strncmp(pszKeyName, "_G", sizeof("_G")) == 0 ||
			strncmp(pszKeyName, "__", 2) == 0)
		{
			lua_pop(L, 2);
			continue;
		}

        for (int i = 0; i < nTableDepth; i++)
            strxcat(szTab, "\t", SCRIPT_NAME_LEN - i);

		pszValueName = luaL_tolstring(L, -2, NULL);
		if (pszValueName == NULL)
		{
			WRN("NULL value name!");
			lua_pop(L, 2);
			continue;
		}

		snprintf(szMessage, SCRIPT_NAME_LEN, "%s : %s", pszKeyName, pszValueName);
        WRN(szMessage);
		lua_pop(L, 2);

		if (lua_type(L, -1) == LUA_TTABLE && nTableDepth < nMaxDepth)
			_enum_table(L, -1, nTableDepth + 1, nMaxDepth);

        lua_pop(L, 1);
		nFieldCount++;

		if (nFieldCount >= MAX_PRINT_LOCAL_VAR)
		{
			WRN("\tfield var exceeds %d ", MAX_PRINT_LOCAL_VAR);
			break;
		}
    }

    lua_settop(L, nTop);

    return;
}

int32_t CLuaScript::_lua_err_func(lua_State* L)
{
    lua_Debug ar;
    int32_t nLevel = 1;
    static BOOL s_bInErrFunc = FALSE;

    if (s_bInErrFunc)
    {
        return 1;
    }

    s_bInErrFunc = TRUE;

    while (lua_getstack(L, nLevel, &ar) && nLevel < 16)
    {
        if (lua_getinfo(L, "Snlu", &ar))
        {
            char szMessage[1024] = {0};
            if (ar.namewhat[0] != 0)
                snprintf(szMessage, 1024, "[%d]%s:%d in function %s", nLevel, ar.short_src, ar.currentline, ar.name);
            else if (*ar.what == 'm')
                snprintf(szMessage, 1024, "[%d]%s:%d in main chunk", nLevel, ar.short_src, ar.currentline);
            else if (*ar.what == 'C' || *ar.what == 't')
                snprintf(szMessage, 1024, "[%d]%s:?", nLevel, ar.short_src);
            else
                snprintf(szMessage, 1024, "[%d]%s:%d in function <%s:%d>", nLevel, ar.short_src,
                ar.currentline, ar.short_src, ar.linedefined);
            WRN(szMessage);
        }

        int nLocalIndex = 1;
        const char* pcszVarName = lua_getlocal(L, &ar, nLocalIndex);
        while (pcszVarName)
        {
			if (strncmp(pcszVarName, "__", 2) != 0)
			{
				char szMessage[1024] = { 0 };
				switch (lua_type(L, -1))
				{
				case LUA_TNONE:
				{
					snprintf(szMessage, 1024, "\t%s : LUA_TNONE", pcszVarName);
					break;
				}
				case LUA_TUSERDATA:
				{
					snprintf(szMessage, 1024, "\t%s : LUA_TUSERDATA", pcszVarName);
					break;
				}
				case LUA_TNIL:
				{
					snprintf(szMessage, 1024, "\t%s : LUA_TNIL", pcszVarName);
					break;
				}
				case LUA_TNUMBER:
				{
					if (lua_isinteger(L, -1))
						snprintf(szMessage, 1024, "\t%s : LUA_TNUMBER %lld", pcszVarName, lua_tointeger(L, -1));
					else
						snprintf(szMessage, 1024, "\t%s : LUA_TNUMBER %f", pcszVarName, lua_tonumber(L, -1));
					break;
				}
				case LUA_TSTRING:
				{
					const char* pszValue = lua_tostring(L, -1);
					if (pszValue)
						snprintf(szMessage, 1024, "\t%s : LUA_TSTRING %s", pcszVarName, pszValue);
					else
						snprintf(szMessage, 1024, "\t%s : LUA_TSTRING NULL", pcszVarName);
					break;
				}
				case LUA_TTABLE:
				{
					snprintf(szMessage, 1024, "\t%s : LUA_TTABLE", pcszVarName);
					break;
				}
				case LUA_TFUNCTION:
				{
					snprintf(szMessage, 1024, "\t%s : LUA_TFUNCTION", pcszVarName);
					break;
				}
				}
				strfix(szMessage);
				WRN(szMessage);

				if (lua_type(L, -1) == LUA_TTABLE)
					_enum_table(L, -1, 2, 2);
			}

            lua_pop(L, 1);
            nLocalIndex++;
			if (nLocalIndex >= MAX_PRINT_LOCAL_VAR)
			{
				WRN("\tlocal var exceeds %d ", MAX_PRINT_LOCAL_VAR);

				break;
			}
            pcszVarName = lua_getlocal(L, &ar, nLocalIndex);
        }

        nLevel++;
    }

    s_bInErrFunc = FALSE;

    return 1;
}

int32_t CLuaScript::_print(lua_State* L)
{
	static char szMessage[1024];
	
	const char* pcszMessage = NULL;
    LOG_PROCESS_ERROR(lua_gettop(L) > 0);

	if(lua_istable(L, 1))
		_enum_table(L, -1, 0, 10);
	else
	{
		pcszMessage = lua_tostring(L, 1);
		LOG_PROCESS_ERROR(pcszMessage);

		strxcpy(szMessage, pcszMessage, 1024);
		strfix(szMessage);
	
		CLuaScript* pThis = get_script_from_state(L);
		if (pThis)
			INF("%s : %s", pThis->get_script_name(), szMessage);
		else
			INF("null script : %s", szMessage);
	}

Exit0:
    return 0;
}

int32_t CLuaScript::_warn(lua_State * L)
{
	static char szMessage[1024];

	const char* pcszMessage = NULL;
	LOG_PROCESS_ERROR(lua_gettop(L) > 0);

	pcszMessage = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszMessage);
	strxcpy(szMessage, pcszMessage, 1024);
	strfix(szMessage);
	WRN("%s", szMessage);

Exit0:
	return 0;
}

int32_t CLuaScript::_get_host_name(lua_State * L)
{
	int32_t nRetCode = 0;
	char szHostName[256] = { 0 };

	nRetCode = gethostname(szHostName, 256);
	LOG_PROCESS_ERROR(nRetCode == 0);

	lua_pushstring(L, szHostName);

	return 1;
Exit0:
	return 0;
}

int32_t CLuaScript::_get_host_ip(lua_State* L)
{
#if defined WIN32
	char chBuf[256] = "";
	hostent* pHost = NULL;
	char* pCharIP = NULL;

	gethostname(chBuf, 256);

	pHost = gethostbyname(chBuf);
	pCharIP = inet_ntoa(*((struct in_addr *)pHost->h_addr_list[0]));

	lua_pushstring(L, pCharIP);

#else
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	while (ifAddrStruct != NULL)
	{
		if (ifAddrStruct->ifa_addr->sa_family == AF_INET && strcmp(ifAddrStruct->ifa_name, "eth1") == 0)
		{
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			INF("%s IPV4 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);

			lua_pushstring(L, addressBuffer);
			INF("get %s : %s as IPV4 Address \n", ifAddrStruct->ifa_name, addressBuffer);

			return 1;
		}

		ifAddrStruct = ifAddrStruct->ifa_next;
	}

	lua_pushstring(L, "127.0.0.1");
#endif

	return 1;
}

int32_t CLuaScript::_get_user_type_mem(lua_State * L)
{
	int64_t llResult = 0;
	uint8_t* pPtr = (uint8_t*)tolua_tousertype(L, 1, 0);
	int64_t llOffset = lua_tointeger(L, 2);
	int64_t llSize = lua_tointeger(L, 3);

	LOG_PROCESS_ERROR(llSize <= 8);

	memcpy(&llResult, pPtr + llOffset, llSize);
	lua_pushinteger(L, llResult);

	return 1;
Exit0:
	return 0;
}

int32_t CLuaScript::_set_user_type_mem(lua_State * L)
{
	uint8_t* pPtr = (uint8_t*)tolua_tousertype(L, 1, 0);
	int64_t llOffset = lua_tointeger(L, 2);
	int64_t llSize = lua_tointeger(L, 3);
	int64_t llValue = lua_tointeger(L, 4);

	LOG_PROCESS_ERROR(llSize <= 8);

	memcpy(pPtr + llOffset, &llValue, llSize);

Exit0:
	return 0;
}
