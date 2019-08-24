#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

typedef BOOL(*SERVER_APP_FUNC)(BOOL bIsResume);

int32_t mg_app_main(int32_t argc, char* argv[],
			SERVER_APP_FUNC pInit,
			SERVER_APP_FUNC pFini,
			SERVER_APP_FUNC pProc,
			SERVER_APP_FUNC pReload,
			SERVER_APP_FUNC pStop = NULL,
			SERVER_APP_FUNC pTick = NULL,
			BOOL			bUseTconnd = FALSE);


#endif