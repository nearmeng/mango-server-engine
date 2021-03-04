#include "gtest.h"

class CTestEnv : public testing::Environment
{
public:
    CTestEnv() {};

    virtual void SetUp() override;
    virtual void TearDown() override;

protected:
};