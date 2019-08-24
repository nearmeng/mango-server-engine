#ifndef _SERVER_MSG_HANDLER_H_
#define _SERVER_MSG_HANDLER_H_

namespace google{	
	namespace protobuf {
		class Message;
	}
}
using google::protobuf::Message;
class CS_HEAD;
class SS_HEAD;
typedef struct tagTFrameHead TFRAMEHEAD;

BOOL recv_conn_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize);
BOOL recv_server_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize);

BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nBuffLen);

typedef void(*CONN_MSG_HANDLER)(int32_t nSrcAddr, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize);
BOOL register_conn_msg_handler(int32_t nEventType, CONN_MSG_HANDLER pMsgHandler);

typedef void(*CLIENT_MSG_HANDLER)(int32_t nSrcAddr, CS_HEAD* pHead, const Message* pMsg);
BOOL register_client_msg_handler(int32_t nCSMsgID, CLIENT_MSG_HANDLER pMsgHandler);

typedef void(*SERVER_MSG_HANDLER)(int32_t nSrcAddr, SS_HEAD* pHead, const Message* pMsg);
BOOL register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler);

#endif