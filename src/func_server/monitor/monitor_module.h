#ifndef _MONITOR_MODULE_H_
#define _MONITOR_MODULE_H_

#include "module/server_module.h"

enum MONITOR_TYPE
{
    mtInvalid,
    mtWeb,
    mtTotal
};

struct MONITOR_TARGET
{
    int32_t     nMonitorType;
    std::string sMonitorData;
    int32_t     nLastRecordTime;
};

class CMonitorModule : public CServerModule
{
public:
    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);
    virtual BOOL set_create_arg(const char* pcszModuleName, va_list args);
    
    virtual void on_frame(void);

    BOOL query_web_data(const char* pWebData);

private:
    static BOOL on_query_web_data(int ret, const std::string& response, const void *cb_data, int cb_len);

private:
    std::vector<MONITOR_TARGET>     m_MonitorTargetVec;
};

#endif