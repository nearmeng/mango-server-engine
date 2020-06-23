#ifndef _CO_MESSAGE_HANDLER_H_
#define _CO_MESSAGE_HANDLER_H_

#include "control_module.h"

BOOL co_msg_send_cmd_by_roleid(const char* pcszCmdType, const char* pcszCmd, uint64_t qwRoleID);
BOOL co_msg_send_cmd_by_addr(const char* pcszCmdType, const char* pcszCmd, int32_t nServerAddr);

#endif	// _GS_MESSAGE_HANDLER_H_
