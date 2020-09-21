#ifndef _SERVER_DEFAULT_SESSION_MODULE_H_
#define _SERVER_DEFAULT_SESSION_MODULE_H_

#include "define/conn_def.h"
#include "define/session_def.h"

#include "module/server_module.h"

#include "session_mgr.h"

class CS_HEAD;
namespace google { namespace protobuf { class Message;  } };

typedef void(*CLIENT_MSG_HANDLER_FOR_SESSION)(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);
typedef void(*CONN_EVENT_HANDLER_FOR_SESSION)(CLIENT_SESSION* pSession);

class CServerDefaultSessionModule : public CServerModule
{
public:
    BOOL init(BOOL bResume);
    BOOL uninit(void);

    virtual void on_frame();

    CLIENT_SESSION* find_session(uint64_t qwSessionID);
    BOOL destroy_session(CLIENT_SESSION* pSession);
    BOOL kick_session(CLIENT_SESSION* pSession, int32_t nReason, uint64_t qwParam);

    static BOOL register_conn_event_handler_for_session(int32_t nEventType, CONN_EVENT_HANDLER_FOR_SESSION pEventHandler);
    static BOOL register_client_msg_handler_for_session(int32_t nCSMsgID, CLIENT_MSG_HANDLER_FOR_SESSION pMsgHandler);
    static BOOL send_to_client(CLIENT_SESSION* pSession, int32_t nMsgID, const google::protobuf::Message* pMsg);
    
    static BOOL send_error_code_to_client(CLIENT_SESSION* pSession, int32_t nErrorCode, uint64_t qwParam);
    static BOOL send_error_code_to_client_by_sessionid(uint64_t qwSessionID, int32_t nErrorCode, uint64_t qwParam);

private:
    static void _on_conn_start_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr); 
    static void _on_conn_stop_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr);
    static void _on_recv_client_msg(uint64_t qwConnID, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);
    static void _on_recv_client_ping(uint64_t qwConnID, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

    friend class CSessionMgr;

private:
    CLIENT_MSG_HANDLER_FOR_SESSION              m_ClientMsgHandler[MAX_MESSAGE_ID];
    CONN_EVENT_HANDLER_FOR_SESSION              m_ConnEventHandler[cetTotal];

    CSessionMgr                                 m_SessionMgr;
};

#define REG_SESSION_CONN_EVENT_HANDLER(__event_type__, __handler__)     (CServerDefaultSessionModule::register_conn_event_handler_for_session(__event_type__, __handler__))
#define REG_SESSION_CLI_MSG_HANDLER(__msgid__, __handler__)             (CServerDefaultSessionModule::register_client_msg_handler_for_session(__msgid__, __handler__))
#define SEND_TO_CLIENT_BY_SESSION(__session__, __msgid__, __msg__)      (CServerDefaultSessionModule::send_to_client(__session__, __msgid__, __msg__))
#define SEND_TO_CLIENT_ERRORCODE_BY_SESSION(__session__, __error_code__, __param__)  (CServerDefaultSessionModule::send_error_code_to_client(__session__, __error_code__, __param__))
#define SEND_TO_CLIENT_ERRORCODE_BY_ID(__sessionid__, __error_code__, __param__)   (CServerDefaultSessionModule::send_error_code_to_client_by_sessionid(__sessionid__, __error_code__, __param__))

#endif