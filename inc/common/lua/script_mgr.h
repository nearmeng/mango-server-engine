#ifndef _SCRIPT_MGR_H_
#define _SCRIPT_MGR_H_

#include <list>
#include <map>
#include <string.h>
#include "lua_script.h"

#define MAX_INCLUDE_PATH	(4)

class CScriptMgr
{
public:
	CScriptMgr(void);
	virtual ~CScriptMgr(void);

    BOOL init(const char* pcszCachePrefix);
    BOOL uninit(void);
	static CScriptMgr& instance(void);
    BOOL reload(int32_t& rnErrorCount, BOOL bForce);
    BOOL reload(const char *pcszScriptName, BOOL bForce);

    CLuaScript* new_script(const char* pcszScriptName);
    BOOL del_script(CLuaScript* poScript);
	CLuaScript* find_script(const char* pcszScriptName);
	CLuaScript* find_script(uint64_t qwNameHash);

    BOOL register_package(const char* pcszPackageName, lua_CFunction pLoadPackage);
    lua_CFunction find_package(const char* pcszPackageName);

    BOOL register_func_list(LUA_FUNC* pFuncList);
	void add_include_path(const char* pcszRootPath);
    const char* get_include_path(int32_t nIndex);
	inline int32_t get_include_path_count(void);
	void set_depend_func(DEPEND_FUNC pDependFunc);

	int64_t print_memory_stat(void);
	inline uint32_t get_crc(void);
	inline const char* get_cache_prefix(void);

private:
    static int32_t _load_package(lua_State* L);

private:
    static CScriptMgr ms_Instance;

	struct ltstr
	{
		inline bool operator()(const char* str1, const char* str2) const
		{
			return strncmp(str1, str2, SCRIPT_NAME_LEN) < 0;
		}
	};

	typedef std::map<const char*, CLuaScript*, ltstr> MAP_NAME_2_SCRIPT;
	MAP_NAME_2_SCRIPT   m_mapScript;

	typedef std::map<uint64_t, CLuaScript*> MAP_HASH_2_SCRIPT;
	MAP_HASH_2_SCRIPT   m_hashScript;

    typedef std::map<const char*, lua_CFunction, ltstr> MAP_NAME_2_PACKAGE;
    MAP_NAME_2_PACKAGE  m_mapPackage;

    typedef std::list<LUA_FUNC*> LST_LUA_FUNC;
    LST_LUA_FUNC        m_lstLuaFunc;

	char				m_szIncludePathList[MAX_INCLUDE_PATH][SCRIPT_NAME_LEN];
	int32_t				m_nIncludePathCount;
	DEPEND_FUNC			m_pDependFunc;
	uint32_t			m_dwCRC;
	char				m_szCachePrefix[SCRIPT_NAME_LEN];
};

inline int32_t CScriptMgr::get_include_path_count(void)
{
	return m_nIncludePathCount;
}

inline uint32_t CScriptMgr::get_crc(void)
{
	return m_dwCRC;
}

inline const char * CScriptMgr::get_cache_prefix(void)
{
	return m_szCachePrefix;
}

#endif // _script_mgr_h__
