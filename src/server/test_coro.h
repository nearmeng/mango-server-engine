#ifndef _TEST_CORO_H_
#define _TEST_CORO_H_

#include "coroutine/coro_stackless.h"

class CTestCoro : public CCoroStackless
{
public:
    CTestCoro() {};
    ~CTestCoro() {};

    void set_start_arg(int32_t nTestValue);
    virtual CORO_STATE coro_process();

private:
    int32_t         m_nTestValue;
};

#endif