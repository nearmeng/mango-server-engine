#include "stdafx.h"
#include "lua/lua_script.h"
#include "define/bt_def.h"

#include "bt/bt_mgr.h"

extern int lua_create_bt_node(lua_State* L);
extern int lua_create_bt_tree(lua_State* L);
extern int lua_create_bt_event(lua_State* L);
extern int lua_print_node(lua_State* L);
extern int lua_find_bt_action_id(lua_State* L);
extern int lua_register_bt_action(lua_State* L);
extern int lua_set_node_ret_value(lua_State* L);
extern int lua_register_global_event(lua_State* L);

extern int tolua_bt_open(lua_State* tolua_S);

int32_t lua_print_tree(lua_State* L)
{
	int32_t nTreeID = (int32_t)lua_tointeger(L, 1);

	uint32_t dwRoot = CBTMgr::instance().find_bt_tree_node(nTreeID);
	CBTMgr::instance().print(dwRoot, 0);

	return 0;
}

LUA_FUNC g_ServerBasePackageList[] = 
{
    { "bt.pkg",          tolua_bt_open },
    { "", 0 }
};

LUA_FUNC g_ServerBaseLuaFunc[] =
{
	{ "create_bt_node",			lua_create_bt_node },
	{ "create_bt_tree",			lua_create_bt_tree },
	{ "create_bt_event",		lua_create_bt_event },
	{ "print_node",				lua_print_node },
	{ "find_bt_action_id",		lua_find_bt_action_id },
	{ "register_bt_action",		lua_register_bt_action },
	{ "set_node_ret_value",		lua_set_node_ret_value },
	{ "c_register_global_event",lua_register_global_event },
	{ "print_tree",				lua_print_tree },

    { 0, 0 }
};
