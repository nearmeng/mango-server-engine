#ifndef  _LOG_H_
#define  _LOG_H_

#include "tlog/tlog.h"

extern LPTLOGCATEGORYINST g_pLogCategory;

#ifdef __linux__
	#define CRI(__fmt__, args...) 	tlog_crit(g_pLogCategory, 0, 0, __fmt__, ##args)
	#define WRN(__fmt__, args...) 	tlog_warn(g_pLogCategory, 0, 0, __fmt__, ##args)
	#define ERR(__fmt__, args...)   tlog_error(g_pLogCategory, 0, 0, __fmt__, ##args)
	#define INF(__fmt__, args...)   tlog_info(g_pLogCategory, 0, 0, __fmt__, ##args)
	#define DBG(__fmt__, args...)   tlog_debug(g_pLogCategory, 0, 0, __fmt__, ##args)
#else
	#define CRI(__fmt__, ...) 	tlog_crit(g_pLogCategory, 0, 0, __fmt__, __VA_ARGS__)
	#define WRN(__fmt__, ...) 	tlog_warn(g_pLogCategory, 0, 0, __fmt__, __VA_ARGS__)
	#define ERR(__fmt__, ...)   tlog_error(g_pLogCategory, 0, 0, __fmt__, __VA_ARGS__)
	#define INF(__fmt__, ...)   tlog_info(g_pLogCategory, 0, 0, __fmt__, __VA_ARGS__)
	#define DBG(__fmt__, ...)   tlog_debug(g_pLogCategory, 0, 0, __fmt__, __VA_ARGS__)
#endif

#endif   /* ----- #ifndef _LOG_H_  ----- */

