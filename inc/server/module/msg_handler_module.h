#ifndef _SESSION_MODULE_H_
#define _SESSION_MODULE_H_

#include "define/conn_def.h"
#include "define/session_def.h"

#include "module/server_module.h"

class CS_HEAD;
class CRole;
class CRoleModule;
class CSessionModule;
namespace google { namespace protobuf { class Message;  } };

typedef void(*UPPER_CONN_EVENT_HANDLER)(CLIENT_SESSION* pSession);
typedef void(*UPPER_ROLE_MSG_HANDLER)(CRole* pRole, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);
typedef void(*UPPER_CLIENT_MSG_HANDLER)(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

class CMsgHandlerModule : public CServerModule
{
public:
    BOOL init(BOOL bResume);
    BOOL uninit(void);

    virtual void on_frame();

    BOOL register_conn_event_handler(int32_t nEventType, UPPER_CONN_EVENT_HANDLER pEventHandler);
    BOOL register_client_msg_handler(int32_t nCSMsgID, UPPER_CLIENT_MSG_HANDLER pMsgHandler);
    BOOL register_role_msg_handler(int32_t nCSMsgID, UPPER_ROLE_MSG_HANDLER pMsgHandler);
    BOOL register_server_msg_handler(int32_t nSSMsgID, SERVER_MSG_HANDLER pMsgHandler);

    BOOL send_to_client(uint64_t qwSessionID, int32_t nMsgID, const google::protobuf::Message* pMsg);
    BOOL send_to_client(CLIENT_SESSION* pSession, int32_t nMsgID, const google::protobuf::Message* pMsg);
    
    BOOL send_error_code_to_client(CLIENT_SESSION* pSession, int32_t nErrorCode, uint64_t qwParam);
    BOOL send_error_code_to_client_by_sessionid(uint64_t qwSessionID, int32_t nErrorCode, uint64_t qwParam);

    void on_conn_timeout_event(CLIENT_SESSION* pSession);
private:
    static void _on_conn_start_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr); 
    static void _on_conn_stop_event(uint64_t qwConnID, const char* pcszOpenID, int32_t nConnServerAddr);
    static void _on_recv_client_msg(uint64_t qwConnID, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

private:
    UPPER_ROLE_MSG_HANDLER                              m_RoleMsgHandler[cetTotal];
    UPPER_CLIENT_MSG_HANDLER                            m_ClientMsgHandler[MAX_MESSAGE_ID];
    UPPER_CONN_EVENT_HANDLER                            m_ConnEventHandler[cetTotal];

    CSessionModule*                                     m_pSessionModule;
    CRoleModule*                                        m_pRoleModule;
};

extern CMsgHandlerModule*  g_MsgHandlerModule;

#endif