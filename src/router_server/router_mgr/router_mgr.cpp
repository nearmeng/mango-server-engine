#include "stdafx.h"
#include "router_mgr.h"

CRouterMgr CRouterMgr::ms_Instance;

BOOL CRouterMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;
		
	nRetCode = m_ShmServiceData.init(stdRouterServiceData, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	if (!bResume)
	{
		m_pServiceData = m_ShmServiceData.get_obj();
		LOG_PROCESS_ERROR(m_pServiceData);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterMgr::uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterMgr::add_server_info(SERVER_INFO stServerInfo)
{
	int32_t nRetCode = 0;

	nRetCode = m_pServiceData->add_server_node(stServerInfo);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterMgr::del_server_info(int32_t nServerAddr)
{
	int32_t nRetCode = 0;

	nRetCode = m_pServiceData->del_server_node(nServerAddr);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CRouterMgr::get_router_by_server_addr(int32_t nServerAddr)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL;

	pServerInfo = get_server_info_by_addr(nServerAddr);
	LOG_PROCESS_ERROR(pServerInfo);

	return pServerInfo->nRouterAddr;
Exit0:
	return 0;
}
