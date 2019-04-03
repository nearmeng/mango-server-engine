/*
** Lua binding: npc
** Generated automatically by toluapp-1.0.94 on Wed Apr  3 11:22:44 2019.
*/

#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "toluapp.h"

/* Exported function */
TOLUA_API int tolua_npc_open (lua_State* tolua_S);

#include "../../../inc/define/npc.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* Open function */
TOLUA_API int tolua_npc_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_npc (lua_State* tolua_S) {
 return tolua_npc_open(tolua_S);
};
#endif

