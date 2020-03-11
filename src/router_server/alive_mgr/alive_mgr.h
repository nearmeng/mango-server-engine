#ifndef _ALIVE_MGR_H_
#define _ALIVE_MGR_H_

#include "shm/shm_pool.h"
#include "define/router_def.h"

struct SERVER_ALIVE_INFO
{
	int32_t		nTbusAddr;
	BOOL		bAlive;
	int32_t		nLoadValue;
	uint32_t	dwLastHeartBeatTime;
};

struct ALIVE_DATA
{
	int32_t				nServerCount;
	SERVER_ALIVE_INFO 	ServerAliveInfo[MAX_SERVER_PER_ROUTER];		//建立在单个router下挂的节点不会太多 过多可以继续优化

	int32_t				dwLastReportTime;
};

class CAliveMgr
{
public:
	CAliveMgr() {};
	~CAliveMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	void mainloop();
	inline static CAliveMgr& get_instance();

	inline ALIVE_DATA* get_alive_data(void);
	SERVER_ALIVE_INFO* get_server_alive_info(int32_t nTbusAddr);

	BOOL add_server_alive_info(int32_t nTbusAddr);
	BOOL del_server_alive_info(int32_t nTbusAddr);

	int32_t get_server_count(int32_t nServiceType);
	int32_t get_alive_server_count();
 
private:
	static CAliveMgr			ms_Instance;
	CShmObject<ALIVE_DATA>		m_ShmAliveData;
	ALIVE_DATA*					m_pAliveData;
};
	
inline CAliveMgr& CAliveMgr::get_instance()
{
	return ms_Instance;
}

inline ALIVE_DATA* CAliveMgr::get_alive_data(void)
{
	return m_pAliveData;
}

#endif