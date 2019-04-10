#include "stdafx.h"
#include <time.h>

#include "tloghelp/tlogload.h"
#include "lua/lua_script.h"
#include "lua/script_mgr.h"

#include "define/role.h"

extern LUA_FUNC g_RegPackageList[];
extern LUA_FUNC g_RegLuaFunc[];

CRole role;

void init_script(void)
{
	int32_t nRetCode = 0;
	CLuaScript* pScript = NULL;

	INF("begin to init script");

	nRetCode = CScriptMgr::instance().init("1.1.1.1");
	LOG_PROCESS_ERROR(nRetCode);

	CScriptMgr::instance().add_include_path("../script");

	for(int32_t i = 0; g_RegPackageList[i].pFunc; i++)
	{
		nRetCode = CScriptMgr::instance().register_package(g_RegPackageList[i].pcszFuncName, g_RegPackageList[i].pFunc);
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = CScriptMgr::instance().register_func_list(g_RegLuaFunc);
	LOG_PROCESS_ERROR(nRetCode);

	INF("begin to new script");

	pScript = CScriptMgr::instance().new_script("test.lua");
	LOG_PROCESS_ERROR(pScript);

	nRetCode = pScript->call_function("create_role", "o", &role, "CRole");
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

uint64_t get_tick_count()
{
#ifdef __linux__
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_nsec / 1000000 + ts.tv_sec * 1000;
#else
	return GetTickCount64();
#endif
}

struct TEST_DATA
{
	int32_t a;
	int32_t b;
};
	
struct TRAVERSE_DATA
{
	int32_t operator()(int32_t id, TEST_DATA* test_data)
	{
		INF("traverse id is %d, test_data %d %d\n", id, test_data->a, test_data->b);

		return 0;
	}
};

LPTLOGCTX pLogCtx = NULL;

int main()
{
	uint64_t result[6] = { 0 };
	int32_t unit_count = 1000000;

	int32_t ret = 0;
	int32_t key = 0x777;
	int32_t size = 500 * 1024 * 1024;

	CShmObjectPool<TEST_DATA, int32_t> test_pool;

	result[0] = get_tick_count();

	pLogCtx = tlog_init_from_file("./tlog.xml");
	if(pLogCtx == NULL)
	{
		printf("log init from file failed");
		return -1;
	}

	g_pLogCategory = tlog_get_category(pLogCtx, "text");
	if(g_pLogCategory == NULL)
	{
		printf("log init failed");
		return -1;
	}

	INF("--game log is begin--");
	
	init_script();

	ret = CShmMgr::get_instance().init(key, size, false);
	if(ret != 0)
	{
		ERR("shm mgr init failed, ret %d", ret);
		return ret;
	}

	test_pool.init(stdType1, unit_count, false);
	
	result[1] = get_tick_count();

	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = test_pool.new_object(i);
		if(test_data == NULL)
		{
			ERR("new obj failed");
			ERR("new obj failed");
			return -1;
		}

		test_data->a = 1;
		test_data->b = 2;
	}
	
	result[2] = get_tick_count();
	
	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = test_pool.find_object(i);
		if(test_data == NULL)
		{
			ERR("find object failed, %d", i);
			return -1;
		}
	}
	
	result[3] = get_tick_count();
	
	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = test_pool.find_object(i);
		test_pool.delete_object(test_data);
	}
	
	result[4] = get_tick_count();

	//TRAVERSE_DATA traverse_data;
	//test_pool.traverse(traverse_data);

	ret = CShmMgr::get_instance().uninit();
	if(ret != 0)
	{
		ERR("shm mgr uninit failed, ret %d", ret);
		return -1;
	}
	
	result[5] = get_tick_count();

/*

	CIDObjectPool<TEST_DATA, int32_t> m_TestPool;

	result[0] = get_tick_count();

	m_TestPool.init(unit_count, "1.1.1.1");
	
	result[1] = get_tick_count();

	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = m_TestPool.new_object(i);
		if(test_data == NULL)
		{
			ERR("new object failed\n");
			return -1;
		}
		
		test_data->a = 1;
		test_data->b = 2;
	}
	
	result[2] = get_tick_count();

	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = m_TestPool.find(i);
		if(test_data == NULL)
		{
			ERR("find object failed\n");
			return -1;
		}
	}
	
	result[3] = get_tick_count();

	for(int32_t i = 1; i <= unit_count; i++)
	{
		TEST_DATA* test_data = m_TestPool.find(i);
		m_TestPool.delete_object(test_data);
	}
	
	result[4] = get_tick_count();

	m_TestPool.uninit();
	
	result[5] = get_tick_count();

#endif

	INF("init %lf, new %lf, find %lf, delete %lf, uninit %lf\n", 
			(double)(result[1] - result[0]), 
			(double)(result[2] - result[1]), 
			(double)(result[3] - result[2]),
			(double)(result[4] - result[3]),
			(double)(result[5] - result[4]));

	PROCESS_ERROR(1);
	PROCESS_SUCCESS(0);
	LOG_CHECK_ERROR(1);
	LOG_PROCESS_ERROR(1);
	LOG_PROCESS_ERROR_DETAIL(0, "test failed %d", 1);
*/

	return 0;

Exit1:
	return 0;

	// this is the test code

Exit0:
	return 0;
}
