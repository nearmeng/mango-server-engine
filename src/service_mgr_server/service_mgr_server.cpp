#include "stdafx.h"

#include "app/server_app.h"
#include "define/server_def.h"

#include "message/sms_message_handler.h"
#include "object_mgr/sms_object_mgr.h"
#include "service_mgr/service_mgr.h"
#include "trans/sms_trans.h"

#include "game_data/global_config.h"
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
		mg_set_state(svstInit);
	else
		mg_set_state(svstInService);

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


BOOL server_proc(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

BOOL server_reload(TAPPCTX* pCtx, BOOL bResume)
{
	return TRUE;
}

BOOL server_stop(TAPPCTX* pCtx, BOOL bResume)
{
	int32_t nRetCode = 0;
	uint64_t qwCurrTick = CTimeMgr::instance().get_server_tick();

	switch (mg_get_state())
	{
	case svstInService:
	case svstInit:
	{
        if (mg_get_stop_timer() == 0)
        {
            mg_set_stop_timer(qwCurrTick + g_ServerConfig.Common.nServerStopTimeout);
		}
		else if (qwCurrTick > mg_get_stop_timer())
		{
			INF("wait for stop msg timeout, begin to server complete");
			mg_set_state(svstEndService);
		}
		break;
	}
	case svstEndService:
	{
		mg_set_stop_timer(qwCurrTick + g_ServerConfig.Common.nServerEndWaitTimeout);
		mg_set_state(svstInvalid);
		break;
	}
	case svstInvalid:
	{
		if (qwCurrTick > mg_get_stop_timer())
		{
			INF("begin to finish stop");
			return TRUE;
		}
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
	return TRUE;
}

BOOL server_frame(TAPPCTX* pCtx, BOOL bResume)
{
	CSMSMessageHandler::instance().frame();
	CSmsTransMgr::get_instance().mainloop();
	CServiceMgr::get_instance().mainloop();

	return TRUE;
}

int main(int argc, char* argv[])
{
	mg_set_user_msg_handler(svrTotal, CSMSMessageHandler::msg_handler);
	mg_app_main(argc, argv, server_init, server_fini, server_frame, server_reload, server_stop, server_proc, FALSE, TRUE);
}