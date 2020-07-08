#include "stdafx.h"
#include "game_data/global_config.h"

#include "file/ifile.h"
#include "lua/lua_script.h"
#include "string/string_ex.h"

SHARED_CONFIG g_SharedConfig;
SERVER_CONFIG g_ServerConfig;

static CLuaScript g_ConfigScript;
extern int tolua_global_config_open(lua_State* tolua_S);
extern int tolua_enum_open(lua_State* tolua_S);

BOOL load_global_server_config(void)
{
	int32_t nRetCode = 0;
	char szFileName[256];

	{
		memset(&g_ServerConfig, 0, sizeof(g_ServerConfig));
		memset(&g_SharedConfig, 0, sizeof(g_SharedConfig));

		g_ConfigScript.uninit();

		nRetCode = g_ConfigScript.init();
		LOG_PROCESS_ERROR(nRetCode);

		tolua_global_config_open(g_ConfigScript.get_lua_state());
        tolua_enum_open(g_ConfigScript.get_lua_state());

		snprintf(szFileName, 256, "../server_config/server_config.lua");
		nRetCode = g_ConfigScript.load_from_file(szFileName, TRUE);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = g_ConfigScript.call_function("load_file", "os", &g_ServerConfig, "SERVER_CONFIG", "ServerConfig");
		LOG_PROCESS_ERROR(nRetCode);
	}

	g_ConfigScript.uninit();

	return TRUE;
Exit0:
	g_ConfigScript.uninit();

	return FALSE;
}
