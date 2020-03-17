#include "stdafx.h"
#include "app/server_app.h"

#include "game_data/global_config.h"

#include "bt/bt_mgr.h"
#include "bt/bt_event.h"

#include "time/time_mgr.h"
#include "guid/guid.h"
#include "tbus/tbus_wrapper.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"
#include "define/time_def.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

#include "router_client/router_client_api.h"

static TAPPCTX stAppCtx = { 0 };
static BOOL gs_bNeedExitClean = TRUE;
static BOOL gs_bUseTconnd = FALSE;
static BOOL gs_bBasicMode = FALSE;
static int32_t nServerFrame = 0;
static SERVER_APP_FUNC pUserInit;
static SERVER_APP_FUNC pUserFini;
static SERVER_APP_FUNC pUserProc;
static SERVER_APP_FUNC pUserReload;
static SERVER_APP_FUNC pUserFrame;
static SERVER_APP_FUNC pUserStop;

static uint32_t g_dwLastReloadTime;
extern LUA_FUNC g_ServerBasePackageList[];
extern LUA_FUNC g_ServerBaseLuaFunc[];

static USER_MSG_HANDLER g_UserMsgHandler[svrTotal + 1];
static int32_t g_nState;
static uint64_t g_qwStopTimer;

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

#if defined(WIN32)
	init_console_window();
#endif

	nRetCode = tapp_get_category("text", &g_pSvrLogCat);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	//nRetCode = tapp_get_category("osslog", &g_pOssLogCat);
	//LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	printf("log system is init failed\n");
	return FALSE;
}

static BOOL server_app_msg_recv_proc(const char* pBuff, size_t nSize, int32_t nSrcAddr)
{
	int32_t nServerType = tbus_get_type(nSrcAddr);
	LOG_PROCESS_ERROR_DETAIL(nServerType > svrInvalid && nServerType < svrTotal, "type %d", nServerType);
		
	if (g_UserMsgHandler[nServerType])
	{
		//user server type process
		g_UserMsgHandler[nServerType](pBuff, nSize, nSrcAddr);
	}
	else if (g_UserMsgHandler[svrTotal])
	{
		// user default process
		g_UserMsgHandler[svrTotal](pBuff, nSize, nSrcAddr);
	}
	else
	{
		//default process
		if (nServerType == svrTconnd)
		{
			LOG_PROCESS_ERROR(gs_bUseTconnd);
			recv_conn_msg_proc(nSrcAddr, pBuff, nSize);
		}
		else
		{
			recv_server_msg_proc(nSrcAddr, pBuff, nSize);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

static void server_frame_timeout(uint64_t qwTimerID, void* pCbData, int32_t nCbDataLen)
{
	LOG_PROCESS_ERROR(pUserFrame);

	pUserFrame(&stAppCtx, is_app_resume(&stAppCtx));
	nServerFrame++;

Exit0:
	return;
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

	//guid
	nRetCode = guid_init(tbus_get_inst(pCtx->iId), bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//timer
	nRetCode = CTimeMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//frame
	if (pUserFrame)
	{
		nRetCode = CTimeMgr::instance().set_timer_callback(ttGameFrame, server_frame_timeout);
		LOG_PROCESS_ERROR(nRetCode);

		if (!bResume)
		{
			nRetCode = CTimeMgr::instance().add_timer(0, MS2TICK(1000 / GAME_FPS), INT_MAX,
				ttGameFrame, NULL, 0);
			LOG_PROCESS_ERROR(nRetCode);

			INF("server add frame timer success");
		}
	}
	
	//router client or tbus 
	if (gs_bBasicMode)
	{
		nRetCode = tbus_system_init(pCtx->iId, pCtx->iBus, server_app_msg_recv_proc, bResume);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = CRouterClient::instance().init(pCtx->iBus, pCtx->iId, server_app_msg_recv_proc, bResume);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
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

	if (gs_bUseTconnd)
	{
		nRetCode = tconnapi_init((unsigned int)atol(pCtx->pszGCIMKey));
		LOG_PROCESS_ERROR(nRetCode == 0);
	}

	LOG_PROCESS_ERROR(pUserInit);

	nRetCode = pUserInit(pCtx, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	INF("server inited success");

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_fini(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

	if (pUserFini)
		pUserFini(pCtx, is_app_resume(pCtx));

	if (gs_bUseTconnd)
		tconnapi_fini();

	nRetCode = guid_uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CGlobalEventListMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CEventMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CScriptMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	if (gs_bBasicMode)
	{
		nRetCode = tbus_system_uninit();
		LOG_CHECK_ERROR(nRetCode);
	}
	else
	{
		nRetCode = CRouterClient::instance().uninit();
		LOG_CHECK_ERROR(nRetCode);
	}

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

	if (gs_bBasicMode)
		nRetCode = tbus_recv_data();
	else
		nRetCode = CRouterClient::instance().mainloop();

	CTimeMgr::instance().mainloop();

	if (pUserProc)
		pUserProc(pCtx, is_app_resume(pCtx));

	return nRetCode ? 0 : -1;
}

static int32_t server_app_reload(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;
	int32_t nCurrTime = CTimeMgr::instance().get_time_sec();

	LOG_PROCESS_ERROR(nCurrTime - g_dwLastReloadTime >= MAX_SERVER_RELOAD_INTERVAL / 1000);

	nRetCode = load_global_server_config();
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(pUserReload);
	
	nRetCode = pUserReload(pCtx, is_app_resume(pCtx));
	LOG_PROCESS_ERROR(nRetCode);

	g_dwLastReloadTime = nCurrTime;

	return 0;
Exit0:
	return -1;
}

static int32_t server_app_stop(TAPPCTX* pCtx, void* pArg)
{
	//used to process stop procedure
	int32_t nRetCode = 0;
	uint64_t qwCurrTick = CTimeMgr::instance().get_server_tick();

	switch (mg_get_state())
	{
	case svstInit:
	case svstInService:
	{
        if (mg_get_stop_timer() == 0)
        {
            mg_set_stop_timer(qwCurrTick + g_ServerConfig.Common.nServerStopTimeout);
		}
		else if (qwCurrTick > mg_get_stop_timer())
        {
            INF("wait for stop msg timeout, begin to server complete");
			mg_set_stop_timer(0);
            mg_set_state(svstEndService);
        }
		break;
	}
	case svstEndService:
	{
		if (pUserStop)
		{
			if (pUserStop(pCtx, is_app_resume(pCtx)))
			{
				return 0;
			}
		}
		else
			return 0;

		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
	return -1;
}

static int32_t server_app_quit(TAPPCTX* pCtx, void* pArg)
{
	INF("recv server quit signal, used to resume");

	gs_bNeedExitClean = FALSE;

	return -1;
}

int32_t mg_app_main(int32_t argc, char* argv[],
	SERVER_APP_FUNC pInit,
	SERVER_APP_FUNC pFini,
	SERVER_APP_FUNC pFrame,
	SERVER_APP_FUNC pReload,
	SERVER_APP_FUNC pStop,
	SERVER_APP_FUNC pProc,
	BOOL bUseTconnd,
	BOOL bBasicMode)
{
	int32_t nRetCode = 0;
	void* arg = malloc(1);
	gs_bUseTconnd = bUseTconnd;
	gs_bBasicMode = bBasicMode;

	stAppCtx.argc = argc;
	stAppCtx.argv = argv;
	stAppCtx.pfnInit = server_app_init;
	stAppCtx.pfnFini = server_app_fini;
	stAppCtx.pfnProc = server_app_proc;
	stAppCtx.pfnReload = server_app_reload;
	stAppCtx.pfnStop = server_app_stop;
	stAppCtx.pfnQuit = server_app_quit;

	pUserInit = pInit;
	pUserFini = pFini;
	pUserProc = pProc;
	pUserReload = pReload;
	pUserStop = pStop;
	pUserFrame = pFrame;

	nRetCode = tapp_def_init(&stAppCtx, arg);
	LOG_PROCESS_ERROR_DETAIL(nRetCode == 0, "tapp init failed");

	tapp_def_mainloop(&stAppCtx, arg);

	if(gs_bNeedExitClean)
		tapp_def_fini(&stAppCtx, arg);

	return 0;
Exit0:
	return -1;
}

BOOL mg_set_user_msg_handler(int32_t nServerType, USER_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(pMsgHandler);
	LOG_PROCESS_ERROR_DETAIL(nServerType > svrInvalid && nServerType <= svrTotal, "invalid server type %d", nServerType);

	g_UserMsgHandler[nServerType] = pMsgHandler;

Exit0:
	return FALSE;
}

BOOL mg_set_state(int32_t nState)
{
	g_nState = nState;
	return TRUE;
}

int32_t mg_get_state(void)
{
	return g_nState;
}

BOOL mg_set_stop_timer(uint64_t qwServerTick)
{
	g_qwStopTimer = qwServerTick;
	return TRUE;
}

int32_t mg_get_stop_timer(void)
{
	return g_qwStopTimer;
}

int32_t mg_get_tbus_addr(void)
{
	return stAppCtx.iId;
}

int32_t mg_get_game_frame(void)
{
	return nServerFrame;
}