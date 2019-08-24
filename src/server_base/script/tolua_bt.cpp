/*
** Lua binding: bt
** Generated automatically by toluapp-1.0.94 on Wed Aug 21 14:20:36 2019.
*/

#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "toluapp.h"

/* Exported function */
TOLUA_API int tolua_bt_open (lua_State* tolua_S);

#include "../../../inc/define/bt_def.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* Open function */
TOLUA_API int tolua_bt_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"bptInvalid",bptInvalid);
  tolua_constant(tolua_S,"bptNormal",bptNormal);
  tolua_constant(tolua_S,"bptLuaFunc",bptLuaFunc);
  tolua_constant(tolua_S,"bptLocalEventVar",bptLocalEventVar);
  tolua_constant(tolua_S,"bptLocalTriggerVar",bptLocalTriggerVar);
  tolua_constant(tolua_S,"bptRoleVar",bptRoleVar);
  tolua_constant(tolua_S,"bptSceneVar",bptSceneVar);
  tolua_constant(tolua_S,"bptTotal",bptTotal);
  tolua_constant(tolua_S,"brvError",brvError);
  tolua_constant(tolua_S,"brvFail",brvFail);
  tolua_constant(tolua_S,"brvSuccess",brvSuccess);
  tolua_constant(tolua_S,"brvRunning",brvRunning);
  tolua_constant(tolua_S,"brvTotal",brvTotal);
  tolua_constant(tolua_S,"btInvalid",btInvalid);
  tolua_constant(tolua_S,"btSelector",btSelector);
  tolua_constant(tolua_S,"btSequencer",btSequencer);
  tolua_constant(tolua_S,"btParallel",btParallel);
  tolua_constant(tolua_S,"btNot",btNot);
  tolua_constant(tolua_S,"btDebug",btDebug);
  tolua_constant(tolua_S,"btRollBack",btRollBack);
  tolua_constant(tolua_S,"btRandom",btRandom);
  tolua_constant(tolua_S,"btRepeat",btRepeat);
  tolua_constant(tolua_S,"btUserDefine",btUserDefine);
  tolua_constant(tolua_S,"btTotal",btTotal);
  tolua_constant(tolua_S,"botInvalid",botInvalid);
  tolua_constant(tolua_S,"botRole",botRole);
  tolua_constant(tolua_S,"botScene",botScene);
  tolua_constant(tolua_S,"botTotal",botTotal);
  tolua_constant(tolua_S,"evtInvalid",evtInvalid);
  tolua_constant(tolua_S,"evtStaticBegin",evtStaticBegin);
  tolua_constant(tolua_S,"evtStaticRoleBegin",evtStaticRoleBegin);
  tolua_constant(tolua_S,"evtRoleCreate",evtRoleCreate);
  tolua_constant(tolua_S,"evtStaticRoleEnd",evtStaticRoleEnd);
  tolua_constant(tolua_S,"evtStaticEnd",evtStaticEnd);
  tolua_constant(tolua_S,"evtDynamicBegin",evtDynamicBegin);
  tolua_constant(tolua_S,"evtDynamicRoleBegin",evtDynamicRoleBegin);
  tolua_constant(tolua_S,"evtRoleKillNpc",evtRoleKillNpc);
  tolua_constant(tolua_S,"evtDynamicRoleEnd",evtDynamicRoleEnd);
  tolua_constant(tolua_S,"evtDynamicEnd",evtDynamicEnd);
  tolua_constant(tolua_S,"evtTotal",evtTotal);
  tolua_constant(tolua_S,"tarInvalid",tarInvalid);
  tolua_constant(tolua_S,"tarOwner",tarOwner);
  tolua_constant(tolua_S,"tarTarget",tarTarget);
  tolua_constant(tolua_S,"tarTotal",tarTotal);
  tolua_constant(tolua_S,"boLT",boLT);
  tolua_constant(tolua_S,"boLTE",boLTE);
  tolua_constant(tolua_S,"boGT",boGT);
  tolua_constant(tolua_S,"boGTE",boGTE);
  tolua_constant(tolua_S,"boEQ",boEQ);
  tolua_constant(tolua_S,"boNEQ",boNEQ);
  tolua_constant(tolua_S,"batInvalid",batInvalid);
  tolua_constant(tolua_S,"batSelector",batSelector);
  tolua_constant(tolua_S,"batSequencer",batSequencer);
  tolua_constant(tolua_S,"batParallel",batParallel);
  tolua_constant(tolua_S,"batNot",batNot);
  tolua_constant(tolua_S,"batDebug",batDebug);
  tolua_constant(tolua_S,"batRollback",batRollback);
  tolua_constant(tolua_S,"batRandom",batRandom);
  tolua_constant(tolua_S,"batRepeat",batRepeat);
  tolua_constant(tolua_S,"batBegin",batBegin);
  tolua_constant(tolua_S,"batReturnSuccess",batReturnSuccess);
  tolua_constant(tolua_S,"batReturnFail",batReturnFail);
  tolua_constant(tolua_S,"batPrint",batPrint);
  tolua_constant(tolua_S,"batSetLocalVar",batSetLocalVar);
  tolua_constant(tolua_S,"batGetLocalVar",batGetLocalVar);
  tolua_constant(tolua_S,"batCheckRoleLevel",batCheckRoleLevel);
  tolua_constant(tolua_S,"batRoleRun",batRoleRun);
  tolua_constant(tolua_S,"batEnd",batEnd);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_bt (lua_State* tolua_S) {
 return tolua_bt_open(tolua_S);
};
#endif

