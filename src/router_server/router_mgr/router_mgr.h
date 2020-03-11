#ifndef _ROUTER_MGR_H_
#define _ROUTER_MGR_H_

#include "define/service_def.h"
#include "shm/shm_pool.h"

class CRouterMgr
{
public:
	CRouterMgr() {};
	~CRouterMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CRouterMgr& get_instance();

	inline SERVICE_INFO* get_service_info(int32_t nServiceType);
	inline SERVER_INFO* get_server_info_by_index(int32_t nServerInfoIndex);
	inline SERVER_INFO* get_server_info_by_addr(int32_t nServerAddr);
	inline int32_t get_server_info_count(void);

	BOOL add_server_info(SERVER_INFO stServerInfo);
	BOOL del_server_info(int32_t nServerAddr);

	int32_t get_router_by_server_addr(int32_t nServerAddr);
	inline BOOL set_all_in_service(void);

private:
	static CRouterMgr ms_Instance;

	CShmObject<FULL_SERVICE_DATA>	m_ShmServiceData;
	FULL_SERVICE_DATA*				m_pServiceData;
};

inline CRouterMgr& CRouterMgr::get_instance()
{
	return ms_Instance;
}

inline SERVICE_INFO* CRouterMgr::get_service_info(int32_t nServiceType)
{
	LOG_PROCESS_ERROR(nServiceType > 0 && nServiceType < MAX_SERVICE_COUNT);

	return &(m_pServiceData->ServiceInfo[nServiceType]);
Exit0:
	return NULL;
}
	
inline int32_t CRouterMgr::get_server_info_count(void)
{
	return m_pServiceData->nServerCount;
}

inline SERVER_INFO* CRouterMgr::get_server_info_by_index(int32_t nServerInfoIndex)
{
	return m_pServiceData->get_server_info_by_index(nServerInfoIndex);
}
	
inline SERVER_INFO* CRouterMgr::get_server_info_by_addr(int32_t nServerAddr)
{
	return m_pServiceData->get_server_info_by_addr(nServerAddr);
}
	
inline BOOL CRouterMgr::set_all_in_service(void)
{
	return m_pServiceData->set_in_service();
}

#endif