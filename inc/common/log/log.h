#ifndef  _LOG_H_
#define  _LOG_H_

#include "tlog/tlog.h"

extern LPTLOGCATEGORYINST g_pSvrLogCat;
extern LPTLOGCATEGORYINST g_pOssLogCat;

#ifdef __linux__
	#ifndef _DEBUG
		#define CRI(__fmt__, args...) 	tlog_crit(g_pSvrLogCat, 0, 0, __fmt__, ##args)
		#define WRN(__fmt__, args...) 	tlog_warn(g_pSvrLogCat, 0, 0, __fmt__, ##args)
		#define ERR(__fmt__, args...)   tlog_error(g_pSvrLogCat, 0, 0, __fmt__, ##args)
		#define INF(__fmt__, args...)   tlog_info(g_pSvrLogCat, 0, 0, __fmt__, ##args)
		#define DBG(__fmt__, args...)   tlog_debug(g_pSvrLogCat, 0, 0, __fmt__, ##args)
	#else
		extern char g_szLogMessage[4096];
		extern bool g_bNeedConsole;
		void set_need_console(bool bNeedConsole);

		#define CONSOLE_LOG(__priority__, __fmt__, args...)						\
				do																\
				{																\
					__priority__(g_pSvrLogCat, 0, 0, __fmt__, ##args);			\
					if (g_bNeedConsole)                                         \
					{                                                           \
						snprintf(g_szLogMessage, 1024, __fmt__, ##args);		\
						strxcat(g_szLogMessage, "\n", 1024);					\
						printf(g_szLogMessage);									\
					}                                                          \
				} while (0)
		#define CRI(__fmt__, args...)		CONSOLE_LOG(tlog_crit, __fmt__, ##args)
		#define ERR(__fmt__, args...)		CONSOLE_LOG(tlog_error, __fmt__, ##args)
		#define WRN(__fmt__, args...)		CONSOLE_LOG(tlog_warn, __fmt__, ##args)
		#define INF(__fmt__, args...)		CONSOLE_LOG(tlog_info, __fmt__, ##args)
		#define DBG(__fmt__, args...)		CONSOLE_LOG(tlog_debug, __fmt__, ##args)
	#endif
#else
	void init_console_window(void);

	#define CC_CRI		(FOREGROUND_RED)
	#define CC_ERR		(FOREGROUND_BLUE | FOREGROUND_RED)
	#define CC_WRN		(FOREGROUND_GREEN | FOREGROUND_RED)
	#define CC_INF		(FOREGROUND_GREEN)
	#define CC_DBG		(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED)

	#include <windows.h>
	extern HANDLE g_hConsole;
	extern char g_szLogMessage[4096];
	#define CONSOLE_LOG(__priority__, __color__, __fmt__, ...)						\
				do																	\
				{																	\
					__priority__(g_pSvrLogCat, 0, 0, __fmt__, __VA_ARGS__);			\
					snprintf(g_szLogMessage, 1024, __fmt__, __VA_ARGS__);			\
					strxcat(g_szLogMessage, "\n", 1024);							\
					SetConsoleTextAttribute(g_hConsole, __color__);					\
					printf(g_szLogMessage);											\
					OutputDebugString(g_szLogMessage);								\
				} while (0)

		#define CRI(__fmt__, ...)		CONSOLE_LOG(tlog_crit, CC_CRI, __fmt__, __VA_ARGS__)
		#define ERR(__fmt__, ...)		CONSOLE_LOG(tlog_error, CC_ERR, __fmt__, __VA_ARGS__)
		#define WRN(__fmt__, ...)		CONSOLE_LOG(tlog_warn, CC_WRN, __fmt__, __VA_ARGS__)
		#define INF(__fmt__, ...)		CONSOLE_LOG(tlog_info, CC_INF, __fmt__, __VA_ARGS__)
		#define DBG(__fmt__, ...)		CONSOLE_LOG(tlog_debug, CC_DBG, __fmt__, __VA_ARGS__)
#endif

#endif   /* ----- #ifndef _LOG_H_  ----- */

