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

void recv_conn_transfer_msg(int32_t nSrcAddr, const char* pBuff, size_t dwSize);
void recv_conn_ntf_event(int32_t nSrcAddr, const char* pBuff, size_t dwSize);

//api for server connected with tconnd directly
BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD* pFrameHead, const SC_HEAD* pHead, const Message* pMsg);
BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize);
//api for server connected with conn server
BOOL send_to_client(int32_t nConnServerAddr, uint64_t qwConnID, int32_t nMsgID, const Message* pMsg);
BOOL send_to_conn_server(int32_t nConnServerAddr, uint64_t qwConnID, int32_t nEventType, uint64_t qwEventParam0, uint64_t qwEventParam1);

BOOL send_server_msg_by_routerid(uint64_t qwRouterID, int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_by_service_type(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_by_service_inst(int32_t nServiceType, int32_t nInstID, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_by_addr(int32_t nDstServerAddr, int32_t nMsgID,const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_by_objid(uint64_t qwObjID, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_by_load(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);
BOOL send_server_msg_to_mgr(int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID = 0);

BOOL send_server_msg_by_routerid_coro(uint64_t qwRouterID, int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_service_type_coro(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_service_inst_coro(int32_t nServiceType, int32_t nInstID, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_addr_coro(int32_t nDstServerAddr, int32_t nMsgID,const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_objid_coro(uint64_t qwObjID, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_by_load_coro(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize);
BOOL send_server_msg_to_mgr_coro(int32_t nMsgID, const void* pBuffer, size_t dwSize);

typedef void(*CONN_EVENT_HANDLER)(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr);
BOOL register_conn_event_handler(int32_t nEventType, CONN_EVENT_HANDLER pEventHandler);

typedef void(*CONN_MSG_HANDLER)(uint64_t qwConnID, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize);
BOOL register_conn_msg_handler(int32_t nMsgType, CONN_MSG_HANDLER pMsgHandler);

typedef void(*CLIENT_MSG_HANDLER)(uint64_t qwConnID, const CS_HEAD* pHead, const Message* pMsg);
BOOL register_client_msg_handler(int32_t nCSMsgID, CLIENT_MSG_HANDLER pMsgHandler);

typedef void(*SERVER_MSG_HANDLER)(int32_t nSrcAddr, const char* pBuffer, size_t dwSize);
BOOL register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler);

#endif