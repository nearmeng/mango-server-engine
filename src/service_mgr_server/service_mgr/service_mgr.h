#ifndef _SERVICE_MGR_H_
#define _SERVICE_MGR_H_

#include "shm/shm_pool.h"
#include "define/service_def.h"

struct SMS_ROUTER_INFO
{
	int32_t			nTbusAddr;
	uint32_t		dwLastReportTime;
	uint32_t		dwLastPingTime;
	uint32_t		dwCheckDownFlag;
	uint32_t		dwLastDownReportTime;
	int32_t			nState;
};

struct SMS_ROUTER_MGR
{
	int32_t				nRouterCount;
	SMS_ROUTER_INFO		RouterInfo[MAX_SERVER_COUNT_PER_SERVICE];
};

class CServiceMgr
{
public:
	CServiceMgr() {};
	~CServiceMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CServiceMgr& get_instance();
	void mainloop(void);

	inline SERVICE_INFO* get_service_info(int32_t nServiceType);
	inline SERVER_INFO* get_server_info_by_index(int32_t nServerInfoIndex);
	inline SERVER_INFO* get_server_info_by_addr(int32_t nServerAddr);
	inline int32_t get_server_info_count(void);
	inline BOOL set_all_in_service(void);

	inline BOOL add_server_info(SERVER_INFO stServerInfo);
	inline BOOL del_server_info(int32_t nServerAddr);

	BOOL add_router(int32_t nRouterAddr);
	BOOL del_router(int32_t nRouterAddr);
	SMS_ROUTER_INFO* get_router(int32_t nRouterAddr);
	SMS_ROUTER_INFO* get_rand_router(void);
	inline SMS_ROUTER_MGR* get_router_mgr(void);
	int32_t get_reported_router_count(void);
	BOOL router_down_process(int32_t nReportMgrAddr, int32_t nRouterAddr);

	BOOL pack_service_data(char** pszData, int32_t &nDataSize);
	BOOL set_service_data(const char* pczData, int32_t nDataSize);

private:
	static CServiceMgr		ms_Instance;

	CShmObject<FULL_SERVICE_DATA>	m_ShmServiceData;
	FULL_SERVICE_DATA*				m_pShmServiceData;

	CShmObject<SMS_ROUTER_MGR>		m_ShmRouterMgr;
	SMS_ROUTER_MGR*					m_pShmRouterMgr;
};

inline CServiceMgr& CServiceMgr::get_instance()
{
	return ms_Instance;
}

inline SERVICE_INFO* CServiceMgr::get_service_info(int32_t nServiceType)
{
	LOG_PROCESS_ERROR(nServiceType > 0 && nServiceType < MAX_SERVICE_COUNT);

	return &(m_pShmServiceData->ServiceInfo[nServiceType]);
Exit0:
	return NULL;
}

inline SERVER_INFO* CServiceMgr::get_server_info_by_index(int32_t nServerInfoIndex)
{
	return m_pShmServiceData->get_server_info_by_index(nServerInfoIndex);
}
	
inline SERVER_INFO* CServiceMgr::get_server_info_by_addr(int32_t nServerAddr)
{
	return m_pShmServiceData->get_server_info_by_addr(nServerAddr);
}

BOOL CServiceMgr::add_server_info(SERVER_INFO stServerInfo)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pShmServiceData);

	nRetCode = m_pShmServiceData->add_server_node(stServerInfo);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CServiceMgr::del_server_info(int32_t nServerAddr)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pShmServiceData);

	nRetCode = m_pShmServiceData->del_server_node(nServerAddr);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}
	
inline int32_t CServiceMgr::get_server_info_count(void)
{
	LOG_PROCESS_ERROR(m_pShmServiceData);

	return m_pShmServiceData->nServerCount;
Exit0:
	return 0;
}
	
inline SMS_ROUTER_MGR* CServiceMgr::get_router_mgr(void)
{
	return m_pShmRouterMgr;
}
	
inline BOOL CServiceMgr::set_all_in_service(void)
{
	return m_pShmServiceData->set_in_service();
}

#endif