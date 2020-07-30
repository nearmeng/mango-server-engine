#include "stdafx.h"
#include "db_proxy_client/db_proxy_client.h"

#include "protocol/db_proxy_message.h"
#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "define/redis_def.h"

#include "hiredis.h"

CDBProxyClient CDBProxyClient::ms_Instance;

static void _free_redis_reply(redisReply* pReply)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pReply);

    free(pReply->str);
    if (pReply->elements > 0)
    {
        for (int32_t i = 0; i < pReply->elements; i++)
        {
            _free_redis_reply(pReply->element[i]);
        }
        free(pReply->element);
    }

Exit0:
    return;
}

static BOOL _unpack_redis_reply(const char*& pPackedBuffer, int32_t& nPackedSize, redisReply** ppReply)
{
    int32_t nRetCode = 0;
    uint32_t dwDataSize = 0;

    LOG_PROCESS_ERROR(pPackedBuffer);
    LOG_PROCESS_ERROR(ppReply);
    LOG_PROCESS_ERROR(nPackedSize > 0);

    *ppReply = (redisReply*)malloc(sizeof(redisReply));
    LOG_PROCESS_ERROR(*ppReply);
    
    dwDataSize = offsetof(redisReply, str);
    memcpy((char*)*ppReply, pPackedBuffer, dwDataSize);
    pPackedBuffer += dwDataSize;
    nPackedSize -= dwDataSize;

    (*ppReply)->str = (char*)malloc((*ppReply)->len + 1);
    LOG_PROCESS_ERROR((*ppReply)->str);

    memcpy((*ppReply)->str, pPackedBuffer, (*ppReply)->len);
    (*ppReply)->str[(*ppReply)->len] = '\0';
    pPackedBuffer += (*ppReply)->len;
    nPackedSize -= (*ppReply)->len;

    dwDataSize = offsetof(redisReply, element) - offsetof(redisReply, vtype[0]);
    memcpy((*ppReply)->vtype, pPackedBuffer, dwDataSize);
    pPackedBuffer += dwDataSize;
    nPackedSize -= dwDataSize;

    (*ppReply)->element = (redisReply**)calloc((*ppReply)->elements, sizeof(redisReply*));
    LOG_PROCESS_ERROR((*ppReply)->element);

    for (int32_t i = 0; i < (*ppReply)->elements; i++)
    {
        _unpack_redis_reply(pPackedBuffer, nPackedSize, &((*ppReply)->element[i]));
    }

    return TRUE;
Exit0:
    if(*ppReply)
        _free_redis_reply(*ppReply);
    return FALSE;
}

static void on_db_proxy_client_redis_rsp(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
    redisReply* pReply = NULL;
    REDIS_CMD_CALLBACK pCallBack = NULL;
    DB_PROXY_CLIENT_REDIS_RSP* msg = (DB_PROXY_CLIENT_REDIS_RSP*)pBuffer;
    const char* pReplyBuffer = msg->szReplyBuffer;
    
    LOG_PROCESS_ERROR(msg->nCmdID > 0 && msg->nCmdID < MAX_CMD_HANDLER_COUNT);

    pCallBack = CDBProxyClient::instance().get_redis_callback(msg->nCmdID);
    LOG_PROCESS_ERROR(pCallBack);

    nRetCode = _unpack_redis_reply(pReplyBuffer, msg->nReplySize, &pReply);
    LOG_PROCESS_ERROR(nRetCode);

    pCallBack(pReply, msg->szUserData, msg->nUserDataSize);

    _free_redis_reply(pReply);

Exit0:
    return;
}

BOOL CDBProxyClient::init(BOOL bResume)
{
    memset(&m_RedisHandler, 0, sizeof(m_RedisHandler));

    register_server_msg_handler(db_proxy_client_redis_rsp, on_db_proxy_client_redis_rsp);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBProxyClient::uninit(void)
{
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBProxyClient::mainloop(void)
{
    int32_t nRetCode = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBProxyClient::redis_command(int32_t nCmdID, const char * pUserData, size_t dwUserDataLen, const char * format, ...)
{
    int32_t nRetCode = 0;
    va_list args;

    va_start(args, format);
    nRetCode = _command(nCmdID, pUserData, dwUserDataLen, NULL, format, args);
    va_end(args);
    
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CDBProxyClient::_command(int32_t nCmdID, const char* pUserData, size_t dwUserDataLen, const char* pcszScript, const char* format, va_list args) 
{
    int32_t nRetCode = 0;
    DB_PROXY_CLIENT_REDIS_REQ* pMsg = (DB_PROXY_CLIENT_REDIS_REQ*)alloca(sizeof(DB_PROXY_CLIENT_REDIS_REQ) + MAX_REDIS_COMMAND_SIZE);
    char* pCommandBuffer = pMsg->szCommandBuffer;
    int32_t& nCommandSize = pMsg->nCommandSize;
    
    pMsg->nCmdID = nCmdID;
    pMsg->nCommandSize = 0;
    if (pUserData &&dwUserDataLen > 0)
    {
        memcpy(pMsg->szUserData, pUserData, dwUserDataLen);
        pMsg->nUserDataSize = dwUserDataLen;
    }

    if (pcszScript)
    {
        nRetCode = _pack_redis_eval_script(pCommandBuffer, nCommandSize, pcszScript);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = _pack_redis_command(pCommandBuffer, nCommandSize, format, args);
    LOG_PROCESS_ERROR(nRetCode);

    LOG_PROCESS_ERROR(pMsg->nCommandSize > 0);
    DBG("redis command, packed command size %d", pMsg->nCommandSize);

    nRetCode = send_server_msg_by_routerid(CMGApp::instance().get_tbus_addr(), svrDBProxy, db_proxy_client_redis_req, pMsg, sizeof(DB_PROXY_CLIENT_REDIS_REQ) + nCommandSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
    
BOOL CDBProxyClient::_pack_redis_eval_script(char*& pPackedBuffer, int32_t &nPackedSize, const char* pcszScript)
{
    int32_t nRetCode = 0;
    const char* cmd = "EVAL";

    LOG_PROCESS_ERROR(pPackedBuffer);
    LOG_PROCESS_ERROR(pcszScript);
    
    *(int*)pPackedBuffer = strlen(cmd);
    nPackedSize += sizeof(int);
    pPackedBuffer += sizeof(int);

    strxcpy(pPackedBuffer, cmd, strlen(cmd));
    pPackedBuffer += strlen(cmd);
    nPackedSize += strlen(cmd);

    *(int*)pPackedBuffer = strlen(pcszScript);
    nPackedSize += sizeof(int);
    pPackedBuffer += sizeof(int);

    strxcpy(pPackedBuffer, pcszScript, strlen(pcszScript));
    nPackedSize += strlen(pcszScript);
    pPackedBuffer += strlen(pcszScript);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBProxyClient::redis_eval(int32_t nCmdID, const char * pUserData, size_t dwUserDataLen, const char * pcszScript, const char * format, ...)
{
    int32_t nRetCode = 0;
    va_list args;
    
    LOG_PROCESS_ERROR(pUserData);
    LOG_PROCESS_ERROR(pcszScript);
    LOG_PROCESS_ERROR(format);

    va_start(args, format);
    nRetCode = _command(nCmdID, pUserData, dwUserDataLen, pcszScript, format, args);
    va_end(args);

    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBProxyClient::_pack_redis_command(char* pPackedBuffer, int32_t &nPackedSize, const char* format, va_list valist)
{
    int32_t nRetCode = 0;
    const char* pAnalysis = format;

    LOG_PROCESS_ERROR(pPackedBuffer);
    LOG_PROCESS_ERROR(format);

    while (*pAnalysis != '\0')
    {
        if(*pAnalysis == ' ')
        {
            pAnalysis++;
            continue;
        }
        
        if (*pAnalysis != '%')
        {
            //command string or arg string
            DBG("get base string");

            const char* pArg = pAnalysis;
            uint32_t dwSize = 0;
            const char* pCurr = pAnalysis;
            while (*pCurr != ' ' && *pCurr != '\0')
                pCurr++;

            dwSize = pCurr - pArg;
            pAnalysis = pCurr;

            nPackedSize += dwSize + sizeof(int);
            LOG_PROCESS_ERROR(nPackedSize < MAX_REDIS_COMMAND_SIZE);

            *(int*)pPackedBuffer = (int)dwSize;
            memcpy(pPackedBuffer + sizeof(int), pArg, dwSize);
            pPackedBuffer += sizeof(int) + dwSize;
        }
        else
        {
            if (pAnalysis[1] == '\0') //end
                break;
            else
            {
                char* pArg = NULL;
                size_t dwSize = 0;

                switch (pAnalysis[1])
                {
                case 's':
                    DBG("get string");
                    pArg = va_arg(valist, char*);
                    dwSize = strlen(pArg);

                    pAnalysis += 2;
                    break;
                case 'b':
                    DBG("get binary");
                    pArg = va_arg(valist, char*);
                    dwSize = va_arg(valist, size_t);

                    pAnalysis += 2;
                    break;

                default:
                {
                    static const char intfmts[] = "diouxX";
                    static const char doublefmts[] = "eEfFgGaA";
                    static const char flags[] = "#0-+ ";
                    char format[32];
                    char szArgStr[128];
                    int32_t nFormatSize = 0;
                    const char* pCurr = pAnalysis + 1;
                    va_list vaCopy;

                    // flags
                    while (*pCurr != '\0' && strchr(flags, *pCurr) != NULL) pCurr++;

                    // field width
                    while (*pCurr != '\0' && isdigit(*pCurr)) pCurr++;

                    //precision
                    if (*pCurr == '.')
                    {
                        pCurr++;
                        while (*pCurr != '\0' && isdigit(*pCurr)) pCurr++;
                    }

                    //copy
                    va_copy(vaCopy, valist);

                    //interger conversion
                    if (strchr(intfmts, *pCurr) != NULL)
                        va_arg(valist, int);

                    //double conversion
                    if (strchr(doublefmts, *pCurr) != NULL)
                        va_arg(valist, double);

                    //short
                    if (*pCurr == 'h')
                    {
                        pCurr++;
                        LOG_PROCESS_ERROR(*pCurr != '\0' && strchr(intfmts, *pCurr) != NULL);
                        va_arg(valist, int);
                    }

                    //long long
                    if (*pCurr == 'l' && *(pCurr + 1) == 'l')
                    {
                        pCurr += 2;
                        LOG_PROCESS_ERROR(*pCurr != '\0' && strchr(intfmts, *pCurr) != NULL);
                        va_arg(valist, long long);
                    }

                    //long
                    if (*pCurr == 'l')
                    {
                        pCurr += 1;
                        LOG_PROCESS_ERROR(*pCurr != '\0' && strchr(intfmts, *pCurr) != NULL);
                        va_arg(valist, long);
                    }

                    nFormatSize = pCurr + 1 - pAnalysis;
                    LOG_PROCESS_ERROR(nFormatSize < sizeof(format));
                    memcpy(format, pAnalysis, nFormatSize);
                    format[nFormatSize] = '\0';

                    vsprintf(szArgStr, format, vaCopy);
                    pAnalysis = pCurr + 1;

                    pArg = szArgStr;
                    dwSize = strlen(szArgStr);

                    break;
                }
                }

                nPackedSize += (int)dwSize + sizeof(int);
                LOG_PROCESS_ERROR(nPackedSize < MAX_REDIS_COMMAND_SIZE);

                *(int*)pPackedBuffer = (int)dwSize;
                memcpy(pPackedBuffer + sizeof(int), pArg, dwSize);
                pPackedBuffer += sizeof(int) + dwSize;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}
    