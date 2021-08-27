#include "coro_stackless.h"
#ifndef _CORO_STACKLESS_INL_H_
#define _CORO_STACKLESS_INL_H_

template<class T>
CCoroStacklessMgr<T> CCoroStacklessMgr<T>::ms_Instance;

inline uint64_t CCoroStackless::get_coro_id(void)
{
    return m_qwCoroID;
}

inline int32_t CCoroStackless::get_yield_id(void)
{
    return m_nYieldID;
}

inline uint32_t CCoroStackless::get_create_time(void)
{
    return m_dwCreateTime;
}

inline uint32_t CCoroStackless::get_yield_time(void)
{
    return m_dwYieldTime;
}

inline int32_t CCoroStackless::get_state(void)
{
    return m_nState;
}

inline CORO_REPLY& CCoroStackless::get_coro_reply(void)
{
    return m_Reply;
}

inline int32_t CCoroStackless::get_coro_ret_code()
{
    return m_nCoroRetCode;
}
    
inline void CCoroStackless::set_coro_ret_code(int32_t nRetCode)
{
    m_nCoroRetCode = nRetCode;
}

inline void CCoroStackless::set_state(int32_t nState)
{
    m_nState = nState;
}

inline void CCoroStackless::set_coro_reply(int32_t nReplyType, void* pReplyData, uint32_t dwDataLen)
{
    m_Reply.nReplyType = nReplyType;
    m_Reply.pReplyData = pReplyData;
    m_Reply.nReplyLen = dwDataLen;
}

template<class T>
CCoroStacklessMgr<T>::CCoroStacklessMgr()
{
    CGlobalStacklessMgr::instance().add_coro_stackless_mgr(typeid(T).name(), this);
}

template<class T>
BOOL CCoroStacklessMgr<T>::init(int32_t nShmType, BOOL nInitCoroCount, BOOL bResume)
{
    int32_t nRetCode = 0;

    m_nMgrIndex = nShmType - stdStacklessCoreBegin;
    LOG_PROCESS_ERROR(m_nMgrIndex >= 0);

    nRetCode = m_CoroPool.init(nShmType, nInitCoroCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    memset(&m_ReadyLinkHead, 0, sizeof(m_ReadyLinkHead));
    memset(&m_RunLinkHead, 0, sizeof(m_RunLinkHead));
    memset(&m_DeleteLinkHead, 0, sizeof(m_DeleteLinkHead));

    if (bResume)
    {
        //traverse resume
        TRAVERSE_CORO_RESUME coro_resume;
        m_CoroPool.traverse(coro_resume);

        //construct index
        T* pCoro = m_CoroPool.get_first_object();
        while (pCoro)
        {
			uint64_t qwCoroMid = SHM_PTR2MID(pCoro);

            switch (pCoro->get_state())
            {
            case crsReady:
                add_tail<T>(&m_ReadyLinkHead, qwCoroMid);
                break;
            case crsStart:
                CRI("[CORO]: coro %llu was cored when execute, so discard", pCoro->get_coro_id());
                add_tail<T>(&m_DeleteLinkHead, qwCoroMid);
                break;
            case crsRunning:
                add_tail<T>(&m_RunLinkHead, qwCoroMid);
                break;
            case crsStop:
            case crsFailed:
                add_tail<T>(&m_DeleteLinkHead, qwCoroMid);
                break;
            default:
                CRI("[CORO]: coro %llu has invalid state %d, so discard", pCoro->get_coro_id(), pCoro->get_state());
                add_tail<T>(&m_DeleteLinkHead, qwCoroMid);
                break;
            }
            pCoro = m_CoroPool.get_next_object();
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

template<class T>
BOOL CCoroStacklessMgr<T>::uninit(void)
{
    return TRUE;
}

template<class T>
void CCoroStacklessMgr<T>::mainloop(void)
{
    int32_t nRetCode = 0;
    int32_t nCounter = 0;
    const int32_t nExecuteReadyCoroPerLoop = 100;
    const int32_t nExecuteDeleteCoroPerLoop = 100;

    //check ready
    while (m_ReadyLinkHead.qwHeadObjMid != 0 && nCounter < nExecuteReadyCoroPerLoop)
    {
        T* pCoro = (T*)SHM_MID2PTR(m_ReadyLinkHead.qwHeadObjMid);
        if (pCoro == NULL)
        {
            CRI("[CORO]: get coro from mid %llu failed", m_ReadyLinkHead.qwHeadObjMid);
            del_node<T>(&m_ReadyLinkHead, m_ReadyLinkHead.qwHeadObjMid);
            continue;
        }

        nRetCode = del_node<T>(&m_ReadyLinkHead, m_ReadyLinkHead.qwHeadObjMid);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = start_coro(pCoro, TRUE);
        LOG_CHECK_ERROR(nRetCode);

        nCounter++;
    }

    //check run timeout
    while (m_RunLinkHead.qwHeadObjMid != 0)
    {
        T* pCoro = (T*)SHM_MID2PTR(m_RunLinkHead.qwHeadObjMid);
        if (pCoro == NULL)
        {
            CRI("[CORO]: get coro from mid %llu failed", m_RunLinkHead.qwHeadObjMid);
            del_node<T>(&m_RunLinkHead, m_RunLinkHead.qwHeadObjMid);
            continue;
        }

        if (CTimeMgr::instance().get_time_sec() > pCoro->get_yield_time() + g_ServerConfig.Common.nCoroWaitTimeInterval / 1000)
        {

            pCoro->set_coro_ret_code(crcTimeout);

            nRetCode = resume_coro(pCoro);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
            break;
    }

    //check delete
    nCounter = 0;
    while (m_DeleteLinkHead.qwHeadObjMid != 0 && nCounter < nExecuteReadyCoroPerLoop)
    {
        T* pCoro = (T*)SHM_MID2PTR(m_DeleteLinkHead.qwHeadObjMid);
        if (pCoro == NULL)
        {
            CRI("[CORO]: get coro from mid %llu failed", m_DeleteLinkHead.qwHeadObjMid);
            del_node<T>(&m_DeleteLinkHead, m_DeleteLinkHead.qwHeadObjMid);
            continue;
        }
        
        nRetCode = del_node<T>(&m_DeleteLinkHead, m_DeleteLinkHead.qwHeadObjMid);
        LOG_CHECK_ERROR(nRetCode);
        
        nRetCode = del_coro(pCoro);
        LOG_CHECK_ERROR(nRetCode);

        nCounter++;
    }

}

template<class T>
inline CCoroStacklessMgr<T> CCoroStacklessMgr<T>::instance(void)
{
    return ms_Instance;
}
    
template<class T>
T* CCoroStacklessMgr<T>::new_coro(void)
{
    int32_t nRetCode = 0;
    T* pCoro = NULL;
    uint64_t qwCoroID = 0;
    
    qwCoroID = CGlobalStacklessMgr::instance().generate_coro_id(m_nMgrIndex);
    LOG_PROCESS_ERROR(qwCoroID > 0);

    pCoro = m_CoroPool.new_object(qwCoroID);
    LOG_PROCESS_ERROR(pCoro);

    nRetCode = pCoro->init(qwCoroID);
    LOG_PROCESS_ERROR(nRetCode);

    DBG("[CORO]: coro %llu created", qwCoroID);

    return pCoro;
Exit0:
    if (pCoro)
        m_CoroPool.delete_object(pCoro);
    return NULL;
}

template<class T>
T* CCoroStacklessMgr<T>::find_coro(uint64_t qwCoroID)
{
    return m_CoroPool.find_object(qwCoroID);
}

template<class T>
BOOL CCoroStacklessMgr<T>::del_coro(T* pCoro)
{
    DBG("[CORO]: coro %llu deleted", pCoro->get_coro_id());
    return m_CoroPool.delete_object(pCoro);
}

template<class T>
BOOL CCoroStacklessMgr<T>::_on_coro_process_run(CCoroStackless* pCoro, int32_t nReturnState)
{
    int32_t nRetCode = 0;
	uint64_t qwCoroMid = SHM_PTR2MID(pCoro);

    pCoro->set_state(nReturnState);

    switch (nReturnState)
    {
    case crsRunning:
        DBG("[CORO]: coro %llu is yield", pCoro->get_coro_id());
        LOG_CHECK_ERROR(add_tail<T>(&m_RunLinkHead, qwCoroMid));
        break;
    case crsStop:
    case crsFailed:
        if (nReturnState == crsStop)
            DBG("[CORO]: coro %llu is finished", pCoro->get_coro_id());
        else
        {
            DBG("[CORO]: coro %llu is failed for internal error", pCoro->get_coro_id());
            pCoro->set_coro_ret_code(crcInternalError);
        }
        LOG_CHECK_ERROR(add_tail<T>(&m_DeleteLinkHead, qwCoroMid));
        break;
    case crsStart:
    default:
        LOG_PROCESS_ERROR(FALSE);
    }

    return TRUE;
Exit0:
    LOG_CHECK_ERROR(add_tail<T>(&m_DeleteLinkHead, qwCoroMid));
    return FALSE;
}

template<class T>
BOOL CCoroStacklessMgr<T>::start_coro(CCoroStackless* pCoro, BOOL bImmediate)
{
    int32_t nRetCode = 0;
    int32_t nReturnState = 0;

    LOG_PROCESS_ERROR(pCoro);

    if (bImmediate)
    {
        pCoro->set_state(crsStart);

        CGlobalStacklessMgr::instance().push_curr_coro(pCoro);
        nReturnState = pCoro->coro_process();
        CGlobalStacklessMgr::instance().pop_curr_coro();

        nRetCode = _on_coro_process_run(pCoro, nReturnState);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
	    uint64_t qwCoroMid = SHM_PTR2MID(pCoro);
        pCoro->set_state(crsReady);

        add_tail<T>(&m_ReadyLinkHead, qwCoroMid);
        INF("[CORO]: coro %llu put into ready list to execute", pCoro->get_coro_id());
    }

    return TRUE;
Exit0:
    if (pCoro)
    {
        CRI("[CORO]: coro %llu start failed", pCoro->get_coro_id());
    }

    return FALSE;
}

template<class T>
BOOL CCoroStacklessMgr<T>::resume_coro(CCoroStackless* pCoro)
{
    int32_t nRetCode = 0;
    int32_t nReturnState = 0;
	uint64_t qwCoroMid = SHM_PTR2MID(pCoro);

    LOG_PROCESS_ERROR(pCoro);
    LOG_PROCESS_ERROR(pCoro->get_state() == crsRunning);
            
    nRetCode = del_node<T>(&m_RunLinkHead, qwCoroMid);
    LOG_CHECK_ERROR(nRetCode);

    pCoro->set_state(crsStart);

    CGlobalStacklessMgr::instance().push_curr_coro(pCoro);
    nReturnState = pCoro->coro_process();
    CGlobalStacklessMgr::instance().pop_curr_coro();

    nRetCode = _on_coro_process_run(pCoro, nReturnState);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pCoro)
    {
        CRI("[CORO]: coro %llu resume failed", pCoro->get_coro_id());
    }
    return FALSE;
}

inline CGlobalStacklessMgr& CGlobalStacklessMgr::instance(void)
{
    // to guarantee constructed when call instance function, otherwise
    // global stack less mgr may construct later than add stack less mgr function
    if (ms_Instance == NULL)
        ms_Instance = new CGlobalStacklessMgr;

    return *ms_Instance;
}

inline CCoroStackless* CGlobalStacklessMgr::get_curr_coro(void)
{
    if (m_nCurrCoroStackIndex <= 0)
        return NULL;
    else
        return m_pCurrCoro[m_nCurrCoroStackIndex - 1];
}

#endif