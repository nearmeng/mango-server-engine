#ifndef _ROUTER_CLIENT_API_H_
#define _ROUTER_CLIENT_API_H_

#include "define/router_def.h"
#include "define/service_def.h"

class CRouterClient
{
public:
	CRouterClient() {};
	~CRouterClient() {};

	BOOL init(int32_t nTbusHandle, int32_t nTbusID, MESSAGE_FUNC pMsgHandle, BOOL bResume);
	BOOL uninit(void);
	BOOL mainloop(void);

	inline static CRouterClient& instance();

	//负载上报
	inline BOOL set_load_report(int32_t nInterval, SERVER_LOAD_REPORT_FUNC pFunc);

	//状态对象上报
	BOOL obj_report(uint64_t qwObjID, BOOL bRegister);

	//设置服务节点事件回调
	inline BOOL set_server_event_handler(int32_t nEventType, SERVER_EVENT_HANDLE pServerEventHandle);

	//主动获取节点信息 通过事件通知
	void get_service_info(int32_t nServiceType);

	//发送族接口
	BOOL send_by_routerid(uint64_t qwRouterID, int32_t nServiceType, const void* pBuffer, size_t dwSize, 
							int32_t nTransferHandleType = rtnhtCache);
	BOOL send_by_service_type(int32_t nServiceType, const void* pBuffer, size_t dwSize);
	BOOL send_by_service_inst(int32_t nServiceType, int32_t nInstID, const void* pBuffer, size_t dwSize);
	BOOL send_by_addr(int32_t nDstServerAddr, const void* pBuffer, size_t dwSize);
	BOOL send_by_objid(uint64_t qwObjID, const void* pBuffer, size_t dwSize);
	BOOL send_by_load(int32_t nServiceType, const void* pBuffer, size_t dwSize);
	BOOL send_to_mgr(const void* pBuffer, size_t dwSize); 

private:
	BOOL _init_internal_msgid(void);

	BOOL _send_to_router(const void* pBuffer, size_t dwSize);
	BOOL _send_register(BOOL bResume);
	BOOL _send_heart_beat(void);
	BOOL _send_load_report(void);
	BOOL _send_unregister(void);
	BOOL _send_server_event_ack(int32_t nEventType, int32_t nEventParam);
	BOOL _send_msg_clear_ack(int32_t nServiceType, int32_t nEventType, int32_t nEventParam);
	BOOL _send_get_service_info(int32_t nServiceType);
	
	BOOL on_router_ntf_server_event(const char* pBuffer, size_t dwSize);
	BOOL on_router_ntf_msg_clear(const char* pBuffer, size_t dwSize);

	static BOOL	router_msg_handle(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

private: 
	static CRouterClient ms_RouterClient;

	int32_t				 m_nTbusHandle;
	int32_t				 m_nTbusID;
	int32_t				 m_nRouterTbusID;
	uint64_t			 m_qwLastHeartBeatTime;
	uint64_t			 m_qwLastLoadReportTime;

	int32_t				 m_nLoadReportInterval;
	SERVER_LOAD_REPORT_FUNC		m_pLoadReportFunc;

	MESSAGE_FUNC		m_pMsgHandle;
    
	typedef BOOL (CRouterClient::*INTER_MESSAGE_FUNC)(const char* pBuffer, size_t dwSize);
    INTER_MESSAGE_FUNC	m_InternalMessageFuncList[MAX_ROUTER_CLIENT_INTERNAL_MSG_COUNT];

	SERVER_EVENT_HANDLE		m_pServerEventHandle[setTotal];
};

inline CRouterClient& CRouterClient::instance()
{
	return ms_RouterClient;
}

inline BOOL CRouterClient::set_load_report(int32_t nInterval, SERVER_LOAD_REPORT_FUNC pFunc)
{
	LOG_PROCESS_ERROR(pFunc);
	LOG_PROCESS_ERROR(nInterval > 0);

	m_nLoadReportInterval = nInterval;
	m_pLoadReportFunc = pFunc;
	
	return TRUE;
Exit0:
	return FALSE;
}
	
inline BOOL CRouterClient::set_server_event_handler(int32_t nEventType, SERVER_EVENT_HANDLE pServerEventHandle)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pServerEventHandle);
	LOG_PROCESS_ERROR(nEventType > setInvalid && nEventType < setTotal);

	m_pServerEventHandle[nEventType] = pServerEventHandle;

	return TRUE;
Exit0:
	return FALSE;
}

#endif