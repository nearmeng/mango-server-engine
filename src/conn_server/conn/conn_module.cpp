#include "stdafx.h"
#include "conn_module.h"
#include "conn_msg_handler.h"

#include "config/global_config.h"

#include "protocol/external_message.pb.h"
#include "protocol/proto_msgid.pb.h"

#include "tconnd/inc/tconnapi/tframehead.h"

BOOL CConnModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = m_SessionMgr.init(stdSession, g_ServerConfig.CONN.nInitSessionMgrCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _init_msg_handler();
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
    
BOOL CConnModule::kick_conn_by_session(uint64_t qwSessionID, int32_t nReason)
{
    int32_t nRetCode = 0;
    CONN_SESSION* pSession = NULL;
    SC_MESSAGE_ERROR_CODE msg;

    pSession = find_session(qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    msg.set_error_code(nReason);
    nRetCode = send_to_client(pSession, sc_message_error_code, msg);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = send_to_tconnd(pSession, TFRAMEHEAD_CMD_STOP, NULL, 0);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}