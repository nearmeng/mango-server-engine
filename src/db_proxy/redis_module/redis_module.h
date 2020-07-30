#ifndef _REDIS_MODULE_H_
#define _REDIS_MODULE_H_

#include "module/server_module.h"

class CRedisCli;

class CRedisModule : CServerModule
{
public:
    CRedisModule() {};
	virtual ~CRedisModule() {};

	virtual BOOL init(BOOL bResume);
	virtual BOOL uninit(void);
    virtual void on_frame(void);
    virtual void on_proc(void);
    inline static CRedisModule* instance(const char* pcszModuleName, ...);

    inline CRedisCli* get_test_client();

private:
    BOOL _init_msg_handler();

private:
    CRedisCli* pTestRedisCli;
};

inline CRedisModule* CRedisModule::instance(const char* pcszModuleName, ...)
{
    CRedisModule* pModule = new CRedisModule;

    pModule->set_name(pcszModuleName);

    return pModule;
}

inline CRedisCli* CRedisModule::get_test_client(void)
{
    return pTestRedisCli;
}

#endif