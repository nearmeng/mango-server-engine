#include "stdafx.h"
#include "sms_trans.h"

#include "message/sms_message_handler.h"
#include "service_mgr/service_mgr.h"

CSmsTransMgr CSmsTransMgr::ms_Instance;

BOOL CSmsTransMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	m_pShmTransData = NULL;

	nRetCode = m_ShmTransData.init(stdSMSTrans, bResume);
	LOG_PROCESS_ERROR(nRetCode);
		
	m_pShmTransData = m_ShmTransData.get_obj();
	LOG_PROCESS_ERROR(m_pShmTransData);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::uninit(void)
{
	int32_t nRetCode = 0;
	
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::add_trans_cmd(SMS_TRANS_CMD stCmd)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(m_pShmTransData);

	m_pShmTransData->CmdList.push_back(stCmd);

	INF("[trans]: add trans cmd, event_type %d service_type %d server_addr %s param %d", stCmd.nEventType, stCmd.nServiceType,
					tbus_get_str(stCmd.nServerAddr), stCmd.nParam);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::clear_trans_cmd(void)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(m_pShmTransData);

	m_pShmTransData->CmdList.clear();

	INF("[trans]: clear trans cmd");

	return TRUE;
Exit0:
	return FALSE;
}

SMS_TRANS_CMD* CSmsTransMgr::get_trans_front_cmd(void)
{
	LOG_PROCESS_ERROR(m_pShmTransData);
	return m_pShmTransData->CmdList.front();
Exit0:
	return NULL;
}

BOOL CSmsTransMgr::pop_trans_front_cmd(void)
{
	LOG_PROCESS_ERROR(m_pShmTransData);

	m_pShmTransData->CmdList.pop_front();
	m_pShmTransData->bInProcess = FALSE;

	INF("trans size is %d", m_pShmTransData->CmdList.size());

	return TRUE;
Exit0:
	return FALSE;
}

void CSmsTransMgr::mainloop(void)
{
	int32_t nRetCode = 0;
	const SMS_TRANS_CMD* pTransCmd = NULL;

	LOG_PROCESS_ERROR(m_pShmTransData);

	if (m_pShmTransData->CmdList.empty() || m_pShmTransData->bInProcess)
		return;

	pTransCmd = m_pShmTransData->CmdList.front();
	LOG_PROCESS_ERROR(pTransCmd);
	
	m_pShmTransData->bInProcess = TRUE;

	nRetCode = _process_cmd(pTransCmd);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}


BOOL CSmsTransMgr::_process_cmd(const SMS_TRANS_CMD* pCmd)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pCmd);

	switch (pCmd->nEventType)
	{
	case setServerDown:
	{
		nRetCode = _process_cmd_node_down(pCmd);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case setServerRecover:
	{
		nRetCode = _process_cmd_node_recover(pCmd);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case setServerExpand:
	{
		nRetCode = _process_cmd_node_expand(pCmd);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case setServerReduce:
	{
		nRetCode = _process_cmd_node_reduce(pCmd);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSmsTransMgr::_process_cmd_node_down(const SMS_TRANS_CMD* pCmd)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL;

	LOG_PROCESS_ERROR(pCmd);

	pServerInfo = CServiceMgr::get_instance().get_server_info_by_addr(pCmd->nServerAddr);
	LOG_PROCESS_ERROR(pServerInfo);

	pServerInfo->nState = svstTransfer;

	m_pShmTransData->nRouterReportCount = 0;
	nRetCode = CSMSMessageHandler::instance().do_ntf_server_event(setServerDown, pCmd->nServerAddr, sopExecute);
	LOG_PROCESS_ERROR(nRetCode);

	INF("[trans]: node %s is down, goto execute on router", tbus_get_str(pCmd->nServerAddr));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::_process_cmd_node_recover(const SMS_TRANS_CMD* pCmd)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL;

	LOG_PROCESS_ERROR(pCmd);
	
	pServerInfo = CServiceMgr::get_instance().get_server_info_by_addr(pCmd->nServerAddr);
	LOG_PROCESS_ERROR(pServerInfo);

	pServerInfo->nState = svstTransfer;

	m_pShmTransData->nRouterReportCount = 0;
	nRetCode = CSMSMessageHandler::instance().do_ntf_server_event(setServerRecover, pCmd->nServerAddr, sopExecute);
	LOG_PROCESS_ERROR(nRetCode);
	
	INF("[trans]: node %s is realive, goto execute on router", tbus_get_str(pCmd->nServerAddr));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::_process_cmd_node_expand(const SMS_TRANS_CMD* pCmd)
{
	int32_t nRetCode = 0;
	int32_t nEventParam = 0;
	SERVICE_INFO* pServiceInfo = NULL;

	LOG_PROCESS_ERROR(pCmd);

	nEventParam = (pCmd->nServiceType << 16) + pCmd->nParam;

	pServiceInfo = CServiceMgr::get_instance().get_service_info(pCmd->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);
	LOG_PROCESS_ERROR(pCmd->nParam > pServiceInfo->nServiceServerCount);
	LOG_PROCESS_ERROR(pCmd->nParam <= pServiceInfo->nServerCount);

	pServiceInfo->nState = svcstTransfer;

	for (int32_t i = 0; i < pServiceInfo->nServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo)
		{
			pServerInfo->nState = svstTransfer;
		}
	}

	m_pShmTransData->nRouterReportCount = 0;
	nRetCode = CSMSMessageHandler::instance().do_ntf_server_event(setServerExpand, nEventParam, sopPrepare);
	LOG_PROCESS_ERROR(nRetCode);
	
	INF("[trans]: service %d is expand to %d, goto execute on router", pCmd->nServiceType, pCmd->nParam);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSmsTransMgr::_process_cmd_node_reduce(const SMS_TRANS_CMD* pCmd)
{
	int32_t nRetCode = 0;
	int32_t nEventParam = 0;
	SERVICE_INFO* pServiceInfo = NULL;

	LOG_PROCESS_ERROR(pCmd);

	nEventParam = (pCmd->nServiceType << 16) + pCmd->nParam;
	
	pServiceInfo = CServiceMgr::get_instance().get_service_info(pCmd->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);
	LOG_PROCESS_ERROR(pCmd->nParam > 0);
	LOG_PROCESS_ERROR(pCmd->nParam < pServiceInfo->nServiceServerCount);

	pServiceInfo->nState = svcstTransfer;

	for (int32_t i = 0; i < pServiceInfo->nServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo)
		{
			pServerInfo->nState = svstTransfer;
		}
	}

	m_pShmTransData->nRouterReportCount = 0;
	nRetCode = CSMSMessageHandler::instance().do_ntf_server_event(setServerReduce, nEventParam, sopPrepare);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}