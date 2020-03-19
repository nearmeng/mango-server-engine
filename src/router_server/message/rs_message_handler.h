#ifndef _RS_MESSAGE_HANDLER_H_
#define _RS_MESSAGE_HANDLER_H_

#include "protocol/internal_message_header.h"
#include "shm/shm_pool.h"
#include "queue/async_msg_queue.h"

#include "define/server_def.h"
#include "define/service_def.h"

struct SERVICE_MGR_DATA
{
	int32_t			nMgrTbusAddr;			// curr mgr
	int32_t			nInitServerNodeCount;
	int32_t			nServerEventAckCount;
	int32_t			nMgrCount;
	int32_t			nMgrAddr[MAX_SERVICE_MGR_COUNT];
	uint32_t		dwLastHeartbeatTime;
};

class CRSMessageHandler
{
public:
	CRSMessageHandler() {};
	virtual ~CRSMessageHandler() {};

    BOOL init(int32_t nTbusHandle, int32_t nTbusAddr, BOOL bResume);
    BOOL uninit(void);
    inline static CRSMessageHandler& instance(void);

    BOOL mainloop(void);
	static BOOL msg_handler(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

    /************************************************************************/
	//send to service mgr
    BOOL do_register(int32_t nDstAddr);
	BOOL do_heartbeat(void);
	BOOL do_alive_report(void);
	BOOL do_obj_report(uint64_t qwObjID, int32_t nTbusAddr, BOOL bRegister);
	BOOL do_resume_report(int32_t nServerAddr);
	BOOL do_unregister_report(int32_t nServerAddr);
	BOOL do_server_evevt_prepare_report(int32_t nEventType, int32_t nEventParam);
	BOOL do_server_event_execute_report(int32_t nEventType, int32_t nEventParam);
	BOOL do_server_event_finish_report(int32_t nEventType, int32_t nEventParam);

	//send to router client
	BOOL do_router_send_by_addr(int32_t nRouterAddr, int32_t nDstServerAddr, const char* pBuffer, uint32_t dwSize);
	BOOL do_router_ntf_server_init();
	BOOL do_router_ntf_end_service();
	BOOL do_router_ntf_server_event(int32_t nEventType, int32_t nEventParam, int32_t nExtraParam);
	BOOL do_router_ntf_msg_clear(int32_t nServiceType, int32_t nEventType, int32_t nEventParam);
	
	inline int32_t get_init_server_node_count(void) { return m_pShmServiceMgrData->nInitServerNodeCount; };

private:
	BOOL _init_msgid(void);
	BOOL send(int32_t nDstAddr, const void* pBuffer, size_t dwSize);
	BOOL send(int32_t nDstAddr, const void* pHeader, size_t dwHeaderSize, const void* pBuffer, size_t dwSize);
	BOOL send_to_mgr(const void* pBuffer, size_t dwSize);
	BOOL send_to_all_mgr(const void* pBuffer, size_t dwSize);
	BOOL broadcast_to_local_server(const void* pBuffer, size_t dwSize);
	BOOL broadcast_to_local_server(int32_t nServiceType, const void* pBuffer, size_t dwSize);

    /************************************************************************/
    void on_register(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
    void on_unregister(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
    void on_heartbeat(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_load_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_obj_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_routerid(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_service_type(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_service_inst(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_addr(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_objid(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_by_load(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_send_to_mgr(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_server_event_ack(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_ntf_msg_clear_ack(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_get_service_info(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

	void on_service_mgr_sync_data_begin(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_server_info(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_obj(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_data_end(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_obj_ntf(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_server_info_ntf(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_in_service(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_end_service(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_ntf_server_event(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_ntf_mgr_leader(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

private:
	void _on_server_event_prepare(int32_t nEventType, int32_t nEventParam);
	void _on_server_event_execute(int32_t nEventType, int32_t nEventParam);
	void _on_server_event_finish(int32_t nEventType, int32_t nEventParam);

private:
    static CRSMessageHandler ms_Instance;

    typedef void (CRSMessageHandler::*MESSAGE_FUNC)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
    MESSAGE_FUNC	m_InternalMessageFuncList[internal_message_end];

	int32_t								m_nTbusHandle;
	int32_t								m_nTbusAddr;
	CShmMsgQueue						m_ShmMsgQueue;
	CShmObject<SERVICE_MGR_DATA>		m_ShmServiceMgrData;
	SERVICE_MGR_DATA*					m_pShmServiceMgrData;
};

inline CRSMessageHandler& CRSMessageHandler::instance(void)
{
    return ms_Instance;
}


#endif	// _GS_MESSAGE_HANDLER_H_
