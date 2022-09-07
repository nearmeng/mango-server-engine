#include "stdafx.h"
#include "test_env.h"

#include "guid/guid.h"
#include "coroutine/coro_stackless.h"
#include "res_mgr/global_res_mgr.h"
#include "event/server_event.h"
#include "bt/bt_mgr.h"

extern LUA_FUNC g_ServerBasePackageList[];
extern LUA_FUNC g_ServerBaseLuaFunc[];
extern int tolua_test_role_open(lua_State* tolua_S);

LUA_FUNC g_RegPackageList[] =
{
    { "test_role.pkg",			tolua_test_role_open },
    { "", 0 }
};

void CTestEnv::SetUp()
{
	int32_t nRetCode = 0;

	CScriptMgr::instance().init("1.0.0.1");
	CScriptMgr::instance().add_include_path("../script");
	CScriptMgr::instance().add_include_path("../server_config");

	load_global_server_config();
	
	CShmMgr::instance().init(0x1234, 200 * 1024 * 1024, FALSE);
	CTimeMgr::instance().init(FALSE);
	CGlobalStacklessMgr::instance().init(12345, FALSE);
	guid_init(1, FALSE);
    CGlobalResMgr::instance().init(rlmCsv, FALSE);

	for(int32_t i = 0; g_ServerBasePackageList[i].pFunc; i++)
	{
		nRetCode = CScriptMgr::instance().register_package(g_ServerBasePackageList[i].pcszFuncName, g_ServerBasePackageList[i].pFunc);
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = CScriptMgr::instance().register_func_list(g_ServerBaseLuaFunc);
	LOG_PROCESS_ERROR(nRetCode);

	for (int32_t i = 0; g_RegPackageList[i].pFunc; i++)
	{
		nRetCode = CScriptMgr::instance().register_package(g_RegPackageList[i].pcszFuncName, g_RegPackageList[i].pFunc);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
	nRetCode = CEventMgr::instance().init(FALSE);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGlobalEventListMgr::instance().init(FALSE);
	LOG_PROCESS_ERROR(nRetCode);

	CBTMgr::instance().set_debug_mode(TRUE);

Exit0:
	return;
}

void CTestEnv::TearDown()
{
	CGlobalStacklessMgr::instance().uninit();
	CTimeMgr::instance().uninit();
    CShmMgr::instance().uninit();
	CScriptMgr::instance().uninit();
	guid_uninit();
    CGlobalResMgr::instance().uninit();
	CScriptMgr::instance().uninit();
	CEventMgr::instance().uninit();
	CGlobalEventListMgr::instance().uninit();
}
