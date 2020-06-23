#ifndef _SERVER_MSG_HANDLER_H_
#define _SERVER_MSG_HANDLER_H_

namespace google{	
	namespace protobuf {
		class Message;
	}
}
using google::protobuf::Message;
class CS_HEAD;
class SC_HEAD;
class SS_HEAD;
typedef struct tagTFrameHead TFRAMEHEAD;

BOOL unpack_client_msg_head(const char* pBuff, int32_t nSize, char* pbyHeadLen, Message* pHead);

BOOL recv_conn_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize);
BOOL recv_server_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize);
BOOL recv_client_msg_proc(uint64_t qwConnID, const char* pBuff, int32_t nSize);

BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD* pFrameHead, const SC_HEAD* pHead, const Message* pMsg);

BOOL send_server_msg_by_routerid(uint64_t qwRouterID, int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_service_type(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_service_inst(int32_t nServiceType, int32_t nInstID, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_addr(int32_t nDstServerAddr, int32_t nMsgID,const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_objid(uint64_t qwObjID, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_load(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_to_mgr(int32_t nMsgID, const void* pBuffer, size_t dwSize);

typedef void(*CONN_MSG_HANDLER)(uint64_t qwConnID, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize);
BOOL register_conn_msg_handler(int32_t nEventType, CONN_MSG_HANDLER pMsgHandler);

typedef void(*CLIENT_MSG_HANDLER)(int32_t nSrcAddr, const CS_HEAD* pHead, const Message* pMsg);
BOOL register_client_msg_handler(int32_t nCSMsgID, CLIENT_MSG_HANDLER pMsgHandler);

typedef void(*SERVER_MSG_HANDLER)(int32_t nSrcAddr, const char* pBuffer, size_t dwSize);
BOOL register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler);

#endif