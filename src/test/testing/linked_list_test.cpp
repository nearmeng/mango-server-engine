#include "gtest.h"
#include "shm/shm_pool.h"
#include "linked_list/linked_list.h"

struct SHM_TEST_DATA_LINK : LINK_NODE
{
    int a;
    char szName[16];
};

TEST(LIST_TEST, COMMON_TEST)
{
	int32_t nRetCode = 0;
	LINK_HEAD Head = { 0 };

    CShmObjectPool<int32_t, SHM_TEST_DATA_LINK> TestDataPool;
    nRetCode = TestDataPool.init(100, 10, false);
    ASSERT_TRUE(nRetCode);

    SHM_TEST_DATA_LINK* pData1 = TestDataPool.new_object(123);
    ASSERT_TRUE(pData1 != NULL);
    
	SHM_TEST_DATA_LINK* pData2 = TestDataPool.new_object(456);
    ASSERT_TRUE(pData2 != NULL);

	nRetCode = add_tail<SHM_TEST_DATA_LINK>(&Head, SHM_PTR2MID(pData1));
	ASSERT_TRUE(nRetCode);

	nRetCode = add_tail<SHM_TEST_DATA_LINK>(&Head, SHM_PTR2MID(pData2));
	ASSERT_TRUE(nRetCode);

	nRetCode = del_node<SHM_TEST_DATA_LINK>(&Head, SHM_PTR2MID(pData1));
	ASSERT_TRUE(nRetCode);
	
	nRetCode = del_node<SHM_TEST_DATA_LINK>(&Head, SHM_PTR2MID(pData2));
	ASSERT_TRUE(nRetCode);
}