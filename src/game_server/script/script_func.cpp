#include "stdafx.h"

extern int tolua_role_open(lua_State* tolua_S);
extern int tolua_npc_open(lua_State* tolua_S);

LUA_FUNC g_RegPackageList[] =
{
    { "role.pkg",			tolua_role_open },
	{ "npc.pkg",            tolua_npc_open  },

    { "", 0 }
};

LUA_FUNC g_RegLuaFunc[] =
{

    { 0, 0 }
};
