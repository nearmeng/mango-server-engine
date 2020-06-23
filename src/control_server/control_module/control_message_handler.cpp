#include "stdafx.h"
#include "control_message_handler.h"

#include "define/server_def.h"

#include "game_data/global_config.h"
#include "protocol/common_message.h"
#include "app/server_msg_handler.h"
#include "tbus/tbus_wrapper.h"

void on_a2a_control_ack(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	char szResult[512] = { 0 };
	A2A_CONTROL_ACK* msg = (A2A_CONTROL_ACK*)pBuffer;

	snprintf(szResult, sizeof(szResult), "recv server %s result: %s \n\r", tbus_get_str(nSrcAddr), msg->szDesc);
    nRetCode = tappctrl_send_string(szResult);

Exit0:
	return;
}

BOOL CControlModule::_msg_handler_init()
{
	register_server_msg_handler(a2a_control_ack, on_a2a_control_ack);
	return TRUE;
}
BOOL CControlModule::_msg_handler_uninit()
{
	return TRUE;
}

BOOL co_msg_send_cmd_by_roleid(const char* pcszCmdType, const char* pcszCmd, uint64_t qwRoleID)
{
	int32_t nRetCode = 0;
    A2A_CONTROL_REQ msg;

	LOG_PROCESS_ERROR(pcszCmdType);
	LOG_PROCESS_ERROR(pcszCmd);
	LOG_PROCESS_ERROR(qwRoleID > 0);

	strxcpy(msg.szCommandType, pcszCmdType, sizeof(msg.szCommandType));
    strxcpy(msg.szCommandContent, pcszCmd, sizeof(msg.szCommandContent));
	msg.qwParam = qwRoleID;

	nRetCode = send_server_msg_by_objid(qwRoleID, a2a_control_req, &msg, sizeof(msg));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL co_msg_send_cmd_by_addr(const char* pcszCmdType, const char* pcszCmd, int32_t nServerAddr)
{
    int32_t nRetCode = 0;
    int32_t nServerType = tbus_get_type(nServerAddr);
    int32_t nServerInst = tbus_get_inst(nServerAddr);
	A2A_CONTROL_REQ msg;

	LOG_PROCESS_ERROR(nServerType != svrRouter);

	strxcpy(msg.szCommandType, pcszCmdType, sizeof(msg.szCommandType));
	strxcpy(msg.szCommandContent, pcszCmd, sizeof(msg.szCommandContent));
	msg.qwParam = 0;

	if (nServerType == svrServiceMgr)
	{
		nRetCode = send_server_msg_to_mgr(a2a_control_req, &msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if (nServerType == 0)
	{
		for (int32_t nServerType = svrInvalid; nServerType < svrTotal; nServerType++)
		{
			if(nServerType == svrTconnd || nServerType == svrControl || nServerType == svrRouter || nServerType == svrServiceMgr)
				continue;
		
			nRetCode = send_server_msg_by_service_type(nServerType, a2a_control_req, &msg, sizeof(msg));
			LOG_PROCESS_ERROR(nRetCode);
		}
	}
    else if (nServerInst == 0)
    {
		nRetCode = send_server_msg_by_service_type(nServerType, a2a_control_req, &msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
    }
	else
	{
		nRetCode = send_server_msg_by_addr(nServerAddr, a2a_control_req, &msg, sizeof(msg));
		LOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}
