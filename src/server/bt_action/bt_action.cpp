#include "stdafx.h"

#include "define/bt_def.h"
#include "bt/bt_mgr.h"
#include "object/role.h"

extern int32_t check_operator(int32_t nOperator, INT64 nLeftValue, INT64 nRightValue);

int32_t bat_check_role_level(BT_CTX& rCtx, int32_t nParam[])
{
	CRole* pRole = NULL;

	pRole = (CRole*)rCtx.pOwner;

	return check_operator(nParam[0], pRole->get_level(), nParam[1]);
Exit0:
	return brvError;
}

int32_t bat_role_run(BT_CTX& rCtx, int32_t nParam[])
{
	INF("role begin to run, p0 %d", nParam[0]);
	return brvSuccess;
}

//=================================================================================
void* bt_get_role(uint64_t qwOwnerID)
{
	CRole* pRole = NULL;

	//pRole = CRoleMgr::instance().find_role();

	return pRole;
}

void* bt_get_scene(uint64_t qwOwnerID)
{
	return NULL;
}

int32_t bt_get_role_var(uint64_t qwOwnerID, int32_t nVarType, int32_t nVarIndex)
{
	return 0;
}

int32_t bt_get_scene_var(uint64_t qwOwnerID, int32_t nVarType, int32_t nVarIndex)
{
	return 0;
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

	REG_BT_OWNER(botRole, "CRole", bt_get_role, bt_get_role_var);
	REG_BT_OWNER(botScene, "CScene", bt_get_scene, bt_get_scene_var);

	return TRUE;
Exit0:
	return FALSE;
}