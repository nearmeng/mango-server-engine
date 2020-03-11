#ifndef _TIME_H_
#define _TIME_H_

#include "define/time_def.h"

class CTimeMgr
{
public:
	CTimeMgr() {};
	~CTimeMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CTimeMgr& instance(void);
	void mainloop(void);

	//time
	void refresh_time(void);
	int64_t get_real_time_tick(void);
	uint64_t get_server_tick(void);

	inline int32_t get_time_sec(void);
	inline int64_t get_time_usec(void);
	inline int64_t get_time_msec(void);
	inline int64_t get_tick(void);
	
	static const char* get_time_str(uint32_t dwTime);
	static int32_t get_day_count(int32_t nTime, int32_t nTimeZone, int32_t nOffset);

	//timer
	BOOL add_timer(int32_t nFirstInterval, int32_t nRepeatInterval, int32_t nRepeatTimes, 
						int32_t nTimerType, const void* pCbData, size_t nCbDataLen);
	BOOL del_timer(uint64_t qwTimerMid);
	BOOL set_timer_callback(int32_t nTimerType, TIMEOUT_FUNC pFunc);

private:
	LINK_HEAD* _find_handle_list(int64_t llExpires);
	void _internal_run_timer(void);
	void _cascade(LINK_HEAD* pTimerList);

private:
	TIME_MGR_DATA*							m_pTimeMgr;
	CShmObject<TIME_MGR_DATA>				m_ShmTimerMgr;
	CShmObjectPool<TIMER_HANDLER, uint64_t> m_ShmTimerPool;

	static CTimeMgr ms_Instance;
};

inline int32_t CTimeMgr::get_time_sec()
{
	return m_pTimeMgr->nCurrTimeSec;
}

inline int64_t CTimeMgr::get_time_usec()
{
	return m_pTimeMgr->llCurrTimeUSec;
}

inline int64_t CTimeMgr::get_time_msec()
{
	return m_pTimeMgr->llCurrTimeUSec / 1000;
}

inline int64_t CTimeMgr::get_tick()
{
	return m_pTimeMgr->llCurTick;
}

inline CTimeMgr & CTimeMgr::instance(void)
{
	return ms_Instance;
}

#endif