#include "stdafx.h"
#include "app/server_msg_handler.h"

#include "tconnd/inc/tconnapi/tframehead.h"
#include "tconnd/inc/tconnapi/tconnapi.h"

#include "protocol/proto_head.pb.h"
#include "protocol/proto_msgid.pb.h"

#include "tbus/tbus_wrapper.h"

#define MAX_MESSAGE_ID			(65535)
#define MAX_CONN_EVENT_COUNT	(64)

enum MSG_TYPE_DEF 
{
	mtdInvalid,

	mtdClient,
	mtdServer,

	mtdTotal
};

static CONN_MSG_HANDLER g_ConnMsgHandler[MAX_CONN_EVENT_COUNT];
static CLIENT_MSG_HANDLER g_ClientMsgHandler[MAX_MESSAGE_ID];
static SERVER_MSG_HANDLER g_ServerMsgHandler[MAX_MESSAGE_ID];
static Message* g_CacheMsgInst[MAX_MESSAGE_ID];

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
    LOG_PROCESS_ERROR(pDescriptor);

    pType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    LOG_PROCESS_ERROR(pType);

    pMessage = pType->New();
    LOG_PROCESS_ERROR(msg);

	g_CacheMsgInst[nMsgID] = pMessage;

Exit0:
	return pMessage;
}

static BOOL pack_msg(int32_t nMsgType, char* pBuff, int32_t* nSize, const Message* pHead, const Message* pMsg)
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


static BOOL unpack_msg(int32_t nMsgType, const char* pBuff, int32_t nSize, Message* pHead, Message** pMsg)
{
	int32_t nRetCode = 0;
	char byHeadlen = 0;
	int32_t nBodyLen = 0;
	int32_t nMsgID = 0;
	Message* pMsgInst = NULL;

	LOG_PROCESS_ERROR(pHead);
	LOG_PROCESS_ERROR(nSize > 0);

	byHeadlen = *(char*)pBuff;
	
	nRetCode = pHead->ParseFromArray(pBuff + 1, byHeadlen);
	LOG_PROCESS_ERROR(nRetCode);

	switch(nMsgType)
	{
	case mtdClient:
	{
		CS_HEAD* pCSHead = (CS_HEAD*)pHead;
		nMsgID = pCSHead->msgid();
		break;
	}
	case mtdServer:
	{
		SS_HEAD* pSSHead = (SS_HEAD*)pHead;
		nMsgID = pSSHead->msgid();
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
	}
		
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

	nRetCode = unpack_msg(mtdClient, pBuff, nSize, &CsHead, &pMsg);
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
	SS_HEAD SSHead;
	Message* pMsg = NULL;

	LOG_PROCESS_ERROR(pBuff);
	LOG_PROCESS_ERROR(nSize > 0);

	nRetCode = unpack_msg(mtdServer, pBuff, nSize, &SSHead, &pMsg);
	LOG_PROCESS_ERROR(nRetCode);
	LOG_PROCESS_ERROR_DETAIL(g_ServerMsgHandler[SSHead.msgid()], "client msg is not registerd, msgid: %d", SSHead.msgid());

	g_ServerMsgHandler[SSHead.msgid()](nSrcAddr, &SSHead, pMsg);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL send_conn_msg(int32_t nDstAddr, TFRAMEHEAD * pFrameHead, const char * pBuff, int32_t nBuffLen)
{
	int32_t nRetCode = 0;
	char szHeadBuff[sizeof(TFRAMEHEAD)];
	int32_t nHeadLen = sizeof(szHeadBuff);
	struct iovec vecs[2];

	LOG_PROCESS_ERROR(pFrameHead);

	nRetCode = tconnapi_encode(szHeadBuff, &nHeadLen, pFrameHead);
	LOG_PROCESS_ERROR(nRetCode);

	vecs[0].iov_len = nHeadLen;
	vecs[0].iov_base = szHeadBuff;
	vecs[1].iov_len = nBuffLen;
	vecs[1].iov_base = (char*)pBuff;

	return tbus_sendv_data(nDstAddr, vecs, 2);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL register_conn_msg_handler(int32_t nEventType, CONN_MSG_HANDLER pMsgHandler)
{
	LOG_PROCESS_ERROR(nEventType > 0 && nEventType < MAX_CONN_EVENT_COUNT);
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

	g_ServerMsgHandler[nSSMsgID] = pMsgHandler;

	return TRUE;
Exit0:
	return FALSE;
}