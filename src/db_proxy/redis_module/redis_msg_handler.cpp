#include "stdafx.h"
#include "redis_msg_handler.h"

#include "protocol/db_proxy_message.h"
#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "redis_mgr.h"
#include "redis_module.h"

static BOOL _unpack_redis_command(const char* pForParseCommand, int32_t nCommandSize, REDIS_ARG_DATA* pArgData)
{
    int32_t nRetCode = 0;
    int32_t nLeftSize = nCommandSize;
    const char* pAnalysis = pForParseCommand;

    LOG_PROCESS_ERROR(pForParseCommand);
    LOG_PROCESS_ERROR(pArgData);

    pArgData->nArgc = 0;
    while (nLeftSize > 0)
    {
        uint32_t dwArgSize = *(int*)pAnalysis;
        pArgData->dwArgvSize[pArgData->nArgc] = dwArgSize;
        pAnalysis += sizeof(int);
        pArgData->pArgv[pArgData->nArgc] = (char*)pAnalysis;
        pArgData->nArgc++;

        pAnalysis += dwArgSize;
        nLeftSize -= sizeof(int) + dwArgSize;
    }

    return TRUE;
Exit0:
    return FALSE;
}

void on_db_proxy_client_redis_req(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
    CRedisModule* pModule = NULL;
    CRedisCli* pRedisClient = NULL;
    REDIS_ARG_DATA stArgData = { 0 };
    DB_PROXY_CLIENT_REDIS_REQ* msg = (DB_PROXY_CLIENT_REDIS_REQ*)pBuffer;
    
    pModule = MG_GET_MODULE(CRedisModule);
    LOG_PROCESS_ERROR(pModule);

    pRedisClient = pModule->get_test_client();
    LOG_PROCESS_ERROR(pRedisClient);

    nRetCode = _unpack_redis_command(msg->szCommandBuffer, msg->nCommandSize, &stArgData);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = pRedisClient->command_arg(nSrcAddr, msg->nCmdID, msg->qwCoroID, msg->szUserData, msg->nUserDataSize, stArgData.nArgc, (const char**)stArgData.pArgv, stArgData.dwArgvSize);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

BOOL CRedisModule::_init_msg_handler()
{
    int32_t nRetCode = 0;

    register_server_msg_handler(db_proxy_client_redis_req, on_db_proxy_client_redis_req);
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL send_db_proxy_client_redis_rsp(int32_t nTbusAddr, int32_t nCmdID, uint64_t qwCoroID, uint32_t dwUserDataLen, const char* pUserData, 
                                                const char* pPackedBuffer, int32_t nPackedSize)
{
    int32_t nRetCode = 0;
    uint32_t dwTotalSize = 0;
    DB_PROXY_CLIENT_REDIS_RSP* msg = NULL; 
    
    LOG_PROCESS_ERROR(pUserData);
    LOG_PROCESS_ERROR(pPackedBuffer);
    
    dwTotalSize = sizeof(DB_PROXY_CLIENT_REDIS_RSP) + nPackedSize;
    msg = (DB_PROXY_CLIENT_REDIS_RSP*)alloca(dwTotalSize);
    LOG_PROCESS_ERROR(msg);

    msg->nCmdID = nCmdID;
    msg->qwCoroID = qwCoroID;
    msg->nUserDataSize = dwUserDataLen;
    memcpy(msg->szUserData, pUserData, dwUserDataLen);
    msg->nReplySize = nPackedSize;
    memcpy(msg->szReplyBuffer, pPackedBuffer, nPackedSize);

    nRetCode = send_server_msg_by_addr(nTbusAddr, db_proxy_client_redis_rsp, msg, dwTotalSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}