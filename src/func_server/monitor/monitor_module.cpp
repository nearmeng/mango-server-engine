#include "stdafx.h"
#include "monitor_module.h"

MG_REGISTER_MODULE(CMonitorModule, mtWeb, "http://appletuan.com/quotes/1");

BOOL CMonitorModule::init(BOOL bResume)
{
    int nRetCode = 0;

    nRetCode = CCurlClient::instance().init();
    LOG_PROCESS_ERROR(nRetCode == 0);
    
    nRetCode = query_web_data("https://appletuan.com/quotes/1");
    //nRetCode = query_web_data("https://baidu.com");
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMonitorModule::uninit(void)
{
    int nRetCode = 0;

    nRetCode = CCurlClient::instance().fini();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CMonitorModule::set_create_arg(const char* pcszModuleName, va_list args)
{
    MONITOR_TARGET target;
    int nTargetType = 0;
    char* pszTargetString = NULL;

	nTargetType = (int32_t)(va_arg(args, int32_t));
    LOG_PROCESS_ERROR(nTargetType > mtInvalid && nTargetType < mtTotal);

    pszTargetString = va_arg(args, char*);
    LOG_PROCESS_ERROR(pszTargetString);

    target.nMonitorType = nTargetType;
    target.sMonitorData = pszTargetString;
    target.nLastRecordTime = 0;

    m_MonitorTargetVec.push_back(target);

    return TRUE;
Exit0:
    return FALSE;
}

void CMonitorModule::on_frame()
{
    CCurlClient::instance().mainloop();
    return ;
}
    
BOOL CMonitorModule::query_web_data(const char* pWebData)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pWebData);

    nRetCode = CCurlClient::instance().set_host(pWebData);
    LOG_PROCESS_ERROR(nRetCode == 0);
    
    nRetCode = CCurlClient::instance().get(NULL, CMonitorModule::on_query_web_data, NULL, NULL, NULL, 0);
    LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CMonitorModule::on_query_web_data(int ret, const std::string& response, const void *cb_data, int cb_len)
{
    INF("query web data, ret %d, get %s", ret, response.c_str());

    return TRUE;
}