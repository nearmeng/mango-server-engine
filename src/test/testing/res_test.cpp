#include "gtest.h"
#include "res_mgr/res_mgr.h"

#include "res_def/achieve_res.h"

TEST(RES_TEST, READ_TEST)
{
    int32_t nCount = 0;
    const ACHIEVE_RES* pRes = CResMgr<ACHIEVE_RES>::instance().get_first_res();
    while (pRes)
    {
        nCount++;
        printf("all read count %d\n", nCount);
        printf("res_data: id %d name %s cond_type %d achieve_type %d cond_param %d reward_id %d score %d\n", pRes->nID, pRes->szName, pRes->nCondType, pRes->nAchieveType, pRes->nCondParam[0], pRes->nRewardID, pRes->nScore);
        pRes = CResMgr<ACHIEVE_RES>::instance().get_next_res();
    }

	ASSERT_TRUE(nCount > 0);
};