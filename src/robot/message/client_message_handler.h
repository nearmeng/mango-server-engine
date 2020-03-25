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

	void recv(const char* pBuffer, int32_t nSize, CRobotUser *pUser);
	virtual BOOL send(int32_t nConnID, int32_t nMsgID, google::protobuf::Message& Msg);

private:
	static CClientMessageHandler ms_Instance;

	typedef void (CClientMessageHandler::*MESSAGE_FUNC)(SC_HEAD* pSCHead, google::protobuf::Message* pMsg, CRobotUser* pUser);
	MESSAGE_FUNC	MessageFuncList[MAX_MESSAGE_ID];
};

inline CClientMessageHandler& CClientMessageHandler::instance(void)
{
	return ms_Instance;
}

#endif	// _CLIENT_MESSAGE_HANDLER_H_