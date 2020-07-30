#include "stdafx.h"
#include "service_mgr.h"

#include "config/global_config.h"
#include "message/sms_message_handler.h"
#include "trans/sms_trans.h"

#include "time/time_mgr.h"
#include "app/server_app.h"

CServiceMgr CServiceMgr::ms_Instance;
static char g_szPackBuff[256 * 1024];

BOOL CServiceMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_ShmServiceData.init(stdSMSServiceData, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ShmRouterMgr.init(stdSMSRouterMgr, bResume);
	LOG_PROCESS_ERROR(nRetCode);
		
	m_pShmServiceData = m_ShmServiceData.get_obj();
	LOG_PROCESS_ERROR(m_pShmServiceData);

	m_pShmRouterMgr = m_ShmRouterMgr.get_obj();
	LOG_PROCESS_ERROR(m_pShmRouterMgr);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CServiceMgr::uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CServiceMgr::add_router(int32_t nRouterAddr)
{
	int32_t nRetCode = 0;
	int32_t nInstID = tbus_get_inst(nRouterAddr);

	LOG_PROCESS_ERROR(m_pShmRouterMgr);
	LOG_PROCESS_ERROR(nInstID > 0 && nInstID < MAX_SERVER_COUNT_PER_SERVICE);
		
	if (m_pShmRouterMgr->nRouterCount < nInstID)
	{
		//expand service info if the server inst is too large
		for (int32_t nIndex = m_pShmRouterMgr->nRouterCount; nIndex < nInstID; nIndex++)
		{
			m_pShmRouterMgr->RouterInfo[nIndex].nTbusAddr = 0;
			m_pShmRouterMgr->RouterInfo[nIndex].dwLastReportTime = 0;
		}
		m_pShmRouterMgr->nRouterCount = nInstID;
	}

	m_pShmRouterMgr->RouterInfo[nInstID - 1].nTbusAddr = nRouterAddr;
	m_pShmRouterMgr->RouterInfo[nInstID - 1].dwLastReportTime = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CServiceMgr::del_router(int32_t nRouterAddr)
{
	int32_t nRetCode = 0;
	int32_t nInstID = tbus_get_inst(nRouterAddr);

	m_pShmRouterMgr->RouterInfo[nInstID - 1].nTbusAddr = 0;
	m_pShmRouterMgr->RouterInfo[nInstID - 1].dwLastReportTime = 0;

	if (nInstID == m_pShmRouterMgr->nRouterCount)
		m_pShmRouterMgr->nRouterCount--;

	return TRUE;
Exit0:
	return FALSE;
}

SMS_ROUTER_INFO* CServiceMgr::get_router(int32_t nRouterAddr)
{
	int32_t nInstID = tbus_get_inst(nRouterAddr);
	PROCESS_ERROR(m_pShmRouterMgr->nRouterCount > 0);
	LOG_PROCESS_ERROR(nInstID > 0 && nInstID <= m_pShmRouterMgr->nRouterCount);

	return &(m_pShmRouterMgr->RouterInfo[nInstID - 1]);
Exit0:
	//CRI("invalid router inst id %d", nInstID);
	return NULL;
}
	
SMS_ROUTER_INFO* CServiceMgr::get_rand_router(void)
{
	int32_t nRandIndex = 0;
	LOG_PROCESS_ERROR(m_pShmRouterMgr);

	nRandIndex = rand_ex(m_pShmRouterMgr->nRouterCount);
	return &(m_pShmRouterMgr->RouterInfo[nRandIndex]);

Exit0:
	return NULL;
}
	
int32_t CServiceMgr::get_reported_router_count(void)
{
	int32_t nCount = 0;
	for (int32_t i = 0; i < m_pShmRouterMgr->nRouterCount; i++)
	{
		if (m_pShmRouterMgr->RouterInfo[i].dwLastReportTime > 0)
			nCount++;
	}
	return nCount;
}
	
BOOL CServiceMgr::pack_service_data(char** ppszData, int32_t &nDataSize)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(ppszData);
	LOG_PROCESS_ERROR(sizeof(FULL_SERVICE_DATA) + sizeof(SMS_ROUTER_MGR) < sizeof(g_szPackBuff));

	memcpy(g_szPackBuff, m_pShmServiceData, sizeof(FULL_SERVICE_DATA));
	memcpy(g_szPackBuff + sizeof(FULL_SERVICE_DATA), m_pShmRouterMgr, sizeof(SMS_ROUTER_MGR));

	*ppszData = (char*)g_szPackBuff;
	nDataSize = sizeof(FULL_SERVICE_DATA) + sizeof(SMS_ROUTER_MGR);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CServiceMgr::set_service_data(const char* pszData, int32_t nDataSize)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pszData);
	LOG_PROCESS_ERROR(nDataSize == sizeof(FULL_SERVICE_DATA) + sizeof(SMS_ROUTER_MGR));

	memcpy(m_pShmServiceData, pszData, sizeof(FULL_SERVICE_DATA));
	memcpy(m_pShmRouterMgr, pszData + sizeof(FULL_SERVICE_DATA), sizeof(SMS_ROUTER_MGR));

	return TRUE;
Exit0:
	return FALSE;
}
	
void CServiceMgr::mainloop(void)
{
	int32_t nRetCode = 0;
	int32_t nCurrTime = CTimeMgr::instance().get_time_sec();

	LOG_PROCESS_ERROR(m_pShmRouterMgr);

	for (int32_t i = 0; i < m_pShmRouterMgr->nRouterCount; i++)
	{
		SMS_ROUTER_INFO& rRouterInfo = m_pShmRouterMgr->RouterInfo[i];
		if (nCurrTime > rRouterInfo.dwLastPingTime + g_ServerConfig.SC.nHeartBeatTimeOut / 1000 && 
			rRouterInfo.nState != svstDown)
		{
			rRouterInfo.dwCheckDownFlag |= (1 << tbus_get_inst(CMGApp::instance().get_tbus_addr()));

			if (nCurrTime > rRouterInfo.dwLastDownReportTime + g_ServerConfig.SC.nRouterDownReportTimeInterval / 1000)
			{
				rRouterInfo.dwLastDownReportTime = nCurrTime;

				nRetCode = CSMSMessageHandler::instance().do_ntf_router_down(rRouterInfo.nTbusAddr);
				LOG_CHECK_ERROR(nRetCode);
			}
		}
	}

Exit0:
	return;
}
	
BOOL CServiceMgr::router_down_process(int32_t nReportMgrAddr, int32_t nRouterAddr)
{
	int32_t nRetCode = 0;
	BOOL bAllCheckDown = TRUE;
	SMS_ROUTER_INFO* pRouter = NULL;

	pRouter = CServiceMgr::get_instance().get_router(nRouterAddr);
	LOG_PROCESS_ERROR(pRouter);

	pRouter->dwCheckDownFlag |= (1 << tbus_get_inst(nReportMgrAddr));

	for (int32_t i = 0; i < CSMSMessageHandler::instance().get_mgr_count(); i++)
	{
		int32_t nMgrAddr = CSMSMessageHandler::instance().get_mgr_addr(i);
		if ((pRouter->dwCheckDownFlag & (1 << tbus_get_inst(nMgrAddr))) == 0)
		{
			bAllCheckDown = FALSE;
			break;
		}
	}

	if (bAllCheckDown && pRouter->nState != svstDown)
	{
		int32_t nServerCount = CServiceMgr::get_instance().get_server_info_count();

		INF("[service mgr]: router %s is all checked down, begin clean service in this router!", tbus_get_str(pRouter->nTbusAddr));
		pRouter->nState = svstDown;
	
		for (int32_t i = 0; i < nServerCount; i++)
		{
			SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(i);
			LOG_CHECK_ERROR(pServerInfo);

			if (pServerInfo && pServerInfo->nState == svstInService)
			{
				if (!CSmsTransMgr::get_instance().is_server_in_cmd_list(pServerInfo->nTbusAddr))
				{
					SMS_TRANS_CMD cmd;
					cmd.nEventType = setServerDown;
					cmd.nServerAddr = pServerInfo->nTbusAddr;
					cmd.nServiceType = tbus_get_type(pServerInfo->nTbusAddr);

					nRetCode = CSmsTransMgr::get_instance().add_trans_cmd(cmd);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					CRI("router is down, while the server node %s is in cmd list", tbus_get_str(pServerInfo->nTbusAddr));
				}
			}
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}