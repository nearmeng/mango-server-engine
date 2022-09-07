#include "gtest.h"
#include "lua/script_mgr.h"

#include "define/bt_def.h"
#include "bt/bt_mgr.h"
#include "event/server_event.h"
#include "test_role.h"

extern int32_t check_operator(int32_t nOperator, INT64 nLeftValue, INT64 nRightValue);

int32_t bat_check_role_level(BT_CTX& rCtx, int32_t nParam[])
{
	CTestRole* pRole = NULL;

	pRole = (CTestRole*)rCtx.pOwner;

	return check_operator(nParam[0], pRole->get_level(), nParam[1]);
Exit0:
	return brvError;
}

int32_t bat_role_run(BT_CTX& rCtx, int32_t nParam[])
{
	printf("role begin to run, p0 %d", nParam[0]);
	return brvSuccess;
}

void* bt_get_role(uint64_t qwOwnerID)
{
	CTestRole* pRole = new CTestRole();
	return pRole;
}

void* bt_get_scene(uint64_t qwOwnerID)
{
	return NULL;
}

int32_t bt_get_role_var(uint64_t qwOwnerID, int32_t nVarType, int32_t nVarIndex)
{
	printf("try get role var, var_type is %d value index is %d\n", nVarType, nVarIndex);
	return 10;
}

int32_t bt_get_scene_var(uint64_t qwOwnerID, int32_t nVarType, int32_t nVarIndex)
{
	printf("try get scene var, var_type is %d value index is %d\n", nVarType, nVarIndex);
	return 20;
}
	
BOOL reg_bt_action(void)
{
	int32_t nRetCode = 0;

	REG_BT_C_ACTION(batCheckRoleLevel, bat_check_role_level);
	REG_BT_C_ACTION(batRoleRun, bat_role_run);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL reg_bt_owner(void)
{
	int32_t nRetCode = 0;

	REG_BT_OWNER(otRole, "CTestRole", bt_get_role, bt_get_role_var);

	return TRUE;
Exit0:
	return FALSE;
}

TEST(SCRIPT_TEST, CALL_FUNC)
{
	CTestRole role;
	int32_t nRetCode = 0;
	CLuaScript* pScript = NULL;

	role.init();
	
	pScript = CScriptMgr::instance().new_script("test1.lua");
    ASSERT_TRUE(pScript != NULL);

	nRetCode = pScript->call_function("create_role", "o", &role, "CTestRole");
    ASSERT_TRUE(nRetCode);
}

TEST(SCRIPT_TEST, EVENT_TEST)
{
	CTestRole role;
	int32_t nRetCode = 0;
	CLuaScript* pScript = NULL;

	role.init();

	nRetCode = CBTMgr::instance().init(reg_bt_owner, FALSE);
	ASSERT_TRUE(nRetCode);

	CBTMgr::instance().set_debug_mode(TRUE);

	nRetCode = reg_bt_action();
	ASSERT_TRUE(nRetCode);

	pScript = CScriptMgr::instance().new_script("test2.lua");
    ASSERT_TRUE(pScript != NULL);

	nRetCode = CGlobalEventListMgr::instance().trigger_global_event(evtRoleKillNpc, 10, 0, &role, 123);
	ASSERT_TRUE(nRetCode);
}
