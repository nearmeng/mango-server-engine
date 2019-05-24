#include "stdafx.h"

#include "define/bt_def.h"

int32_t check_operator(int32_t nOperator, INT64 nLeftValue, INT64 nRightValue)
{
	switch (nOperator)
	{
	case boLT:
		if (nLeftValue < nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boLTE:
		if (nLeftValue <= nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boGT:
		if (nLeftValue > nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boGTE:
		if (nLeftValue >= nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boEQ:
		if (nLeftValue == nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boNEQ:
		if (nLeftValue != nRightValue)
			return brvSuccess;
		else
			return brvFail;
	default:
		return brvError;
	}
}

int32_t bat_return_success(BT_CTX& rCtx, int32_t nParam[])
{
	return brvSuccess;
}

int32_t bat_return_fail(BT_CTX& rCtx, int32_t nParam[])
{
	return brvFail;
}