#include "stdafx.h"
#include "app/server_app.h"

#include "config/global_config.h"
#include "res_mgr/global_res_mgr.h"

#include "bt/bt_mgr.h"
#include "bt/bt_event.h"

#include "time/time_mgr.h"
#include "guid/guid.h"
#include "tbus/tbus_wrapper.h"
#include "app/server_msg_handler.h"

#include "define/server_def.h"
#include "define/time_def.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

#include "protocol/conn_message.h"

#include "coroutine/coro_stackless.h"

#include "router_client/router_client_api.h"
#include "db_proxy_client/db_proxy_client.h"

extern LUA_FUNC g_ServerBasePackageList[];
extern LUA_FUNC g_ServerBaseLuaFunc[];

CMGApp CMGApp::ms_Instance;

BOOL CMGApp::_init_tlog(void)
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

BOOL CMGApp::_default_msg_recv_proc(const char* pBuff, size_t nSize, int32_t nSrcAddr)
{
	int32_t nServerType = tbus_get_type(nSrcAddr);
	LOG_PROCESS_ERROR_DETAIL(nServerType > svrInvalid && nServerType < svrTotal, "type %d", nServerType);
		
	if (ms_Instance.m_UserMsgHandler[nServerType])
	{
		//user server type process
		ms_Instance.m_UserMsgHandler[nServerType](pBuff, nSize, nSrcAddr);
	}
	else if (ms_Instance.m_UserMsgHandler[svrTotal])
	{
		// user default process
		ms_Instance.m_UserMsgHandler[svrTotal](pBuff, nSize, nSrcAddr);
	}
	else
	{
		//default process
		if (nServerType == svrTconnd)
		{
			LOG_PROCESS_ERROR(ms_Instance.m_Config.bUseTconnd);
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

void CMGApp::_frame_timeout(uint64_t qwTimerID, void* pCbData, int32_t nCbDataLen)
{
    //module cont
    for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
    {
        CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
        LOG_CHECK_ERROR(pModule);

        if(pModule)
        { 
            pModule->on_frame();
        }
    }

    //coro
    CGlobalStacklessMgr::instance().mainloop();

    //user callback
    if (ms_Instance.m_Config.pAppFrame)
    {
        ms_Instance.m_Config.pAppFrame();
    }

    ms_Instance.m_nServerFrame++;

Exit0:
	return;
}

int32_t CMGApp::_app_init(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;
	BOOL bResume = ms_Instance.is_resume();
    int32_t nShmSize = (ms_Instance.m_Config.nInitShmSize == 0) ? g_ServerConfig.Common.nInitShmSize : ms_Instance.m_Config.nInitShmSize;
    int32_t nResMode = (ms_Instance.m_Config.nResMode == 0) ? g_ServerConfig.Common.nResMode : ms_Instance.m_Config.nResMode;

	//log
	nRetCode = ms_Instance._init_tlog();
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

	//server config
	nRetCode = load_global_server_config();
	LOG_PROCESS_ERROR(nRetCode);

	//shm
	nRetCode = CShmMgr::get_instance().init(pCtx->iId, nShmSize, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//guid
	nRetCode = guid_init(tbus_get_inst(pCtx->iId), bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//timer
	nRetCode = CTimeMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	//frame
	nRetCode = CTimeMgr::instance().set_timer_callback(ttGameFrame, _frame_timeout);
	LOG_PROCESS_ERROR(nRetCode);

	if (!bResume)
	{
		nRetCode = CTimeMgr::instance().add_timer(0, MS2TICK(1000 / GAME_FPS), INT_MAX,
			ttGameFrame, NULL, 0);
		LOG_PROCESS_ERROR(nRetCode);

		INF("server add frame timer success");
	}
	
	//router client or tbus 
	if (!ms_Instance.m_Config.bUseRouter)
	{
		nRetCode = tbus_system_init(pCtx->iId, pCtx->iBus, _default_msg_recv_proc, bResume);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = CRouterClient::instance().init(pCtx->iBus, pCtx->iId, _default_msg_recv_proc, bResume);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
	//event
	nRetCode = CEventMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGlobalEventListMgr::instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	if (ms_Instance.m_Config.bUseTconnd)
	{
		nRetCode = tconnapi_init((unsigned int)atol(pCtx->pszGCIMKey));
		LOG_PROCESS_ERROR(nRetCode == 0);
	}

    //res
    if (ms_Instance.m_Config.bUseRouter)
    {
        nRetCode = CGlobalResMgr::instance().init(nResMode, bResume);
        LOG_PROCESS_ERROR(nRetCode);
    }

    //coro
    nRetCode = CGlobalStacklessMgr::instance().init(pCtx->iId, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    //module cont
    for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
    {
        CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
        LOG_PROCESS_ERROR(pModule);

        nRetCode = pModule->init(bResume);
        LOG_PROCESS_ERROR(nRetCode);
    }

    if (ms_Instance.m_Config.bUseConn)
    {
        register_server_msg_handler(conn_ntf_event, recv_conn_ntf_event);
        register_server_msg_handler(conn_transfer_msg, recv_conn_transfer_msg);
    }

    if (ms_Instance.m_Config.bUseDBProxy)
    {
        nRetCode = CDBProxyClient::instance().init(bResume);
        LOG_PROCESS_ERROR(nRetCode);
    }

    //user callback
    if (ms_Instance.m_Config.pAppInit)
    {
        nRetCode = ms_Instance.m_Config.pAppInit(pCtx, ms_Instance.is_resume());
        LOG_PROCESS_ERROR(nRetCode);
    }

	INF("%s inited success", ms_Instance.m_szServerName);

	return 0;
Exit0:
	return -1;
}

int32_t CMGApp::_app_fini(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;
    BOOL bResume = ms_Instance.is_resume();

    //user callback
    if (ms_Instance.m_Config.pAppFini)
    {
        nRetCode = ms_Instance.m_Config.pAppFini(pCtx, ms_Instance.is_resume());
        LOG_CHECK_ERROR(nRetCode);
    }

    //module cont
    for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
    {
        CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
        LOG_CHECK_ERROR(pModule);

        if (pModule)
        {
            nRetCode = pModule->uninit();
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    //coro
    nRetCode = CGlobalStacklessMgr::instance().uninit();
    LOG_CHECK_ERROR(nRetCode);

	if (ms_Instance.m_Config.bUseTconnd)
		tconnapi_fini();

	nRetCode = guid_uninit();
	LOG_CHECK_ERROR(nRetCode);

    if (ms_Instance.m_Config.bUseDBProxy)
    {
        nRetCode = CDBProxyClient::instance().uninit();
        LOG_CHECK_ERROR(nRetCode);
    }
    
    if (ms_Instance.m_Config.bUseRouter)
    {
        nRetCode = CGlobalResMgr::instance().uninit();
        LOG_CHECK_ERROR(nRetCode);
    }

	nRetCode = CGlobalEventListMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CEventMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CScriptMgr::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	if (!ms_Instance.m_Config.bUseRouter)
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

int32_t CMGApp::_app_proc(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;

    //tbus
	if (!ms_Instance.m_Config.bUseRouter)
		nRetCode = tbus_recv_data();
	else
		nRetCode = CRouterClient::instance().mainloop();

    //timer
	CTimeMgr::instance().mainloop();

    //module cont
    for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
    {
        CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
        if (pModule)
        {
            pModule->on_proc();
        }
    }

	return nRetCode ? 0 : -1;
}

int32_t CMGApp::_app_reload(TAPPCTX* pCtx, void* pArg)
{
	int32_t nRetCode = 0;
	int32_t nCurrTime = CTimeMgr::instance().get_time_sec();

	LOG_PROCESS_ERROR(nCurrTime - ms_Instance.m_dwLastReloadTime >= MAX_SERVER_RELOAD_INTERVAL / 1000);

    //global config
	nRetCode = load_global_server_config();
	LOG_PROCESS_ERROR(nRetCode);

    //res
    nRetCode = CGlobalResMgr::instance().reload(FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    //module cont
    for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
    {
        CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
        LOG_CHECK_ERROR(pModule);

        if (pModule)
        {
            nRetCode = pModule->on_reload();
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    //user callback
    if (ms_Instance.m_Config.pAppReload)
    {
        ms_Instance.m_Config.pAppReload(pCtx, ms_Instance.is_resume());
    }

	ms_Instance.m_dwLastReloadTime = nCurrTime;

	return 0;
Exit0:
	return -1;
}

int32_t CMGApp::_app_stop(TAPPCTX* pCtx, void* pArg)
{
	//used to process stop procedure
	int32_t nRetCode = 0;
	uint64_t qwCurrTick = CTimeMgr::instance().get_server_tick();

	switch (ms_Instance.m_nState)
	{
	case svstInit:
	case svstInService:
	{
        //wait for config time
        if (ms_Instance.m_qwStopTimer == 0)
        {
            ms_Instance.set_stop_timer(qwCurrTick + g_ServerConfig.Common.nServerStopTimeout);
		}
		else if (qwCurrTick > ms_Instance.m_qwStopTimer)
        {
            INF("wait for stop msg timeout, begin to server complete");
            ms_Instance.set_stop_timer(0);
            ms_Instance.m_nState = svstEndService;
        }
		break;
	}
	case svstEndService:
	{
        //call user stop
        BOOL bAllStop = TRUE;
        for (int32_t nIndex = 0; nIndex < ms_Instance.m_ModuleCont.get_module_count(); nIndex++)
        {
            CServerModule* pModule = ms_Instance.m_ModuleCont.get_module_by_idx(nIndex);
            LOG_CHECK_ERROR(pModule);

            if (pModule)
            {
                nRetCode = pModule->on_stop();
                if (nRetCode == FALSE)
                {
                    bAllStop = FALSE;
                    break;
                }
            }
        }

        if (ms_Instance.m_Config.pAppStop)
        {
            nRetCode = ms_Instance.m_Config.pAppStop(pCtx, ms_Instance.is_resume());
            if (nRetCode == FALSE)
                bAllStop = FALSE;
        }

		if (bAllStop)
		{
			return 0;
		}
        else 
		    break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
	return -1;
}

int32_t CMGApp::_app_quit(TAPPCTX* pCtx, void* pArg)
{
	INF("recv server quit signal, used to resume");

	ms_Instance.m_bNeedExitClean = FALSE;

	return -1;
}

CMGApp::CMGApp(void)
{
    m_nState = svstInvalid;
    m_qwStopTimer = 0;
    m_nServerFrame = 0;
    memset(&m_stAppCtx, 0, sizeof(m_stAppCtx));

    m_dwLastReloadTime = 0;
    m_bNeedExitClean = TRUE;
    memset(&m_Config, 0, sizeof(m_Config));
    memset(m_UserMsgHandler, 0, sizeof(m_UserMsgHandler));
    m_ModuleCont.init();
}

BOOL CMGApp::init(const char* pcszServerName, int32_t argc, char* argv[])
{
    int32_t nRetCode = 0;
	void* arg = malloc(1);

#if defined(WIN32)
	WSADATA wsaData;
	nRetCode = WSAStartup(0x202, &wsaData);
	LOG_PROCESS_ERROR(nRetCode == 0);
#endif	// WIN32

    strxcpy(m_szServerName, pcszServerName, sizeof(m_szServerName));

	m_stAppCtx.argc = argc;
	m_stAppCtx.argv = argv;
	m_stAppCtx.pfnInit = _app_init;
	m_stAppCtx.pfnFini = _app_fini;
	m_stAppCtx.pfnProc = _app_proc;
	m_stAppCtx.pfnReload = _app_reload;
	m_stAppCtx.pfnStop = _app_stop;
	m_stAppCtx.pfnQuit = _app_quit;

	nRetCode = tapp_def_init(&m_stAppCtx, arg);
	LOG_PROCESS_ERROR_DETAIL(nRetCode == 0, "tapp init failed");

    return TRUE;
Exit0:
    return FALSE;
}

void CMGApp::run_mainloop(void)
{
	void* arg = malloc(1);

	tapp_def_mainloop(&m_stAppCtx, arg);

    return;
}

void CMGApp::fini()
{
	void* arg = malloc(1);

	if(m_bNeedExitClean)
		tapp_def_fini(&m_stAppCtx, arg);

    return ;
}
    
BOOL CMGApp::set_user_msg_handler(int32_t nServerType, USER_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(pMsgHandler);
	LOG_PROCESS_ERROR_DETAIL(nServerType > svrInvalid && nServerType <= svrTotal, "invalid server type %d", nServerType);

	m_UserMsgHandler[nServerType] = pMsgHandler;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMGApp::reload(void)
{
    int32_t nRetCode = 0;

    nRetCode = _app_reload(&m_stAppCtx, NULL);
    LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CMGApp::register_module(CServerModule* pModule)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pModule);

    m_ModuleCont.add_module(pModule);

    return TRUE;
Exit0:
    return FALSE;
}
    
CServerModule* CMGApp::get_module(const char* pcszModuleName)
{
    LOG_PROCESS_ERROR(pcszModuleName);

    return m_ModuleCont.get_module_by_name(pcszModuleName);

Exit0:
    return NULL;
}

void CMGApp::set_config(MG_CONFIG& config)
{
    m_Config = config;
    
    m_stAppCtx.pfnControllerInit = config.pControlInit;
    m_stAppCtx.pfnPreprocCmdLine = config.pControlPreProc;
    m_stAppCtx.pfnControllerFini = config.pControlFini;
    m_stAppCtx.pfnProcCmdLine = config.pControlProc;
    m_stAppCtx.pfnGetCtrlUsage = config.pControlHelp;
}