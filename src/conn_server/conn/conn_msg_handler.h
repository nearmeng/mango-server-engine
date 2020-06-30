#ifndef _CONN_MSG_HANDLER_H_
#define _CONN_MSG_HANDLER_H_

#include "conn_module.h"

namespace google{	
	namespace protobuf {
		class Message;
	}
}

BOOL send_to_client(CONN_SESSION* pSession, int32_t nMsgID, const google::protobuf::Message& msg);
BOOL send_to_client(CONN_SESSION* pSession, const char* pBuffer, uint32_t dwSize);
BOOL send_to_tconnd(CONN_SESSION* pSession, int32_t nCmd, const char* pBuffer, uint32_t dwSize);

#endif