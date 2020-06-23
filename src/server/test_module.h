#ifndef _TEST_MODULE_H_
#define _TEST_MODULE_H_

#include "module/server_module.h"

class CTestModule : CServerModule
{
public:
    CTestModule() {};
    virtual ~CTestModule() {};

    static CTestModule* instance(const char* pcszModuleName, ...);

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);

private:
};

#endif