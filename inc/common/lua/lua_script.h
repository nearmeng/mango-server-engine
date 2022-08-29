#ifndef _LUA_SCRIPT_H_
#define _LUA_SCRIPT_H_

#define SCRIPT_NAME_LEN	(256)

#include "pal/ttypes.h"
#include "lua.h"

struct LUA_FUNC
{
	const char*		pcszFuncName;
	lua_CFunction	pFunc;
};

typedef void(*DEPEND_FUNC)(const char* pcszName, uint32_t& rdwCRC);

class CLuaScript
{
public:
	CLuaScript();
	virtual ~CLuaScript();

	BOOL init(void);
	BOOL uninit(void);

	BOOL load_from_file(const char* pcszFileName, BOOL bForce);
	BOOL load_from_buffer(const char* pcszScriptName, const char* pcszBuffer, uint32_t dwSize);

	BOOL register_func(LUA_FUNC FuncList[], int32_t nFuncCount);

	BOOL call_function(const char* pcszFuncName, const char* pcszFormat, ...);
    BOOL execute(void);

    BOOL is_func_exist(const char* pcszFuncName);
    BOOL is_table_exist(const char* pcszTableName);
	BOOL is_value_exist(const char* pcszValueName);

	const char* get_script_name(void);
	uint64_t get_name_hash(void);
	lua_State* get_lua_state(void);
	static CLuaScript* get_script_from_state(lua_State* L);

    BOOL collect_garbage(int32_t nGCSize);
	uint32_t get_crc(void);
	uint32_t get_file_size(void);
	uint64_t get_load_tick(void);
	void set_depend_func(DEPEND_FUNC pFunc);

private:
    BOOL _call_function(const char* pcszFuncName, const char* pcszFormat, va_list valist);
	void _get_crc(const char* pszFileName, int32_t nLevel);
	BOOL _load_from_file(const char* pcszFileName, BOOL bForce);
	BOOL _load_from_cache(const char* pcszFileName, uint32_t dwFileCRC);
	BOOL _save_into_cache(const char* pcszCacheName, uint32_t dwFileCRC);

	static void _fix_script_name(const char* pcszScriptName, char* pcszFixedName);

private:
	char				m_szScriptName[SCRIPT_NAME_LEN];
	char				m_szFixedScriptName[SCRIPT_NAME_LEN];
	lua_State*			m_pLuaState;
	uint64_t			m_qwNameHash;
	uint32_t			m_dwCRC;
	uint32_t			m_dwFileSize;
	uint64_t			m_qwLoadTick;
	DEPEND_FUNC			m_pDependFunc;
	BOOL				m_bExecuting;
	BOOL				m_bScriptCache;

	static int32_t _include(lua_State* L);
	static int32_t _depend(lua_State* L);
	static int32_t _is_file_exist(lua_State* L);
	static int32_t _lua_err_func(lua_State* L);
    static void _enum_table(lua_State* L, int32_t nTableIndex, int32_t nTableDepth, int32_t nMaxDepth);
    static int32_t _print(lua_State* L);
	static int32_t _warn(lua_State* L);
	static int32_t _get_host_name(lua_State* L);
	static int32_t _get_host_ip(lua_State* L);
	static int32_t _get_user_type_mem(lua_State* L);
	static int32_t _set_user_type_mem(lua_State* L);
};


#endif	// _LUA_SCRIPT_H_
