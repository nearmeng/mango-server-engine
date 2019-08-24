#include "stdafx.h"

#include "time/time_mgr.h"
#include "shm/shm_pool.h"
#include "guid/guid.h"

#include "game_data/global_config.h"

CTimeMgr CTimeMgr::ms_Instance;

BOOL CTimeMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;
	
	nRetCode = m_ShmTimerMgr.init(stdTimeMgr, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ShmTimerPool.init(stdTimerPool, g_ServerConfig.Common.nInitTimerPoolCount, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	m_pTimeMgr = m_ShmTimerMgr.get_obj();
	LOG_PROCESS_ERROR(m_pTimeMgr);

	if (!bResume)
	{
		memset(m_pTimeMgr, 0, sizeof(TIME_MGR_DATA));

		struct timeval tv;
		gettimeofday(&tv, NULL);

		m_pTimeMgr->nOffSet = 0;
		m_pTimeMgr->nCurrTimeSec = tv.tv_sec + m_pTimeMgr->nOffSet;
		m_pTimeMgr->llCurrTimeUSec = (int64_t)(m_pTimeMgr->nCurrTimeSec * 1000000) + tv.tv_usec;
		m_pTimeMgr->nStartTimeSec = tv.tv_sec;
		m_pTimeMgr->llCurTick = MS2TICK(tv.tv_usec / 1000);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimeMgr::uninit(void)
{
	return TRUE;
}

void CTimeMgr::refresh_time(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	m_pTimeMgr->nCurrTimeSec = tv.tv_sec + m_pTimeMgr->nOffSet;
	m_pTimeMgr->llCurrTimeUSec = (int64_t)(m_pTimeMgr->nCurrTimeSec) * 1000000 + tv.tv_usec;
}

int64_t CTimeMgr::get_real_time_tick(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return MS2TICK((tv.tv_sec - m_pTimeMgr->nStartTimeSec) * 1000 + (tv.tv_usec / 1000));
}

void CTimeMgr::mainloop(void)
{
	int64_t llRealTick = 0;

	llRealTick = get_real_time_tick();
	while (llRealTick > m_pTimeMgr->llCurTick)
	{
		int64_t nTickCount = llRealTick - m_pTimeMgr->llCurTick;
		for (int32_t i = 0; i < nTickCount; ++i)
		{
			_internal_run_timer();
			(m_pTimeMgr->llCurTick)++;
		}

		llRealTick = get_real_time_tick();
	}

	refresh_time();

Exit0:
	return;
}
	
BOOL CTimeMgr::add_timer(int32_t nFirstInterval, int32_t nRepeatInterval, int32_t nRepeatTimes,
	int32_t nTimerType, const void* pCbData, size_t nCbDataLen)
{
	int32_t nRetCode = 0;
	uint64_t qwTimerID = 0;
	uint64_t qwHandlerMid = 0;
	LINK_HEAD* pLinkList = NULL;
	TIMER_HANDLER* pTimerHandler = NULL;

	LOG_PROCESS_ERROR(nFirstInterval >= 0);
	LOG_PROCESS_ERROR(nRepeatInterval > 0);
	LOG_PROCESS_ERROR(nRepeatTimes > 0);
	LOG_PROCESS_ERROR(nTimerType > ttInvalid && nTimerType < ttTotal);
	LOG_PROCESS_ERROR(nCbDataLen < MAX_TIMER_CB_DATA_LEN);
	LOG_PROCESS_ERROR(m_pTimeMgr->pTimeoutFunc[nTimerType]);

	if (nCbDataLen > 0)
		LOG_PROCESS_ERROR(pCbData);

	qwTimerID = guid_alloc(gtdTimer);
	LOG_PROCESS_ERROR(qwTimerID > 0);

	pTimerHandler = m_ShmTimerPool.new_object(qwTimerID);
	LOG_PROCESS_ERROR(pTimerHandler);

	pTimerHandler->llExpires = m_pTimeMgr->llCurTick + nFirstInterval;
	pTimerHandler->nInterval = nRepeatInterval;
	pTimerHandler->wTimerType = nTimerType;
	pTimerHandler->nRepeatTimes = nRepeatTimes;
	pTimerHandler->bRunning = FALSE;
	pTimerHandler->wDataLen = nCbDataLen;
	if (pTimerHandler->wDataLen > 0)
		memcpy(pTimerHandler->byData, pCbData, nCbDataLen);

	pLinkList = _find_handle_list(pTimerHandler->llExpires);
	pTimerHandler->llListOffset = (int64_t)pLinkList - (int64_t)m_pTimeMgr;
	
	qwHandlerMid = CShmMgr::get_instance().ptr2mid<TIMER_HANDLER>(pTimerHandler);
	add_tail<TIMER_HANDLER>(pLinkList, qwHandlerMid);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimeMgr::del_timer(uint64_t qwTimerMid)
{
	int32_t nRetCode = 0;
	uint64_t qwHanderMid = 0;
	LINK_HEAD* pLinkHead = NULL;
	TIMER_HANDLER* pTimerHandler = NULL;
	
	LOG_PROCESS_ERROR(qwTimerMid > 0);

	pTimerHandler = CShmMgr::get_instance().mid2ptr<TIMER_HANDLER>(qwTimerMid);
	LOG_PROCESS_ERROR(pTimerHandler);

	if (pTimerHandler->bRunning)
	{
		pTimerHandler->nRepeatTimes = 0;
		return TRUE;
	}

	pLinkHead = (LINK_HEAD*)(m_pTimeMgr + pTimerHandler->llListOffset);
	LOG_PROCESS_ERROR(pLinkHead);

	del_node<TIMER_HANDLER>(pLinkHead, qwHanderMid);

	nRetCode = m_ShmTimerPool.delete_object(pTimerHandler);
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimeMgr::set_timer_callback(int32_t nTimerType, TIMEOUT_FUNC pFunc)
{
	LOG_PROCESS_ERROR(nTimerType > ttInvalid && nTimerType < ttTotal);
	LOG_PROCESS_ERROR(pFunc);
	
	m_pTimeMgr->pTimeoutFunc[nTimerType] = pFunc;

	return TRUE;
Exit0:
	return FALSE;
}

LINK_HEAD* CTimeMgr::_find_handle_list(int64_t llExpires)
{
	int64_t	llIndex = llExpires - m_pTimeMgr->llCurTick;

	if (llIndex < TV1_MAX)
	{
		return &(m_pTimeMgr->tv1.stVec[TV1_IDX(llExpires)]);
	}
	else if (llIndex < TV2_MAX)
	{
		return &(m_pTimeMgr->tv2.stVec[TV2_IDX(llExpires)]);
	}
	else if (llIndex < TV3_MAX)
	{
		return &(m_pTimeMgr->tv3.stVec[TV3_IDX(llExpires)]);
	}
	else if (llIndex < TV4_MAX)
	{
		return &(m_pTimeMgr->tv4.stVec[TV4_IDX(llExpires)]);
	}
	else
	{
		if (llIndex > 0xFFFFFFFFUL)
		{
			llIndex = 0xFFFFFFFFUL;
			llExpires = llIndex + m_pTimeMgr->llCurTick;
		}
		return &(m_pTimeMgr->tv5.stVec[TV5_IDX(llExpires)]);
	}
}

void CTimeMgr::_internal_run_timer(void)
{
	int32_t nRetCode = 0;
	LINK_HEAD* pTimerList = NULL;
	int32_t nTv1Idx = TV1_IDX(m_pTimeMgr->llCurTick);

	do 
	{
		if (nTv1Idx != 0) 
			break;	// curr has tv1 timer to handle
		
		int32_t nTv2Idx = TV2_IDX(m_pTimeMgr->llCurTick);
		_cascade(&m_pTimeMgr->tv2.stVec[nTv2Idx]);
		if (nTv2Idx != 0) 
			break;

		int32_t nTv3Idx = TV3_IDX(m_pTimeMgr->llCurTick);
		_cascade(&m_pTimeMgr->tv3.stVec[nTv3Idx]);
		if (nTv3Idx != 0) 
			break;
		
		int32_t nTv4Idx = TV4_IDX(m_pTimeMgr->llCurTick);
		_cascade(&m_pTimeMgr->tv4.stVec[nTv4Idx]);
		if (nTv4Idx != 0) 
			break;
		
		int32_t nTv5Idx = TV5_IDX(m_pTimeMgr->llCurTick);
		_cascade(&m_pTimeMgr->tv5.stVec[nTv5Idx]);

	} while (0);

	pTimerList = &m_pTimeMgr->tv1.stVec[nTv1Idx];

	while (pTimerList->qwHeadObjMid != 0)
	{
		uint64_t qwTimerMid = pTimerList->qwHeadObjMid;
		TIMER_HANDLER* pTimerHandler = CShmMgr::get_instance().mid2ptr<TIMER_HANDLER>(qwTimerMid);
		LOG_PROCESS_ERROR(pTimerHandler);

		// perhaps core when running
		if (pTimerHandler->bRunning)
		{
			CRI("timer was crashed at last time, timer_type %d", pTimerHandler->wTimerType);
			if (pTimerHandler->nRepeatTimes <= 0)
			{
				del_node<TIMER_HANDLER>(pTimerList, qwTimerMid);
				LOG_CHECK_ERROR(m_ShmTimerPool.delete_object(pTimerHandler));

				continue;
			}
		}

		pTimerHandler->bRunning = TRUE;
		pTimerHandler->nRepeatTimes--;

		m_pTimeMgr->pTimeoutFunc[pTimerHandler->wTimerType](qwTimerMid, pTimerHandler->byData, pTimerHandler->wDataLen);

		// just in cast of delete self in timeout func
		pTimerHandler = CShmMgr::get_instance().mid2ptr<TIMER_HANDLER>(qwTimerMid);
		if (pTimerHandler == NULL)
		{
			continue;
		}

		pTimerHandler->bRunning = FALSE;

		if (pTimerHandler->nRepeatTimes <= 0)
		{
			INF("timer type %d is finished success", pTimerHandler->wTimerType);
			del_node<TIMER_HANDLER>(pTimerList, qwTimerMid);
			LOG_CHECK_ERROR(m_ShmTimerPool.delete_object(pTimerHandler));
			continue;
		}
		else
		{
			pTimerHandler->llExpires = m_pTimeMgr->llCurTick + pTimerHandler->nInterval;
			LINK_HEAD* pNewList = _find_handle_list(pTimerHandler->llExpires);
			pTimerHandler->llListOffset = (int64_t)pNewList - (int64_t)m_pTimeMgr;
			del_node<TIMER_HANDLER>(pTimerList, qwTimerMid);
			add_tail<TIMER_HANDLER>(pNewList, qwTimerMid);
		}
	}

Exit0:
	return;
}

void CTimeMgr::_cascade(LINK_HEAD* pTimerList)
{
	int32_t nRetCode = 0;
	uint64_t qwTimerMid = 0;

	LOG_PROCESS_ERROR(pTimerList);

	qwTimerMid = pTimerList->qwHeadObjMid;
	while (qwTimerMid != 0)
	{
		uint64_t qwNextTimerMid = 0;
		LINK_HEAD* pNewList = NULL;
		TIMER_HANDLER* pTimerHandler = NULL; 
		
		pTimerHandler = CShmMgr::get_instance().mid2ptr<TIMER_HANDLER>(qwTimerMid);
		LOG_PROCESS_ERROR(pTimerHandler);

		pNewList = _find_handle_list(pTimerHandler->llExpires);
		LOG_PROCESS_ERROR(pNewList);

		qwNextTimerMid = pTimerHandler->qwNextObjMid;

		pTimerHandler->llListOffset = (int64_t)pTimerHandler - (int64_t)m_pTimeMgr;
		del_node<TIMER_HANDLER>(pTimerList, qwTimerMid);
		add_tail<TIMER_HANDLER>(pNewList, qwTimerMid);

		qwTimerMid = qwNextTimerMid;
	}

	pTimerList->qwHeadObjMid = 0;
	pTimerList->qwTailObjMid = 0;

Exit0:
	return;
}