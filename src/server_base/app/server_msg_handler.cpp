#include "stdafx.h"
#include "app/server_msg_handler.h"

#include "tconnd/inc/tconnapi/tframehead.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

#include "protocol/proto_head.pb.h"
#include "protocol/proto_msgid.pb.h"
#include "protocol/external_message.pb.h"
#include "protocol/internal_message_header.h"
#include "protocol/conn_message.h"

#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"

#include "tbus/tbus_wrapper.h"
#include "app/server_app.h"

#include "define/conn_def.h"

#include "router_client/router_client_api.h"

#include "coroutine/coro_stackless.h"

#define MAX_CONN_EVENT_COUNT	(64)
#define MAX_SS_MSG_LEN			(256 * 1024)
#define MAX_SEND_DATA_BUFF_LEN	(512 * 1024)

static CONN_EVENT_HANDLER g_ConnEventHandler[cetTotal];
static CONN_MSG_HANDLER g_ConnMsgHandler[MAX_CONN_EVENT_COUNT];
static CLIENT_MSG_HANDLER g_ClientMsgHandler[MAX_MESSAGE_ID];
static SERVER_MSG_HANDLER g_ServerMsgHandler[MAX_MESSAGE_ID];
static Message* g_CacheMsgInst[MAX_MESSAGE_ID];

#define INIT_INTERNAL_MSG_VEC()					                                            \
	INTERNAL_MESSAGE_HEADER* pstHeader = (INTERNAL_MESSAGE_HEADER*)pBuffer;                 \
	pstHeader->wMsg = nMsgID; pstHeader->nMsgSrcAddr = CMGApp::instance().get_tbus_addr();	\
    if(qwCoroID > 0) pstHeader->qwCoroID = qwCoroID; else pstHeader->qwCoroID = 0;          \
	

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

BOOL unpack_client_msg_head(const char* pBuff, int32_t nSize, char* pbyHeadLen, Message* pHead)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pBuff);
    LOG_PROCESS_ERROR(nSize > 0);
    LOG_PROCESS_ERROR(pbyHeadLen);
    LOG_PROCESS_ERROR(pHead);

    *pbyHeadLen = *(char*)pBuff;

	nRetCode = pHead->ParseFromArray(pBuff + 1, *pbyHeadLen);
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

static BOOL unpack_client_msg(const char* pBuff, int32_t nSize, CS_HEAD* pCSHead, Message** pMsg)
{
	int32_t nRetCode = 0;
	char byHeadlen = 0;
	int32_t nBodyLen = 0;
	int32_t nMsgID = 0;
	Message* pMsgInst = NULL;

    LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(pCSHead);
	LOG_PROCESS_ERROR(nSize > 0);

    nRetCode = unpack_client_msg_head(pBuff, nSize, &byHeadlen, pCSHead);
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

void recv_conn_transfer_msg(int32_t nSrcAddr, const char* pBuff, size_t dwSize)
{
    int32_t nRetCode = 0;
    CONN_TRANSFER_MSG* msg = (CONN_TRANSFER_MSG*)pBuff;

    nRetCode = recv_client_msg_proc(msg->qwConnID, msg->szMsg, msg->nMsgSize);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void recv_conn_ntf_event(int32_t nSrcAddr, const char* pBuff, size_t dwSize)
{
    int32_t nRetCode = 0;
    CONN_NTF_EVENT* msg = (CONN_NTF_EVENT*)pBuff;
    CONN_NTF_EVENT_ACK ack;

    if (msg->nEventType == cetStart)
    {
        //send ack back
        ack.qwConnID = msg->qwConnID;
        ack.nEventType = msg->nEventType;
        nRetCode = send_server_msg_by_addr(nSrcAddr, conn_ntf_event_ack, &ack, sizeof(ack));
        LOG_CHECK_ERROR(nRetCode);
    }

    //call user func
    if (g_ConnEventHandler[msg->nEventType])
    {
        g_ConnEventHandler[msg->nEventType](msg->qwConnID, msg->szOpenID, nSrcAddr);
    }

Exit0:
    return;
}

BOOL recv_client_msg_proc(uint64_t qwConnID, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	CS_HEAD CsHead;
	Message* pMsg = NULL;

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize > 0);

	nRetCode = unpack_client_msg(pBuff, nSize, &CsHead, &pMsg);
	LOG_PROCESS_ERROR(nRetCode);
	LOG_PROCESS_ERROR_DETAIL(g_ClientMsgHandler[CsHead.msgid()], "client msg is not registerd, msgid: %d", CsHead.msgid());
		
    {
        std::string sMsgStr;
        std::string sMsgName = CS_MESSAGE_ID_Name((CS_MESSAGE_ID)CsHead.msgid());

        nRetCode = google::protobuf::TextFormat::PrintToString(*pMsg, &sMsgStr);
        LOG_CHECK_ERROR(nRetCode);

        INF("recv msg %s from conn <%llX>: \n%s", sMsgName.c_str(), qwConnID, sMsgStr.c_str());
    }

	g_ClientMsgHandler[CsHead.msgid()](qwConnID, &CsHead, pMsg);

    return TRUE;

Exit0:
	return FALSE;
}

BOOL recv_conn_msg_proc(int32_t nSrcAddr, const char* pBuff, int32_t nSize)
{
	int32_t nRetCode = 0;
	static TFRAMEHEAD FrameHead;
	int32_t nHeadLen = sizeof(FrameHead);

	nRetCode = tconnapi_decode(pBuff, nSize, &FrameHead, &nHeadLen);
	LOG_PROCESS_ERROR(nRetCode == 0);

	//default proc
	if (FrameHead.chCmd == TFRAMEHEAD_CMD_INPROC)
	{
		if (g_ConnMsgHandler[FrameHead.chCmd] == NULL)
		{
			nRetCode = recv_client_msg_proc(MAKE_SESSION_ID(nSrcAddr, FrameHead.iConnIdx), pBuff + nHeadLen, nSize - nHeadLen);
            LOG_CHECK_ERROR(nRetCode);

			return TRUE;
		}
	}

	LOG_PROCESS_ERROR(g_ConnMsgHandler[FrameHead.chCmd]);
	g_ConnMsgHandler[FrameHead.chCmd](MAKE_SESSION_ID(nSrcAddr, FrameHead.iConnIdx), &FrameHead, pBuff + nHeadLen, nSize - nHeadLen);

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

    if (pHeader->qwCoroID > 0 && GET_CORO_SERVER_ADDR(pHeader->qwCoroID) == CMGApp::instance().get_tbus_addr())
    {
        CCoroStackless* pCoro = CGlobalStacklessMgr::instance().get_coro(pHeader->qwCoroID);
        LOG_PROCESS_ERROR(pCoro);

        pCoro->set_coro_ret_code(crcSuccess);
        pCoro->set_coro_reply(crtMsg, (void*)pBuff, nSize);

        nRetCode = CGlobalStacklessMgr::instance().resume_coro(pCoro);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
	    LOG_PROCESS_ERROR_DETAIL(g_ServerMsgHandler[pHeader->wMsg], "msgid is %d", pHeader->wMsg);
        g_ServerMsgHandler[pHeader->wMsg](pHeader->nMsgSrcAddr, pBuff, nSize);
    }

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_to_conn_server(int32_t nConnServerAddr, uint64_t qwConnID, int32_t nEventType, uint64_t qwEventParam0, uint64_t qwEventParam1)
{
    int32_t nRetCode = 0;
    CONN_BUSINESS_EVENT msg;

    msg.qwConnID = qwConnID;
    msg.nEventType = nEventType;
    msg.qwEventParam0 = qwEventParam0;
    msg.qwEventParam1 = qwEventParam1;

    nRetCode = send_server_msg_by_addr(nConnServerAddr, conn_business_event, &msg, sizeof(msg));
    LOG_PROCESS_ERROR(nRetCode);
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL send_to_client(int32_t nConnServerAddr, uint64_t qwConnID, int32_t nMsgID, const Message* pMsg)
{
    int32_t nRetCode = 0;
    SC_HEAD head;
	char szDataBuff[MAX_SEND_DATA_BUFF_LEN];
	int32_t nDataLen = sizeof(szDataBuff);
    int32_t nAllDataSize = 0;
    CONN_TRANSFER_MSG* msg = NULL;

    head.set_msgid(nMsgID);
    head.set_seqid(0);
    
	if (pMsg)
	{
		nRetCode = pack_msg(szDataBuff, &nDataLen, &head, pMsg);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
		nDataLen = 0;

    nAllDataSize = sizeof(CONN_TRANSFER_MSG) + nDataLen;
    msg = (CONN_TRANSFER_MSG*)alloca(nAllDataSize);

    msg->qwConnID = qwConnID;
    msg->nMsgSize = nDataLen;
    memcpy(msg->szMsg, szDataBuff, nDataLen);
    
    {
        std::string sMsgStr;
        std::string sMsgName = SC_MESSAGE_ID_Name((SC_MESSAGE_ID)nMsgID);

        nRetCode = google::protobuf::TextFormat::PrintToString(*pMsg, &sMsgStr);
        LOG_CHECK_ERROR(nRetCode);

        INF("send msg %s to conn <%llX>: \n%s", sMsgName.c_str(), qwConnID, sMsgStr.c_str());
    }

    nRetCode = send_server_msg_by_addr(nConnServerAddr, conn_transfer_msg, msg, nAllDataSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD* pFrameHead, const char* pBuff, int32_t nSize)
{
    int32_t nRetCode = 0;
	static char szHeadBuff[sizeof(TFRAMEHEAD) * 2];
	int32_t nHeadLen = sizeof(szHeadBuff);
	struct iovec vecs[2];

    LOG_PROCESS_ERROR(pFrameHead);

	nRetCode = tconnapi_encode(szHeadBuff, &nHeadLen, pFrameHead);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
    vecs[0].iov_len = nHeadLen;
	vecs[0].iov_base = szHeadBuff;
	vecs[1].iov_len = nSize;
	vecs[1].iov_base = (char*)pBuff;

	return tbus_sendv_data(nDstAddr, vecs, 2);
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

Exit0:
	return FALSE;
}

BOOL register_conn_event_handler(int32_t nEventType, CONN_EVENT_HANDLER pEventHandler)
{
    LOG_PROCESS_ERROR(nEventType > cetInvalid && nEventType < cetTotal);
    LOG_PROCESS_ERROR(pEventHandler);
    LOG_PROCESS_ERROR(g_ConnEventHandler[nEventType] == NULL);

    g_ConnEventHandler[nEventType] = pEventHandler;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL register_conn_msg_handler(int32_t nMsgType, CONN_MSG_HANDLER pMsgHandler)
{
    LOG_PROCESS_ERROR(pMsgHandler);
	LOG_PROCESS_ERROR(nMsgType >= 0 && nMsgType < MAX_CONN_EVENT_COUNT);
	LOG_PROCESS_ERROR(g_ConnMsgHandler[nMsgType] == NULL);

	g_ConnMsgHandler[nMsgType] = pMsgHandler;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL register_client_msg_handler(int32_t nCSMsgID, CLIENT_MSG_HANDLER pMsgHandler)
{
    LOG_PROCESS_ERROR(pMsgHandler);
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

BOOL send_server_msg_by_routerid(uint64_t qwRouterID, int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_routerid(qwRouterID, nServiceType, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_service_type(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_service_type(nServiceType, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_service_inst(int32_t nServiceType, int32_t nInstID, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_service_inst(nServiceType, nInstID, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_addr(int32_t nDstServerAddr, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_addr(nDstServerAddr, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_objid(uint64_t qwObjID, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_objid(qwObjID, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_by_load(int32_t nServiceType, int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_by_load(nServiceType, pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_server_msg_to_mgr(int32_t nMsgID, const void* pBuffer, size_t dwSize, uint64_t qwCoroID)
{
	int32_t nRetCode = 0;

	INIT_INTERNAL_MSG_VEC();

	nRetCode = CRouterClient::instance().send_to_mgr(pBuffer, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}