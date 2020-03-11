#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

// tolua_begin

struct SHARED_CONFIG
{
    uint32_t	dwLoginTag;
};

struct SERVER_CONFIG
{
    struct COMMON
    {
		int32_t nTestValue;	
		int32_t nInitBtCtxCount;
		int32_t nInitBtEventCount;
		int32_t nInitBtGlobalEventListCount;
		int32_t nInitTimerPoolCount;

		int32_t nInitShmSize;

		int32_t nServerStopTimeout;
		int32_t nServerEndWaitTimeout;
    } Common;

    struct GS_CONFIG
    {
    	int32_t nTestValue;
	} GS;

	struct SERVICE_CONFIG
	{
		int32_t nInitObjMgrCount;
		int32_t nHeartBeatTimeOut;
		int32_t nRouterReportInterval;
		int32_t nRouterHeartBeatInterval;
		int32_t nRouterDownReportTimeInterval;
		BOOL	bRouterLogEnable;
	} SC;
};

extern SERVER_CONFIG g_ServerConfig;
extern SHARED_CONFIG g_SharedConfig;

// tolua_end

BOOL load_global_server_config(void);

#endif	// _GLOBAL_CONFIG_H_
