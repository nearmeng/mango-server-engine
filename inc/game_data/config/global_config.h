#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

#include "define/redis_def.h"

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
        int32_t nInitDefaultClientSessionCount;

		int32_t nInitShmSize;

		int32_t nServerStopTimeout;
		int32_t nServerEndWaitTimeout;

        int32_t nClientPingTimeout;

        int32_t nResMode;
        char    szScriptPath[64];
        char    szResPath[64];

        struct DUMP_CONFIG
        {
            BOOL bIsMiniDump;
            int32_t nDumpInterval;
        };
        DUMP_CONFIG DumpConfig;

        int32_t nCommonCoroCount;
        int32_t nCoroWaitTimeInterval;

    } Common;

    struct CONN_CONFIG
    {
        int32_t nInitSessionMgrCount;
    }CONN;

    struct GS_CONFIG
    {
    	int32_t nTestValue;
        int32_t nUserCount;
        int32_t nRoleCount;
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

    struct DB_PROXY_CONFIG
    {
        int32_t nSyncConnectTimeout;
        int32_t nSyncDisConnectTimeout;
        int32_t nDBReqExpireTime;
        int32_t nDBPingInterval;
        int32_t nDBPingTimeout;
        int32_t nDBReConnectInterval;
    }DP;
    
    struct OS_CONFIG
    {
    	int32_t nUserObjCount;
	} OS;
};

struct REDIS_SCRIPT
{
    char szScript[MAX_REDIS_SCIRPT_LEN];
};

struct REDIS_SCRIPT_CONFIG
{
     REDIS_SCRIPT  scripts[MAX_CMD_HANDLER_COUNT];
};

extern SERVER_CONFIG g_ServerConfig;
extern SHARED_CONFIG g_SharedConfig;
extern REDIS_SCRIPT_CONFIG g_RedisScriptConfig;

// tolua_end

BOOL load_global_server_config(void);

#endif	// _GLOBAL_CONFIG_H_
