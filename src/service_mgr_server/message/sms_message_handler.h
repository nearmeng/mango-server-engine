#ifndef _GS_MESSAGE_HANDLER_H_
#define _GS_MESSAGE_HANDLER_H_

#include "define/server_def.h"
#include "define/service_def.h"
#include "define/raft_def.h"

#include "protocol/internal_message_header.h"
#include "shm/shm_pool.h"
#include "queue/async_msg_queue.h"

struct SMS_MESSAGE_DATA
{
	int32_t				nInitRouterCount;
	uint8_t				byUnRegisterGsFlag[MAX_SERVER_COUNT_PER_SERVICE / 8];
	int32_t				nUnRegisterServerCount;
	int32_t				nState;
	int32_t				nMgrCount;
	int32_t				nMgrAddr[MAX_SERVICE_MGR_COUNT];
};

class CSMSMessageHandler
{
public:
    CSMSMessageHandler();
    virtual ~CSMSMessageHandler();

    BOOL init(int32_t nTbusHandle, int32_t nTbusAddr, const char* pcszTag, BOOL bResume);
    BOOL uninit(void);
    inline static CSMSMessageHandler& instance(void);

	void frame();
	static BOOL msg_handler(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

    /************************************************************************/
    // common
    BOOL do_register(int32_t nDstAddr);
    BOOL do_ping(int32_t nDstAddr);
	BOOL do_control_ack(int32_t nResult, const char* pDesc, int32_t nDstAddr);

    /************************************************************************/
	BOOL do_sync_mgr_data_begin(int32_t nDstAddr);
	BOOL do_sync_obj_info(int32_t nDstAddr);
	BOOL do_sync_router_info(int32_t nDstAddr);
	BOOL do_sync_server_info(int32_t nDstAddr);
	BOOL do_sync_mgr_data_end(int32_t nDstAddr);
	BOOL do_obj_info_ntf(uint64_t qwObjID, int32_t nTbusAddr, BOOL bRegister);
	BOOL do_sync_in_service(void);
	BOOL do_sync_end_service(int32_t nServerType);
	BOOL do_ntf_server_event(int32_t nEventType, int32_t nEventParam, int32_t nPhaseType);
	BOOL do_ntf_router_down(int32_t nRouterAddr);

	void set_state(int32_t nState);
	BOOL is_state_ready(int32_t nState);
	BOOL is_all_state_ready(void);
	BOOL set_raft_data(int32_t nType, int64_t llParam);

	inline int32_t get_mgr_count(void);
	inline int32_t get_mgr_addr(int32_t nIndex);

	BOOL send(int32_t nDstAddr, const void* pBuffer, size_t dwSize);
	static BOOL send_raft_msg(int32_t nNodeID, int32_t nMsgType, const char* pBuff, int32_t nSize);

private:
	void keep_alive(void);
	BOOL _init_msgid(void);
	BOOL broadcast_to_router(const void* pBuffer, size_t dwSize);
	BOOL broadcast_to_mgr(const void* pBuffer, size_t dwSize);

    /************************************************************************/
    // common
    void on_register(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
    void on_control(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

    /************************************************************************/
	void on_router_heart_beat(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_alive_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_obj_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_resume_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_unregister_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_event_prepare_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_event_execute_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_router_event_finish_report(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

	void on_service_mgr_sync_data_begin(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_obj(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_router(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_sync_data_end(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_end_service(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
	void on_service_mgr_router_down(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

	void on_service_mgr_raft_msg(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

private:
	int32_t _judge_server_node_status(SERVER_INFO* pCurrInfo, BOOL bAlive);
	BOOL _process_control_cmd(const char* pcszCommand, int32_t nLen, int32_t nSrcAddr);
	BOOL _process_control_cmd_expand(int32_t nServiceType, int32_t nAfterExpandServerCount, std::string& sErrStr);
	BOOL _process_control_cmd_reduce(int32_t nServiceType, int32_t nAfterReduceServerCount, std::string& sErrStr);

private:
    static CSMSMessageHandler ms_Instance;

    typedef void (CSMSMessageHandler::*MESSAGE_FUNC)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
    MESSAGE_FUNC	m_InternalMessageFuncList[internal_message_end];

	int32_t							m_nTbusHandle;
	int32_t							m_nTbusAddr;

	CShmMsgQueue					m_ShmMsgQueue;
	CShmObject<SMS_MESSAGE_DATA>	m_ShmMessageData;
	SMS_MESSAGE_DATA*				m_pShmMessageData;
	RAFT_MGR*						m_pRaftMgr;
};

inline CSMSMessageHandler& CSMSMessageHandler::instance(void)
{
    return ms_Instance;
}
	
inline int32_t CSMSMessageHandler::get_mgr_count(void)
{
	LOG_PROCESS_ERROR(m_pShmMessageData);

	return m_pShmMessageData->nMgrCount;
Exit0:
	return 0;
}
inline int32_t CSMSMessageHandler::get_mgr_addr(int32_t nIndex)
{
	LOG_PROCESS_ERROR(m_pShmMessageData);
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_pShmMessageData->nMgrCount);

	return m_pShmMessageData->nMgrAddr[nIndex];
Exit0:
	return 0;
}

#endif	// _SMS_MESSAGE_HANDLER_H_
