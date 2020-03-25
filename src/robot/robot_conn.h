#ifndef  _ROBOT_CONN_H_
#define  _ROBOT_CONN_H_

#include <vector>

#include "tgcpapi.h"
#include "tgcpapi_qq_login.h"

class CRobotUser;
typedef void (*WAIT_MSG_CALLBACK)(CRobotUser *pUser, const char* pcszData, int32_t nSize);

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

enum CONNECT_STATE
{
	CONNECT_STATE_INIT = 0,
	CONNECT_STATE_CONNECTED = 1,
	CONNECT_STATE_DISCONNECTED = 2
};

struct ROBOT_CONNECTION
{
	HTGCPAPI pHandle;
	CONNECT_STATE state;
	std::vector<int32_t> vLuaWaitMsgId;
	std::vector<int32_t> vLocalWaitMsgId;
	WAIT_MSG_CALLBACK Callback;
	
	int32_t nUserID;
	std::string sUserName;
};

class CRobotConnMgr
{
public:
	CRobotConnMgr() {};
	~CRobotConnMgr() {};

	inline static CRobotConnMgr& instance(void);
	BOOL init(void);
	BOOL fini(void);

	int32_t connect(int32_t nUserID, const char* pcszUserName, const char* pcszSvrUrl);
	BOOL disconnect(int32_t nConnID);

	BOOL send(ROBOT_CONNECTION* pConn, int32_t nMsgID, google::protobuf::Message& msg);
	BOOL send(ROBOT_CONNECTION* pConn, int32_t nMsgID, lua_State* L);
	BOOL recv(CRobotUser* pUser);

	BOOL set_lua_wait_msgid(int32_t nConnID, std::vector<int32_t> &vMsgId);
	BOOL check_lua_wait_msgid(ROBOT_CONNECTION* pConn, int32_t nMsgID);
	
	ROBOT_CONNECTION* get_conn(int32_t nConnID);
private:
	BOOL add_conn(int32_t nConnID, const char* pcszUserName, int32_t nUserID, HTGCPAPI pHandle);
	BOOL del_conn(int32_t nConnID);

	int32_t alloc_conn_id(void);
	int32_t disconnect(int32_t nConID, BOOL isActive);

	BOOL notify_lua_event(ROBOT_CONNECTION* pConn, CRobotUser* pUser, int32_t nMsgId, CLuaScript* pLuaScript);

private:
	static CRobotConnMgr m_Instance;
	int32_t m_nConnIDGen;
	std::map<int32_t, ROBOT_CONNECTION> mConnMgr;
};

inline CRobotConnMgr& CRobotConnMgr::instance(void)
{
	return m_Instance;
}

#endif

