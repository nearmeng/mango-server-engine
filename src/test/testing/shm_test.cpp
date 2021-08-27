#include "gtest.h"
#include "shm/shm_pool.h"

struct SHM_TEST_DATA
{
    int a;
    char szName[16];
};

TEST(SHM_TEST, SHM_POOL_TEST)
{
	int32_t nRetCode = 0;
    
	nRetCode = CShmMgr::instance().init(0x1234, 10 * 1024 * 1024, false);
    ASSERT_TRUE(nRetCode);

	CShmObject<SHM_TEST_DATA> ShmObject;
	nRetCode = ShmObject.init(2, false);
	ASSERT_TRUE(nRetCode);

	SHM_TEST_DATA* pData = ShmObject.get_obj();
	ASSERT_TRUE(pData != NULL);

	pData->a = 100;
    strncpy(pData->szName, "hello", sizeof(pData->szName));

	uint64_t qwMid = SHM_PTR2MID(pData);
	pData = (SHM_TEST_DATA*)SHM_MID2PTR(qwMid);

	ASSERT_EQ(pData->a, 100);
	ASSERT_STREQ(pData->szName, "hello");

    nRetCode = ShmObject.uninit();
    ASSERT_TRUE(nRetCode);

    nRetCode = CShmMgr::instance().uninit();
    ASSERT_TRUE(nRetCode);
}

TEST(SHM_TEST, ID_OBJECT_POOL_TEST)
{
    BOOL nRetCode = 0;

    nRetCode = CShmMgr::instance().init(0x1234, 10 * 1024 * 1024, false);
    ASSERT_TRUE(nRetCode);

    CShmObjectPool<SHM_TEST_DATA, int32_t> TestDataPool;

    nRetCode = TestDataPool.init(1, 10, false);
    ASSERT_TRUE(nRetCode);

    for (int i = 1; i <= 10; i++)
    {
        SHM_TEST_DATA* pData = TestDataPool.new_object(i);
        ASSERT_TRUE(pData != NULL);

        pData->a = i;
        strncpy(pData->szName, "hello", sizeof(pData->szName));
    }

    for (int i = 1; i <= 10; i++)
    {
        SHM_TEST_DATA* pData = TestDataPool.find_object(i);
        ASSERT_TRUE(pData != NULL);

        ASSERT_EQ(pData->a, i);
        ASSERT_STREQ(pData->szName, "hello");
    }

	SHM_TEST_DATA* pData = TestDataPool.find_object(5);
	ASSERT_TRUE(pData != NULL);
	
	uint64_t qwMid = SHM_PTR2MID(pData);
	pData = (SHM_TEST_DATA*)SHM_MID2PTR(qwMid);

	ASSERT_EQ(pData->a, 5);
	ASSERT_STREQ(pData->szName, "hello");

	nRetCode = TestDataPool.delete_object(pData);
	ASSERT_TRUE(nRetCode);

	pData = TestDataPool.find_object(5);
	ASSERT_TRUE(pData == NULL);

	ASSERT_TRUE(TestDataPool.get_count() == 10);
	ASSERT_TRUE(TestDataPool.get_free_count() == 1);
	ASSERT_TRUE(TestDataPool.get_used_count() == 9);

	pData = TestDataPool.get_first_object();
	while (pData)
	{
		ASSERT_TRUE(pData->a > 0);
		pData = TestDataPool.get_next_object();
	}

    nRetCode = TestDataPool.uninit();
    ASSERT_TRUE(nRetCode);

    nRetCode = CShmMgr::instance().uninit();
    ASSERT_TRUE(nRetCode);
}