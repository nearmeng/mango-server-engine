#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

typedef BOOL(*SERVER_APP_FUNC)(TAPPCTX* pCtx, BOOL bIsResume);
typedef BOOL(*SERVER_CONTROL_FUNC)(unsigned short argc, const char** argv);
typedef const char*(*SERVER_HELP_FUNC)(void);
typedef BOOL(*USER_MSG_HANDLER)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

struct CONTROL_FUNCS
{
	SERVER_APP_FUNC		pInit;
	SERVER_APP_FUNC		pFini;
	SERVER_CONTROL_FUNC pPreProcCmdLine;
	SERVER_CONTROL_FUNC pProcCmdLine;
	SERVER_HELP_FUNC	pHelpInfo;
};

int32_t mg_app_main(int32_t argc, char* argv[],
	        SERVER_APP_FUNC  pInit,
	        SERVER_APP_FUNC  pFini,
	        SERVER_APP_FUNC  pFrame,
	        SERVER_APP_FUNC  pReload,
	        SERVER_APP_FUNC  pStop = NULL,
	        SERVER_APP_FUNC  pProc = NULL,
	        CONTROL_FUNCS*   pControl = NULL,
			BOOL			 bUseTconnd = FALSE, 
			BOOL			 bBasicMode = FALSE);

BOOL mg_set_user_msg_handler(int32_t nServerType, USER_MSG_HANDLER pMsgHandler);
BOOL mg_set_state(int32_t nState);
BOOL mg_set_stop_timer(uint64_t qwServerTick);
BOOL mg_reload(void);

int32_t mg_get_state(void);
int32_t mg_get_stop_timer(void);
int32_t mg_get_tbus_addr(void);
int32_t mg_get_game_frame(void);

#endif