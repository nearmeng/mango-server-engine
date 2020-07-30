#include "stdafx.h"

#include "app/server_app.h"
#include "define/server_def.h"

#include "message/sms_message_handler.h"
#include "object_mgr/sms_object_mgr.h"
#include "service_mgr/service_mgr.h"
#include "trans/sms_trans.h"

#include "config/global_config.h"
#include "time/time_mgr.h"

BOOL server_init(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;
    
    nRetCode = CSMSMessageHandler::instance().init(pCtx->iBus, pCtx->iId, pCtx->pszId, bResume);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CSMSObjMgr::get_instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CServiceMgr::get_instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CSmsTransMgr::get_instance().init(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	if (!bResume)
		CMGApp::instance().set_state(svstInit);
	else
		CMGApp::instance().set_state(svstInService);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL server_fini(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = CSMSMessageHandler::instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CSMSObjMgr::get_instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CSmsTransMgr::get_instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CServiceMgr::get_instance().uninit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL server_reload(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

BOOL server_stop(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;
	uint64_t qwCurrTick = CTimeMgr::instance().get_server_tick();
    int32_t nStopTimer = CMGApp::instance().get_stop_timer();
        
	if (nStopTimer == 0)
    {
		// delay for server unregister
		CMGApp::instance().set_stop_timer(qwCurrTick + g_ServerConfig.Common.nServerEndWaitTimeout);
	}
	else if (qwCurrTick > nStopTimer)
	{
		INF("begin to finish stop");
		return TRUE;
	}

Exit0:
	return TRUE;
}

BOOL server_frame(void)
{
	CSMSMessageHandler::instance().frame();
	CSmsTransMgr::get_instance().mainloop();
	CServiceMgr::get_instance().mainloop();

	return TRUE;
}

int main(int argc, char* argv[])
{
    int32_t nRetCode = 0;
    CMGApp* pServer = &CMGApp::instance();

    pServer->set_user_msg_handler(svrTotal, CSMSMessageHandler::msg_handler);
    pServer->set_app_func(server_init, server_fini, server_frame, server_reload, server_stop);

    nRetCode = pServer->init("service_mgr_server", argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    pServer->run_mainloop();

    pServer->fini();

Exit0:
    return 0;
}