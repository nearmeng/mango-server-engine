#include "stdafx.h"
#include "app/server_msg_handler.h"

#include "tconnd/inc/tconnapi/tframehead.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

#include "protocol/proto_head.pb.h"
#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/internal_message_header.h"

#include "tbus/tbus_wrapper.h"
#include "app/server_app.h"

#include "router_client/router_client_api.h"

#define MAX_MESSAGE_ID			(65535)
#define MAX_CONN_EVENT_COUNT	(64)
#define MAX_SS_MSG_LEN			(256 * 1024)
#define MAX_SEND_DATA_BUFF_LEN	(512 * 1024)

static CONN_MSG_HANDLER g_ConnMsgHandler[MAX_CONN_EVENT_COUNT];
static CLIENT_MSG_HANDLER g_ClientMsgHandler[MAX_MESSAGE_ID];
static SERVER_MSG_HANDLER g_ServerMsgHandler[MAX_MESSAGE_ID];
static Message* g_CacheMsgInst[MAX_MESSAGE_ID];

#define INIT_INTERNAL_MSG_VEC()					\
	INTERNAL_MESSAGE_HEADER stHeader; struct iovec vecs[2];				\
	stHeader.wMsg = nMsgID;	 stHeader.nMsgSrcAddr = mg_get_tbus_addr();	\
	vecs[0].iov_base = (void*)(&stHeader);	vecs[0].iov_len = sizeof(stHeader);	\
	vecs[1].iov_base = (void*)pBuffer;		vecs[1].iov_len = dwSize;
	

static Message* get_msg_inst(int32_t nMsgID)
{
	int32_t nRetCode = 0;
    std::string sMsgName;
	Message* pMessage = NULL;

    const google::protobuf::Descriptor* pDescriptor = NULL;
    const google::protobuf::Message* pType = NULL;
    const google::protobuf::Reflection* pRef = NULL;
    google::protobuf::Message* msg = NULL;

	if (g_CacheMsgInst[nMsgID])
		return g_CacheMsgInst[nMsgID];

    sMsgName = CS_MESSAGE_ID_Name((CS_MESSAGE_ID)nMsgID);
    transform(sMsgName.begin(), sMsgName.end(), sMsgName.begin(), ::toupper);

    pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(sMsgName.c_str());
    LOG_PROCESS_ERROR_DETAIL(pDescriptor, "can not find msg, name: %s", sMsgName.c_str());

    pType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    LOG_PROCESS_ERROR(pType);

    pMessage = pType->New();
    LOG_PROCESS_ERROR(pMessage);

	g_CacheMsgInst[nMsgID] = pMessage;

Exit0:
	return pMessage;
}

static BOOL pack_msg(char* pBuff, int32_t* nSize, const Message* pHead, const Message* pMsg)
{
	int32_t nRetCode = 0;
	int32_t nHeadLen = 0;
	int32_t nBodyLen = 0;

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize);

	nRetCode = pHead->SerializeToArray(pBuff + 1, *nSize - 1);
	LOG_PROCESS_ERROR(nRetCode);

	nHeadLen = pHead->GetCachedSize();
	LOG_PROCESS_ERROR(nHeadLen > 0 && nHeadLen < 0xFF);

	*((char*)pBuff) = nHeadLen;

	nRetCode = pMsg->SerializeToArray(pBuff + 1 + nHeadLen, *nSize - 1 - nHeadLen);
	LOG_PROCESS_ERROR(nRetCode);

	nBodyLen = pMsg->GetCachedSize();

	*nSize = 1 + nHeadLen + nBodyLen;

	return TRUE;
Exit0:
	return FALSE;
}


static BOOL unpack_msg(const char* pBuff, int32_t nSize, Message* pHead, Message** pMsg)
{
	int32_t nRetCode = 0;
	char byHeadlen = 0;
	int32_t nBodyLen = 0;
	int32_t nMsgID = 0;
	Message* pMsgInst = NULL;
	CS_HEAD* pCSHead = (CS_HEAD*)pHead;

	LOG_PROCESS_ERROR(pHead);
	LOG_PROCESS_ERROR(nSize > 0);

	byHeadlen = *(char*)pBuff;

	nRetCode = pHead->ParseFromArray(pBuff + 1, byHeadlen);
	LOG_PROCESS_ERROR(nRetCode);
	
	nMsgID = pCSHead->msgid();
	pMsgInst = get_msg_inst(nMsgID);
	LOG_PROCESS_ERROR(pMsgInst);

	nBodyLen = nSize - 1 - byHeadlen;
	nRetCode = pMsgInst->ParseFromArray(pBuff + 1 + byHeadlen, nBodyLen);
	LOG_PROCESS_ERROR(nRetCode);

	*pMsg = pMsgInst;

	return TRUE;
Exit0:
	return FALSE;
}

static void recv_client_msg_proc(int32_t nSrcAddr, TFRAMEHEAD* rFramHead, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	CS_HEAD CsHead;
	Message* pMsg = NULL;

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize > 0);

	nRetCode = unpack_msg(pBuff, nSize, &CsHead, &pMsg);
	LOG_PROCESS_ERROR(nRetCode);
	LOG_PROCESS_ERROR_DETAIL(g_ClientMsgHandler[CsHead.msgid()], "client msg is not registerd, msgid: %d", CsHead.msgid());

	g_ClientMsgHandler[CsHead.msgid()](nSrcAddr, &CsHead, pMsg);

Exit0:
	return;
}

BOOL recv_conn_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	TFRAMEHEAD FrameHead;
	int32_t nHeadLen = sizeof(FrameHead);

	nRetCode = tconnapi_decode(pBuff, nSize, &FrameHead, &nHeadLen);
	LOG_PROCESS_ERROR(nRetCode == 0);

	//default proc
	if (FrameHead.chCmd == TFRAMEHEAD_CMD_INPROC)
	{
		if (g_ConnMsgHandler[FrameHead.chCmd] == NULL)
		{
			recv_client_msg_proc(nSrcAddr, &FrameHead, pBuff + nHeadLen, nSize - nHeadLen);
			return TRUE;
		}
	}

	LOG_PROCESS_ERROR(g_ConnMsgHandler[FrameHead.chCmd]);
	g_ConnMsgHandler[FrameHead.chCmd](nSrcAddr, &FrameHead, pBuff + nHeadLen, nSize - nHeadLen);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL recv_server_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	INTERNAL_MESSAGE_HEADER* pHeader = (INTERNAL_MESSAGE_HEADER*)pBuff;

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize > 0);
	LOG_PROCESS_ERROR(pHeader->wMsg >= internal_message_begin && pHeader->wMsg <= internal_message_end);
	LOG_PROCESS_ERROR_DETAIL(g_ServerMsgHandler[pHeader->wMsg], "msgid is %d", pHeader->wMsg);

	g_ServerMsgHandler[pHeader->wMsg](pHeader->nMsgSrcAddr, pBuff + sizeof(INTERNAL_MESSAGE_HEADER), nSize - sizeof(INTERNAL_MESSAGE_HEADER));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD * pFrameHead, const SC_HEAD* pHead, const Message* pMsg)
{
	int32_t nRetCode = 0;
	char szHeadBuff[sizeof(TFRAMEHEAD) * 2];
	int32_t nHeadLen = sizeof(szHeadBuff);
	char szDataBuff[MAX_SEND_DATA_BUFF_LEN];
	int32_t nDataLen = sizeof(szDataBuff);
	struct iovec vecs[2];

	LOG_PROCESS_ERROR(pFrameHead);

	nRetCode = tconnapi_encode(szHeadBuff, &nHeadLen, pFrameHead);
	LOG_PROCESS_ERROR(nRetCode == 0);

	if (pHead && pMsg)
	{
		nRetCode = pack_msg(szDataBuff, &nDataLen, pHead, pMsg);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
		nDataLen = 0;

	vecs[0].iov_len = nHeadLen;
	vecs[0].iov_base = szHeadBuff;
	vecs[1].iov_len = nDataLen;
	vecs[1].iov_base = (char*)szDataBuff;

	return tbus_sendv_data(nDstAddr, vecs, 2);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL register_conn_msg_handler(int32_t nEventType, CONN_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(nEventType >= 0 && nEventType < MAX_CONN_EVENT_COUNT);
	LOG_PROCESS_ERROR(g_ConnMsgHandler[nEventType] == NULL);

	g_ConnMsgHandler[nEventType] = pMsgHandler;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL register_client_msg_handler(int32_t nCSMsgID, CLIENT_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(nCSMsgID > 0 && nCSMsgID < MAX_MESSAGE_ID);
	LOG_PROCESS_ERROR(g_ClientMsgHandler[nCSMsgID] == NULL);

	g_ClientMsgHandler[nCSMsgID] = pMsgHandler;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(nSSMsgID > 0 && nSSMsgID < MAX_MESSAGE_ID);
	LOG_PROCESS_ERROR(g_ServerMsgHandler[nSSMsgID] == NULL);
	LOG_PROCESS_ERROR(pMsgHandler);

	g_ServerMsgHandler[nSSMsgID] = pMsgHandler;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_routerid(uint64_t qwRouterID, int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_routerid(qwRouterID, nServiceType, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_service_type(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_service_type(nServiceType, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_service_inst(int32_t nServiceType, int32_t nInstID, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_service_inst(nServiceType, nInstID, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_addr(int32_t nDstServerAddr, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_addr(nDstServerAddr, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_objid(uint64_t qwObjID, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_objid(qwObjID, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_load(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_by_load(nServiceType, vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_to_mgr(int32_t nMsgID, const void* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().sendv_to_mgr(vecs, 2);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}