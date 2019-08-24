#ifndef __TLOG_H__
#define __TLOG_H__

#include "TLogDefine.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void TLog (T_LOG_PRIORITY pri, const char *fmt,...);

#define LogInfo(fmt,...) TLog(TLOG_Pri_Info,fmt,##__VA_ARGS__)
#define LogDebug(fmt,...) TLog(TLOG_Pri_Debug,fmt,##__VA_ARGS__)
#define LogWarning(fmt,...) TLog(TLOG_Pri_Warning,fmt,##__VA_ARGS__)
#define LogError(fmt,...) TLog(TLOG_Pri_Error,fmt,##__VA_ARGS__)
#define LogEvent(fmt,...) TLog(TLOG_Pri_Event,fmt,##__VA_ARGS__)

    const char* GetLogLevelString(T_LOG_PRIORITY pri);
    void SetLogLevel(T_LOG_PRIORITY pri);

#if defined(_WIN32) || defined(_WIN64)
    typedef void (__stdcall *TLogCallback)(T_LOG_PRIORITY pri, const char* msg);
#else
    typedef void (*TLogCallback)(T_LOG_PRIORITY pri, const char* msg);
#endif

    void SetTLogCallback(TLogCallback callback);

    char* GetErrMsg();

#ifdef __cplusplus
}
#endif

#endif
