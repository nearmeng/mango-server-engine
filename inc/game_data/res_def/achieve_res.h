#ifndef  _ACHIEVE_RES_H_
#define  _ACHIEVE_RES_H_

#include "game_data/res_mgr/res_mgr.h"

// tolua_begin

enum ACHIEVE_TYPE
{
    achtInvalid,

    achtNormal,

    achtTotal
};

struct ACHIEVE_RES : RES_BASE
{
	int32_t nCondType;
	int32_t nAchieveType;
	int32_t nCondParam[6];
	int32_t nRewardID;
	int32_t nScore;
	int32_t nLevelLimit;
	int32_t nDayLimit;
};

// tolua_end

#endif