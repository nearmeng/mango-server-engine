#include "gtest.h"
#include "shm/shm_pool.h"

struct SHM_TEST_DATA
{
    int a;
    char szName[16];
};

TEST(SHM_TEST, NORMAL_TEST)
{
    BOOL nRetCode = 0;

    nRetCode = CShmMgr::get_instance().init(0x1234, 10 * 1024 * 1024, false);
    ASSERT_TRUE(nRetCode);

    CShmObjectPool<SHM_TEST_DATA, int32_t> TestDataPool;
    nRetCode = TestDataPool.init(1, 10, false);
    ASSERT_TRUE(nRetCode);

    for (int i = 1; i <= 10; i++)
    {
        SHM_TEST_DATA* pData = TestDataPool.new_object(i);
        ASSERT_TRUE(pData != NULL);

        pData->a = 1;
        strncpy(pData->szName, "hello", sizeof(pData->szName));
    }

    for (int i = 1; i <= 10; i++)
    {
        SHM_TEST_DATA* pData = TestDataPool.find_object(i);
        ASSERT_TRUE(pData != NULL);

        ASSERT_EQ(pData->a, 1);
        ASSERT_STREQ(pData->szName, "hello");

        nRetCode = TestDataPool.delete_object(pData);
        ASSERT_TRUE(nRetCode);
    }

    nRetCode = TestDataPool.uninit();
    ASSERT_TRUE(nRetCode);

    nRetCode = CShmMgr::get_instance().uninit();
    ASSERT_TRUE(nRetCode);
}