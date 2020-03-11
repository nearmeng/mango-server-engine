#ifndef _SMS_TRANS_H_
#define _SMS_TRANS_H_

#include "define/service_def.h"

#include "container/sk_container.h"
#include "shm/shm_pool.h"

struct SMS_TRANS_CMD
{
	SMS_TRANS_CMD()
	{
		nEventType = 0;
		nServiceType = 0;
		nServerAddr = 0;
		nParam = 0;
	}

	int32_t		nEventType;
	int32_t		nServiceType;
	int32_t		nServerAddr;
	int32_t		nParam;
};

struct SMS_TRANS_DATA
{
	BOOL bInProcess;
	int32_t nRouterReportCount;
	sk::fixed_queue<SMS_TRANS_CMD,MAX_TRANS_CMD_COUNT>		CmdList;
};

class CSmsTransMgr
{
public:	
	CSmsTransMgr() {};
	~CSmsTransMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	void mainloop(void);
	inline static CSmsTransMgr& get_instance(void);

	BOOL add_trans_cmd(SMS_TRANS_CMD stCmd);
	BOOL clear_trans_cmd(void);
	SMS_TRANS_CMD* get_trans_front_cmd(void);
	BOOL pop_trans_front_cmd(void);

	inline void set_trans_router_report(int32_t nCount);
	inline void add_trans_router_report(int32_t nCount);
	inline int32_t get_trans_router_report(void);
	
	inline BOOL is_server_in_cmd_list(int32_t nServerAddr);
	inline BOOL is_in_process(void);

private:
	BOOL _process_cmd(const SMS_TRANS_CMD* pCmd);

	BOOL _process_cmd_node_down(const SMS_TRANS_CMD* pCmd);
	BOOL _process_cmd_node_recover(const SMS_TRANS_CMD* pCmd);
	BOOL _process_cmd_node_expand(const SMS_TRANS_CMD* pCmd);
	BOOL _process_cmd_node_reduce(const SMS_TRANS_CMD* pCmd)
		;

private:
	static CSmsTransMgr				ms_Instance;
	CShmObject<SMS_TRANS_DATA>		m_ShmTransData;
	SMS_TRANS_DATA*					m_pShmTransData;
};

inline CSmsTransMgr& CSmsTransMgr::get_instance()
{
	return ms_Instance;
}
	
inline void CSmsTransMgr::set_trans_router_report(int32_t nCount)
{
	LOG_PROCESS_ERROR(m_pShmTransData);
	m_pShmTransData->nRouterReportCount;
Exit0:
	return;
}
	
inline void CSmsTransMgr::add_trans_router_report(int32_t nCount)
{
	LOG_PROCESS_ERROR(m_pShmTransData);
	m_pShmTransData->nRouterReportCount += nCount;
Exit0:
	return;
}

inline int32_t CSmsTransMgr::get_trans_router_report(void)
{
	LOG_PROCESS_ERROR(m_pShmTransData);
	return m_pShmTransData->nRouterReportCount;
Exit0:
	return 0;
}

inline BOOL CSmsTransMgr::is_server_in_cmd_list(int32_t nServerAddr)
{
	sk::fixed_queue<SMS_TRANS_CMD, MAX_TRANS_CMD_COUNT>::iterator it = m_pShmTransData->CmdList.begin();
	LOG_PROCESS_ERROR(m_pShmTransData);

	for (; it != m_pShmTransData->CmdList.end(); it++)
	{
		if (it->nServerAddr == nServerAddr)
			return TRUE;
	}

Exit0:
	return FALSE;
}
	
inline BOOL CSmsTransMgr::is_in_process(void)
{
	LOG_PROCESS_ERROR(m_pShmTransData);

	return m_pShmTransData->bInProcess;
Exit0:
	return FALSE;
}

#endif