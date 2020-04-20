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

	void on_login(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	
	BOOL do_gm_command(const char* pGMCommand, CRobotUser* pUser);

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