#include "stdafx.h"

#include "define/server_def.h"
#include "define/error_def.h"

#include "robot_user.h"
#include "protocol/external_message.pb.h"
#include "protocol/proto_head.pb.h"

#include "message/client_message_handler.h"


#define PING_TIME_THRES			(1* 1000)
#define MAX_SEND_BUFFER_SIZE	(512 * 1024)

CRobotUserMgr CRobotUserMgr::ms_Instance;
static int32_t              g_nUserIDGen = 0;

BOOL CRobotUser::init(const char* pcszName)
{
	m_nUserID = generate_user_id();
	m_nConnID = 0;
	m_szCoName[0] = '\0';
	strxcpy(m_szUserName, pcszName, sizeof(m_szUserName));
    m_nConnCount = 0;
    m_szSvrUrl[0] = '\0';
	m_pConnection = NULL;

	return TRUE;
}

BOOL CRobotUser::fini(void)
{
	return TRUE;
}

BOOL CRobotUser::connect(const char* pcszSvrUrl)
{
	int32_t nConnID = 0;
	 
	INF("robot begin to connect to %s", pcszSvrUrl);
	++this->m_nConnCount;

	nConnID = CRobotConnMgr::instance().connect(m_nUserID, m_szUserName, pcszSvrUrl);
	LOG_PROCESS_ERROR(nConnID != 0);

    this->m_nConnID = nConnID;
    set_svr_url(pcszSvrUrl);
    return TRUE;
Exit0:
	return FALSE;
} 

BOOL CRobotUser::disconnect(void)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(this->m_nConnID != 0);
	LOG_PROCESS_ERROR(this->m_pConnection);
	nRetCode = CRobotConnMgr::instance().disconnect(this->m_nConnID);
	LOG_PROCESS_ERROR(nRetCode);

	this->m_nConnID = 0;
	this->m_pConnection = NULL;

	INF("user %s end to disconn, nConnid %d", get_user_name(), get_connid());

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotUser::send(int32_t nMsgID, google::protobuf::Message& Msg)
{
	int32_t nRetCode = 0;
	
	LOG_PROCESS_ERROR(m_pConnection);

	nRetCode = CRobotConnMgr::instance().send(m_pConnection, nMsgID, Msg);
	LOG_PROCESS_ERROR(nRetCode);
	
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotUser::send(int32_t nMsgId, lua_State* L)
{
	int32_t nRetCode = 0;
	ROBOT_CONNECTION* pConn = NULL;

	INF("robot begin to send, coname %s nConnID %d msgid %d", this->get_user_name(), this->m_nConnID, nMsgId);

	LOG_PROCESS_ERROR(m_pConnection);

	nRetCode = CRobotConnMgr::instance().send(pConn, nMsgId, L);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CRobotUser::generate_user_id(void)
{
	g_nUserIDGen++;
	return g_nUserIDGen;
}

BOOL CRobotUser::set_lua_wait_msgid(int32_t nMsgId1, int32_t nMsgId2)
{
	int32_t nRetCode = 0;
	std::vector<int32_t> vMsgID;
	
	LOG_PROCESS_ERROR(this->m_nConnID != 0);
	
	vMsgID.push_back(nMsgId1);
	vMsgID.push_back(nMsgId2);
	nRetCode = CRobotConnMgr::instance().set_lua_wait_msgid(this->m_nConnID, vMsgID);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotUserMgr::init()
{
	return TRUE;
}

BOOL CRobotUserMgr::fini()
{
	return TRUE;
}

CRobotUser* CRobotUserMgr::create_user(const char* pcszUserName)
{
	int32_t nRetCode = 0;
	CRobotUser* pUser = NULL;
	std::string sUserName = pcszUserName;
	
	LOG_PROCESS_ERROR(pcszUserName);

	pUser = new CRobotUser();
	LOG_PROCESS_ERROR(pUser);

	nRetCode = pUser->init(pcszUserName);
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(mUsersID.find(pUser->get_user_id()) == mUsersID.end());
    mUsersID[pUser->get_user_id()] = pUser;
    mUsersName[pUser->get_user_name()] = pUser;

	return pUser;
Exit0:
    if (pUser)
        SAFE_DELETE(pUser);
	pUser = NULL;
	return pUser;
}

BOOL CRobotUserMgr::destroy_user(CRobotUser* pUser)
{
	int32_t nRetCode = FALSE;
	std::string sUserName;
	int32_t nUserID = 0;
	
	LOG_PROCESS_ERROR(pUser);

	nUserID = pUser->get_user_id();
	sUserName = pUser->get_user_name();

	LOG_CHECK_ERROR(pUser->get_connid() == 0);

    SAFE_DELETE(pUser);
    mUsersID.erase(nUserID);
    mUsersName.erase(sUserName);

	INF("finish to destroy user %s size %d", sUserName.c_str(), mUsersID.size());

	return TRUE;
Exit0:
	return FALSE;
}

CRobotUser* CRobotUserMgr::find_user(int32_t nUserID)
{
	CRobotUser* pUser = NULL;

	if (mUsersID.find(nUserID) == mUsersID.end())
	{
		return NULL;
	}
	return mUsersID[nUserID];
}

CRobotUser* CRobotUserMgr::rand_user()
{
	if (mUsersID.empty())
	{
		return NULL;
	}

	uint32_t iIndex = rand_ex(mUsersID.size() - 1);
	uint32_t iNum = 0;
	for (std::map<int32_t, CRobotUser*>::iterator it = mUsersID.begin(); it != mUsersID.end(); ++it)
	{
		if (iNum++ >= iIndex)
			return it->second;
	}
	return NULL;
}

CRobotUser* CRobotUserMgr::find_user(const std::string& sUserName)
{
    CRobotUser* pUser = NULL;

    if (mUsersName.find(sUserName) == mUsersName.end())
    {
        return NULL;
    }
    return mUsersName[sUserName];
}

void CRobotUserMgr::mainloop(void)
{
	CRobotUser* pUser = NULL;
	int32_t nRetCode = 0;
	int32_t nUserID = 0;
	uint32_t dwCurrentTime = 0;
	BOOL bRun = TRUE;
	int32_t nUsers[MAX_USER_COUNT];
	CRobotUser* RobotUsers[MAX_USER_COUNT];
	int32_t nUserCount = 0;
	static int nReconnectUserCount = 0;

	dwCurrentTime = time(NULL);
	
	if (mUsersID.size() == 0)
		return;

	for (std::map<int32_t, CRobotUser*>::iterator it = mUsersID.begin(); it != mUsersID.end() && nUserCount < MAX_USER_COUNT; ++it)
	{
		nUsers[nUserCount] = it->first;
		RobotUsers[nUserCount] = it->second;

		nUserCount++;
	}

	for (int32_t nIndex = 0; nIndex < nUserCount; nIndex++)
	{
		pUser = RobotUsers[nIndex];
		if (pUser == NULL)
		{
			CRI("user is null, userid: %d", nUserID);
			continue;
		}

		CRobotConnMgr::instance().recv(pUser);
	}

Exit0:
	return;
}

int32_t CRobotUserMgr::get_user_count()
{
	int32_t count = 0;

	for (std::map<int32_t, CRobotUser*>::iterator it = mUsersID.begin(); it != mUsersID.end(); ++it)
	{
		if (it->second->get_conn() != NULL)
		{
			count++;
		}
	}

	return count;
}
