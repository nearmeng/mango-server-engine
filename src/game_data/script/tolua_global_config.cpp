/*
** Lua binding: global_config
** Generated automatically by toluapp-1.0.94 on Wed Aug 21 14:20:36 2019.
*/

#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "toluapp.h"

/* Exported function */
TOLUA_API int tolua_global_config_open (lua_State* tolua_S);

#include "../../../inc/game_data/global_config.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"SERVER_CONFIG::GS_CONFIG");
 tolua_usertype(tolua_S,"SERVER_CONFIG::COMMON");
 tolua_usertype(tolua_S,"SERVER_CONFIG");
 tolua_usertype(tolua_S,"SHARED_CONFIG");
}

/* get function: dwLoginTag of class  SHARED_CONFIG */
#ifndef TOLUA_DISABLE_tolua_get_SHARED_CONFIG_dwLoginTag
static int tolua_get_SHARED_CONFIG_dwLoginTag(lua_State* tolua_S)
{
  SHARED_CONFIG* self = (SHARED_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwLoginTag'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->dwLoginTag);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwLoginTag of class  SHARED_CONFIG */
#ifndef TOLUA_DISABLE_tolua_set_SHARED_CONFIG_dwLoginTag
static int tolua_set_SHARED_CONFIG_dwLoginTag(lua_State* tolua_S)
{
  SHARED_CONFIG* self = (SHARED_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwLoginTag'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwLoginTag = ((uint32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTestValue of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nTestValue
static int tolua_get_SERVER_CONFIG__COMMON_nTestValue(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTestValue'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nTestValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTestValue of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nTestValue
static int tolua_set_SERVER_CONFIG__COMMON_nTestValue(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTestValue'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTestValue = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitBtCtxCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nInitBtCtxCount
static int tolua_get_SERVER_CONFIG__COMMON_nInitBtCtxCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtCtxCount'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nInitBtCtxCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitBtCtxCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nInitBtCtxCount
static int tolua_set_SERVER_CONFIG__COMMON_nInitBtCtxCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtCtxCount'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitBtCtxCount = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitBtEventCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nInitBtEventCount
static int tolua_get_SERVER_CONFIG__COMMON_nInitBtEventCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtEventCount'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nInitBtEventCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitBtEventCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nInitBtEventCount
static int tolua_set_SERVER_CONFIG__COMMON_nInitBtEventCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtEventCount'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitBtEventCount = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitBtGlobalEventListCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount
static int tolua_get_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtGlobalEventListCount'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nInitBtGlobalEventListCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitBtGlobalEventListCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount
static int tolua_set_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitBtGlobalEventListCount'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitBtGlobalEventListCount = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitTimerPoolCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nInitTimerPoolCount
static int tolua_get_SERVER_CONFIG__COMMON_nInitTimerPoolCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitTimerPoolCount'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nInitTimerPoolCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitTimerPoolCount of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nInitTimerPoolCount
static int tolua_set_SERVER_CONFIG__COMMON_nInitTimerPoolCount(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitTimerPoolCount'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitTimerPoolCount = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitShmSize of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__COMMON_nInitShmSize
static int tolua_get_SERVER_CONFIG__COMMON_nInitShmSize(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitShmSize'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nInitShmSize);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitShmSize of class  COMMON */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__COMMON_nInitShmSize
static int tolua_set_SERVER_CONFIG__COMMON_nInitShmSize(lua_State* tolua_S)
{
  SERVER_CONFIG::COMMON* self = (SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitShmSize'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitShmSize = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: Common of class  SERVER_CONFIG */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG_Common
static int tolua_get_SERVER_CONFIG_Common(lua_State* tolua_S)
{
  SERVER_CONFIG* self = (SERVER_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Common'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->Common,"SERVER_CONFIG::COMMON");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: Common of class  SERVER_CONFIG */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG_Common
static int tolua_set_SERVER_CONFIG_Common(lua_State* tolua_S)
{
  SERVER_CONFIG* self = (SERVER_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Common'",NULL);
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"SERVER_CONFIG::COMMON",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->Common = *((SERVER_CONFIG::COMMON*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTestValue of class  GS_CONFIG */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG__GS_CONFIG_nTestValue
static int tolua_get_SERVER_CONFIG__GS_CONFIG_nTestValue(lua_State* tolua_S)
{
  SERVER_CONFIG::GS_CONFIG* self = (SERVER_CONFIG::GS_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTestValue'",NULL);
#endif
 tolua_pushinteger(tolua_S,(lua_Integer)self->nTestValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTestValue of class  GS_CONFIG */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG__GS_CONFIG_nTestValue
static int tolua_set_SERVER_CONFIG__GS_CONFIG_nTestValue(lua_State* tolua_S)
{
  SERVER_CONFIG::GS_CONFIG* self = (SERVER_CONFIG::GS_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTestValue'",NULL);
 if (!tolua_isinteger(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTestValue = ((int32_t)  tolua_tointeger(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: GS of class  SERVER_CONFIG */
#ifndef TOLUA_DISABLE_tolua_get_SERVER_CONFIG_GS
static int tolua_get_SERVER_CONFIG_GS(lua_State* tolua_S)
{
  SERVER_CONFIG* self = (SERVER_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'GS'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->GS,"SERVER_CONFIG::GS_CONFIG");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: GS of class  SERVER_CONFIG */
#ifndef TOLUA_DISABLE_tolua_set_SERVER_CONFIG_GS
static int tolua_set_SERVER_CONFIG_GS(lua_State* tolua_S)
{
  SERVER_CONFIG* self = (SERVER_CONFIG*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'GS'",NULL);
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"SERVER_CONFIG::GS_CONFIG",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->GS = *((SERVER_CONFIG::GS_CONFIG*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: g_ServerConfig */
#ifndef TOLUA_DISABLE_tolua_get_g_ServerConfig
static int tolua_get_g_ServerConfig(lua_State* tolua_S)
{
  tolua_pushusertype(tolua_S,(void*)&g_ServerConfig,"SERVER_CONFIG");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: g_ServerConfig */
#ifndef TOLUA_DISABLE_tolua_set_g_ServerConfig
static int tolua_set_g_ServerConfig(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"SERVER_CONFIG",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  g_ServerConfig = *((SERVER_CONFIG*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: g_SharedConfig */
#ifndef TOLUA_DISABLE_tolua_get_g_SharedConfig
static int tolua_get_g_SharedConfig(lua_State* tolua_S)
{
  tolua_pushusertype(tolua_S,(void*)&g_SharedConfig,"SHARED_CONFIG");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: g_SharedConfig */
#ifndef TOLUA_DISABLE_tolua_set_g_SharedConfig
static int tolua_set_g_SharedConfig(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"SHARED_CONFIG",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  g_SharedConfig = *((SHARED_CONFIG*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_global_config_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,1);
 tolua_beginmodule(tolua_S,NULL);
 tolua_cclass(tolua_S,"SHARED_CONFIG","SHARED_CONFIG","",NULL);
 tolua_beginmodule(tolua_S,"SHARED_CONFIG");
  tolua_variable(tolua_S,"dwLoginTag",tolua_get_SHARED_CONFIG_dwLoginTag,tolua_set_SHARED_CONFIG_dwLoginTag);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"SERVER_CONFIG","SERVER_CONFIG","",NULL);
 tolua_beginmodule(tolua_S,"SERVER_CONFIG");
  tolua_cclass(tolua_S,"COMMON","SERVER_CONFIG::COMMON","",NULL);
  tolua_beginmodule(tolua_S,"COMMON");
   tolua_variable(tolua_S,"nTestValue",tolua_get_SERVER_CONFIG__COMMON_nTestValue,tolua_set_SERVER_CONFIG__COMMON_nTestValue);
   tolua_variable(tolua_S,"nInitBtCtxCount",tolua_get_SERVER_CONFIG__COMMON_nInitBtCtxCount,tolua_set_SERVER_CONFIG__COMMON_nInitBtCtxCount);
   tolua_variable(tolua_S,"nInitBtEventCount",tolua_get_SERVER_CONFIG__COMMON_nInitBtEventCount,tolua_set_SERVER_CONFIG__COMMON_nInitBtEventCount);
   tolua_variable(tolua_S,"nInitBtGlobalEventListCount",tolua_get_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount,tolua_set_SERVER_CONFIG__COMMON_nInitBtGlobalEventListCount);
   tolua_variable(tolua_S,"nInitTimerPoolCount",tolua_get_SERVER_CONFIG__COMMON_nInitTimerPoolCount,tolua_set_SERVER_CONFIG__COMMON_nInitTimerPoolCount);
   tolua_variable(tolua_S,"nInitShmSize",tolua_get_SERVER_CONFIG__COMMON_nInitShmSize,tolua_set_SERVER_CONFIG__COMMON_nInitShmSize);
  tolua_endmodule(tolua_S);
  tolua_variable(tolua_S,"Common",tolua_get_SERVER_CONFIG_Common,tolua_set_SERVER_CONFIG_Common);
  tolua_cclass(tolua_S,"GS_CONFIG","SERVER_CONFIG::GS_CONFIG","",NULL);
  tolua_beginmodule(tolua_S,"GS_CONFIG");
   tolua_variable(tolua_S,"nTestValue",tolua_get_SERVER_CONFIG__GS_CONFIG_nTestValue,tolua_set_SERVER_CONFIG__GS_CONFIG_nTestValue);
  tolua_endmodule(tolua_S);
  tolua_variable(tolua_S,"GS",tolua_get_SERVER_CONFIG_GS,tolua_set_SERVER_CONFIG_GS);
 tolua_endmodule(tolua_S);
 tolua_variable(tolua_S,"g_ServerConfig",tolua_get_g_ServerConfig,tolua_set_g_ServerConfig);
 tolua_variable(tolua_S,"g_SharedConfig",tolua_get_g_SharedConfig,tolua_set_g_SharedConfig);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_global_config (lua_State* tolua_S) {
 return tolua_global_config_open(tolua_S);
};
#endif

