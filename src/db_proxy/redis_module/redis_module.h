#ifndef _REDIS_MODULE_H_
#define _REDIS_MODULE_H_

#include "module/server_module.h"

class CRedisCli;

class CRedisModule : public CServerModule
{
public:
    CRedisModule() {};
	virtual ~CRedisModule() {};

	virtual BOOL init(BOOL bResume);
	virtual BOOL uninit(void);
    virtual void on_frame(void);
    virtual void on_proc(void);

    inline CRedisCli* get_test_client();

private:
    BOOL _init_msg_handler();

private:
    CRedisCli* pTestRedisCli;
};

inline CRedisCli* CRedisModule::get_test_client(void)
{
    return pTestRedisCli;
}

#endif