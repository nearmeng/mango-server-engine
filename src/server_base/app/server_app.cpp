#include "stdafx.h"
#include "app/server_app.h"

#include "game_data/global_config.h"

#include "bt/bt_mgr.h"
#include "bt/bt_event.h"

#include "time/time_mgr.h"
#include "guid/guid.h"
#include "app/server_tbus.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

static TAPPCTX stAppCtx = { 0 };
static BOOL bNeedExitClean = TRUE;
static BOOL bUseTconnd = FALSE;
static SERVER_APP_FUNC pUserInit;
static SERVER_APP_FUNC pUserFini;
static SERVER_APP_FUNC pUserProc;
static SERVER_APP_FUNC pUserReload;
static SERVER_APP_FUNC pUserTick;
static SERVER_APP_FUNC pUserStop;

extern LUA_FUNC g_ServerBasePackageList[];
extern LUA_FUNC g_ServerBaseLuaFunc[];

typedef BOOL(*MSG_HANDLER)(int32_t nSrcAddr, const char* pBuff, size_t dwSize);

static BOOL is_app_resume(TAPPCTX* pCtx)
{
	LOG_PROCESS_ERROR(pCtx);
	return (pCtx->iStartMode == TAPP_START_MODE_RESUME);
Exit0:
	return FALSE;
}

static BOOL server_app_init_tlog(void)
{
	int32_t nRetCode = 0;

	nRetCode = tapp_get_category("osslog", &g_pOssLogCat);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = tapp_get_category("text", &g_pSvrLogCat);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	printf("log system is init failed\n");
	return FALSE;
}

static BOOL server_app_msg_recv_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize)
{
	int32_t nServerType = tbus_get_type(nSrcAddr);
	LOG_PROCESS_ERROR(nServerType > svrInvalid && nServerType < svrTotal);

	if (nServerType == svrTconnd)
	{
		LOG_PROCESS_ERROR(bUseTconnd);
		recv_conn_msg_proc(nSrcAddr, pBuff, nSize);
	}
	else
	{
		recv_server_msg_proc(nSrcAddr, pBuff, nSize);
	}

	return TRUE;
Exit0:
	return FALSE;
}

static int32_t server_app_init(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;
	BOOL bResume = is_app_resume(pCtx);

	//log
	nRetCode = server_app_init_tlog();
	LOG_PROCESS_ERROR(nRetCode);

	//server config
	nRetCode = load_global_server_config();
	LOG_PROCESS_ERROR(nRetCode);

	//shm
	nRetCode = CShmMgr::get_instance().init(pCtx->iId, g_ServerConfig.Common.nInitShmSize, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//tbus 
	nRetCode = tbus_system_init(pCtx->iId, pCtx->iBus, server_app_msg_recv_proc, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//guid
	nRetCode = guid_init(tbus_get_inst(pCtx->iId), bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//timer
	nRetCode = CTimeMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);
	
	//script
	nRetCode = CScriptMgr::instance().init(pCtx->pszId);
	LOG_PROCESS_ERROR(nRetCode);

	CScriptMgr::instance().add_include_path("../script");
	CScriptMgr::instance().add_include_path("../server_config");
	
	for(int32_t i = 0; g_ServerBasePackageList[i].pFunc; i++)
	{
		nRetCode = CScriptMgr::instance().register_package(g_ServerBasePackageList[i].pcszFuncName, g_ServerBasePackageList[i].pFunc);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
	//event
	nRetCode = CEventMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGlobalEventListMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	if (bUseTconnd)
	{
		nRetCode = tconnapi_init((unsigned int)atol(pCtx->pszGCIMKey));
		LOG_PROCESS_ERROR(nRetCode == 0);
	}

	LOG_PROCESS_ERROR(pUserInit);

	nRetCode = pUserInit(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_fini(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

	if (pUserFini)
		pUserFini(is_app_resume(pCtx));

	if (bUseTconnd)
		tconnapi_fini();

	nRetCode = guid_uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CGlobalEventListMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CEventMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CScriptMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CTimeMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CShmMgr::get_instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_proc(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

	nRetCode = tbus_recv_data();
	CTimeMgr::instance().mainloop();

	return nRetCode ? 0 : -1;
}

static int32_t server_app_reload(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

	nRetCode = load_global_server_config();
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(pUserReload);
	
	nRetCode = pUserReload(is_app_resume(pCtx));
	LOG_PROCESS_ERROR(nRetCode);

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_tick(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

	if (pUserTick)
	{
		nRetCode = pUserTick(is_app_resume(pCtx));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_stop(TAPPCTX* pCtx, void* pArg)
{
	//used to process stop procedure
	int32_t nRetCode = 0;
	
	if (pUserStop)
	{
		nRetCode = pUserStop(is_app_resume(pCtx));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_quit(TAPPCTX* pCtx, void* pArg)
{
	INF("recv server quit signal, used to resume");

	bNeedExitClean = FALSE;

	return -1;
}

int32_t mg_app_main(int32_t argc, char* argv[],
	SERVER_APP_FUNC pInit,
	SERVER_APP_FUNC pFini,
	SERVER_APP_FUNC pProc,
	SERVER_APP_FUNC pReload,
	SERVER_APP_FUNC pStop,
	SERVER_APP_FUNC pTick,
	BOOL bUseTconnd)
{
	int32_t nRetCode = 0;
	void* arg = malloc(1);
	bUseTconnd = bUseTconnd;

	stAppCtx.argc = argc;
	stAppCtx.argv = argv;
	stAppCtx.pfnInit = server_app_init;
	stAppCtx.pfnFini = server_app_fini;
	stAppCtx.pfnProc = server_app_proc;
	stAppCtx.pfnReload = server_app_reload;
	stAppCtx.pfnTick = server_app_tick;
	stAppCtx.pfnStop = server_app_stop;
	stAppCtx.pfnQuit = server_app_quit;

	pUserInit = pInit;
	pUserFini = pFini;
	pUserProc = pProc;
	pUserReload = pReload;
	pUserStop = pStop;
	pUserTick = pTick;

	nRetCode = tapp_def_init(&stAppCtx, arg);
	LOG_PROCESS_ERROR_DETAIL(nRetCode == 0, "tapp init failed");

	tapp_def_mainloop(&stAppCtx, arg);

	if(bNeedExitClean)
		tapp_def_fini(&stAppCtx, arg);

	return 0;
Exit0:
	return -1;
}