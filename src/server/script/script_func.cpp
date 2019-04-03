#include "stdafx.h"

extern int lua_info_log(lua_State* L);

extern int tolua_role_open(lua_State* tolua_S);
extern int tolua_npc_open(lua_State* tolua_S);

int lua_info_log(lua_State* L)
{
	const char* pcszString = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszString);

	INF("lua print info log: %s\n", pcszString);

	return TRUE;
Exit0:
	return FALSE;
}

LUA_FUNC g_RegPackageList[] =
{
    { "role.pkg",			tolua_role_open },
	{ "npc.pkg",            tolua_npc_open  },

    { "", 0 }
};

LUA_FUNC g_RegLuaFunc[] =
{
	{ "INF_LOG",			lua_info_log},

    { 0, 0 }
};
