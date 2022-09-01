#ifndef _CLIENT_MESSAGE_HANDLER_H_
#define _CLIENT_MESSAGE_HANDLER_H_

#define MAX_MESSAGE_ID			(65535)

class SC_HEAD;

class CClientMessageHandler 
{
public:
	CClientMessageHandler();
	virtual ~CClientMessageHandler();

	BOOL init(void);
	BOOL uninit(void);
	inline static CClientMessageHandler& instance(void);

	void recv(SC_HEAD* pScHead, google::protobuf::Message* pMsg, CRobotUser *pUser);
	virtual BOOL send(CRobotUser* pUser, int32_t nMsgID, google::protobuf::Message& Msg);

	void on_conn_start(ROBOT_CONNECTION* pConn);
	void on_conn_stop(ROBOT_CONNECTION* pConn);

	void on_allow_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	void on_register(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	void on_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	void on_sync_role_list(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	void on_recv_error_code(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	void on_sync_role_data(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	
	BOOL do_gm_command(const char* pGMCommand, CRobotUser* pUser);

private:
    BOOL do_create_role(CRobotUser* pUser, const char* pcszRoleName);
    BOOL do_select_role(CRobotUser* pUser, uint64_t qwRoleID);
    BOOL do_logout(CRobotUser* pUser);

private:
	static CClientMessageHandler ms_Instance;

	typedef void (CClientMessageHandler::*MESSAGE_FUNC)(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	MESSAGE_FUNC	s_MessageFuncList[MAX_MESSAGE_ID];
};

inline CClientMessageHandler& CClientMessageHandler::instance(void)
{
	return ms_Instance;
}

#endif	// _CLIENT_MESSAGE_HANDLER_H_