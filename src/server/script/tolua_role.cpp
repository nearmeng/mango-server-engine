/*
** Lua binding: role
** Generated automatically by toluapp-1.0.94 on Wed Apr  3 11:22:44 2019.
*/

#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "toluapp.h"

/* Exported function */
TOLUA_API int tolua_role_open (lua_State* tolua_S);

#include "../../../inc/define/role.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CRole");
}

/* method: get_name of class  CRole */
#ifndef TOLUA_DISABLE_tolua_role_CRole_get_name00
static int tolua_role_CRole_get_name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CRole",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CRole* self = (CRole*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_name'", NULL);
#endif
 {
  const char* tolua_ret = (const char*)  self->get_name();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_name of class  CRole */
#ifndef TOLUA_DISABLE_tolua_role_CRole_set_name00
static int tolua_role_CRole_set_name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CRole",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CRole* self = (CRole*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_name'", NULL);
#endif
 {
  self->set_name(pcszName);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_age of class  CRole */
#ifndef TOLUA_DISABLE_tolua_role_CRole_get_age00
static int tolua_role_CRole_get_age00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CRole",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CRole* self = (CRole*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_age'", NULL);
#endif
 {
  int32_t tolua_ret = (int32_t)  self->get_age();
 tolua_pushinteger(tolua_S,(lua_Integer)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_age'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_age of class  CRole */
#ifndef TOLUA_DISABLE_tolua_role_CRole_set_age00
static int tolua_role_CRole_set_age00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CRole",0,&tolua_err) ||
 !tolua_isinteger(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CRole* self = (CRole*)  tolua_tousertype(tolua_S,1,0);
  int32_t nAge = ((int32_t)  tolua_tointeger(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_age'", NULL);
#endif
 {
  self->set_age(nAge);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_age'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_role_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_cclass(tolua_S,"CRole","CRole","",NULL);
 tolua_beginmodule(tolua_S,"CRole");
#ifndef TOLUA_DISABLE_tolua_role_CRole_get_name00
  tolua_function(tolua_S,"get_name",tolua_role_CRole_get_name00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_role_CRole_set_name00
  tolua_function(tolua_S,"set_name",tolua_role_CRole_set_name00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_role_CRole_get_age00
  tolua_function(tolua_S,"get_age",tolua_role_CRole_get_age00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_role_CRole_set_age00
  tolua_function(tolua_S,"set_age",tolua_role_CRole_set_age00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_role (lua_State* tolua_S) {
 return tolua_role_open(tolua_S);
};
#endif

