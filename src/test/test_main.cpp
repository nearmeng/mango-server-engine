#include "stdafx.h"
#include "gtest.h"

#include "test_env.h"

int main(int argc, char* argv[])
{
    testing::Environment* pEnv = new CTestEnv();
    LOG_PROCESS_ERROR(pEnv);

    testing::AddGlobalTestEnvironment(pEnv);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
Exit0:
    return -1;
}