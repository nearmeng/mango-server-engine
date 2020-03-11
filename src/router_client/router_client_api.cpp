#include "stdafx.h"
#include "router_client/router_client_api.h"

#include "define/router_def.h"
#include "define/server_def.h"
#include "define/service_def.h"

#include "tbus/tbus_internal.h"
#include "tbus/tbus_wrapper.h"
#include "time/time_mgr.h"

#include "proto/common_message.h"
#include "proto/router_message.h"

CRouterClient CRouterClient::ms_RouterClient;

BOOL CRouterClient::router_msg_handle(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
    INTERNAL_MESSAGE_HEADER* pHeader = (INTERNAL_MESSAGE_HEADER*)pBuffer;
	LOG_PROCESS_ERROR(ms_RouterClient.m_pMsgHandle);

	if (tbus_get_type(nSrcAddr) == svrTconnd)
	{
		ms_RouterClient.m_pMsgHandle(pBuffer, dwSize, nSrcAddr);
	}
	else
	{
		INTER_MESSAGE_FUNC pServerMsgHandle = ms_RouterClient.m_InternalMessageFuncList[pHeader->wMsg];

		if (pServerMsgHandle)
			((&ms_RouterClient)->*pServerMsgHandle)(pBuffer, dwSize);
		else
			ms_RouterClient.m_pMsgHandle(pBuffer, dwSize, pHeader->nMsgSrcAddr);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_init_internal_msgid(void)
{
	int32_t nRetCode = 0;

	m_InternalMessageFuncList[router_ntf_server_event] = &CRouterClient::on_router_ntf_server_event;
	m_InternalMessageFuncList[router_ntf_msg_clear] = &CRouterClient::on_router_ntf_msg_clear;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::init(int32_t nTbusHandle, int32_t nTbusID, MESSAGE_FUNC pMsgHandle, BOOL bResume)
{
	int32_t nRetCode = 0;
	int32_t nPeerCount = 0;
    int32_t* pnAddrList = NULL;
	int32_t nCheckRounterCount = 0;

	LOG_PROCESS_ERROR(pMsgHandle);

	m_nTbusHandle = nTbusHandle;
	m_nTbusID = nTbusID;
	m_nRouterTbusID = 0;
	m_qwLastHeartBeatTime = 0; 
	m_qwLastLoadReportTime = 0;
	m_pLoadReportFunc = NULL;
	m_nLoadReportInterval = 0;
	m_pMsgHandle = pMsgHandle;
	memset(m_InternalMessageFuncList, 0, sizeof(m_InternalMessageFuncList));
	memset(m_pServerEventHandle, 0, sizeof(m_pServerEventHandle));

	nRetCode = _init_internal_msgid();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = tbus_system_init(nTbusID, nTbusHandle, router_msg_handle, bResume);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = tbus_get_peer_count(m_nTbusHandle, m_nTbusID, &nPeerCount);
    LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

    pnAddrList = (int32_t*)alloca(sizeof(int32_t) * nPeerCount);
    LOG_PROCESS_ERROR(pnAddrList);

    nRetCode = tbus_get_peer_addr(m_nTbusHandle, m_nTbusID, pnAddrList, &nPeerCount);
    LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	for (int32_t nIndex = 0; nIndex < nPeerCount; nIndex++)
	{
		int32_t nServerType = tbus_get_type(pnAddrList[nIndex]);
		if (nServerType == svrRouter)
		{
			m_nRouterTbusID = pnAddrList[nIndex];
			nCheckRounterCount++;
		}
	}

	LOG_PROCESS_ERROR(nCheckRounterCount == 1);

	nRetCode = _send_register(bResume);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::uninit(void)
{
	int32_t nRetCode = 0;

	nRetCode = _send_unregister();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = tbus_system_uninit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL CRouterClient::mainloop(void)
{
	int32_t nRetCode = 0;
	uint64_t qwCurrTime = CTimeMgr::instance().get_server_tick();

	if (qwCurrTime - m_qwLastHeartBeatTime > ROUTER_CLIENT_HEART_BEAT_INTERVAL)
	{
		nRetCode = _send_heart_beat();
		LOG_CHECK_ERROR(nRetCode);

		m_qwLastHeartBeatTime = qwCurrTime;
	}

	if (m_pLoadReportFunc && qwCurrTime - m_qwLastLoadReportTime > m_nLoadReportInterval)
	{
		nRetCode = _send_load_report();
		LOG_CHECK_ERROR(nRetCode);

		m_qwLastLoadReportTime = qwCurrTime;
	}
	
	return tbus_recv_data();
}

BOOL CRouterClient::send_by_addr(int32_t nDstServerAddr, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_ADDR* msg = NULL;
	INTERNAL_MESSAGE_HEADER* pHeader = NULL;

	pHeader = (INTERNAL_MESSAGE_HEADER*)pBuffer;
	dwAllSize = sizeof(ROUTER_SEND_BY_ADDR) + dwSize;

	msg = (ROUTER_SEND_BY_ADDR*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_addr;
	msg->nDstServerAddr = nDstServerAddr;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::send_by_service_type(int32_t nServiceType, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_SERVICE_TYPE* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_BY_SERVICE_TYPE) + dwSize;

	msg = (ROUTER_SEND_BY_SERVICE_TYPE*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_service_type;
	msg->nServiceType = nServiceType;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::send_by_service_inst(int32_t nServiceType, int32_t nInstID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_SERVICE_INST* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_BY_SERVICE_INST) + dwSize;

	msg = (ROUTER_SEND_BY_SERVICE_INST*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_service_inst;
	msg->nServiceType = nServiceType;
	msg->nServiceInst = nInstID;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::send_by_routerid(uint64_t qwRouterID, int32_t nServerType, const void* pBuffer, size_t dwSize,
		int32_t nTransferHandleType)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_ROUTERID* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_BY_ROUTERID) + dwSize;

	msg = (ROUTER_SEND_BY_ROUTERID*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_routerid;
	msg->qwRouterID = qwRouterID;
	msg->nServiceType = nServerType;
	msg->nTransferHandleType = nTransferHandleType;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::send_by_objid(uint64_t qwObjID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_OBJID* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_BY_OBJID) + dwSize;

	msg = (ROUTER_SEND_BY_OBJID*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_objid;
	msg->qwObjID = qwObjID;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::send_by_load(int32_t nServiceType, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_BY_LOAD* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_BY_LOAD) + dwSize;
	
	msg = (ROUTER_SEND_BY_LOAD*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_by_load;
	msg->nServiceType = nServiceType;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::send_to_mgr(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	uint32_t dwAllSize = 0;
	ROUTER_SEND_TO_MGR* msg = NULL;

	dwAllSize = sizeof(ROUTER_SEND_TO_MGR) + dwSize;
	msg = (ROUTER_SEND_TO_MGR*)alloca(dwAllSize);
	LOG_PROCESS_ERROR(msg);

	msg->wMsg = router_send_to_mgr;
	msg->nDataSize = dwSize;
	memcpy(msg->szData, pBuffer, dwSize);

	nRetCode = _send_to_router(msg, dwAllSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_send_to_router(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

    nRetCode = tbus_send(m_nTbusHandle, &m_nTbusID, &m_nRouterTbusID, pBuffer, dwSize, 0);
    PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	return TRUE;
Exit0:
	CRI("tbus send failed, ret code %d", nRetCode);
	return FALSE;
}

BOOL CRouterClient::_send_heart_beat(void)
{
	int32_t nRetCode = 0;
	A2A_PING msg;

	msg.wMsg = a2a_ping;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_send_unregister(void)
{
	int32_t nRetCode = 0;
	A2A_UNREGISTER msg;

	msg.wMsg = a2a_unregister;
	msg.nSrcAddr = m_nTbusID;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::_send_msg_clear_ack(int32_t nServiceType, int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_NTF_MSG_CLEAR_ACK msg;

	msg.wMsg = router_ntf_msg_clear_ack;
	msg.nServiceType = nServiceType;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::_send_get_service_info(int32_t nServiceType)
{
	int32_t nRetCode = 0;
	ROUTER_GET_SERVICE_INFO msg;

	msg.wMsg = router_get_service_info;
	msg.nServiceType = nServiceType;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_send_server_event_ack(int32_t nEventType, int32_t nEventParam)
{
	int32_t nRetCode = 0;
	ROUTER_SERVER_EVENT_ACK msg;

	msg.wMsg = router_server_event_ack;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_send_load_report(void)
{
	int32_t nRetCode = 0;
	ROUTER_LOAD_REPORT msg;

	PROCESS_ERROR(m_pLoadReportFunc);

	msg.wMsg = router_load_report;
	msg.nLoadValue = (*m_pLoadReportFunc)();

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::obj_report(uint64_t qwObjID, BOOL bRegister)
{
	int32_t nRetCode = 0;
	ROUTER_OBJ_REPORT msg;

	msg.wMsg = router_obj_report;
	msg.qwObjID = qwObjID;
	msg.nTbusAddr = m_nTbusID;
	msg.bRegister = bRegister;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRouterClient::_send_register(BOOL bResume)
{
	int32_t nRetCode = 0;
	A2A_REGISTER msg;

	msg.wMsg = a2a_register;
	msg.bResume = bResume;

	nRetCode = _send_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::on_router_ntf_msg_clear(const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	ROUTER_NTF_MSG_CLEAR* msg = (ROUTER_NTF_MSG_CLEAR*)pBuffer;

	INF("router client recv msg clear, begin to ack, service_type %d event_type %d event_param %d", msg->nServiceType, msg->nEventType, msg->nEventParam);
	
	nRetCode = _send_msg_clear_ack(msg->nServiceType, msg->nEventType, msg->nEventParam);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CRouterClient::on_router_ntf_server_event(const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	int32_t nServerAddr = 0;
	ROUTER_NTF_SERVER_EVENT* msg = (ROUTER_NTF_SERVER_EVENT*)pBuffer;

	LOG_PROCESS_ERROR(msg->nEventType > setInvalid && msg->nEventType < setTotal);

	INF("on router ntf event, event_type %d event_param %d", msg->nEventType, msg->nEventParam);

	switch (msg->nEventType)
	{
	case setServerInit:
	case setServerResume:
	{
		if(m_pServerEventHandle[msg->nEventType])
			(*m_pServerEventHandle[msg->nEventType])(tbus_get_type(msg->nEventParam), msg->nEventParam, 0, 0);

		break;
	}
	case setServerEndService:
	{
		if (m_pServerEventHandle[setServerEndService])
			(*m_pServerEventHandle[setServerEndService])(0, 0, 0, 0);

		break;
	}
	case setServerDown:
	case setServerRecover:
	{
		if (m_pServerEventHandle[msg->nEventType])
			(*m_pServerEventHandle[msg->nEventType])(tbus_get_type(msg->nEventParam), msg->nEventParam, msg->nExtraParam, 0);

		nRetCode = _send_server_event_ack(msg->nEventType, msg->nEventParam);
		LOG_PROCESS_ERROR(nRetCode);

		break;
	}
	case setServerExpand:
	case setServerReduce:
	{
		if (m_pServerEventHandle[msg->nEventType])
			(*m_pServerEventHandle[msg->nEventType])((msg->nEventParam) >> 16, 0, (msg->nEventParam) & 0xFFFF, msg->nExtraParam);

		nRetCode = _send_server_event_ack(msg->nEventType, msg->nEventParam);
		LOG_PROCESS_ERROR(nRetCode);

		break;
	}
	case setServerInfoNtf:
	{
		if (m_pServerEventHandle[msg->nEventType])
			(*m_pServerEventHandle[msg->nEventType])(tbus_get_type(msg->nEventParam), msg->nEventParam, msg->nExtraParam, 0);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}

void CRouterClient::get_service_info(int32_t nServiceType)
{
	int32_t nRetCode = 0;

	nRetCode = _send_get_service_info(nServiceType);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}
