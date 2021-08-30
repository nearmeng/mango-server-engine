#include "stdafx.h"
#include "conn_module.h"
#include "conn_msg_handler.h"

#include "config/global_config.h"

#include "protocol/external_message.pb.h"
#include "protocol/proto_msgid.pb.h"

#include "tconnd/inc/tconnapi/tframehead.h"

MG_REGISTER_MODULE(CConnModule);

BOOL CConnModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = m_SessionMgr.init(stdSession, g_ServerConfig.CONN.nInitSessionMgrCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _init_msg_handler();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _init_tconnd_addr();
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CConnModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_SessionMgr.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CConnModule::kick_conn_by_session(uint64_t qwSessionID, int32_t nReason, uint64_t qwParam)
{
    int32_t nRetCode = 0;
    CONN_SESSION* pSession = NULL;
    SC_ERROR_CODE msg;

    pSession = find_session(qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    msg.set_error_code(nReason);
    msg.set_param(qwParam);
    nRetCode = send_to_client(pSession, sc_error_code, msg);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = send_to_tconnd(pSession, TFRAMEHEAD_CMD_STOP, NULL, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_SessionMgr.delete_object(pSession);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
	
BOOL CConnModule::_init_tconnd_addr(void)
{
	int32_t nRetCode = 0;
	int32_t nPeerCount = 0;
    int32_t* pnAddrList = NULL;
	int32_t nTbusHandle = CMGApp::instance().get_tbus_handle();
	int32_t nTbusAddr = CMGApp::instance().get_tbus_addr();
	
	nRetCode = tbus_get_peer_count(nTbusHandle, nTbusAddr, &nPeerCount);
	LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	pnAddrList = (int32_t*)alloca(sizeof(int32_t) * nPeerCount);
	LOG_PROCESS_ERROR(pnAddrList);

	nRetCode = tbus_get_peer_addr(nTbusHandle, nTbusAddr, pnAddrList, &nPeerCount);
	LOG_PROCESS_ERROR(nRetCode == TBUS_SUCCESS);

	m_nTconndAddrCount = 0;
	for (int32_t nIndex = 0; nIndex < nPeerCount; nIndex++)
	{
		int32_t nServerType = tbus_get_type(pnAddrList[nIndex]);
		if (nServerType == svrTconnd)
		{
			LOG_PROCESS_ERROR(m_nTconndAddrCount < MAX_TCONND_COUNT);
			m_nTconndAddr[m_nTconndAddrCount++] = pnAddrList[nIndex];
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}