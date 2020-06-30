#include "stdafx.h"
#include "rs_message_handler.h"

#include "tbus/tbus.h"
#include "time/time_mgr.h"

#include "define/server_def.h"
#include "define/error_def.h"

#include "game_data/global_config.h"
#include "protocol/common_message.h"
#include "protocol/router_message.h"
#include "protocol/service_manage_message.h"

#include "tbus/tbus_wrapper.h"

#include "alive_mgr/alive_mgr.h"
#include "object_mgr/rs_object_mgr.h"
#include "router_mgr/router_mgr.h"
#include "hash/hash.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"
#include <set>

#define MAX_RECV_PER_LOOP	(1024 * 4)

CRSMessageHandler CRSMessageHandler::ms_Instance;
static BOOL bInEventProcess = FALSE;

BOOL CRSMessageHandler::_init_msgid(void)
{
	int32_t nRetCode = 0;

    m_InternalMessageFuncList[a2a_register] = &CRSMessageHandler::on_register;
	m_InternalMessageFuncList[a2a_unregister] = &CRSMessageHandler::on_unregister;
	m_InternalMessageFuncList[a2a_ping] = &CRSMessageHandler::on_heartbeat;

	m_InternalMessageFuncList[router_load_report] = &CRSMessageHandler::on_router_load_report;
	m_InternalMessageFuncList[router_send_by_routerid] = &CRSMessageHandler::on_router_send_by_routerid;
	m_InternalMessageFuncList[router_send_by_service_type] = &CRSMessageHandler::on_router_send_by_service_type;
	m_InternalMessageFuncList[router_send_by_service_inst] = &CRSMessageHandler::on_router_send_by_service_inst;
	m_InternalMessageFuncList[router_send_by_addr] = &CRSMessageHandler::on_router_send_by_addr;
	m_InternalMessageFuncList[router_obj_report] = &CRSMessageHandler::on_router_obj_report;
	m_InternalMessageFuncList[router_send_by_objid] = &CRSMessageHandler::on_router_send_by_objid;
	m_InternalMessageFuncList[router_send_by_load] = &CRSMessageHandler::on_router_send_by_load;
	m_InternalMessageFuncList[router_send_to_mgr] = &CRSMessageHandler::on_router_send_to_mgr;
	m_InternalMessageFuncList[router_server_event_ack] = &CRSMessageHandler::on_router_server_event_ack;
	m_InternalMessageFuncList[router_ntf_msg_clear_ack] = &CRSMessageHandler::on_router_ntf_msg_clear_ack;
	m_InternalMessageFuncList[router_get_service_info] = &CRSMessageHandler::on_router_get_service_info;

	m_InternalMessageFuncList[service_mgr_sync_data_begin] = &CRSMessageHandler::on_service_mgr_sync_data_begin;
	m_InternalMessageFuncList[service_mgr_sync_server_info] = &CRSMessageHandler::on_service_mgr_sync_server_info;
	m_InternalMessageFuncList[service_mgr_sync_obj] = &CRSMessageHandler::on_service_mgr_sync_obj;
	m_InternalMessageFuncList[service_mgr_sync_data_end] = &CRSMessageHandler::on_service_mgr_sync_data_end;
	m_InternalMessageFuncList[service_mgr_obj_ntf] = &CRSMessageHandler::on_service_mgr_obj_ntf;
	m_InternalMessageFuncList[service_mgr_server_info_ntf] = &CRSMessageHandler::on_service_mgr_server_info_ntf;
	m_InternalMessageFuncList[service_mgr_in_service] = &CRSMessageHandler::on_service_mgr_in_service;
	m_InternalMessageFuncList[service_mgr_end_service] = &CRSMessageHandler::on_service_mgr_end_service;
	m_InternalMessageFuncList[service_mgr_ntf_server_event] = &CRSMessageHandler::on_service_mgr_ntf_server_event;
	m_InternalMessageFuncList[service_mgr_ntf_mgr_leader] = &CRSMessageHandler::on_service_mgr_ntf_mgr_leader;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::init(int32_t nTbusHandle, int32_t nTbusAddr, BOOL bResume)
{
	int32_t nRetCode = 0;
	int32_t nPeerCount = 0;
    int32_t* pnAddrList = NULL;
	
	m_pShmServiceMgrData = NULL;
	m_nTbusAddr = nTbusAddr;
	m_nTbusHandle = nTbusHandle;

	nRetCode = _init_msgid();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ShmMsgQueue.init(bResume);
	LOG_PROCESS_ERROR(nRetCode);
	
	nRetCode = m_ShmServiceMgrData.init(stdRouterServiceMgr, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	if (!bResume)
	{
		m_pShmServiceMgrData = m_ShmServiceMgrData.get_obj();
		LOG_PROCESS_ERROR(m_pShmServiceMgrData);

		nRetCode = tbus_get_peer_count(m_nTbusHandle, m_nTbusAddr, &nPeerCount);
		LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

		pnAddrList = (int32_t*)alloca(sizeof(int32_t) * nPeerCount);
		LOG_PROCESS_ERROR(pnAddrList);

		nRetCode = tbus_get_peer_addr(m_nTbusHandle, m_nTbusAddr, pnAddrList, &nPeerCount);
		LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

		for (int32_t nIndex = 0; nIndex < nPeerCount; nIndex++)
		{
			int32_t nServerType = tbus_get_type(pnAddrList[nIndex]);
			int32_t nServerIndex = tbus_get_inst(pnAddrList[nIndex]);
			if (nServerType == svrServiceMgr)
			{
				m_pShmServiceMgrData->nMgrAddr[m_pShmServiceMgrData->nMgrCount++] = pnAddrList[nIndex];
				INF("find service mgr %s", tbus_get_str(pnAddrList[nIndex]));
			}
			else if (nServerType == svrRouter)
			{
			}
			else
			{
				INF("init server node %s", tbus_get_str(pnAddrList[nIndex]));
				m_pShmServiceMgrData->nInitServerNodeCount++;
			}
		}

		for (int32_t i = 0; i < m_pShmServiceMgrData->nMgrCount; i++)
		{
			nRetCode = do_register(m_pShmServiceMgrData->nMgrAddr[i]);
			LOG_CHECK_ERROR(nRetCode);
		}
		
		INF("[router network]: router inited!");
	}
	else
	{
		INF("[router network]: router resumed!");
	}

	return TRUE;
Exit0:
    return FALSE;
}

BOOL CRSMessageHandler::uninit(void)
{
	int32_t nRetCode = 0;

	nRetCode = m_ShmMsgQueue.uninit();
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::msg_handler(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	INTERNAL_MESSAGE_HEADER* pHeader = (INTERNAL_MESSAGE_HEADER*)pBuffer;

	LOG_PROCESS_ERROR(pHeader->wMsg >= internal_message_begin &&
		pHeader->wMsg <= internal_message_end);

	LOG_PROCESS_ERROR(ms_Instance.m_InternalMessageFuncList[pHeader->wMsg]);

	{
		MESSAGE_FUNC pMsgHandle = ms_Instance.m_InternalMessageFuncList[pHeader->wMsg];
		((&ms_Instance)->*pMsgHandle)(pBuffer, dwSize, nSrcAddr);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::mainloop(void)
{
	int32_t nRetCode = 0;
	int32_t nCurTime = CTimeMgr::instance().get_time_sec();
	static char s_szMessageData[MAX_INTERNAL_MESSAGE_SIZE * 5];

	while (CMGApp::instance().get_state() == svstInService && !bInEventProcess && m_ShmMsgQueue.has_msg())
	{
		int32_t nSrcAddr = 0;
		size_t dwSize = 0;

		nRetCode = m_ShmMsgQueue.pop_msg(nSrcAddr, s_szMessageData, dwSize);
		LOG_CHECK_ERROR(nRetCode);

		INF("[router network]: begin to consume cached msg");

		if (nRetCode)
		{
			INTERNAL_MESSAGE_HEADER* pHeader = (INTERNAL_MESSAGE_HEADER*)s_szMessageData;
			LOG_CHECK_ERROR(pHeader->wMsg >= internal_message_begin &&
				pHeader->wMsg <= internal_message_end);
			LOG_CHECK_ERROR(m_InternalMessageFuncList[pHeader->wMsg]);
			
			if(pHeader->wMsg >= internal_message_begin && pHeader->wMsg <= internal_message_end && m_InternalMessageFuncList[pHeader->wMsg])
				(this->*m_InternalMessageFuncList[pHeader->wMsg])(s_szMessageData, dwSize, nSrcAddr);
		}
	}

	if (nCurTime > m_pShmServiceMgrData->dwLastHeartbeatTime + g_ServerConfig.SC.nRouterHeartBeatInterval / 1000)
	{
		do_heartbeat();
		m_pShmServiceMgrData->dwLastHeartbeatTime = nCurTime;
	}

    return TRUE;
}

BOOL CRSMessageHandler::send(int32_t nDstAddr, const void* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;

    nRetCode = tbus_send(m_nTbusHandle, &m_nTbusAddr, &nDstAddr, pBuffer, dwSize, 0);
    LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

    return TRUE;
Exit0:
	CRI("[router network]: failed to send to %s by tbus", tbus_get_str(nDstAddr));
    return FALSE;
}

BOOL CRSMessageHandler::send(int32_t nDstAddr, const void* pHeader, size_t dwHeaderSize, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	struct iovec vecs[2];

	LOG_PROCESS_ERROR(pHeader);
	LOG_PROCESS_ERROR(pBuffer);

	vecs[0].iov_base = (void*)pHeader;
	vecs[0].iov_len = dwHeaderSize;
	vecs[0].iov_base = (void*)pBuffer;
	vecs[0].iov_len = dwSize;

	nRetCode = tbus_sendv(m_nTbusHandle, &m_nTbusAddr, &nDstAddr, vecs, 2, 0);
	LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	return TRUE;
Exit0:
	return FALSE;
}
	
	
BOOL CRSMessageHandler::broadcast_to_local_server(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	ALIVE_DATA* pAliveData = NULL;

	pAliveData = CAliveMgr::get_instance().get_alive_data();
	LOG_PROCESS_ERROR(pAliveData);
	
	for (int32_t i = 0; i < pAliveData->nServerCount; i++)
	{
		SERVER_ALIVE_INFO& rAliveInfo = pAliveData->ServerAliveInfo[i];

		nRetCode = send(rAliveInfo.nTbusAddr, pBuffer, dwSize);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::broadcast_to_local_server(int32_t nServiceType, const void * pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	ALIVE_DATA* pAliveData = NULL;

	pAliveData = CAliveMgr::get_instance().get_alive_data();
	LOG_PROCESS_ERROR(pAliveData);
	
	for (int32_t i = 0; i < pAliveData->nServerCount; i++)
	{
		SERVER_ALIVE_INFO& rAliveInfo = pAliveData->ServerAliveInfo[i];
		if (tbus_get_type(rAliveInfo.nTbusAddr) == nServiceType)
		{
			nRetCode = send(rAliveInfo.nTbusAddr, pBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::send_to_mgr(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	PROCESS_ERROR(m_pShmServiceMgrData->nMgrTbusAddr > 0);

	nRetCode = send(m_pShmServiceMgrData->nMgrTbusAddr, pBuffer, dwSize);
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::send_to_all_mgr(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(m_pShmServiceMgrData);
	
	for (int32_t i = 0; i < m_pShmServiceMgrData->nMgrCount; i++)
	{
		nRetCode = send(m_pShmServiceMgrData->nMgrAddr[i], pBuffer, dwSize);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL CRSMessageHandler::do_register(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	A2A_REGISTER msg;

	msg.wMsg = a2a_register;

	nRetCode = send(nDstAddr, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	INF("send register to %s", tbus_get_str(nDstAddr));

    return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_heartbeat(void)
{
	int32_t nRetCode = 0;
	A2A_PING msg;

	msg.wMsg = a2a_ping;

	nRetCode = send_to_all_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_router_ntf_end_service()
{
	int32_t nRetCode = 0;

	nRetCode = do_router_ntf_server_event(setServerEndService, 0, 0);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::do_router_ntf_server_event(int32_t nEventType, int32_t nEventParam, int32_t nExtraParam)
{
	int32_t nRetCode = 0;
	ROUTER_NTF_SERVER_EVENT msg;

	msg.wMsg = router_ntf_server_event;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;
	msg.nExtraParam = nExtraParam;

	nRetCode = broadcast_to_local_server(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_router_ntf_msg_clear(int32_t nServiceType, int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_NTF_MSG_CLEAR msg;

	msg.wMsg = router_ntf_msg_clear;
	msg.nServiceType = nServiceType;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = broadcast_to_local_server(nServiceType, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::do_router_ntf_server_init()
{
	int32_t nRetCode = 0;
	ROUTER_NTF_SERVER_EVENT msg;

	msg.wMsg = router_ntf_server_event;
	msg.nEventType = setServerInit;
	msg.nExtraParam = 0;
	for (int32_t i = 0; i < CRouterMgr::get_instance().get_server_info_count(); i++)
	{
		SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(i);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo)
		{
			msg.nEventParam = pServerInfo->nTbusAddr;

			nRetCode = broadcast_to_local_server(&msg, sizeof(msg));
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_router_send_by_addr(int32_t nRouterAddr, int32_t nDstServerAddr, const char* pBuffer, uint32_t dwSize)
{
	int32_t nRetCode = 0;
	ROUTER_SEND_BY_ADDR msg;

	msg.wMsg = router_send_by_addr;
	msg.nDstServerAddr = nDstServerAddr;

	nRetCode = send(nRouterAddr, &msg, sizeof(msg), pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::do_unregister_report(int32_t nServerAddr)
{
	int32_t nRetCode = 0;
	ROUTER_UNREGISTER_REPORT msg;

	msg.wMsg = router_unregister_report;
	msg.nServerAddr = nServerAddr;

	nRetCode = send_to_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_server_event_execute_report(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_EVENT_EXECUTE_REPORT msg;

	msg.wMsg = router_event_execute_report;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = send_to_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_server_event_finish_report(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_EVENT_FINISH_REPORT msg;

	msg.wMsg = router_event_finish_report;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = send_to_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_server_evevt_prepare_report(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_EVENT_PREPARE_REPORT msg;

	msg.wMsg = router_event_prepare_report;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = send_to_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::do_obj_report(uint64_t qwObjID, int32_t nTbusAddr, BOOL bRegister)
{
	int32_t nRetCode = 0;
	ROUTER_OBJ_REPORT msg;

	msg.wMsg = router_obj_report;
	msg.qwObjID = qwObjID;
	msg.nTbusAddr = nTbusAddr;
	msg.bRegister = bRegister;

	nRetCode = send_to_all_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRSMessageHandler::do_resume_report(int32_t nServerAddr)
{
	int32_t nRetCode = 0;
	ROUTER_RESUME_REPORT msg;

	msg.wMsg = router_resume_report;
	msg.nTbusAddr = nServerAddr;

	nRetCode = send_to_mgr(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRSMessageHandler::do_alive_report(void)
{
	int32_t nRetCode = 0;
	ROUTER_ALIVE_REPORT* msg = NULL;
	ALIVE_DATA* pAliveData = NULL;
	uint32_t dwMsgSize = 0;

	pAliveData = CAliveMgr::get_instance().get_alive_data();
	LOG_PROCESS_ERROR(pAliveData);

	dwMsgSize = sizeof(ROUTER_ALIVE_REPORT) + sizeof(ROUTER_ALIVE_REPORT::REPORT_INFO) * pAliveData->nServerCount;

	msg = (ROUTER_ALIVE_REPORT*)alloca(dwMsgSize);
	msg->wMsg = router_alive_report;

	msg->nReportCount = 0;
	for (int32_t i = 0; i < pAliveData->nServerCount; i++)
	{
		SERVER_ALIVE_INFO& rAliveInfo = pAliveData->ServerAliveInfo[i];

		msg->ReportInfo[i].nTbusAddr = rAliveInfo.nTbusAddr;
		msg->ReportInfo[i].nLoadValue = rAliveInfo.nLoadValue;
		msg->ReportInfo[i].byAlive = (uint8_t)rAliveInfo.bAlive;

		msg->nReportCount++;
	}

    if (m_pShmServiceMgrData->nMgrTbusAddr > 0)
    {
        nRetCode = send_to_mgr(msg, dwMsgSize);
        LOG_PROCESS_ERROR(nRetCode);
    }

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

void CRSMessageHandler::on_register(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	A2A_REGISTER* msg = (A2A_REGISTER*)pBuffer;

	INF("[router network]: recv register msg from server %s", tbus_get_str(nSrcAddr));

	if (!msg->bResume)
	{
		nRetCode = CAliveMgr::get_instance().add_server_alive_info(nSrcAddr);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		SERVER_ALIVE_INFO* pAliveInfo = CAliveMgr::get_instance().get_server_alive_info(nSrcAddr);
		LOG_PROCESS_ERROR(pAliveInfo);

		pAliveInfo->nLoadValue = 0;
		pAliveInfo->dwLastHeartBeatTime = CTimeMgr::instance().get_time_sec();

		nRetCode = do_resume_report(nSrcAddr);
		LOG_CHECK_ERROR(nRetCode);
	}

Exit0:
    return;
}

void CRSMessageHandler::on_unregister(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	A2A_UNREGISTER* msg = (A2A_UNREGISTER*)pBuffer;

	INF("[router network]: recv unregister msg from server %s", tbus_get_str(msg->nSrcAddr));

	nRetCode = do_unregister_report(msg->nSrcAddr);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CRSMessageHandler::on_heartbeat(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVER_ALIVE_INFO* pAliveInfo = NULL;

	pAliveInfo = CAliveMgr::get_instance().get_server_alive_info(nSrcAddr);
	LOG_PROCESS_ERROR(pAliveInfo);

	pAliveInfo->dwLastHeartBeatTime = CTimeMgr::instance().get_time_sec();

	return;
Exit0:
	CRI("[route network]: failed to process heartbeat from %s", tbus_get_str(nSrcAddr));
	return;
}
	
void CRSMessageHandler::on_router_obj_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_OBJ_REPORT* msg = (ROUTER_OBJ_REPORT*)pBuffer;

	nRetCode = do_obj_report(msg->qwObjID, msg->nTbusAddr, msg->bRegister);
	LOG_PROCESS_ERROR(nRetCode);

	if(g_ServerConfig.SC.bRouterLogEnable)
		INF("[router network]: recv obj report from server %s obj %llX", tbus_get_str(nSrcAddr), msg->qwObjID);

Exit0:
	return;
}

void CRSMessageHandler::on_router_load_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVER_ALIVE_INFO* pAliveInfo = NULL;
	ROUTER_LOAD_REPORT* msg = (ROUTER_LOAD_REPORT*)pBuffer;

	pAliveInfo = CAliveMgr::get_instance().get_server_alive_info(nSrcAddr);
	LOG_PROCESS_ERROR(pAliveInfo);

	pAliveInfo->nLoadValue = msg->nLoadValue;

	INF("[router network]: recv server load report, server %s load %d", tbus_get_str(nSrcAddr), msg->nLoadValue);

Exit0:
	return;
}

void CRSMessageHandler::on_router_send_by_routerid(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nHashType = 0;
	int32_t nTargetServerAddr = 0;
	SERVER_INFO* pServerInfo = NULL;
	ROUTER_SEND_BY_ROUTERID* msg = (ROUTER_SEND_BY_ROUTERID*)pBuffer;

	nHashType = router_hash_get_hash_type(msg->nServiceType);
	LOG_PROCESS_ERROR(nHashType > rhtInvalid && nHashType < rhtTotal);

	nTargetServerAddr = router_hash_get_node(nHashType, msg->nServiceType, msg->qwRouterID, msg->nTransferHandleType);
	LOG_PROCESS_ERROR(nTargetServerAddr > 0);

	pServerInfo = CRouterMgr::get_instance().get_server_info_by_addr(nTargetServerAddr);
	LOG_PROCESS_ERROR(pServerInfo);

	if (pServerInfo->nState == svstTransfer)
	{
		LOG_PROCESS_ERROR(msg->nTransferHandleType == rtnhtCache || msg->nTransferHandleType == rtnhtStillSend);

		INF("[router network]: send routerid failed for transfer, begin to cache msg");

		if (msg->nTransferHandleType == rtnhtCache)
		{
			m_ShmMsgQueue.push_msg(nSrcAddr, pBuffer, dwSize);
			return;
		}
	}

	LOG_PROCESS_ERROR(pServerInfo->nRouterAddr > 0);

	if (pServerInfo->nRouterAddr == m_nTbusAddr)
	{
		nRetCode = send(nTargetServerAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_ROUTERID));
		LOG_PROCESS_ERROR(nRetCode);

		if(g_ServerConfig.SC.bRouterLogEnable)
			INF("[router network]: send to routerid, find local direct send");
	}
	else
	{
		nRetCode = do_router_send_by_addr(pServerInfo->nRouterAddr, nTargetServerAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_ROUTERID));
		LOG_PROCESS_ERROR(nRetCode);
		
		if(g_ServerConfig.SC.bRouterLogEnable)
			INF("[router network]: send to routerid, cannot find local, send to next router");
	}

Exit0:
	return;
}
	
void CRSMessageHandler::on_router_send_by_service_type(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_INFO* pServiceInfo = NULL; 
	std::set<int32_t> sTargetRouters;
	ROUTER_SEND_BY_SERVICE_TYPE* msg = (ROUTER_SEND_BY_SERVICE_TYPE*)pBuffer;
	
	pServiceInfo = CRouterMgr::get_instance().get_service_info(msg->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = NULL;

		pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo && pServerInfo->nState == svstInService)
		{
			if (pServerInfo->nRouterAddr == m_nTbusAddr)
			{
				nRetCode = send(pServerInfo->nTbusAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_SERVICE_TYPE));
				LOG_PROCESS_ERROR(nRetCode);
			}
			else
				sTargetRouters.insert(pServerInfo->nRouterAddr);
		}
	}

	if (tbus_get_type(nSrcAddr) != svrRouter)
	{
		for (std::set<int32_t>::iterator it = sTargetRouters.begin(); it != sTargetRouters.end(); it++)
		{
			nRetCode = send(*it, pBuffer, dwSize);
			LOG_CHECK_ERROR(nRetCode);
		}
	}

Exit0:
	return;
}

void CRSMessageHandler::on_router_send_by_service_inst(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nRouterAddr = 0;
	SERVICE_INFO* pServiceInfo = NULL; 
	ROUTER_SEND_BY_SERVICE_INST* msg = (ROUTER_SEND_BY_SERVICE_INST*)pBuffer;

	pServiceInfo = CRouterMgr::get_instance().get_service_info(msg->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = NULL;

		pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo && tbus_get_inst(pServerInfo->nTbusAddr) == msg->nServiceInst)
		{
			if (pServerInfo->nRouterAddr == m_nTbusAddr)
			{
				nRetCode = send(pServerInfo->nTbusAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_SERVICE_INST));
				LOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				nRetCode = send(pServerInfo->nRouterAddr, pBuffer, dwSize);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
	}

Exit0:
	return;
}

void CRSMessageHandler::on_router_send_by_addr(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nRouterAddr = 0;
	ROUTER_SEND_BY_ADDR* msg = (ROUTER_SEND_BY_ADDR*)pBuffer;

	nRouterAddr = CRouterMgr::get_instance().get_router_by_server_addr(msg->nDstServerAddr);
	LOG_PROCESS_ERROR(nRouterAddr > 0);

	if (nRouterAddr == m_nTbusAddr)
	{
		nRetCode = send(msg->nDstServerAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_ADDR));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = send(nRouterAddr, pBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:
	std::string src_addr = tbus_get_str(nSrcAddr);
	CRI("invalid dst server addr %s, nSrcAddr %s", tbus_get_str(msg->nDstServerAddr), src_addr.c_str());
	return;
}

void CRSMessageHandler::on_router_send_by_objid(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nDstServerAddr = 0;
	RS_OBJ* pObj = NULL;
	int32_t nRouterAddr = 0;
	ROUTER_SEND_BY_OBJID* msg = (ROUTER_SEND_BY_OBJID*)pBuffer;

	pObj = CRsObjMgr::get_instance().find_object(msg->qwObjID);
	LOG_PROCESS_ERROR(pObj);

	nRouterAddr = CRouterMgr::get_instance().get_router_by_server_addr(pObj->nTbusAddr);
	LOG_PROCESS_ERROR(nRouterAddr > 0);

	if (nRouterAddr == m_nTbusAddr)
	{
		nRetCode = send(pObj->nTbusAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_OBJID));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = send(nRouterAddr, pBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}
	
Exit0:
	return;
}

void CRSMessageHandler::on_router_send_by_load(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nTargetAddr = 0;
	int32_t nRouterAddr = 0;
	SERVICE_INFO* pServiceInfo = NULL;
	ROUTER_SEND_BY_LOAD* msg = (ROUTER_SEND_BY_LOAD*)pBuffer;

	pServiceInfo = CRouterMgr::get_instance().get_service_info(msg->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo && pServerInfo->nState == svstInService && pServerInfo->nLoadValue < MAX_ROUTER_LOAD_THRES)
		{
			nTargetAddr = pServerInfo->nTbusAddr;
			break;
		}
	}

	if (!nTargetAddr)
	{
		int32_t nRand = rand_ex(pServiceInfo->nServiceServerCount);
		SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[nRand]);
		LOG_PROCESS_ERROR(pServerInfo);

		if (pServerInfo->nState == svstInService)
		{
			nTargetAddr = pServerInfo->nTbusAddr;
		}
		else
		{
			for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
			{
				SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
				LOG_CHECK_ERROR(pServerInfo);

				if (pServerInfo && pServerInfo->nState == svstInService)
				{
					nTargetAddr = pServerInfo->nTbusAddr;
					break;
				}
				
			}
		}
	}

	nRouterAddr = CRouterMgr::get_instance().get_router_by_server_addr(nTargetAddr);
	LOG_PROCESS_ERROR(nRouterAddr > 0);

	if (nRouterAddr == m_nTbusAddr)
	{
		nRetCode = send(nTargetAddr, msg->szData, dwSize - sizeof(ROUTER_SEND_BY_LOAD));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = send(nRouterAddr, pBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return;
}

void CRSMessageHandler::on_router_send_to_mgr(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_SEND_TO_MGR* msg = (ROUTER_SEND_TO_MGR*)pBuffer;

	nRetCode = send_to_mgr(msg->szData, dwSize - sizeof(ROUTER_SEND_TO_MGR));
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CRSMessageHandler::on_router_server_event_ack(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_SERVER_EVENT_ACK* msg = (ROUTER_SERVER_EVENT_ACK*)pBuffer;

	LOG_PROCESS_ERROR(m_pShmServiceMgrData);

	INF("on router recv server %s event ack, total %d", tbus_get_str(nSrcAddr), m_pShmServiceMgrData->nInitServerNodeCount);

	m_pShmServiceMgrData->nServerEventAckCount++;

	if (m_pShmServiceMgrData->nServerEventAckCount >= CAliveMgr::get_instance().get_alive_server_count())
	{
		nRetCode = do_server_event_execute_report(msg->nEventType, msg->nEventParam);
		LOG_PROCESS_ERROR(nRetCode);
		
		m_pShmServiceMgrData->nServerEventAckCount = 0;
		INF("[trans_execute]: execute finished, begin to report execute to service mgr, event_type %d, event_param %d", msg->nEventType, msg->nEventParam);
	}

Exit0:
	return;
}

void CRSMessageHandler::on_router_ntf_msg_clear_ack(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_NTF_MSG_CLEAR_ACK* msg = (ROUTER_NTF_MSG_CLEAR_ACK*)pBuffer;

	INF("[trans_prepare]: recv msg clear ack, event_type %d event_param %d tbus_addr %s", msg->nEventType, msg->nEventParam, tbus_get_str(nSrcAddr));
	m_pShmServiceMgrData->nServerEventAckCount++;

	if (CAliveMgr::get_instance().get_server_count(msg->nServiceType) == m_pShmServiceMgrData->nServerEventAckCount)
	{
		nRetCode = do_server_evevt_prepare_report(msg->nEventType, msg->nEventParam);
		LOG_PROCESS_ERROR(nRetCode);
			
		INF("[trans_perpare]: finish, begin to report to service mgr, event_type %d, event_param %d", msg->nEventType, msg->nEventParam);
	}

Exit0:
	return;
}

void CRSMessageHandler::on_router_get_service_info(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_INFO* pServiceInfo = NULL;
	SERVER_INFO* pServerInfo = NULL;
	ROUTER_GET_SERVICE_INFO* msg = (ROUTER_GET_SERVICE_INFO*)pBuffer;

	pServiceInfo = CRouterMgr::get_instance().get_service_info(msg->nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);

	for (int32_t i = 0; i < pServiceInfo->nServerCount; i++)
	{
		pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo)
		{
			nRetCode = do_router_ntf_server_event(setServerInfoNtf, pServerInfo->nTbusAddr, pServerInfo->nState);
			LOG_CHECK_ERROR(nRetCode);
		}
	}

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_sync_data_begin(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_DATA_BEGIN* msg = (SERVICE_MANAGE_SYNC_DATA_BEGIN*)pBuffer;

	m_pShmServiceMgrData->nMgrTbusAddr =  msg->nLeaderMgrAddr;
	
	INF("[router network]: service mgr sync data begin, leader addr %s", tbus_get_str(msg->nLeaderMgrAddr));

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_sync_server_info(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_SERVER_INFO* msg = (SERVICE_MANAGE_SYNC_SERVER_INFO*)pBuffer;

	for (int32_t i = 0; i < msg->nServerCount; i++)
	{
		nRetCode = CRouterMgr::get_instance().add_server_info(msg->ServerInfo[i]);
		LOG_PROCESS_ERROR(nRetCode);
	
		INF("[router network]: service mgr sync server info, server %s router %s state %d load %d", tbus_get_str(msg->ServerInfo[i].nTbusAddr),
									tbus_get_str(msg->ServerInfo[i].nRouterAddr), msg->ServerInfo[i].nState, msg->ServerInfo[i].nLoadValue);
	}

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_sync_obj(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_OBJ* msg = (SERVICE_MANAGE_SYNC_OBJ*)pBuffer;

	INF("recv sync obj from %s", tbus_get_str(nSrcAddr));

	for (int32_t i = 0; i < msg->nObjCount; i++)
	{
		RS_OBJ* pObj = NULL; 
		
		pObj = CRsObjMgr::get_instance().find_object(msg->ObjInfo[i].qwObjID);
		if (pObj == NULL)
		{
			pObj = CRsObjMgr::get_instance().new_object(msg->ObjInfo[i].qwObjID);
			LOG_PROCESS_ERROR(pObj);
		}

		pObj->nTbusAddr = msg->ObjInfo[i].nTbusAddr;

		INF("[router network]: service mgr sync obj, obj %llx server %s", msg->ObjInfo[i].qwObjID, tbus_get_str(msg->ObjInfo[i].nTbusAddr));
	}

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_sync_data_end(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;

	INF("[router network]: service mgr sync data finished");

Exit0:
	return;
}
	
void CRSMessageHandler::on_service_mgr_obj_ntf(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	RS_OBJ* pObj = NULL; 
	SERVICE_MANAGE_OBJ_NTF* msg = (SERVICE_MANAGE_OBJ_NTF*)pBuffer;
		
	if (msg->bRegister)
	{
		pObj = CRsObjMgr::get_instance().find_object(msg->qwObjID);
		if (pObj == NULL)
		{
			pObj = CRsObjMgr::get_instance().new_object(msg->qwObjID);
			LOG_PROCESS_ERROR(pObj);
		}
		pObj->nTbusAddr = msg->nTbusAddr;
	}
	else
	{
		pObj = CRsObjMgr::get_instance().find_object(msg->qwObjID);
		LOG_PROCESS_ERROR(pObj);

		nRetCode = CRsObjMgr::get_instance().del_object(pObj);
		LOG_PROCESS_ERROR(nRetCode);
	}

	INF("[router network]: service mgr ntf obj %llx server %s registered %d", msg->qwObjID, tbus_get_str(msg->nTbusAddr), msg->bRegister);

Exit0:
	return;
}
	
void CRSMessageHandler::on_service_mgr_server_info_ntf(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SERVER_INFO_NTF* msg = (SERVICE_MANAGE_SERVER_INFO_NTF*)pBuffer;

	for (int32_t i = 0; i < msg->nServerInfoCount; i++)
	{
		SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_addr(msg->ServerInfo[i].nTbusAddr);
		if (pServerInfo == NULL)
		{
			nRetCode = CRouterMgr::get_instance().add_server_info(msg->ServerInfo[i]);
			LOG_PROCESS_ERROR(nRetCode);

			if (CMGApp::instance().get_state() == svstInService)
			{
				nRetCode = do_router_ntf_server_event(setServerInit, msg->ServerInfo[i].nTbusAddr, 0);
				LOG_CHECK_ERROR(nRetCode);
			}
		}
		else
		{
			*pServerInfo = msg->ServerInfo[i];
		}
		
		INF("[router network]: service mgr ntf server info, server %s router %s state %d load %d", tbus_get_str(msg->ServerInfo[i].nTbusAddr),
									tbus_get_str(msg->ServerInfo[i].nRouterAddr), msg->ServerInfo[i].nState, msg->ServerInfo[i].nLoadValue);
	}

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_in_service(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;

	nRetCode = CRouterMgr::get_instance().set_all_in_service();
	LOG_PROCESS_ERROR(nRetCode);

	CMGApp::instance().set_state(svstInService);

	nRetCode = do_router_ntf_server_init();
	LOG_PROCESS_ERROR(nRetCode);

	INF("[router network]: service mgr ntf in service");
Exit0:
	return;
}
	
void CRSMessageHandler::on_service_mgr_end_service(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	
	CMGApp::instance().set_state(svstEndService);
	
	nRetCode = do_router_ntf_end_service();
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_ntf_mgr_leader(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_NTF_MGR_LEADER* msg = (SERVICE_MANAGE_NTF_MGR_LEADER*)pBuffer;

	LOG_PROCESS_ERROR(m_pShmServiceMgrData);

	m_pShmServiceMgrData->nMgrTbusAddr = msg->nMgrLeader;

	INF("service mgr leader is change to %s", tbus_get_str(msg->nMgrLeader));

	nRetCode = do_register(m_pShmServiceMgrData->nMgrTbusAddr);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CRSMessageHandler::on_service_mgr_ntf_server_event(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL; 
	SERVICE_MANAGE_NTF_SERVER_EVENT* msg = (SERVICE_MANAGE_NTF_SERVER_EVENT*)pBuffer;

	LOG_PROCESS_ERROR(m_pShmServiceMgrData);

	switch (msg->nPhaseType)
	{
		case sopPrepare:
		{
			_on_server_event_prepare(msg->nEventType, msg->nEventParam);
			break;
		}
		case sopExecute:
		{
			_on_server_event_execute(msg->nEventType, msg->nEventParam);
			break;
		}
		case sopFinish:
		{
			_on_server_event_finish(msg->nEventType, msg->nEventParam);
			break;
		}
		default:
			LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
	return;
}

void CRSMessageHandler::_on_server_event_prepare(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;

	switch (nEventType)
	{
	case setServerExpand:
	case setServerReduce:
	{
		SERVICE_INFO* pServiceInfo = NULL;
		int32_t nServiceType = nEventParam >> 16;
		m_pShmServiceMgrData->nServerEventAckCount = 0;

		pServiceInfo = CRouterMgr::get_instance().get_service_info(nServiceType);
		LOG_PROCESS_ERROR(pServiceInfo);

		pServiceInfo->nState = svcstTransfer;
		for (int32_t i = 0; i < pServiceInfo->nServerCount; i++)
		{
			SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
			LOG_CHECK_ERROR(pServerInfo);

			if (pServerInfo)
			{
				pServerInfo->nState = svstTransfer;
			}
		}

		if (CAliveMgr::get_instance().get_server_count(nServiceType) != 0)
		{
			nRetCode = do_router_ntf_msg_clear(nServiceType, nEventType, nEventParam);
			LOG_PROCESS_ERROR(nRetCode);

			INF("[trans_prepare]: begin to clear all msg, service_type %d event_type %d event_param %d", nServiceType, nEventType, nEventParam);
		}
		else
		{
			nRetCode = do_server_evevt_prepare_report(nEventType, nEventParam);
			LOG_PROCESS_ERROR(nRetCode);

			INF("[trans_perpare]: finish, begin to report to service mgr, event_type %d, event_param %d", nEventType, nEventParam);
		}

		break;
	}
	case setServerDown:
	case setServerRecover:
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	bInEventProcess = TRUE;

Exit0:
	return;
}

void CRSMessageHandler::_on_server_event_execute(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;

	switch (nEventType)
	{
	case setServerDown:
	case setServerRecover:
	{
		int32_t nServiceType = 0;
		SERVICE_INFO* pServiceInfo = NULL;
		SERVER_INFO* pServerInfo = NULL;

		nServiceType = tbus_get_type(nEventParam);

		pServerInfo = CRouterMgr::get_instance().get_server_info_by_addr(nEventParam);
		LOG_PROCESS_ERROR(pServerInfo);
		
		pServiceInfo = CRouterMgr::get_instance().get_service_info(nServiceType);
		LOG_PROCESS_ERROR(pServiceInfo);

		pServerInfo->nState = svstTransfer;
		m_pShmServiceMgrData->nServerEventAckCount = 0;

		nRetCode = do_router_ntf_server_event(nEventType, nEventParam, pServiceInfo->nServiceServerCount);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case setServerExpand:
	case setServerReduce:
	{
		int32_t nServiceType = 0;
		SERVICE_INFO* pServiceInfo = NULL;
		m_pShmServiceMgrData->nServerEventAckCount = 0;

		nServiceType = nEventParam >> 16;
		pServiceInfo = CRouterMgr::get_instance().get_service_info(nServiceType);
		LOG_PROCESS_ERROR(pServiceInfo);

		nRetCode = do_router_ntf_server_event(nEventType, nEventParam, pServiceInfo->nServiceServerCount);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	case setServerResume:
	{
		nRetCode = do_router_ntf_server_event(nEventType, nEventParam, 0);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	bInEventProcess = TRUE;

	INF("[trans_execute]: begin to ntf server node to transfer, event_type %d, event_param %d", nEventType, nEventParam);

Exit0:
	return;
}

void CRSMessageHandler::_on_server_event_finish(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;

	switch (nEventType)
	{
	case setServerDown:
	{
		SERVER_INFO* pServerInfo = NULL;

		pServerInfo = CRouterMgr::get_instance().get_server_info_by_addr(nEventParam);
		LOG_PROCESS_ERROR(pServerInfo);

		pServerInfo->nState = svstDown;
		break;
	}
	case setServerRecover:
	{
		SERVER_INFO* pServerInfo = NULL;

		pServerInfo = CRouterMgr::get_instance().get_server_info_by_addr(nEventParam);
		LOG_PROCESS_ERROR(pServerInfo);

		pServerInfo->nState = svstInService;
		break;
	}
	case setServerExpand:
	case setServerReduce:
	{
		SERVICE_INFO* pServiceInfo = NULL;
		int32_t nServiceType = nEventParam >> 16;
		int32_t nServiceCount = nEventParam & 0xFFFF;

		pServiceInfo = CRouterMgr::get_instance().get_service_info(nServiceType);
		LOG_PROCESS_ERROR(pServiceInfo);

		pServiceInfo->nState = svstInService;
		for (int32_t i = 0; i < nServiceCount; i++)
		{
			SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
			LOG_CHECK_ERROR(pServerInfo);
			if (pServerInfo)
			{
				LOG_CHECK_ERROR(pServerInfo->nState == svstTransfer);
				pServerInfo->nState = svstInService;
			}
		}

		if (pServiceInfo->nServiceServerCount > nServiceCount)
		{
			for (int32_t i = nServiceCount; i < pServiceInfo->nServiceServerCount; i++)
			{
				SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
				LOG_CHECK_ERROR(pServerInfo);
				if (pServerInfo)
				{
					LOG_CHECK_ERROR(pServerInfo->nState == svstTransfer);
					pServerInfo->nState = svstAvailable;
				}
			}
		}
		pServiceInfo->nServiceServerCount = nServiceCount;

		break;
	}
	case setServerResume:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	nRetCode = do_server_event_finish_report(nEventType, nEventParam);
	LOG_PROCESS_ERROR(nRetCode);

	bInEventProcess = FALSE;

	INF("[trans_finish]: done finish in router, begin to report finish to service mgr, event_type %d event_param %d", nEventType, nEventParam);

Exit0:
	return;
}