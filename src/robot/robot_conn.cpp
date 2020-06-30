#include "stdafx.h"

#include <algorithm>
#include "protocol/external_message.pb.h"
#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"

#include "robot_conn.h"
#include "robot_user.h"
#include "message/client_message_handler.h"

#include "define/server_def.h"
#include "define/error_def.h"

#define MAX_USER_BUFFER_SIZE		(512 * 1024)
#define MAX_RECV_TIMES_PER_USER (64)

CRobotConnMgr CRobotConnMgr::m_Instance;

static int32_t BKDRHash(const char* pszStr);
static BOOL lua_pushmessage(lua_State* L, const google::protobuf::Message& rMessage);
static void lua_pushrepeatedfield(lua_State* L, const google::protobuf::Message& rMessage,
	const google::protobuf::FieldDescriptor* pFieldDes, int32_t nFieldIndex);
static void lua_pushfield(lua_State* L, const google::protobuf::Message& rMessage,
	const google::protobuf::FieldDescriptor* pFieldDes);
static BOOL unpack_protobuf(CRobotUser* pUser, const SC_HEAD* pHead, const google::protobuf::Message* pMessage, CLuaScript* pLuaScript);
static BOOL unpack_data(const char* pszData, int32_t nDataSize, SC_HEAD* pHead, google::protobuf::Message** pMsg);

BOOL CRobotConnMgr::init()
{
	m_nConnIDGen = 0;
	return TRUE;
}

BOOL CRobotConnMgr::fini()
{
	return TRUE;
}

BOOL CRobotConnMgr::add_conn(int32_t nConnID, const char* pcszUserName, int32_t nUserID, HTGCPAPI pHandle)
{
    ROBOT_CONNECTION stConn = {0};

	LOG_PROCESS_ERROR(mConnMgr.find(nConnID) == mConnMgr.end());

	stConn.pHandle = pHandle;
	stConn.state = CONNECT_STATE_INIT;
	stConn.vLuaWaitMsgId.clear();
	stConn.vLocalWaitMsgId.clear();
	stConn.sUserName = pcszUserName;
	stConn.nUserID = nUserID;

	mConnMgr[nConnID] = stConn;
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotConnMgr::del_conn(int32_t nConnID)
{
	mConnMgr.erase(nConnID);
	return TRUE;
}

ROBOT_CONNECTION* CRobotConnMgr::get_conn(int32_t nConnID)
{
	if (mConnMgr.find(nConnID) == mConnMgr.end())
		return NULL;

	return &mConnMgr[nConnID];
}

int32_t CRobotConnMgr::alloc_conn_id(void)
{
	m_nConnIDGen++;
	return m_nConnIDGen;
}

int32_t CRobotConnMgr::connect(int32_t nUserID, const char* pcszUserName, const char* pcszSvrUrl)
{
	int32_t nRetCode = 0;
	int32_t nConnID = 0;
	int32_t nAuthMode = 0;
	HTGCPAPI pHandle = NULL;
	TGCPACCOUNT stAccount;
	CRobotUser* pRobotUser = NULL;
	ROBOT_CONNECTION* pRobotConnection = NULL;

	LOG_PROCESS_ERROR(pcszSvrUrl);
	LOG_PROCESS_ERROR(pcszUserName);

	nRetCode = tgcpapi_create(&pHandle);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nAuthMode = TGCP_AUTH_NONE;

	nRetCode = tgcpapi_init(pHandle, 0, TGCP_ANDROID, nAuthMode,
			TGCP_ENCRYPT_METHOD_AES, TGCP_KEY_MAKING_INSVR, 64 * 1024);
	LOG_PROCESS_ERROR(nRetCode == 0);

	stAccount.uType = TGCP_ACCOUNT_TYPE_QQ_OPENID;
	stAccount.bFormat = TGCP_ACCOUNT_FORMAT_STRING;
	strxcpy(stAccount.stAccountValue.szID, pcszUserName, sizeof(stAccount.stAccountValue.szID));

	nRetCode = tgcpapi_set_account(pHandle, &stAccount);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = tgcpapi_start(pHandle, pcszSvrUrl);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nConnID = alloc_conn_id();
	nRetCode = add_conn(nConnID, pcszUserName, nUserID, pHandle);
	LOG_PROCESS_ERROR(nRetCode);

	pRobotUser = CRobotUserMgr::instance().find_user(nUserID);
	LOG_PROCESS_ERROR(pRobotUser);
	
	pRobotConnection = get_conn(nConnID);
	LOG_PROCESS_ERROR(pRobotConnection);

	pRobotUser->set_conn(pRobotConnection);

	INF("enter robot conn mgr, robot[%s] begin to connect to %s", pcszUserName, pcszSvrUrl);

	return nConnID;
Exit0:
	INF("robot[%s] fail to connect to %s", pcszUserName, pcszSvrUrl);

	return 0;
	
}

BOOL CRobotConnMgr::disconnect(int32_t nConnID)
{
	return disconnect(nConnID, TRUE);
}

BOOL CRobotConnMgr::disconnect(int32_t nConnID, BOOL isActive)
{
	int32_t nRetCode = 0;
	ROBOT_CONNECTION* pConn = NULL;
	CRobotUser* pUser = NULL;

	pConn = get_conn(nConnID);
	PROCESS_ERROR(pConn);

	tgcpapi_stop(pConn->pHandle);
	tgcpapi_fini(pConn->pHandle);
	tgcpapi_destroy(&(pConn->pHandle));
	
	nRetCode = del_conn(nConnID);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
static BOOL lua_popmessage(lua_State* L, google::protobuf::Message* pMessage);

static BOOL lua_popfield(lua_State* L, google::protobuf::Message* pMessage,
	const google::protobuf::FieldDescriptor* pFieldDes)
{
	const google::protobuf::Reflection* pRef = pMessage->GetReflection();
	switch (pFieldDes->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetString(pMessage, pFieldDes, "");
			break;
		}
		const char* pcszString = lua_tostring(L, 1);
		if (pcszString)
			pRef->SetString(pMessage, pFieldDes, pcszString);
		else
			pRef->SetString(pMessage, pFieldDes, "");
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetInt32(pMessage, pFieldDes, 0);
			break;
		}
		int32_t nValue = lua_tointeger(L, 1);
		pRef->SetInt32(pMessage, pFieldDes, nValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetBool(pMessage, pFieldDes, false);
			break;
		}
		BOOL bValue = lua_toboolean(L, 1);
		pRef->SetBool(pMessage, pFieldDes, bValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetFloat(pMessage, pFieldDes, 0);
			break;
		}
		float fValue = lua_tonumber(L, 1);
		pRef->SetFloat(pMessage, pFieldDes, fValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetDouble(pMessage, pFieldDes, 0);
			break;
		}
		double dValue = lua_tonumber(L, 1);
		pRef->SetDouble(pMessage, pFieldDes, dValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetUInt64(pMessage, pFieldDes, 0);
			break;
		}
		uint64_t qwValue = lua_tointeger(L, 1);
		pRef->SetUInt64(pMessage, pFieldDes, qwValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetInt64(pMessage, pFieldDes, 0);
			break;
		}
		int64_t llValue = lua_tointeger(L, 1);
		pRef->SetInt64(pMessage, pFieldDes, llValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->SetUInt32(pMessage, pFieldDes, 0);
			break;
		}
		uint32_t dwValue = lua_tointeger(L, 1);
		pRef->SetUInt32(pMessage, pFieldDes, dwValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		google::protobuf::Message* pMsg = pRef->MutableMessage(pMessage, pFieldDes);
		int32_t nRetCode = lua_popmessage(L, pMsg);
		LOG_PROCESS_ERROR(nRetCode);
		break;
	}
	default:
	{
		LOG_PROCESS_ERROR(FALSE);
		break;
	}
	}

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL lua_poprepeatedfield(lua_State* L, google::protobuf::Message* pMessage,
	const google::protobuf::FieldDescriptor* pFieldDes, int32_t nFieldIndex)
{
	int32_t nRetCode = 0;
	const google::protobuf::Reflection* pRef = pMessage->GetReflection();
	LOG_PROCESS_ERROR(pRef);

	switch (pFieldDes->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		google::protobuf::Message* pSubMessage = pRef->AddMessage(pMessage, pFieldDes);
		LOG_PROCESS_ERROR(pSubMessage);

		nRetCode = lua_popmessage(L, pSubMessage);
		LOG_PROCESS_ERROR(nRetCode);

		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddString(pMessage, pFieldDes, "");
			break;
		}
		const char* pcszString = lua_tostring(L, 1);
		if (pcszString)
			pRef->AddString(pMessage, pFieldDes, pcszString);
		else
			pRef->AddString(pMessage, pFieldDes, "");
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddInt32(pMessage, pFieldDes, 0);
			break;
		}
		int32_t nValue = lua_tointeger(L, 1);
		pRef->AddInt32(pMessage, pFieldDes, nValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddInt64(pMessage, pFieldDes, 0);
			break;
		}
		int64_t llValue = lua_tointeger(L, 1);
		pRef->AddInt64(pMessage, pFieldDes, llValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddBool(pMessage, pFieldDes, false);
			break;
		}
		BOOL bValue = lua_toboolean(L, 1);
		pRef->AddBool(pMessage, pFieldDes, bValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddFloat(pMessage, pFieldDes, 0);
			break;
		}
		float fValue = lua_tonumber(L, 1);
		pRef->AddFloat(pMessage, pFieldDes, fValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddDouble(pMessage, pFieldDes, 0);
			break;
		}
		double dValue = lua_tonumber(L, 1);
		pRef->AddDouble(pMessage, pFieldDes, dValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddUInt32(pMessage, pFieldDes, 0);
			break;
		}
		uint32_t dwValue = lua_tointeger(L, 1);
		pRef->AddUInt32(pMessage, pFieldDes, dwValue);
		lua_remove(L, 1);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
	{
		if(lua_gettop(L) == 0)
		{
			pRef->AddUInt64(pMessage, pFieldDes, 0);
			break;
		}
		uint64_t qwValue = lua_tointeger(L, 1);
		pRef->AddUInt64(pMessage, pFieldDes, qwValue);
		lua_remove(L, 1);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL lua_popmessage(lua_State* L, google::protobuf::Message* pMessage)
{
	const google::protobuf::FieldDescriptor* pFieldDes = NULL;
	const google::protobuf::Descriptor* pDescriptor = pMessage->GetDescriptor();
	const google::protobuf::Reflection* pRef = pMessage->GetReflection();
	int32_t nRetCode = 0;

	for (int32_t i = 0; i < pDescriptor->field_count(); ++i)
	{
		pFieldDes = pDescriptor->field(i);

		if (pFieldDes->is_repeated())
		{
			if (lua_gettop(L) == 0)
			{
				continue;
			}
			int32_t nArraySize = lua_tonumber(L, 1);
			lua_remove(L, 1);

			for (int32_t i = 0; i < nArraySize; i++)
			{
				nRetCode = lua_poprepeatedfield(L, pMessage, pFieldDes, i);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
		else
		{
			nRetCode = lua_popfield(L, pMessage, pFieldDes);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}
	
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotConnMgr::send(ROBOT_CONNECTION* pConn, int32_t nMsgID, lua_State* L)
{
	int32_t nRetCode = 0;
	std::string pszStr;
	int32_t nHeadlen = 0;
	int32_t nDataLen = 0;
	CS_HEAD Head;
	unsigned char sSendBuff[MAX_USER_BUFFER_SIZE] = {0};
	std::string sMsgName = CS_MESSAGE_ID_Name((CS_MESSAGE_ID)nMsgID);
	
	const google::protobuf::Descriptor* pDescriptor = NULL;
	const google::protobuf::Message* pType = NULL;
	const google::protobuf::Reflection* pRef = NULL;
	const google::protobuf::FieldDescriptor* pFieldDes = NULL;
	google::protobuf::Message* pMessage = NULL;

	LOG_PROCESS_ERROR(pConn);
	
    Head.set_msgid(nMsgID);
	Head.set_seqid(0);
	
	nRetCode = Head.SerializeToArray(sSendBuff + 1, sizeof(sSendBuff) - 1);
	LOG_PROCESS_ERROR(nRetCode);
	nHeadlen = Head.GetCachedSize();

	sSendBuff[0] = nHeadlen;
	
	transform(sMsgName.begin(), sMsgName.end(), sMsgName.begin(), ::toupper);
	//INF("enter tgcp_send, msgid[%d] msg_str[%s]\n", nMsgID, sMsgName.c_str());

	pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(sMsgName.c_str());
	LOG_PROCESS_ERROR(pDescriptor);

	pType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
	LOG_PROCESS_ERROR(pType);

	pMessage = pType->New();
	LOG_PROCESS_ERROR(pMessage);

	lua_remove(L, 1);
	lua_remove(L, 1);

	nRetCode = lua_popmessage(L, pMessage);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = pMessage->SerializeToArray(sSendBuff + 1 + nHeadlen, sizeof(sSendBuff) - 1 - nHeadlen);
	LOG_PROCESS_ERROR(nRetCode);
	nDataLen = pMessage->GetCachedSize();
    
	SAFE_DELETE(pMessage);

	nRetCode = tgcpapi_send(pConn->pHandle, (char*)sSendBuff, 1 + nHeadlen + nDataLen, 0);
	if(nRetCode != 0)
	{
		WRN("tgcpapi_send failed, nRetCode[%d], reason[%s]\n", nRetCode, tgcpapi_error_string(nRetCode));
		return FALSE;
	}

	return TRUE;
Exit0:
    SAFE_DELETE(pMessage);
    return FALSE;
}
	
BOOL CRobotConnMgr::send(ROBOT_CONNECTION* pConn, int32_t nMsgID, google::protobuf::Message& msg)
{
	int32_t nRetCode = 0;
	CS_HEAD Head;
	int32_t nHeadLen = 0;
	static char szSendBuff[128 * 1024];

	LOG_PROCESS_ERROR(pConn);

	Head.set_msgid(nMsgID);
	Head.set_seqid(0);

	nRetCode = Head.SerializeToArray(szSendBuff + 1, sizeof(szSendBuff) - 1);
	LOG_PROCESS_ERROR(nRetCode);

	nHeadLen = Head.GetCachedSize();
	szSendBuff[0] = nHeadLen;

	nRetCode = msg.SerializeToArray(szSendBuff + 1 + nHeadLen, sizeof(szSendBuff) - 1 - nHeadLen);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = tgcpapi_send(pConn->pHandle, szSendBuff, 1 + nHeadLen + msg.GetCachedSize(), 0);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	CRI("tgcpapi send failed for %s", tgcpapi_error_string(nRetCode));
	return FALSE;
}

BOOL CRobotConnMgr::recv(CRobotUser* pUser)
{
	int32_t nRetCode = 0;
	int32_t nDataSize = 0;
	int32_t nUserID = 0;
	int32_t nConnID = 0;
	int32_t nRecvTime = 0;
	int32_t nErrorCode = 0;

	BOOL bIsConnClosed = FALSE;
	const char* pszData = NULL;

	GCPEVENT stEvent;
	ROBOT_CONNECTION* pConn = NULL;

	LOG_PROCESS_ERROR(pUser);

	// disconnect case
	nConnID = pUser->get_connid();
	PROCESS_ERROR(nConnID);

	pConn = pUser->get_conn();
	LOG_PROCESS_ERROR(pConn);

	nRetCode = tgcpapi_update(pConn->pHandle, &stEvent);
	switch (nRetCode)
	{
		case TGCP_ERR_NONE:
		{
		}
		break;
		case TGCP_ERR_NETWORK_EXCEPTION:
		{
			CRI("tgcpapi_update network error,  user:%s retcode %d state %d", pUser->get_user_name(), nRetCode, stEvent.iEvents);
		}
		case TGCP_ERR_PEER_CLOSED_CONNECTION:
		{
			//INF("tgcpapi_update detect perr stop, user:%s retcode %d state %d", pUser->get_user_name(), nRetCode, stEvent.iEvents);
			bIsConnClosed = TRUE;
			PROCESS_ERROR(FALSE);
		}
		break;
		default:
		{
			bIsConnClosed = TRUE;
			CRI("tgcpapi_update network error, user:%s retcode %d state %d", pUser->get_user_name(), nRetCode, stEvent.iEvents);
			PROCESS_ERROR(FALSE);
		}
		break;
	}

	if (stEvent.iEvtNum > 0)
	{
		if (stEvent.iEvents & TGCP_EVENT_DATA_OUT && pConn->state == CONNECT_STATE_INIT)
		{
			//if (check_lua_wait_msgid(pConn, CONNECTED_FLAG))
			//{
			//	notify_lua_event(pConn, pUser, CONNECTED_FLAG, g_pScript);
			//}

			CClientMessageHandler::instance().on_conn_start(pConn);
			pConn->state = CONNECT_STATE_CONNECTED;
		}

		if (stEvent.iEvents & TGCP_EVENT_SSTOPED)
		{
			nErrorCode = tgcpapi_get_sstop_reason(pConn->pHandle);

			CClientMessageHandler::instance().on_conn_stop(pConn);
			INF("user[%d] peer stoped connection for reason: %d", pUser->get_user_id(), nErrorCode);
			bIsConnClosed = TRUE;

			PROCESS_ERROR(FALSE);
		}

		if (stEvent.iEvents & TGCP_EVENT_SVR_IS_FULL)
		{
			CRI("server is full occured");
			CClientMessageHandler::instance().on_conn_stop(pConn);
			bIsConnClosed = TRUE;

			PROCESS_ERROR(FALSE);
		}

		if (stEvent.iEvents & TGCP_EVENT_DATA_IN)
		{
			while (nRecvTime++ < MAX_RECV_TIMES_PER_USER)
			{
				SC_HEAD Head;
				google::protobuf::Message* pMessage = NULL;

				PROCESS_ERROR(CRobotConnMgr::instance().get_conn(nConnID));

				nRetCode = tgcpapi_peek(pConn->pHandle, &pszData, &nDataSize, 0);
				PROCESS_ERROR(nRetCode == 0);

				if (pszData == NULL || nDataSize < 0)
				{
					CRI("robot[%s] recv invalid data from peer", pConn->sUserName.c_str());
					LOG_PROCESS_ERROR(FALSE);
				}

				nRetCode = unpack_data(pszData, nDataSize, &Head, &pMessage);
				LOG_PROCESS_ERROR(nRetCode);

				CClientMessageHandler::instance().recv(&Head, pMessage, pUser);

				//if (check_lua_wait_msgid(pConn, pHeader->wMsg))
				//{
					//INF("robot[%s] recved msg[%d] from tgcp, begin to unpack", pConn->sUserName.c_str(), pHeader->wMsg);

					//nRetCode = unpack_protobuf(pUser, pszData, nDataSize, g_pScript);
					//LOG_PROCESS_ERROR(nRetCode);
				//}

				SAFE_DELETE(pMessage);

				// user may be invalid by lua
				PROCESS_ERROR(pUser->get_conn());
			}
		}
		else
		{
			return  TRUE;
		}
	}

Exit0:
	if (pUser && bIsConnClosed)
	{
		pUser->disconnect();
	}

	return TRUE;
}

BOOL CRobotConnMgr::check_lua_wait_msgid(ROBOT_CONNECTION* pConn, int32_t nMsgID)
{
	std::vector<int32_t>::iterator it;

	LOG_PROCESS_ERROR(pConn);

	it = find(pConn->vLuaWaitMsgId.begin(), pConn->vLuaWaitMsgId.end(), nMsgID);
	if (it != pConn->vLuaWaitMsgId.end())
	{
		pConn->vLuaWaitMsgId.erase(it);
		return TRUE;
	}
Exit0:
	return FALSE;
}
	
BOOL CRobotConnMgr::set_lua_wait_msgid(int32_t nConnID, std::vector<int32_t> &vMsgId)
{
	int32_t nRetCode = 0;
	int32_t nMsgId = 0;
	const char* pszMsgId = NULL;
	ROBOT_CONNECTION* pConn = NULL;

	pConn = get_conn(nConnID);
	PROCESS_ERROR(pConn);

	pConn->vLuaWaitMsgId.clear();
	for(int32_t i = 0; i < vMsgId.size(); ++i)
	{
		if(vMsgId[i] != 0)
			pConn->vLuaWaitMsgId.push_back(vMsgId[i]);
	}
	return TRUE;

Exit0:
	return FALSE;
}

static void lua_pushfield(lua_State* L, const google::protobuf::Message& rMessage, 
	const google::protobuf::FieldDescriptor* pFieldDes)
{
	const google::protobuf::Reflection* pRef = rMessage.GetReflection();

	switch (pFieldDes->cpp_type())
	{
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		{
			std::string sValue = pRef->GetString(rMessage, pFieldDes);
			lua_pushstring(L, sValue.c_str());
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			int32_t nValue = pRef->GetInt32(rMessage, pFieldDes);
			lua_pushinteger(L, nValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		{
			int32_t nValue = pRef->GetBool(rMessage, pFieldDes);
			lua_pushboolean(L, nValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		{
			//int32_t nValue = pRef->GetEnum(*pMessage, pFieldDes);
			//lua_pushinteger(L, nValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		{
			float fValue = pRef->GetFloat(rMessage, pFieldDes);
			lua_pushnumber(L, fValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		{
			double dValue = pRef->GetDouble(rMessage, pFieldDes);
			lua_pushnumber(L, dValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			uint64_t qwValue = pRef->GetUInt64(rMessage, pFieldDes);
			lua_pushinteger(L, qwValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			int64_t uValue = pRef->GetInt64(rMessage, pFieldDes);
			lua_pushinteger(L, uValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			uint32_t dwValue = pRef->GetUInt32(rMessage, pFieldDes);
			lua_pushinteger(L, dwValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		{
			const google::protobuf::Message& rMsg = pRef->GetMessage(rMessage, pFieldDes);
			lua_pushmessage(L, rMsg);
			break;
		}
		default:
		{
			LOG_PROCESS_ERROR(FALSE);
			break;
		}
	}

Exit0:
	return;
}

static void lua_pushrepeatedfield(lua_State* L, const google::protobuf::Message& rMessage,
	const google::protobuf::FieldDescriptor* pFieldDes, int32_t nFieldIndex)
{
	const google::protobuf::Reflection* pRef = rMessage.GetReflection();

	switch (pFieldDes->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		{
			const google::protobuf::Message& rMsg = pRef->GetRepeatedMessage(rMessage, pFieldDes, nFieldIndex);
			lua_pushmessage(L, rMsg);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		{
			const std::string& rStr = pRef->GetRepeatedString(rMessage, pFieldDes, nFieldIndex);
			lua_pushstring(L, rStr.c_str());
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			int32_t nValue = pRef->GetRepeatedInt32(rMessage, pFieldDes, nFieldIndex);
			lua_pushinteger(L, nValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			int64_t uValue = pRef->GetRepeatedInt64(rMessage, pFieldDes, nFieldIndex);
			lua_pushinteger(L, uValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		{
			int32_t nValue = pRef->GetRepeatedBool(rMessage, pFieldDes, nFieldIndex);
			lua_pushinteger(L, nValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		{
			float fValue = pRef->GetRepeatedFloat(rMessage, pFieldDes, nFieldIndex);
			lua_pushnumber(L, fValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		{
			double dValue = pRef->GetRepeatedDouble(rMessage, pFieldDes, nFieldIndex);
			lua_pushnumber(L, dValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			uint32_t dwValue = pRef->GetRepeatedUInt32(rMessage, pFieldDes, nFieldIndex);
			lua_pushinteger(L, dwValue);
			break;
		}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			uint64_t qwValue = pRef->GetRepeatedUInt64(rMessage, pFieldDes, nFieldIndex);
			lua_pushinteger(L, qwValue);
			break;
		}
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

Exit0:
	return;
}

static BOOL lua_pushmessage(lua_State* L, const google::protobuf::Message& rMessage)
{
	const google::protobuf::FieldDescriptor* pFieldDes = NULL;
	const google::protobuf::Descriptor* pDescriptor = rMessage.GetDescriptor();
	const google::protobuf::Reflection* pRef = rMessage.GetReflection();

	lua_newtable(L);

	for (int32_t i = 0; i < pDescriptor->field_count(); ++i)
	{
		pFieldDes = pDescriptor->field(i);
		lua_pushstring(L, pFieldDes->name().c_str());

		if (pFieldDes->is_repeated())
		{
			lua_newtable(L);
			for (int32_t i = 0; i < pRef->FieldSize(rMessage, pFieldDes); i++)
			{
				lua_pushnumber(L, i);
				lua_pushrepeatedfield(L, rMessage, pFieldDes, i);
				lua_settable(L, -3);
			}
		}
		else
		{
			lua_pushfield(L, rMessage, pFieldDes);
		}
		lua_settable(L, -3);
	}

	return TRUE;
}

static BOOL unpack_data(const char* pszData, int32_t nDataSize, SC_HEAD* pHead, google::protobuf::Message** pMsg)
{
	int32_t nRetCode = 0;
	char byHeadlen = 0;
	int32_t nBodyLen = 0;
	int32_t nMsgID = 0;
	std::string sOldMsgStr;
	std::string sMsgStr;
	const google::protobuf::Descriptor* pDescriptor = NULL;
	const google::protobuf::Message* pType = NULL;
	const google::protobuf::Reflection* pRef = NULL;
	google::protobuf::Message* pMessage = NULL;
	SC_HEAD* pSCHead = (SC_HEAD*)pHead;

	LOG_PROCESS_ERROR(pHead);
	LOG_PROCESS_ERROR(nDataSize > 0);

	byHeadlen = *(char*)pszData;

	nRetCode = pHead->ParseFromArray(pszData+ 1, byHeadlen);
	LOG_PROCESS_ERROR(nRetCode);
	
	nMsgID = pSCHead->msgid();
	sMsgStr = SC_MESSAGE_ID_Name((SC_MESSAGE_ID)nMsgID);

	//INF("recv robot[%d] msg id[%d] msg_str[%s] data_size[%d]\n", pUser->get_user_id(), nMsgId, sMsgStr.c_str(), nDataSize);

	sOldMsgStr = sMsgStr;
	transform(sMsgStr.begin(), sMsgStr.end(), sMsgStr.begin(), ::toupper);

	pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(sMsgStr);
	LOG_PROCESS_ERROR(pDescriptor);

	pType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
	LOG_PROCESS_ERROR(pType);

	pMessage = pType->New();
	LOG_PROCESS_ERROR(pMessage);

	nBodyLen = nDataSize - 1 - byHeadlen;

	nRetCode = pMessage->ParseFromArray(pszData+ 1 + byHeadlen, nBodyLen);
	LOG_PROCESS_ERROR(nRetCode);

	*pMsg = pMessage;

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL unpack_protobuf(CRobotUser* pUser, const SC_HEAD* pHead, const google::protobuf::Message* pMessage, CLuaScript* pLuaScript)
{
	uint32_t nMsgId = 0;
	std::string sOldMsgStr;
	std::string sMsgStr;
	bool bSuccess = false;
	int32_t nRetCode = 0;
	lua_State *L = NULL;
	int32_t nStackTop = -1;
	int32_t nHeadLen = 0;
	SC_HEAD Head;

	LOG_PROCESS_ERROR(pLuaScript);
	LOG_PROCESS_ERROR(pUser);
	
	L = pLuaScript->get_lua_state();
	LOG_PROCESS_ERROR(L);

	nStackTop = lua_gettop(L);

	//INF("unpack success, begin to do detail unpack");
	
	nRetCode = lua_pushmessage(L, *pMessage);
	LOG_PROCESS_ERROR(nRetCode);

	lua_pushstring(L, "msgid");
	lua_pushinteger(L, nMsgId);
	lua_settable(L, -3);
	lua_pushstring(L, "userid");
	lua_pushinteger(L, pUser->get_user_id());
	lua_settable(L, -3);

	nRetCode = pLuaScript->call_function("process_data", "ts", pUser->get_coname());
	LOG_PROCESS_ERROR(nRetCode);
	
	lua_settop(L, nStackTop);

Exit0:
	if (nStackTop != -1)
		lua_settop(L, nStackTop);

	return TRUE;
}

BOOL CRobotConnMgr::notify_lua_event(ROBOT_CONNECTION* pConn, CRobotUser* pUser, int32_t nMsgId, CLuaScript* pLuaScript)
{
	int32_t nRetCode = 0;
	lua_State *L = NULL;
	int32_t nStackTop = -1;

	LOG_PROCESS_ERROR(pLuaScript);
	LOG_PROCESS_ERROR(pConn);
	LOG_PROCESS_ERROR(pUser);

	L = pLuaScript->get_lua_state();
	LOG_PROCESS_ERROR(L);

	nStackTop = lua_gettop(L);
	
	lua_newtable(L);
	lua_pushstring(L, "msgid");
	lua_pushinteger(L, nMsgId);
	lua_settable(L, -3);
	lua_pushstring(L, "userid");
	lua_pushinteger(L, pUser->get_user_id());
	lua_settable(L, -3);

	//INF("notify lua event, connID: %d msg_id: %d", pUser->get_connid(), nMsgId);
	nRetCode = pLuaScript->call_function("process_data", "ts", pUser->get_coname());
	LOG_PROCESS_ERROR(nRetCode);
    
	lua_settop(L, nStackTop);
	return TRUE;

Exit0:
	lua_settop(L, nStackTop);
	return FALSE;
}

//BKDR Hash
static int32_t BKDRHash(const char* pszStr)
{
	int32_t seed = 131;

	int32_t hash = 0;
	while (*pszStr)
	{
		hash = hash * seed + (*pszStr++);
	}
	return (hash & 0x7FFFFFFF);
}
