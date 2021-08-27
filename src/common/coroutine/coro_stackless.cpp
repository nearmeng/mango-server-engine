#include "stdafx.h"
#include "coroutine/coro_stackless.h"

#include "time/time_mgr.h"
#include "config/global_config.h"

CGlobalStacklessMgr* CGlobalStacklessMgr::ms_Instance;

class CFakeCoro : public CCoroStackless
{
public:
    virtual CORO_STATE coro_process() { return crsInvalid; };
};

BOOL CCoroStackless::init(uint64_t qwCoroID)
{
    m_qwCoroID = qwCoroID;
    m_nYieldID = 0;
    
    m_dwCreateTime = CTimeMgr::instance().get_time_sec();
    m_dwYieldTime = 0;

    m_nState = crsInvalid;

    memset(&m_Reply, 0, sizeof(m_Reply));
    m_nCoroRetCode = crcSuccess; 

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCoroStackless::uninit()
{
    return TRUE;
}

CORO_STATE CCoroStackless::_yield_func(int32_t nYieldID)
{
    m_nYieldID = nYieldID;
    m_dwYieldTime = CTimeMgr::instance().get_time_sec();
    memset(&m_Reply, 0, sizeof(m_Reply));
    m_nCoroRetCode = crcSuccess; 
    
    return crsRunning;
}

BOOL CGlobalStacklessMgr::init(int32_t nServerAddr, BOOL bResume)
{
    int32_t nRetCode = 0;
    GLOBAL_STACKLESS_MGR_DATA* pMgrData = NULL;

    m_nServerAddr = nServerAddr;
    m_nCurrCoroStackIndex = 0;
    memset(m_pCurrCoro, 0, sizeof(m_pCurrCoro));

    nRetCode = m_MgrData.init(stdStacklessGlobalMgr, bResume);
    LOG_PROCESS_ERROR(nRetCode);

    pMgrData = m_MgrData.get_obj();
    LOG_PROCESS_ERROR(pMgrData);

    INF("[CORO]: global stack mgr init count %d", m_CoroStacklessMgrList.size());

    if (!bResume)
    {
        //construct shm type for every mgr
        int32_t& nInfoCount = pMgrData->nShmTypeInfoCount;
        LOG_PROCESS_ERROR(nInfoCount == 0);

        for (std::vector<CORO_MGR_DATA>::iterator it = m_CoroStacklessMgrList.begin(); it != m_CoroStacklessMgrList.end(); it++)
        {
            LOG_PROCESS_ERROR(nInfoCount < MAX_CORO_TYPE_COUNT);

            strxcpy(pMgrData->stShmTypeInfo[nInfoCount].szMgrName, it->szMgrName, COMMON_NAME_LEN);
            pMgrData->stShmTypeInfo[nInfoCount].nShmType = stdStacklessCoreBegin + nInfoCount;
            LOG_PROCESS_ERROR(pMgrData->stShmTypeInfo[nInfoCount].nShmType <= stdStacklessCoreEnd);

            nInfoCount++;
        }
    }

    for (std::vector<CORO_MGR_DATA>::iterator it = m_CoroStacklessMgrList.begin(); it != m_CoroStacklessMgrList.end(); it++)
    {
        CCoroStacklessMgr<CFakeCoro>* pCoroMgr = (CCoroStacklessMgr<CFakeCoro>*)(it->pMgr);

        int32_t nShmType = _get_shm_type_by_mgr_name(it->szMgrName);
        LOG_PROCESS_ERROR(nShmType > 0);

        DBG("[CORO]: init coro mgr %s with shm_type %d count %d", it->szMgrName, nShmType, g_ServerConfig.Common.nCommonCoroCount);

        nRetCode = pCoroMgr->init(nShmType, g_ServerConfig.Common.nCommonCoroCount, bResume);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGlobalStacklessMgr::uninit(void)
{
    for (std::vector<CORO_MGR_DATA>::iterator it = m_CoroStacklessMgrList.begin(); it != m_CoroStacklessMgrList.end(); it++)
    {
        CCoroStacklessMgr<CFakeCoro>* pCoroMgr = (CCoroStacklessMgr<CFakeCoro>*)(it->pMgr);

        LOG_CHECK_ERROR(pCoroMgr->uninit());
    }
    return TRUE;
}

void CGlobalStacklessMgr::mainloop(void)
{
    std::vector<CORO_MGR_DATA>::iterator it = m_CoroStacklessMgrList.begin();
    for (; it != m_CoroStacklessMgrList.end(); it++)
    {
        CCoroStacklessMgr<CFakeCoro>* pCoroMgr = (CCoroStacklessMgr<CFakeCoro>*)(it->pMgr);

        pCoroMgr->mainloop();
    }

    return;
}

BOOL CGlobalStacklessMgr::add_coro_stackless_mgr(const char* pcszMgrName, void* pMgr)
{
    int32_t nRetCode = 0;
    CORO_MGR_DATA stData;

    LOG_PROCESS_ERROR(pMgr);
    LOG_PROCESS_ERROR(pcszMgrName);

    stData.pMgr = pMgr;
    strxcpy(stData.szMgrName, pcszMgrName, sizeof(stData.szMgrName));
    m_CoroStacklessMgrList.push_back(stData);
    
    INF("[CORO]: add_coro_stack_mgr count %d", m_CoroStacklessMgrList.size());

    return TRUE;
Exit0:
    return FALSE;
}
    
int32_t CGlobalStacklessMgr::_get_shm_type_by_mgr_name(const char* pcszMgrName)
{
    int32_t nRetCode = 0;
    GLOBAL_STACKLESS_MGR_DATA* pMgrData = NULL;

    pMgrData = m_MgrData.get_obj();
    LOG_PROCESS_ERROR(pMgrData);

    for (int32_t i = 0; i < pMgrData->nShmTypeInfoCount; i++)
    {
        if (strncmp(pMgrData->stShmTypeInfo[i].szMgrName, pcszMgrName, sizeof(pMgrData->stShmTypeInfo[i].szMgrName)) == 0)
            return pMgrData->stShmTypeInfo[i].nShmType;
    }

Exit0:
    return 0;
}
    
BOOL CGlobalStacklessMgr::push_curr_coro(CCoroStackless* pCoro)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pCoro);
    LOG_PROCESS_ERROR(m_nCurrCoroStackIndex < MAX_CORO_STACK_DEPTH);

    m_pCurrCoro[m_nCurrCoroStackIndex++] = pCoro;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGlobalStacklessMgr::pop_curr_coro(void)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(m_nCurrCoroStackIndex > 0);

    m_pCurrCoro[m_nCurrCoroStackIndex - 1] = NULL;
    m_nCurrCoroStackIndex--;

    return TRUE;
Exit0:
    return FALSE;
}

uint64_t CGlobalStacklessMgr::generate_coro_id(int32_t nMgrIndex)
{
    int32_t nRetCode = 0;
    GLOBAL_STACKLESS_MGR_DATA* pMgrData = NULL;

    pMgrData = m_MgrData.get_obj();
    LOG_PROCESS_ERROR(pMgrData);

    return MAKE_CORO_ID(m_nServerAddr, nMgrIndex, pMgrData->nCoroIDGenerator++);
Exit0:
    return -1;
}
    
CCoroStackless* CGlobalStacklessMgr::get_coro(uint64_t qwCoroID)
{
    int32_t nRetCode = 0;
    int32_t nMgrIndex = 0;
    CCoroStackless* pCoro = NULL;
    CCoroStacklessMgr<CFakeCoro>* m_pCoroMgr = NULL;
        
    nMgrIndex = GET_CORO_MGR_INDEX(qwCoroID);
    LOG_PROCESS_ERROR(nMgrIndex >= 0 && nMgrIndex < MAX_CORO_TYPE_COUNT);

    m_pCoroMgr = (CCoroStacklessMgr<CFakeCoro>*)m_CoroStacklessMgrList[nMgrIndex].pMgr;
    LOG_PROCESS_ERROR(m_pCoroMgr);

    return m_pCoroMgr->find_coro(qwCoroID);
Exit0:
    return NULL;
}

BOOL CGlobalStacklessMgr::resume_coro(CCoroStackless* pCoro)
{
    int32_t nRetCode = 0;
    int32_t nMgrIndex = 0;
    CCoroStacklessMgr<CFakeCoro>* m_pCoroMgr = NULL;

    LOG_PROCESS_ERROR(pCoro);

    nMgrIndex = GET_CORO_MGR_INDEX(pCoro->get_coro_id());
    LOG_PROCESS_ERROR(nMgrIndex >= 0 && nMgrIndex < MAX_CORO_TYPE_COUNT);

    m_pCoroMgr = (CCoroStacklessMgr<CFakeCoro>*)m_CoroStacklessMgrList[nMgrIndex].pMgr;
    LOG_PROCESS_ERROR(m_pCoroMgr);

    DBG("[CORO]: coro %llu is resume ret_code %d", pCoro->get_coro_id(), pCoro->get_coro_ret_code());

    nRetCode = m_pCoroMgr->resume_coro(pCoro);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}