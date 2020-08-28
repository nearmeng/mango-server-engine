#ifndef _SERVER_DEFAULT_SESSION_MODULE_H_
#define _SERVER_DEFAULT_SESSION_MODULE_H_

#include "module/server_module.h"
#include "define/conn_def.h"
#include "shm/shm_pool.h"

class CS_HEAD;
namespace google { namespace protobuf { class Message;  } };

struct CLIENT_SESSION
{
    uint64_t    qwConnID;
    int32_t     nConnServerAddr;
};

typedef void(*CLIENT_MSG_HANDLER_FOR_SESSION)(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);
typedef void(*CONN_EVENT_HANDLER_FOR_SESSION)(CLIENT_SESSION* pSession);

class CServerDefaultSessionModule : public CServerModule
{
public:
    BOOL init(BOOL bResume);
    BOOL uninit(void);

    static BOOL register_conn_event_handler_for_session(int32_t nEventType, CONN_EVENT_HANDLER_FOR_SESSION pEventHandler);
    static BOOL register_client_msg_handler_for_session(int32_t nCSMsgID, CLIENT_MSG_HANDLER_FOR_SESSION pMsgHandler);
    static BOOL send_to_client(CLIENT_SESSION* pSession, int32_t nMsgID, const google::protobuf::Message* pMsg);

private:
    static void _on_conn_start_event(uint64_t qwConnID, int32_t nConnServerAddr); 
    static void _on_conn_stop_event(uint64_t qwConnID, int32_t nConnServerAddr);
    static void _on_recv_client_msg(uint64_t qwConnID, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

private:
    CLIENT_MSG_HANDLER_FOR_SESSION              m_ClientMsgHandler[MAX_MESSAGE_ID];
    CONN_EVENT_HANDLER_FOR_SESSION              m_ConnEventHandler[cetTotal];
    CShmObjectPool<CLIENT_SESSION, uint64_t>    m_ClientSessionPool;
};

#define REG_SESSION_CONN_EVENT_HANDLER(__event_type__, __handler__)     (CServerDefaultSessionModule::register_conn_event_handler_for_session(__event_type__, __handler__))
#define REG_SESSION_CLI_MSG_HANDLER(__msgid__, __handler__)             (CServerDefaultSessionModule::register_client_msg_handler_for_session(__msgid__, __handler__))
#define SEND_TO_CLIENT_BY_SESSION(__session__, __msgid__, __msg__)      (CServerDefaultSessionModule::send_to_client(__session__, __msgid__, __msg__))

#endif