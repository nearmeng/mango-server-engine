#include "stdafx.h"
#include "redis_mgr.h"

#include "adapters/libevent.h"

#include "time/time_mgr.h"
#include "config/global_config.h"

#include "event.h"
#include "app/server_app.h"

#include "redis_msg_handler.h"

CRedisCliMgr CRedisCliMgr::ms_Instance;

BOOL CRedisCliMgr::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    m_pEventBase = event_base_new();
    LOG_PROCESS_ERROR(m_pEventBase);

    return TRUE;
Exit0:
    LOG_CHECK_ERROR(uninit());
    return FALSE;
}

BOOL CRedisCliMgr::uninit()
{
    int32_t nRetCode = 0;

    if (m_pEventBase)
    {
        event_base_free_nofinalize(m_pEventBase);
        m_pEventBase = NULL;
    }
    
    return TRUE;
}

CRedisCli* CRedisCliMgr::new_client(const char* pcszName, const char* pcszHost, int32_t nPort, const char* pcszPassWord)
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    pClient = new CRedisCli;
    LOG_PROCESS_ERROR(pClient);

    nRetCode = pClient->init(pcszName, pcszHost, nPort, pcszPassWord);
    LOG_PROCESS_ERROR(nRetCode);

    m_ClientList.push_back(pClient);

    return pClient;
Exit0:

    if (pClient)
        LOG_CHECK_ERROR(del_client(pClient));
    return NULL;
}

BOOL CRedisCliMgr::del_client(CRedisCli* pClient)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pClient);
    
    nRetCode = pClient->uninit();
    LOG_CHECK_ERROR(nRetCode);

    delete pClient;

    return TRUE;
Exit0:
    return FALSE;
}

CRedisCli* CRedisCliMgr::find_client(const char* pcszName)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pcszName);
    
    for (std::list<CRedisCli*>::iterator it = m_ClientList.begin(); it!= m_ClientList.end(); it++)
    {
        CRedisCli* pClient = *it;
        if (pClient->get_name() == std::string(pcszName))
        {
            return pClient;
        }
    }

Exit0:
    return NULL;
}

void CRedisCliMgr::on_proc(void)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(m_pEventBase);

    for (int32_t i = 0; i < MAX_LOOP_PER_TIMES; i++)
    {
        nRetCode = event_base_loop(m_pEventBase, EVLOOP_NONBLOCK);
        LOG_PROCESS_ERROR(nRetCode >= 0);
        
        if (nRetCode == 1)
            break;
    }

Exit0:
    return;
}

void CRedisCliMgr::on_frame(void)
{
    for (std::list<CRedisCli*>::iterator it = m_ClientList.begin(); it != m_ClientList.end(); it++)
    {
        (*it)->mainloop();
    }
}

BOOL CRedisCli::init(const char * pcszName, const char * pcszHostName, int32_t nPort, const char * pcszPassWord)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(pcszHostName);
    LOG_PROCESS_ERROR(pcszPassWord);

    m_sClientName = pcszName;
    m_sHostName = pcszHostName;
    m_nPort = nPort;
    m_sPassword = pcszPassWord;
    m_nState = rcsInvalid;
    m_pCtx = NULL;

    m_nPingSec = 0;
    m_nPongSec = 0;
    m_nConnectSec = 0;
    m_nDisConnectSec = 0;
    m_CallBackList.clear();

    memset(&m_Handler, 0, sizeof(m_Handler));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::uninit(void)
{
    int32_t nRetCode = 0;
    switch (m_nState)
    {
    case rcsConnecting:
    case rcsConnected:
        nRetCode = sync_disconnect();
        LOG_CHECK_ERROR(nRetCode);
    }
    return TRUE;
}
    
void CRedisCli::mainloop(void)
{
    int32_t nRetCode = 0;
    int32_t nCurrTime = CTimeMgr::instance().get_time_sec();
    
    switch (m_nState)
    {
    case rcsAuthed:
        //check timeout
        for (std::list<CALLBACK_DATA*>::iterator it = m_CallBackList.begin();
            it != m_CallBackList.end(); )
        {
            CALLBACK_DATA* pCallBack = *it;
            if (nCurrTime > pCallBack->dwExpireTime)
            {
                CRI("callback timeout for cmdid %d", (*it)->nCmdID);

                free(pCallBack);
                m_CallBackList.erase(it++);
            }
            else
            {
                it++;
            }
        }

        // ping
        if (nCurrTime > m_nPingSec + g_ServerConfig.DP.nDBPingInterval / 1000)
        {
            nRetCode = _ping();
            LOG_CHECK_ERROR(nRetCode);
        }

        if (nCurrTime > m_nPongSec + g_ServerConfig.DP.nDBPingTimeout / 1000)
        {
            CRI("[REDIS] ping timeout, begin to disconnect");
            nRetCode = async_disconnect();
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
    case rcsDisConnecting:
    case rcsDisConnected:
        LOG_CHECK_ERROR(m_nDisConnectSec > 0);

        //reconnect
        if (nCurrTime > m_nDisConnectSec + g_ServerConfig.DP.nDBReConnectInterval / 1000)
        {
            nRetCode = sync_connect();
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
    default:
        break;
    }

Exit0:
    return;
}

void _on_connect(const struct redisAsyncContext* pCtx, int nStatus)
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    LOG_PROCESS_ERROR(pCtx);

    pClient = (CRedisCli*)pCtx->data;
    LOG_PROCESS_ERROR(pClient);

    nRetCode = pClient->on_connect(nStatus);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void _on_disconnect(const struct redisAsyncContext* pCtx, int nStatus)
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    LOG_PROCESS_ERROR(pCtx);

    pClient = (CRedisCli*)pCtx->data;
    LOG_PROCESS_ERROR(pClient);
    
    nRetCode = pClient->on_disconnect(nStatus);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void _on_auth(struct redisAsyncContext* pCtx, void* pReply, void* pPriData)
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    LOG_PROCESS_ERROR(pCtx);

    pClient = (CRedisCli*)pCtx->data;
    LOG_PROCESS_ERROR(pClient);

    nRetCode = pClient->on_auth((redisReply*)pReply);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void _on_ping(struct redisAsyncContext* pCtx, void* pReply, void* pPriData) 
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    LOG_PROCESS_ERROR(pCtx);

    pClient = (CRedisCli*)pCtx->data;
    LOG_PROCESS_ERROR(pClient);

    nRetCode = pClient->on_pong((redisReply*)pReply);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void _on_command(struct redisAsyncContext* pCtx, void* pReply, void* pPriData)
{
    int32_t nRetCode = 0;
    CRedisCli* pClient = NULL;

    LOG_PROCESS_ERROR(pCtx);

    pClient = (CRedisCli*)pCtx->data;
    LOG_PROCESS_ERROR(pClient);

    nRetCode = pClient->on_command((redisReply*)pReply, pPriData);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

BOOL CRedisCli::async_connect(void)
{
    int32_t nRetCode = 0;
    void* pEventBase = CRedisCliMgr::instance().get_event_base();

    LOG_CHECK_ERROR(m_nState == rcsInvalid || m_nState == rcsDisConnected || m_nState == rcsDisConnecting);

    m_pCtx = redisAsyncConnect(m_sHostName.c_str(), m_nPort);
    LOG_PROCESS_ERROR(m_pCtx);

    m_pCtx->data = this;

    nRetCode = redisLibeventAttach(m_pCtx, (struct event_base*)pEventBase);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    nRetCode = redisAsyncSetConnectCallback(m_pCtx, _on_connect);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    nRetCode = redisAsyncSetDisconnectCallback(m_pCtx, _on_disconnect);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    m_nState = rcsConnecting;

    INF("[REDIS] connecting to host %s port %d", m_sHostName.c_str(), m_nPort);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::sync_connect(void)
{
    int32_t nRetCode = 0;
    uint64_t qwStartTime = CTimeMgr::instance().get_server_tick();

    LOG_CHECK_ERROR(m_nState == rcsInvalid || m_nState == rcsDisConnected || m_nState == rcsDisConnecting);

    nRetCode = async_connect();
    LOG_PROCESS_ERROR(nRetCode);

    while (true)
    {
        uint64_t qwCurrTime = CTimeMgr::instance().get_server_tick();
        CRedisCliMgr::instance().on_proc();

        if (m_nState == rcsAuthed)
            break;
        else
        {
            if (qwCurrTime - qwStartTime >= g_ServerConfig.DP.nSyncConnectTimeout)
            {
                CRI("[REDIS] sync connect to host %s port %d timeout", m_sHostName.c_str(), m_nPort);
                return FALSE;
            }
            else
                usleep(5000);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::async_disconnect(void)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(m_pCtx);
    LOG_PROCESS_ERROR(m_nState == rcsAuthed || m_nState == rcsConnected);

    redisAsyncDisconnect(m_pCtx);

    m_nState = rcsDisConnecting;
    m_nDisConnectSec = CTimeMgr::instance().get_time_sec();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::sync_disconnect(void)
{
    int32_t nRetCode = 0;
    uint64_t qwStartTime = CTimeMgr::instance().get_server_tick();

    LOG_PROCESS_ERROR(m_nState == rcsAuthed || m_nState == rcsConnected);

    nRetCode = async_disconnect();
    LOG_PROCESS_ERROR(nRetCode);

    while (true)
    {
        uint64_t qwCurrTime = CTimeMgr::instance().get_server_tick();
        CRedisCliMgr::instance().on_proc();

        if (m_nState == rcsDisConnected)
            break;
        else
        {
            if (qwCurrTime - qwStartTime >= g_ServerConfig.DP.nSyncDisConnectTimeout)
            {
                CRI("[REDIS] sync disconnect to host %s port %d timeout", m_sHostName.c_str(), m_nPort);
                return FALSE;
            }
            else
                usleep(5000);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}
    
CRedisCli::CALLBACK_DATA* CRedisCli::_prepare_callback_data(int32_t nExecuteAddr, int32_t nCmdID, uint64_t qwCoroID, const char* pUserData, size_t dwDataLen)
{
    int32_t nRetCode = 0;
    CALLBACK_DATA* pCallBackData = NULL;

    LOG_PROCESS_ERROR(nCmdID >= 0 && nCmdID < MAX_CMD_HANDLER_COUNT);

    pCallBackData = (CALLBACK_DATA*)malloc(sizeof(CALLBACK_DATA));
    LOG_PROCESS_ERROR(pCallBackData);

    pCallBackData->nFence = FENCE_NUM;
    pCallBackData->nCmdID = nCmdID;
    pCallBackData->qwCoroID = qwCoroID;
    pCallBackData->nTbusAddr = nExecuteAddr;
    pCallBackData->dwUserDataLen = dwDataLen;
    pCallBackData->dwExpireTime = CTimeMgr::instance().get_time_sec() + g_ServerConfig.DP.nDBReqExpireTime / 1000;
    memcpy(pCallBackData->szUserData, pUserData, dwDataLen);

    m_CallBackList.push_back(pCallBackData);

    return pCallBackData;
Exit0:
    return NULL;
}

BOOL CRedisCli::command(int32_t nExecuteAddr, int32_t nCmdID, const char * pUserData, size_t dwDataLen, const char * format, ...)
{
    int32_t nRetCode = 0;
    va_list args;
    CALLBACK_DATA* pCallBackData = NULL;
    
    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    pCallBackData = _prepare_callback_data(nExecuteAddr, nCmdID, 0, pUserData, dwDataLen);
    LOG_PROCESS_ERROR(pCallBackData);

    va_start(args, format);
    nRetCode = _command_varlist(_on_command, pCallBackData, format, args);
    va_end(args);

    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::eval(int32_t nExecuteAddr, int32_t nCmdID, const char * pUserData, size_t dwDataLen, const char * format, ...)
{
    int32_t nRetCode = 0;
    va_list args;
    CALLBACK_DATA* pCallBackData = NULL;

    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    pCallBackData = _prepare_callback_data(nExecuteAddr, nCmdID, 0, pUserData, dwDataLen);
    LOG_PROCESS_ERROR(pCallBackData);

    va_start(args, format);
    nRetCode = _eval_varlist(_on_command, pCallBackData, format, args);
    va_end(args);

    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::command_arg(int32_t nExecuteAddr, int32_t nCmdID, uint64_t qwCoroID, const char* pUserData, size_t dwDataLen, int32_t nArgc, const char** pArgv, const size_t * nArgvLen)
{
    int32_t nRetCode = 0;
    CALLBACK_DATA* pCallBackData = NULL;

    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    pCallBackData = _prepare_callback_data(nExecuteAddr, nCmdID, qwCoroID, pUserData, dwDataLen);
    LOG_PROCESS_ERROR(pCallBackData);

    nRetCode = redisAsyncCommandArgv(m_pCtx, _on_command, pCallBackData, nArgc, pArgv, nArgvLen);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::eval_arg(int32_t nExecuteAddr, int32_t nCmdID, uint64_t qwCoroID, const char* pUserData, size_t dwDataLen, int32_t nArgc, const char** pArgv, const size_t* nArgvLen)
{
    int32_t nRetCode = 0;
    CALLBACK_DATA* pCallBackData = NULL;

    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    pCallBackData = _prepare_callback_data(nExecuteAddr, nCmdID, qwCoroID, pUserData, dwDataLen);
    LOG_PROCESS_ERROR(pCallBackData);

    nRetCode = redisAsyncCommandArgv(m_pCtx, _on_command, pCallBackData, nArgc, pArgv, nArgvLen);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::on_connect(int32_t nStatus)
{
    int32_t nRetCode = 0;

    m_nState = rcsConnected;

    if (nStatus == REDIS_OK)
    {
        INF("[REDIS] connected to host %s port %d, begin to do auth", m_sHostName.c_str(), m_nPort);

        nRetCode = _auth();
        LOG_PROCESS_ERROR(nRetCode);

        m_nState = rcsAuthing;
    }
    else
    {
        m_nState = rcsInvalid;
        CRI("[REDIS] connect to host %s port %d failed", m_sHostName.c_str(), m_nPort);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::on_disconnect(int32_t nStatus)
{
    int32_t nRetCode = 0;

    m_nState = rcsDisConnected;
    m_pCtx = NULL;
    m_nPingSec = 0;
    m_nPongSec = 0;
    m_nConnectSec = 0;
    m_nDisConnectSec = CTimeMgr::instance().get_time_sec();

    INF("[REDIS] disconnected from host %s port %d", m_sHostName.c_str(), m_nPort);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::on_auth(redisReply* pReply)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
    LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);
    
    LOG_PROCESS_ERROR(m_nState == rcsAuthing);

    m_nState = rcsAuthed;
    m_nConnectSec = CTimeMgr::instance().get_time_sec();
    m_nDisConnectSec = 0;

    INF("[REDIS] Authed to host %s port %d", m_sHostName.c_str(), m_nPort);
    
    nRetCode = _ping();
    LOG_CHECK_ERROR(nRetCode);

    //init pong sec to avoid check timeout
    m_nPongSec = CTimeMgr::instance().get_time_sec();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::on_pong(redisReply * pReply)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
    LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);

    m_nPongSec = CTimeMgr::instance().get_time_sec();

    return TRUE;
Exit0:
    CRI("state is %d", m_nState);
    return FALSE;
}
    
BOOL CRedisCli::on_command(redisReply* pReply, void* pCallBackData)
{
    int32_t nRetCode = 0;
    static char szRedisReplyBuffer[MAX_REDIS_REPLY_BUFFER_SIZE];
    CALLBACK_DATA* pCallBack = (CALLBACK_DATA*)pCallBackData;

    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pCallBack);
    LOG_PROCESS_ERROR(pCallBack->nFence == FENCE_NUM);

    if (pCallBack->nTbusAddr == CMGApp::instance().get_tbus_addr() || pCallBack->nTbusAddr == 0)
    {
        //local callback
        LOG_PROCESS_ERROR(m_Handler[pCallBack->nCmdID]);

        m_Handler[pCallBack->nCmdID](pReply, pCallBack->szUserData, pCallBack->dwUserDataLen);
    }
    else
    {
        //remote callback
        int32_t nPackedSize = 0;
        char* pBuffer = szRedisReplyBuffer;

        nRetCode = _pack_redis_reply(pReply, pBuffer, nPackedSize);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = send_db_proxy_client_redis_rsp(pCallBack->nTbusAddr, pCallBack->nCmdID, pCallBack->qwCoroID, pCallBack->dwUserDataLen, pCallBack->szUserData, szRedisReplyBuffer, nPackedSize);
        LOG_PROCESS_ERROR(nRetCode);
    }

    m_CallBackList.remove(pCallBack);
    free(pCallBack);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::_pack_redis_reply(redisReply * pReply, char *& pPackedBuffer, int32_t &nPackedSize)
{
    int32_t nRetCode = 0;
    uint32_t dwDataSize = 0;

    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pPackedBuffer);

    dwDataSize = offsetof(redisReply, str);
    memcpy(pPackedBuffer, (char*)pReply, dwDataSize);
    pPackedBuffer += dwDataSize;
    nPackedSize += dwDataSize;

    memcpy(pPackedBuffer, pReply->str, pReply->len);
    pPackedBuffer += pReply->len;
    nPackedSize += pReply->len;

    dwDataSize = offsetof(redisReply, element) - offsetof(redisReply, vtype[0]);
    memcpy(pPackedBuffer, pReply->vtype, dwDataSize);
    pPackedBuffer += dwDataSize;
    nPackedSize += dwDataSize;

    for (int32_t i = 0; i < pReply->elements; i++)
    {
        _pack_redis_reply(pReply->element[i], pPackedBuffer, nPackedSize);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::_auth(void)
{
    int32_t nRetCode = 0;
        
    nRetCode = redisAsyncCommand(m_pCtx, _on_auth, NULL, "AUTH %s", m_sPassword.c_str());
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CRedisCli::_ping(void)
{
    int32_t nRetCode = 0;

    nRetCode = redisAsyncCommand(m_pCtx, _on_ping, NULL, "PING");
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    m_nPingSec = CTimeMgr::instance().get_time_sec();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::_command_varlist(redisCallbackFn * pCallback, void * pPriData, const char * format, va_list valist)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pCallback);
    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    nRetCode = redisvAsyncCommand(m_pCtx, pCallback, pPriData, format, valist);
    LOG_PROCESS_ERROR(nRetCode == REDIS_OK);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::_eval_varlist(redisCallbackFn * pCallback, void* pPriData, const char * format, va_list valist)
{
    int32_t nRetCode = 0;
    static char fmt[128] = { 0 };

    LOG_PROCESS_ERROR(pCallback);
    LOG_PROCESS_ERROR(pPriData);
    LOG_PROCESS_ERROR(m_nState == rcsAuthed);

    snprintf(fmt, sizeof(fmt), "EVAL %%s %s", format);

    nRetCode = _command_varlist(pCallback, pPriData, fmt, valist);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRedisCli::reg_handler(int32_t nCmdID, REDIS_CMD_CALLBACK pCallBack)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pCallBack);
    LOG_PROCESS_ERROR(nCmdID > 0 && nCmdID < MAX_CMD_HANDLER_COUNT);
    LOG_PROCESS_ERROR(m_Handler[nCmdID] == NULL);

    m_Handler[nCmdID] = pCallBack;

    return TRUE;
Exit0:
    return FALSE;
}