#include "stdafx.h"

#include "define/bt_def.h"
#include "bt/bt_mgr.h"
#include "define/role.h"

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

BOOL reg_bt_action(void)
{
	int32_t nRetCode = 0;

	REG_BT_C_ACTION(batCheckRoleLevel, bat_check_role_level);
	REG_BT_C_ACTION(batRoleRun, bat_role_run);

	return TRUE;
Exit0:
	return FALSE;
}