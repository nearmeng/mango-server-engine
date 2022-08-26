#ifndef _TEST_MODULE_H_
#define _TEST_MODULE_H_

#include "module/server_module.h"

class CTestModule : public CServerModule
{
public:
    CTestModule() {};
    virtual ~CTestModule() {};

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);

private:
};

#endif