#include "stdafx.h"
#include "sms_message_handler.h"

#include "tbus/tbus.h"
#include "time/time_mgr.h"

#include "define/router_def.h"
#include "define/server_def.h"
#include "game_data/global_config.h"
#include "tbus/tbus_wrapper.h"

#include "proto/common_message.h"
#include "proto/service_manage_message.h"
#include "proto/router_message.h"
#include "service_mgr/service_mgr.h"
#include "object_mgr/sms_object_mgr.h"
#include "trans/sms_trans.h"

#include "router_client/router_client_api.h"
#include "raft/raft_service.h" 

#include "app/server_app.h"
#include <vector>

enum SERVICE_MGR_STATE
{
	smsInvalid = -1,

	smsSelectLeaderFinish	= 1 << 0,
	smsAllRouterReport		= 1 << 1,

	smsTotal
};

#define MAX_RECV_PER_LOOP	(1024 * 4)
#define SERVICE_MGR_PING_INTERVAL	(1 * 1000)

CSMSMessageHandler CSMSMessageHandler::ms_Instance;

CSMSMessageHandler::CSMSMessageHandler()
{
}

CSMSMessageHandler::~CSMSMessageHandler()
{
}

BOOL CSMSMessageHandler::_init_msgid(void)
{
	int32_t nRetCode = 0;

    m_InternalMessageFuncList[a2a_register] = &CSMSMessageHandler::on_register;
	m_InternalMessageFuncList[a2a_control_req] = &CSMSMessageHandler::on_control;

	m_InternalMessageFuncList[a2a_ping] = &CSMSMessageHandler::on_router_heart_beat;
	m_InternalMessageFuncList[router_alive_report] = &CSMSMessageHandler::on_router_alive_report;
	m_InternalMessageFuncList[router_obj_report] = &CSMSMessageHandler::on_router_obj_report;
	m_InternalMessageFuncList[router_resume_report] = &CSMSMessageHandler::on_router_resume_report;
	m_InternalMessageFuncList[router_unregister_report] = &CSMSMessageHandler::on_router_unregister_report;
	m_InternalMessageFuncList[router_event_prepare_report] = &CSMSMessageHandler::on_router_event_prepare_report;
	m_InternalMessageFuncList[router_event_execute_report] = &CSMSMessageHandler::on_router_event_execute_report;
	m_InternalMessageFuncList[router_event_finish_report] = &CSMSMessageHandler::on_router_event_finish_report;

	m_InternalMessageFuncList[service_mgr_sync_data_begin] = &CSMSMessageHandler::on_service_mgr_sync_data_begin;
	m_InternalMessageFuncList[service_mgr_sync_obj] = &CSMSMessageHandler::on_service_mgr_sync_obj;
	m_InternalMessageFuncList[service_mgr_sync_router] = &CSMSMessageHandler::on_service_mgr_sync_router;
	m_InternalMessageFuncList[service_mgr_sync_data_end] = &CSMSMessageHandler::on_service_mgr_sync_data_end;
	m_InternalMessageFuncList[service_mgr_raft_msg] = &CSMSMessageHandler::on_service_mgr_raft_msg;
	m_InternalMessageFuncList[service_mgr_end_service] = &CSMSMessageHandler::on_service_mgr_end_service;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL raft_become_leader(int32_t nTerm)
{
	int32_t nRetCode = 0;

	INF("[raft]: become leader, term %d", nTerm);

	SMS_ROUTER_MGR* pRouterMgr = CServiceMgr::get_instance().get_router_mgr();
	LOG_PROCESS_ERROR(pRouterMgr);

	for (int32_t i = 0; i < pRouterMgr->nRouterCount; i++)
	{
		int32_t nRouterAddr = pRouterMgr->RouterInfo[i].nTbusAddr;
		nRetCode = CSMSMessageHandler::instance().do_sync_mgr_data_begin(nRouterAddr);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CSMSMessageHandler::instance().do_sync_server_info(nRouterAddr);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CSMSMessageHandler::instance().do_sync_obj_info(nRouterAddr);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CSMSMessageHandler::instance().do_sync_mgr_data_end(nRouterAddr);
		LOG_PROCESS_ERROR(nRetCode);
	}
		
	if (!CSMSMessageHandler::instance().is_state_ready(smsSelectLeaderFinish))
	{
		INF("[service mgr]: new leader is selected!");
		CSMSMessageHandler::instance().set_state(smsSelectLeaderFinish);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL raft_apply_entry(RAFT_MGR* pRaftMgr, RAFT_ENTRY* pEntry, int32_t nIndex)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftMgr);
	LOG_PROCESS_ERROR(pEntry);

	WRN("[raft]: become to apply entry type %d param %llu, curr_log_index %d commit_index %d apply_index %d", pEntry->nType, pEntry->llParam,
				raft_service_get_curr_index(pRaftMgr), raft_service_get_commit_index(pRaftMgr), raft_service_get_last_applied_index(pRaftMgr));

	switch (pEntry->nType)
	{
	case radtInitServiceInfo:
	{
		mg_set_state(svstInService);

		//set data
		nRetCode = CServiceMgr::get_instance().set_service_data((const char*)pEntry->stData.pBuff, pEntry->stData.dwSize);
		LOG_PROCESS_ERROR(nRetCode);

		//make all avaliable server node in service
		nRetCode = CServiceMgr::get_instance().set_all_in_service();
		LOG_PROCESS_ERROR(nRetCode);

		if (raft_service_get_status(pRaftMgr) == rsLeader)
		{
			nRetCode = CSMSMessageHandler::instance().do_sync_in_service();
			LOG_PROCESS_ERROR(nRetCode);
		}

		INF("[service mgr]: enter in service mode!");
		break;
	}
	case radtEventExcute:
	{
		int32_t nEventType = (pEntry->llParam) >> 32;
		int32_t nEventParam = (pEntry->llParam) & 0xFFFFFFFF;

		if (raft_service_get_status(pRaftMgr) == rsLeader)
		{
			nRetCode = CSMSMessageHandler::instance().do_ntf_server_event(nEventType, nEventParam, sopFinish);
			LOG_PROCESS_ERROR(nRetCode);

			INF("[trans_execute]: service mgr deal trans finish, begin to ntf all router trans finish, event_type %d, event_param %d", nEventType, nEventParam);
		}
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::init(int32_t nTbusHandle, int32_t nTbusAddr, const char* pcszTag, BOOL bResume)
{
	int32_t nRetCode = 0;
	int32_t nPeerCount = 0;
    int32_t* pnAddrList = NULL;
	RAFT_NODE* pRaftNode = NULL;
	RAFT_CALLBACK_MGR stCallbackMgr;

	m_pShmMessageData = NULL;
	m_nTbusAddr = nTbusAddr;
	m_nTbusHandle = nTbusHandle;

	nRetCode = _init_msgid();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ShmMsgQueue.init(bResume);
	LOG_PROCESS_ERROR(nRetCode);
		
	nRetCode = m_ShmMessageData.init(stdSMMsgData, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	m_pRaftMgr = raft_service_new(pcszTag, bResume);
	LOG_PROCESS_ERROR(m_pRaftMgr);

	stCallbackMgr.pSendRequest = CSMSMessageHandler::send_raft_msg;
	stCallbackMgr.pApplyEntry = raft_apply_entry;
	stCallbackMgr.pBecomeLeader = raft_become_leader;
	nRetCode = raft_service_set_callback(m_pRaftMgr, stCallbackMgr);
	LOG_PROCESS_ERROR(nRetCode);

	pRaftNode = raft_service_add_node(m_pRaftMgr, nTbusAddr, TRUE);
	LOG_PROCESS_ERROR(pRaftNode);

	m_pShmMessageData = m_ShmMessageData.get_obj();
	LOG_PROCESS_ERROR(m_pShmMessageData);

	if (!bResume)
	{
		memset(m_pShmMessageData, 0, sizeof(SMS_MESSAGE_DATA));

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
				m_pShmMessageData->nMgrAddr[m_pShmMessageData->nMgrCount++] = pnAddrList[nIndex];

				pRaftNode = raft_service_add_node(m_pRaftMgr, pnAddrList[nIndex], FALSE);
				LOG_CHECK_ERROR(pRaftNode);
			}
			else if (nServerType == svrRouter)
				m_pShmMessageData->nInitRouterCount++;
		}
	}

	INF("service mgr handler inited");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSMSMessageHandler::uninit(void)
{
	int32_t nRetCode = 0;

	if (raft_service_get_status(m_pRaftMgr) == rsLeader)
	{
		nRetCode = do_sync_end_service(svrServiceMgr);
		LOG_CHECK_ERROR(nRetCode);
	}

	nRetCode = m_ShmMsgQueue.uninit();
	LOG_CHECK_ERROR(nRetCode);

	if(m_pRaftMgr)
		raft_service_free(m_pRaftMgr);

    return TRUE;
}
	
BOOL CSMSMessageHandler::msg_handler(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
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

void CSMSMessageHandler::frame()
{
	if(m_pRaftMgr)
		raft_service_mainloop(m_pRaftMgr, 1 * 1000 / GAME_FPS);
}

void CSMSMessageHandler::keep_alive(void)
{
	int32_t nRetCode = 0;

Exit0:
	return;
}

BOOL CSMSMessageHandler::send(int32_t nDstAddr, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

    nRetCode = tbus_send(m_nTbusHandle, &m_nTbusAddr, &nDstAddr, pBuffer, dwSize, 0);
    LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::send_raft_msg(int32_t nNodeID, int32_t nMsgType, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	int32_t nRealSize = 0;
	SERVICE_MANAGE_RAFT_MSG* pMsg = NULL; 

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize > 0);

	nRealSize = sizeof(SERVICE_MANAGE_RAFT_MSG) + nSize;
	pMsg = (SERVICE_MANAGE_RAFT_MSG*)alloca(nRealSize);

	pMsg->wMsg = service_mgr_raft_msg;
	pMsg->nMsgType = nMsgType;
	pMsg->nMsgSize = nSize;
	memcpy(pMsg->szMsgData, pBuff, nSize);
	
	nRetCode = CSMSMessageHandler::instance().send(nNodeID, pMsg, nRealSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL CSMSMessageHandler::do_register(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	A2A_REGISTER msg;

	msg.wMsg = a2a_register;

	nRetCode = send(nDstAddr, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSMSMessageHandler::do_ping(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	A2A_PING msg;

	msg.wMsg = a2a_ping;

	nRetCode = send(nDstAddr, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSMSMessageHandler::do_control_ack(int32_t nResult, const char * pDesc, int32_t nDstAddr)
{
	int32_t nRetCode = 0;
    A2A_CONTROL_ACK msg;
	uint32_t dwSize = 0;
	ROUTER_SEND_BY_ADDR* pRouterMsg = NULL;
	SMS_ROUTER_INFO* pRouterInfo;
	
    msg.wMsg = a2a_control_ack;
    msg.nResult = 0;
    msg.nDstAddr = nDstAddr;
	strxcpy(msg.szDesc, pDesc, sizeof(msg.szDesc));
    msg.nDescLen = strxlen(msg.szDesc, sizeof(msg.szDesc));

	dwSize = sizeof(msg) + sizeof(ROUTER_SEND_BY_ADDR);
	pRouterMsg = (ROUTER_SEND_BY_ADDR*)alloca(dwSize);
	LOG_PROCESS_ERROR(pRouterMsg);

	pRouterMsg->wMsg = router_send_by_addr;
	pRouterMsg->nDstServerAddr = nDstAddr;
	pRouterMsg->nDataSize = sizeof(msg);
	memcpy(pRouterMsg->szData, &msg, sizeof(msg));

	pRouterInfo = CServiceMgr::get_instance().get_rand_router();
	LOG_PROCESS_ERROR(pRouterInfo);

	nRetCode = send(pRouterInfo->nTbusAddr, pRouterMsg, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSMSMessageHandler::do_sync_mgr_data_begin(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_DATA_BEGIN msg;

	msg.wMsg = service_mgr_sync_data_begin;
	msg.nLeaderMgrAddr = m_nTbusAddr;

	nRetCode = send(nDstAddr, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

struct TRAVERSE_SMS_OBJECT
{
    int32_t nCount;
	int32_t nDstServerAddr;
	SERVICE_MANAGE_SYNC_OBJ* msg;
    const static int32_t nSendCountEachTime = 1024;

	BOOL operator() (uint64_t qwObjID, SMS_OBJ* pObj)
	{
		msg->ObjInfo[nCount].qwObjID = qwObjID;
		msg->ObjInfo[nCount].nTbusAddr = pObj->nTbusAddr;
		msg->ObjInfo[nCount].dwLastUpdateTime = pObj->dwUpdateTime;
		nCount++;

		if (nCount >= nSendCountEachTime)
		{
            int32_t nRetCode = 0;
            uint32_t dwSize = sizeof(SERVICE_MANAGE_SYNC_OBJ) + sizeof(SERVICE_MANAGE_SYNC_OBJ::OBJ_INFO) * nCount;

			msg->nObjCount = nCount;
            nRetCode = CSMSMessageHandler::instance().send(nDstServerAddr, msg, dwSize);
            LOG_CHECK_ERROR(nRetCode);

            nCount = 0;
		}
		return TRUE;
	}
};

BOOL CSMSMessageHandler::do_sync_obj_info(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	uint32_t dwSize = 0;
	SERVICE_MANAGE_SYNC_OBJ* msg = NULL;
	TRAVERSE_SMS_OBJECT TraverseSmsObject;

	dwSize = sizeof(SERVICE_MANAGE_SYNC_OBJ) + TRAVERSE_SMS_OBJECT::nSendCountEachTime * sizeof(SERVICE_MANAGE_SYNC_OBJ::OBJ_INFO);

	msg = (SERVICE_MANAGE_SYNC_OBJ*)alloca(dwSize);
	msg->wMsg = service_mgr_sync_obj;

	TraverseSmsObject.msg = msg;
	TraverseSmsObject.nCount = 0;
	TraverseSmsObject.nDstServerAddr = nDstAddr;

	nRetCode = CSMSObjMgr::get_instance().traverse_session(TraverseSmsObject);
	LOG_CHECK_ERROR(nRetCode);

    if (TraverseSmsObject.nCount > 0)
    {
        dwSize = sizeof(SERVICE_MANAGE_SYNC_OBJ) + sizeof(SERVICE_MANAGE_SYNC_OBJ::OBJ_INFO) * TraverseSmsObject.nCount;

		msg->nObjCount = TraverseSmsObject.nCount;
        nRetCode = send(nDstAddr, msg, dwSize);
        LOG_CHECK_ERROR(nRetCode);
    }

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::do_sync_router_info(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	uint32_t dwSize = 0;
	SMS_ROUTER_MGR* pRouterMgr = NULL;
	SERVICE_MANAGE_SYNC_ROUTER* msg = NULL;

	pRouterMgr = CServiceMgr::get_instance().get_router_mgr();
	LOG_PROCESS_ERROR(pRouterMgr);

	dwSize = sizeof(SERVICE_MANAGE_SYNC_ROUTER) + pRouterMgr->nRouterCount * sizeof(SERVICE_MANAGE_SYNC_ROUTER::ROUTER_INFO);

	msg = (SERVICE_MANAGE_SYNC_ROUTER*)alloca(dwSize);
	msg->wMsg = service_mgr_sync_router;

	msg->nRouterCount = 0;
	for (int32_t i = 0; i < pRouterMgr->nRouterCount; i++)
	{
		msg->RouterInfo[i].nRouterAddr = pRouterMgr->RouterInfo[i].nTbusAddr;
		msg->RouterInfo[i].dwLastUpdateTime = pRouterMgr->RouterInfo[i].dwLastReportTime;
		msg->nRouterCount++;
	}

	nRetCode = send(nDstAddr, msg, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSMSMessageHandler::do_sync_mgr_data_end(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_DATA_END msg;

	msg.wMsg = service_mgr_sync_data_end;

	nRetCode = send(nDstAddr, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSMSMessageHandler::do_obj_info_ntf(uint64_t qwObjID, int32_t nTbusAddr, BOOL bRegister)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_OBJ_NTF msg;

	msg.wMsg = service_mgr_obj_ntf;
	msg.qwObjID = qwObjID;
	msg.nTbusAddr = nTbusAddr;
	msg.bRegister = bRegister;

	nRetCode = broadcast_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSMSMessageHandler::do_sync_in_service(void)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_IN_SERVICE msg;

	msg.wMsg = service_mgr_in_service;

	nRetCode = broadcast_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::do_sync_end_service(int32_t nServerType)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_END_SERVICE msg;

	LOG_PROCESS_ERROR(nServerType > svrInvalid && nServerType < svrTotal);

	msg.wMsg = service_mgr_end_service;

	switch (nServerType)
	{
	case svrRouter:
		nRetCode = broadcast_to_router(&msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
		break;
	case svrServiceMgr:
		nRetCode = broadcast_to_mgr(&msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
		break;
	default:
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::do_ntf_server_event(int32_t nEventType, int32_t nEventParam, int32_t nPhaseType)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_NTF_SERVER_EVENT msg;

	msg.wMsg = service_mgr_ntf_server_event;
	msg.nEventType = nEventType;
	msg.nEventParam = nEventParam;
	msg.nPhaseType = nPhaseType;

	nRetCode = broadcast_to_router(&msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::do_ntf_router_down(int32_t nRouterAddr)
{
	int32_t nRetCode = 0;
	int32_t nLeaderNodeID = 0;
	SERVICE_MANAGE_NTF_ROUTER_DOWN msg;

	msg.wMsg = service_mgr_ntf_router_down;
	msg.nRouterAddr = nRouterAddr;
	msg.nWatchMgrAddr = m_nTbusAddr;

	nLeaderNodeID = raft_service_get_curr_leader_nodeid(m_pRaftMgr);
	LOG_PROCESS_ERROR(nLeaderNodeID > 0);

	if (nLeaderNodeID != m_nTbusAddr)
	{
		nRetCode = send(nLeaderNodeID, &msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = CServiceMgr::get_instance().router_down_process(m_nTbusAddr, nRouterAddr);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CSMSMessageHandler::do_sync_server_info(int32_t nDstAddr)
{
	int32_t nRetCode = 0;
	int32_t dwSize = 0;
	int32_t nServerCount = 0;
	SERVICE_MANAGE_SYNC_SERVER_INFO* pMsg;

	nServerCount = CServiceMgr::get_instance().get_server_info_count();
	dwSize = sizeof(SERVICE_MANAGE_SYNC_SERVER_INFO) + sizeof(SERVER_INFO) * nServerCount;
	pMsg = (SERVICE_MANAGE_SYNC_SERVER_INFO*)alloca(dwSize);

	pMsg->wMsg = service_mgr_sync_server_info;
	for (int32_t i = 0; i < nServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(i);
		LOG_CHECK_ERROR(pServerInfo);

		if(pServerInfo)
			pMsg->ServerInfo[i] = *pServerInfo;
	}

	nRetCode = send(nDstAddr, pMsg, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

void CSMSMessageHandler::on_register(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
    int32_t nRetCode = 0;
	int32_t nServerType = tbus_get_type(nSrcAddr);
	
	switch (nServerType)
	{
		case svrRouter:
		{
			nRetCode = CServiceMgr::get_instance().add_router(nSrcAddr);
			LOG_PROCESS_ERROR(nRetCode);

			if(raft_service_get_status(m_pRaftMgr) == rsLeader)
			{
				nRetCode = do_sync_mgr_data_begin(nSrcAddr);
				LOG_CHECK_ERROR(nRetCode);

				nRetCode = do_sync_server_info(nSrcAddr);
				LOG_CHECK_ERROR(nRetCode);

				nRetCode = do_sync_obj_info(nSrcAddr);
				LOG_CHECK_ERROR(nRetCode);

				nRetCode = do_sync_mgr_data_end(nSrcAddr);
				LOG_CHECK_ERROR(nRetCode);

				INF("sync data to registered router %d", tbus_get_inst(nSrcAddr));
			}

			INF("[service mgr]: recv router %s registerd", tbus_get_str(nSrcAddr));
			break;
		}
		case svrServiceMgr:
		{
			INF("[service mgr]: recv service mgr %s registerd", tbus_get_str(nSrcAddr));
			break;
		}
		default:
			LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
    return;
}

void CSMSMessageHandler::on_control(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	A2A_CONTROL_REQ* msg = (A2A_CONTROL_REQ*)pBuffer;

	nRetCode = _process_control_cmd(msg->szCommand, msg->nCmdLen, msg->nMsgSrcAddr);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CSMSMessageHandler::on_router_unregister_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nServerType = 0;
	int32_t nServerIndex = 0;
	ROUTER_UNREGISTER_REPORT* msg = (ROUTER_UNREGISTER_REPORT*)pBuffer;
	
	LOG_PROCESS_ERROR(m_pShmMessageData);

	nServerType = tbus_get_type(msg->nServerAddr);
	nServerIndex = tbus_get_inst(msg->nServerAddr);

	INF("[service mgr]: recv unregister report %s unregister count %d", tbus_get_str(msg->nServerAddr), m_pShmMessageData->nUnRegisterServerCount+1);
		
	m_pShmMessageData->nUnRegisterServerCount++; 
	
	if (nServerType == svrGS)
	{
		SERVICE_INFO* pServiceInfo = NULL;
		int32_t nUnRegisterCount = 0;
		m_pShmMessageData->byUnRegisterGsFlag[nServerIndex / 8] |= (1 << (nServerIndex % 8)); 

		for (int32_t i = 0; i < MAX_SERVER_COUNT_PER_SERVICE / 8; i++)
		{
			int32_t nFlag = m_pShmMessageData->byUnRegisterGsFlag[i];
			for (; nFlag != 0; nFlag &= (nFlag - 1))
				nUnRegisterCount++;
		}

		pServiceInfo = CServiceMgr::get_instance().get_service_info(svrGS);
		if (pServiceInfo && pServiceInfo->nServerCount == nUnRegisterCount)
		{
			INF("recv all gs unregistered, begin to enter end service state");

			if (raft_service_get_status(m_pRaftMgr) == rsLeader)
			{
				nRetCode = do_sync_end_service(svrRouter);
				LOG_CHECK_ERROR(nRetCode);
			}

			mg_set_state(svstEndService);
			INF("set game state, state %d", mg_get_state());
		}
	}

Exit0:
	return;
}

void CSMSMessageHandler::on_router_event_prepare_report(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_EVENT_PREPARE_REPORT* msg = (ROUTER_EVENT_PREPARE_REPORT*)pBuffer;
	
	CSmsTransMgr::get_instance().add_trans_router_report(1);
	if (CSmsTransMgr::get_instance().get_trans_router_report() < CServiceMgr::get_instance().get_reported_router_count())
		return;

	CSmsTransMgr::get_instance().set_trans_router_report(0);

	nRetCode = do_ntf_server_event(msg->nEventType, msg->nEventParam, sopExecute);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CSMSMessageHandler::on_router_event_execute_report(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL;
	ROUTER_EVENT_EXECUTE_REPORT* msg = (ROUTER_EVENT_EXECUTE_REPORT*)pBuffer;

	INF("[trans_execute]: recv execute report, event_type %d, event_param %d, router_count %d", msg->nEventType, msg->nEventParam, CServiceMgr::get_instance().get_reported_router_count());

	CSmsTransMgr::get_instance().add_trans_router_report(1);
	if (CSmsTransMgr::get_instance().get_trans_router_report() < CServiceMgr::get_instance().get_reported_router_count())
		return;
	
	INF("[trans_execute]: recv all execute report, event_type %d, event_param %d", msg->nEventType, msg->nEventParam);

	switch (msg->nEventType)
	{
	case setServerDown:
	{
		pServerInfo = CServiceMgr::get_instance().get_server_info_by_addr(msg->nEventParam);
		LOG_PROCESS_ERROR(pServerInfo);

		pServerInfo->nState = svstDown;

		break;
	}
	case setServerRecover:
	{
		pServerInfo = CServiceMgr::get_instance().get_server_info_by_addr(msg->nEventParam);
		LOG_PROCESS_ERROR(pServerInfo);

		pServerInfo->nState = svstInService;

		break;
	}
	case setServerExpand:
	case setServerReduce:
	{
		SERVICE_INFO* pServiceInfo = NULL;
		int32_t nServiceType = msg->nEventParam >> 16;
		int32_t nServiceCount = msg->nEventParam & 0xFFFF;

		pServiceInfo = CServiceMgr::get_instance().get_service_info(nServiceType);
		LOG_PROCESS_ERROR(pServiceInfo);

		pServiceInfo->nState = svstInService;
		for (int32_t i = 0; i < nServiceCount; i++)
		{
			SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
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
				SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
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
	default:
		LOG_PROCESS_ERROR(FALSE);
	}
		
	CSmsTransMgr::get_instance().set_trans_router_report(0);

	// set data
	{
		int64_t llParam = ((int64_t)msg->nEventType << 32) + msg->nEventParam;
		set_raft_data(radtEventExcute, ((int64_t)(msg->nEventType) << 32) + msg->nEventParam);
	}

	
Exit0:
	return;
}

void CSMSMessageHandler::on_router_event_finish_report(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SMS_TRANS_CMD* pCmd = NULL;
	ROUTER_EVENT_FINISH_REPORT* msg = (ROUTER_EVENT_FINISH_REPORT*)pBuffer;

	CSmsTransMgr::get_instance().add_trans_router_report(1);
	if (CSmsTransMgr::get_instance().get_trans_router_report() < CServiceMgr::get_instance().get_reported_router_count())
		return;

	pCmd = CSmsTransMgr::get_instance().get_trans_front_cmd();
	LOG_PROCESS_ERROR(pCmd);

	INF("[trans]: trans finish! event_type %d service_type %d server_addr %s param %d", pCmd->nEventType, pCmd->nServiceType, tbus_get_str(pCmd->nServerAddr), pCmd->nParam);

	CSmsTransMgr::get_instance().pop_trans_front_cmd();

Exit0:
	return;
}
	
void CSMSMessageHandler::on_router_obj_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SMS_OBJ* pObj = NULL;
	ROUTER_OBJ_REPORT* msg = (ROUTER_OBJ_REPORT*)pBuffer;

	if (msg->bRegister)
	{
		pObj = CSMSObjMgr::get_instance().find_object(msg->qwObjID);
		if (pObj == NULL)
		{
			pObj = CSMSObjMgr::get_instance().new_object(msg->qwObjID);
			LOG_PROCESS_ERROR(pObj);
		}

		pObj->nTbusAddr = msg->nTbusAddr;
		pObj->dwUpdateTime = CTimeMgr::instance().get_time_sec();
	}
	else
	{
		pObj = CSMSObjMgr::get_instance().find_object(msg->qwObjID);
		LOG_PROCESS_ERROR(pObj);

		nRetCode = CSMSObjMgr::get_instance().del_object(pObj);
		LOG_PROCESS_ERROR(nRetCode);
	}

	if (raft_service_get_status(m_pRaftMgr) == rsLeader)
	{
		nRetCode = do_obj_info_ntf(msg->qwObjID, msg->nTbusAddr, msg->bRegister);
		LOG_PROCESS_ERROR(nRetCode);
	}

	INF("[service mgr]: recv router obj report, objid {%llx} server %s is_regist %d", msg->qwObjID, tbus_get_str(msg->nTbusAddr), msg->bRegister);

Exit0:
	return;
}

void CSMSMessageHandler::on_router_resume_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	ROUTER_RESUME_REPORT* msg = (ROUTER_RESUME_REPORT*)pBuffer;

	nRetCode = do_ntf_server_event(setServerResume, msg->nTbusAddr, sopExecute);
	LOG_CHECK_ERROR(nRetCode);

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_sync_data_begin(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;

	INF("[service mgr]: sync data begin");

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_sync_obj(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_SYNC_OBJ* msg = (SERVICE_MANAGE_SYNC_OBJ*)pBuffer;

	for (int32_t i = 0; i < msg->nObjCount; i++)
	{
		SMS_OBJ* pObj = NULL; 
		pObj = CSMSObjMgr::get_instance().find_object(msg->ObjInfo[i].qwObjID);
		if (pObj == NULL)
		{
			pObj = CSMSObjMgr::get_instance().new_object(msg->ObjInfo[i].qwObjID);
			LOG_PROCESS_ERROR(pObj);
		}
		pObj->nTbusAddr = msg->ObjInfo[i].nTbusAddr;
	
		INF("[service mgr]: recv peer mgr obj report, objid %llx server %s", msg->ObjInfo[i].qwObjID, tbus_get_str(msg->ObjInfo[i].nTbusAddr));
	}

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_sync_router(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SMS_ROUTER_MGR* pRouterMgr = NULL;
	SERVICE_MANAGE_SYNC_ROUTER* msg = (SERVICE_MANAGE_SYNC_ROUTER*)pBuffer;

	pRouterMgr = CServiceMgr::get_instance().get_router_mgr();
	LOG_PROCESS_ERROR(pRouterMgr);

	pRouterMgr->nRouterCount = 0;
	for (int32_t i = 0; i < msg->nRouterCount; i++)
	{
		pRouterMgr->RouterInfo[i].nTbusAddr = msg->RouterInfo[i].nRouterAddr;
		pRouterMgr->RouterInfo[i].dwLastReportTime = msg->RouterInfo[i].dwLastUpdateTime;

		pRouterMgr->nRouterCount++;

		INF("[service mgr]: recv peer mgr router report, router addr %s upate_time %d", tbus_get_str(msg->RouterInfo[i].nRouterAddr), msg->RouterInfo[i].dwLastUpdateTime);
	}

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_sync_data_end(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	INF("service mgr sync data end, set self backup");
}

void CSMSMessageHandler::on_service_mgr_end_service(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	INF("recv end service info");
	mg_set_state(svstEndService);

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_router_down(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_NTF_ROUTER_DOWN* msg = (SERVICE_MANAGE_NTF_ROUTER_DOWN*)pBuffer;

	nRetCode = CServiceMgr::get_instance().router_down_process(msg->nWatchMgrAddr, msg->nRouterAddr);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CSMSMessageHandler::on_service_mgr_raft_msg(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	SERVICE_MANAGE_RAFT_MSG* msg = (SERVICE_MANAGE_RAFT_MSG*)pBuffer;

	nRetCode = raft_service_recv_msg(m_pRaftMgr, nSrcAddr, msg->nMsgType, msg->szMsgData);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

int32_t CSMSMessageHandler::_judge_server_node_status(SERVER_INFO* pCurrInfo, BOOL bAlive)
{
	int32_t nRetCode = 0;
	int32_t nState = svstInvalid;

	LOG_PROCESS_ERROR(pCurrInfo);

	switch (pCurrInfo->nState)
	{
		case svstAvailable:
		{
			if (bAlive)
				nState = svstAvailable;
			else
				nState = svstDown;
			break;
		}
		case svstInService:
		{
			if (bAlive)
				nState = pCurrInfo->nState;
			else
			{
				nState = pCurrInfo->nState;

				if (!CSmsTransMgr::get_instance().is_server_in_cmd_list(pCurrInfo->nTbusAddr))
				{
					SMS_TRANS_CMD cmd;
					cmd.nEventType = setServerDown;
					cmd.nServerAddr = pCurrInfo->nTbusAddr;
					cmd.nServiceType = tbus_get_type(pCurrInfo->nTbusAddr);

					nRetCode = CSmsTransMgr::get_instance().add_trans_cmd(cmd);
					LOG_PROCESS_ERROR(nRetCode);
				}
				
				INF("[service mgr]: detect server %s is down", tbus_get_str(pCurrInfo->nTbusAddr));
			}
			break;
		}
		case svstDown:
		{
			if (!bAlive)
				nState = pCurrInfo->nState;
			else
			{
				nState = pCurrInfo->nState;

				if (!CSmsTransMgr::get_instance().is_server_in_cmd_list(pCurrInfo->nTbusAddr))
				{
					SMS_TRANS_CMD cmd;
					cmd.nEventType = setServerRecover;
					cmd.nServerAddr = pCurrInfo->nTbusAddr;
					cmd.nServiceType = tbus_get_type(pCurrInfo->nTbusAddr);

					nRetCode = CSmsTransMgr::get_instance().add_trans_cmd(cmd);
					LOG_PROCESS_ERROR(nRetCode);
				}

				INF("[service mgr]: detect server %s is recover", tbus_get_str(pCurrInfo->nTbusAddr));
			}
			break;
		}
		case svstTransfer:
			nState = pCurrInfo->nState;
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
	}

Exit0:
	return nState;
}

BOOL CSMSMessageHandler::_process_control_cmd(const char* pcszCommand, int32_t nLen, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	std::string sResult = "unkown error";
	std::vector<std::string> vControlCmd;

	vControlCmd = strsep(pcszCommand, " ");
	LOG_PROCESS_ERROR(vControlCmd.size() >= 4);
	LOG_PROCESS_ERROR(vControlCmd[0].compare("mgr") == 0);

	if (vControlCmd[1].compare("expand") == 0)
	{
		int32_t nServiceType = atoi(vControlCmd[2].c_str());
		int32_t nAfterExpandServerCount = atoi(vControlCmd[3].c_str());

		nRetCode = _process_control_cmd_expand(nServiceType, nAfterExpandServerCount, sResult);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if (vControlCmd[1].compare("reduce") == 0)
	{
		int32_t nServiceType = atoi(vControlCmd[2].c_str());
		int32_t nAfterReduceServerCount = atoi(vControlCmd[3].c_str());

		nRetCode = _process_control_cmd_reduce(nServiceType, nAfterReduceServerCount, sResult);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		sResult = "command not supported";
		LOG_PROCESS_ERROR(FALSE);
	}

	nRetCode = do_control_ack(0, "expand sucess", nSrcAddr);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;

Exit0:
	nRetCode = do_control_ack(0, sResult.c_str(), nSrcAddr);
	LOG_CHECK_ERROR(nRetCode);

	return FALSE;
}

BOOL CSMSMessageHandler::_process_control_cmd_expand(int32_t nServiceType, int32_t nAfterExpandServerCount, std::string& sErrStr)
{
	SMS_TRANS_CMD cmd;
	int32_t nRetCode = 0;
	SERVICE_INFO* pServiceInfo = NULL;

	LOG_PROCESS_ERROR(nServiceType > svrInvalid && nServiceType < svrTotal);

	pServiceInfo = CServiceMgr::get_instance().get_service_info(nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);
	LOG_PROCESS_ERROR(pServiceInfo->nServerCount > pServiceInfo->nServiceServerCount);
	LOG_PROCESS_ERROR(nAfterExpandServerCount > pServiceInfo->nServiceServerCount);
	LOG_PROCESS_ERROR(nAfterExpandServerCount <= pServiceInfo->nServerCount);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_PROCESS_ERROR(pServerInfo);
		LOG_PROCESS_ERROR(pServerInfo->nState == svstInService);
	}

	for (int32_t i = pServiceInfo->nServiceServerCount; i < pServiceInfo->nServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_PROCESS_ERROR(pServerInfo);
		LOG_PROCESS_ERROR(pServerInfo->nState == svstAvailable);
	}

	cmd.nServiceType = nServiceType;
	cmd.nServerAddr = 0;
	cmd.nEventType = setServerExpand;
	cmd.nParam = nAfterExpandServerCount;

	nRetCode = CSmsTransMgr::get_instance().add_trans_cmd(cmd);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::_process_control_cmd_reduce(int32_t nServiceType, int32_t nAfterReduceServerCount, std::string& sErrStr)
{
	SMS_TRANS_CMD cmd;
	int32_t nRetCode = 0;
	SERVICE_INFO* pServiceInfo = NULL;

	LOG_PROCESS_ERROR(nServiceType > svrInvalid && nServiceType < svrTotal);

	pServiceInfo = CServiceMgr::get_instance().get_service_info(nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);
	LOG_PROCESS_ERROR(nAfterReduceServerCount > 0);
	LOG_PROCESS_ERROR(nAfterReduceServerCount < pServiceInfo->nServiceServerCount);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CServiceMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_PROCESS_ERROR(pServerInfo);
		LOG_PROCESS_ERROR(pServerInfo->nState == svstInService);
	}

	cmd.nServiceType = nServiceType;
	cmd.nServerAddr = 0;
	cmd.nEventType = setServerReduce;
	cmd.nParam = nAfterReduceServerCount;

	nRetCode = CSmsTransMgr::get_instance().add_trans_cmd(cmd);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

void CSMSMessageHandler::on_router_heart_beat(const char * pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	A2A_PING msg;
	SMS_ROUTER_INFO* pRouter = NULL;

	pRouter = CServiceMgr::get_instance().get_router(nSrcAddr);
	PROCESS_ERROR(pRouter);

	pRouter->dwLastPingTime = CTimeMgr::instance().get_time_sec();

Exit0:
	return;
}

void CSMSMessageHandler::on_router_alive_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr)
{
	int32_t nRetCode = 0;
	int32_t nRspSize = 0;
	int32_t nSendCount = 0;
	SMS_ROUTER_INFO* pRouterInfo = NULL;
	SERVICE_MANAGE_SERVER_INFO_NTF* pRsp = NULL;
	ROUTER_ALIVE_REPORT* msg = (ROUTER_ALIVE_REPORT*)pBuffer;

	if (mg_get_stop_timer() > 0 || mg_get_state() == svstEndService)
		return;

	pRouterInfo = CServiceMgr::get_instance().get_router(nSrcAddr);
	LOG_PROCESS_ERROR(pRouterInfo);

	pRouterInfo->dwLastReportTime = CTimeMgr::instance().get_time_sec();
	pRouterInfo->nState = svstInService;

	nRspSize = sizeof(SERVICE_MANAGE_SERVER_INFO_NTF) + sizeof(SERVER_INFO) * MAX_SERVER_PER_ROUTER;
	pRsp = (SERVICE_MANAGE_SERVER_INFO_NTF*)alloca(nRspSize);
	LOG_PROCESS_ERROR(pRsp);

	pRsp->wMsg = service_mgr_server_info_ntf;
	pRsp->nServerInfoCount = 0;

	for (int32_t i = 0; i < msg->nReportCount; i++)
	{
		SERVER_INFO CurServerInfo;
		SERVER_INFO* pServerInfo;

		CurServerInfo.nTbusAddr = msg->ReportInfo[i].nTbusAddr;
		CurServerInfo.nLoadValue = msg->ReportInfo[i].nLoadValue;
		CurServerInfo.nRouterAddr = nSrcAddr;

		pServerInfo = CServiceMgr::get_instance().get_server_info_by_addr(CurServerInfo.nTbusAddr);
		if (pServerInfo == NULL)
		{
			CurServerInfo.nState = svstAvailable;
			nRetCode = CServiceMgr::get_instance().add_server_info(CurServerInfo);
			LOG_PROCESS_ERROR(nRetCode);

			pRsp->ServerInfo[pRsp->nServerInfoCount++] = CurServerInfo;
		}
		else
		{
			//check server node state
			CurServerInfo.nState = _judge_server_node_status(pServerInfo, msg->ReportInfo[i].byAlive);

			if(CurServerInfo.nLoadValue != pServerInfo->nLoadValue || CurServerInfo.nState != pServerInfo->nState)
				pRsp->ServerInfo[pRsp->nServerInfoCount++] = CurServerInfo;

			*pServerInfo = CurServerInfo;
		}

		/*
		std::string server_str = get_tbus_str(CurServerInfo.nTbusAddr);
		std::string router_str = get_tbus_str(CurServerInfo.nRouterAddr);
		INF("[service mgr]: recv router alive report, server %s router %s state %d load %d", server_str.c_str(), 
						router_str.c_str(), CurServerInfo.nState, CurServerInfo.nLoadValue);
		*/	
	}

	if(raft_service_get_status(m_pRaftMgr) == rsLeader)
	{
		nRspSize = sizeof(SERVICE_MANAGE_SERVER_INFO_NTF) + sizeof(SERVER_INFO) * pRsp->nServerInfoCount;
		
		nRetCode = broadcast_to_router(pRsp, nRspSize);
		LOG_PROCESS_ERROR(nRetCode);
	}

	if (!is_state_ready(smsAllRouterReport) && 
			CServiceMgr::get_instance().get_reported_router_count() >= m_pShmMessageData->nInitRouterCount)
	{
		INF("[service mgr]: all router is reported!");

		set_state(smsAllRouterReport);
	}

Exit0:
	return;
}
	
BOOL CSMSMessageHandler::broadcast_to_router(const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	SMS_ROUTER_MGR* pRouterMgr = NULL;

	pRouterMgr = CServiceMgr::get_instance().get_router_mgr();
	LOG_PROCESS_ERROR(pRouterMgr);
	
	for (int32_t i = 0; i < pRouterMgr->nRouterCount; i++)
	{
		nRetCode = send(pRouterMgr->RouterInfo[i].nTbusAddr, pBuffer, dwSize);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::broadcast_to_mgr(const void * pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	
	for (int32_t i = 0; i < m_pShmMessageData->nMgrCount; i++)
	{
		nRetCode = send(m_pShmMessageData->nMgrAddr[i], pBuffer, dwSize);
		LOG_CHECK_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::set_raft_data(int32_t nType, int64_t llParam)
{
	int32_t nRetCode = 0;
	char* pszData = NULL;
	int32_t nDataSize = 0;

	switch (nType)
	{
	case radtInitServiceInfo:
	case radtEventExcute:
	{
		nRetCode = CServiceMgr::get_instance().pack_service_data(&pszData, nDataSize);
		LOG_PROCESS_ERROR(nRetCode);
		LOG_PROCESS_ERROR(pszData);
		LOG_PROCESS_ERROR(nDataSize > 0);

		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}
		
	nRetCode = raft_service_set_data(m_pRaftMgr, nType, llParam, pszData, nDataSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
void CSMSMessageHandler::set_state(int32_t nState)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pShmMessageData);
	LOG_PROCESS_ERROR(nState > smsInvalid && nState < smsTotal);

	m_pShmMessageData->nState |= nState;

	if (is_all_state_ready())
	{
		nRetCode = set_raft_data(radtInitServiceInfo, 0);
		LOG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return;
}

BOOL CSMSMessageHandler::is_state_ready(int32_t nState)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pShmMessageData);
	LOG_PROCESS_ERROR(nState > smsInvalid && nState < smsTotal);

	return m_pShmMessageData->nState & nState;
Exit0:
	return FALSE;
}

BOOL CSMSMessageHandler::is_all_state_ready(void)
{
	LOG_PROCESS_ERROR(m_pShmMessageData);

	PROCESS_ERROR(m_pShmMessageData->nState & smsSelectLeaderFinish);
	PROCESS_ERROR(m_pShmMessageData->nState & smsAllRouterReport);

	return TRUE;
Exit0:
	return FALSE;
}