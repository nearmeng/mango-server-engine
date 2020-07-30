#include "stdafx.h"
#include "alive_mgr.h"

#include "tbus/tbus_wrapper.h"
#include "config/global_config.h"
#include "message/rs_message_handler.h"
#include "time/time_mgr.h"

CAliveMgr CAliveMgr::ms_Instance;

BOOL CAliveMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_ShmAliveData.init(stdRouterAliveData, bResume);
	LOG_PROCESS_ERROR(nRetCode);
		
	m_pAliveData = m_ShmAliveData.get_obj();
	LOG_PROCESS_ERROR(m_pAliveData);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CAliveMgr::uninit(void)
{
	return TRUE;
}

void CAliveMgr::mainloop()
{
	int32_t nRetCode = 0;
	int32_t nCurrTime = CTimeMgr::instance().get_time_sec();

	for (int32_t i = 0; i < m_pAliveData->nServerCount; i++)
	{
		SERVER_ALIVE_INFO& rServerInfo = m_pAliveData->ServerAliveInfo[i];
		if (rServerInfo.bAlive && nCurrTime > rServerInfo.dwLastHeartBeatTime + g_ServerConfig.SC.nHeartBeatTimeOut / 1000)
		{
			rServerInfo.bAlive = FALSE;
			CRI("[router network]: server node %s check is not alive.", tbus_get_str(rServerInfo.nTbusAddr));
		}

		if (!rServerInfo.bAlive && nCurrTime < rServerInfo.dwLastHeartBeatTime + g_ServerConfig.SC.nHeartBeatTimeOut / 1000)
		{
			rServerInfo.bAlive = TRUE;
			INF("[router network]: server node %s check is alive now.", tbus_get_str(rServerInfo.nTbusAddr));
		}
	}
	
	if (nCurrTime > m_pAliveData->dwLastReportTime + g_ServerConfig.SC.nRouterReportInterval / 1000 &&
			m_pAliveData->nServerCount >= CRSMessageHandler::instance().get_init_server_node_count())
	{
		nRetCode = CRSMessageHandler::instance().do_alive_report();
		LOG_CHECK_ERROR(nRetCode);

		m_pAliveData->dwLastReportTime = nCurrTime;
	}

	/*
	if (m_pAliveData->nServerCount >= CRSMessageHandler::instance().get_init_server_node_count())
	{
		WRN("curr register server is less than init count, some server node is down when init!");
	}
	*/

Exit0:
	return;
}
	
SERVER_ALIVE_INFO* CAliveMgr::get_server_alive_info(int32_t nTbusAddr)
{
	for (int32_t i = 0; i < m_pAliveData->nServerCount; i++)
	{
		if(m_pAliveData->ServerAliveInfo[i].nTbusAddr == nTbusAddr)
			return &(m_pAliveData->ServerAliveInfo[i]);
	}

Exit0:
	return NULL;
}
	
BOOL CAliveMgr::add_server_alive_info(int32_t nTbusAddr)
{
	int32_t nRetCode = 0;
	SERVER_ALIVE_INFO* pAliveInfo = NULL;
	int32_t nCurrTime = CTimeMgr::instance().get_time_sec();
	
	pAliveInfo = get_server_alive_info(nTbusAddr);
	if (pAliveInfo == NULL)
	{
		m_pAliveData->ServerAliveInfo[m_pAliveData->nServerCount].nTbusAddr = nTbusAddr;
		m_pAliveData->ServerAliveInfo[m_pAliveData->nServerCount].bAlive = TRUE;
		m_pAliveData->ServerAliveInfo[m_pAliveData->nServerCount].nLoadValue = 0;
		m_pAliveData->ServerAliveInfo[m_pAliveData->nServerCount].dwLastHeartBeatTime = nCurrTime;
		m_pAliveData->nServerCount++;
	}
	else
	{
		pAliveInfo->nLoadValue = 0;
		pAliveInfo->dwLastHeartBeatTime = nCurrTime;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAliveMgr::del_server_alive_info(int32_t nTbusAddr)
{
	int32_t nRetCode = 0;

	for (int32_t i = 0; i < m_pAliveData->nServerCount; i++)
	{
		if (m_pAliveData->ServerAliveInfo[i].nTbusAddr == nTbusAddr)
		{
			for (int32_t j = i + 1; j < m_pAliveData->nServerCount; j++)
			{
				m_pAliveData->ServerAliveInfo[j - 1] = m_pAliveData->ServerAliveInfo[j];
			}
			m_pAliveData->nServerCount--;
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}
	
int32_t CAliveMgr::get_server_count(int32_t nServiceType)
{
	int32_t nCount = 0;
	for (int32_t i = 0; i < m_pAliveData->nServerCount; i++)
	{
		if (tbus_get_type(m_pAliveData->ServerAliveInfo[i].nTbusAddr) == nServiceType)
		{
			nCount++;
		}
	}
	return nCount;
}
	
int32_t CAliveMgr::get_alive_server_count()
{
	int32_t nCount = 0;
	for (int32_t i = 0; i < m_pAliveData->nServerCount; i++)
	{
		if (m_pAliveData->ServerAliveInfo[i].bAlive)
		{
			nCount++;
		}
	}
	return nCount;
}