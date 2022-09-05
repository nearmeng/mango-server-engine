#include "stdafx.h"
#include "test_env.h"

#include "guid/guid.h"
#include "coroutine/coro_stackless.h"
#include "res_mgr/global_res_mgr.h"

void CTestEnv::SetUp()
{
	int32_t nRetCode = 0;

	CScriptMgr::instance().init("1.0.0.1");
	CScriptMgr::instance().add_include_path("../script");
	CScriptMgr::instance().add_include_path("../server_config");

	load_global_server_config();
	
	CShmMgr::instance().init(0x1234, 100 * 1024 * 1024, false);
	CTimeMgr::instance().init(false);
	CGlobalStacklessMgr::instance().init(12345, false);
	guid_init(1, false);
    CGlobalResMgr::instance().init(rlmCsv, false);

}

void CTestEnv::TearDown()
{
	CGlobalStacklessMgr::instance().uninit();
	CTimeMgr::instance().uninit();
    CShmMgr::instance().uninit();
	CScriptMgr::instance().uninit();
	guid_uninit();
    CGlobalResMgr::instance().uninit();
}
