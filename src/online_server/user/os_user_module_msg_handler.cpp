#include "stdafx.h"
#include "os_user_module.h"

#include "protocol/game_online_msg.h"

BOOL do_user_kick(int32_t nDstAddr, int32_t nErrorCode, uint64_t qwUserID, uint64_t qwSessionID)
{
    int32_t nRetCode = 0;
    O2G_USER_KICK msg;

    msg.nErrorCode = nErrorCode;
    msg.qwUserID = qwUserID;
    msg.qwSessionID = qwSessionID;

    nRetCode = send_server_msg_by_addr(nDstAddr, o2g_user_kick, &msg, sizeof(msg));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL do_user_login_ack(int32_t nDstAddr, int32_t nErrorCode, uint64_t qwUserID, uint64_t qwSessionID, uint64_t qwCoroID)
{
    int32_t nRetCode = 0;
    O2G_USER_LOGIN_ACK msg;

    msg.nErrorCode = nErrorCode;
    msg.qwUserID = qwUserID;
    msg.qwSessionID = qwSessionID;

    nRetCode = send_server_msg_by_addr(nDstAddr, o2g_user_login_ack, &msg, sizeof(msg), qwCoroID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL do_user_logout_ack(int32_t nDstAddr, int32_t nErrorCode, uint64_t qwUserID, uint64_t qwSessionID, uint64_t qwCoroID)
{
    int32_t nRetCode = 0;
    O2G_USER_LOGOUT_ACK msg;

    msg.nErrorCode = nErrorCode;
    msg.qwUserID = qwUserID;
    msg.qwSessionID = qwSessionID;

    nRetCode = send_server_msg_by_addr(nDstAddr, o2g_user_logout_ack, &msg, sizeof(msg), qwCoroID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void on_user_login(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
    int32_t nResult = errSuccess;
    CUserModule* pUserModule = NULL;
    OS_USER* pUser = NULL;
    G2O_USER_LOGIN* msg = (G2O_USER_LOGIN*)pBuffer;

    pUserModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pUserModule);

    pUser = pUserModule->find_user(msg->qwUserID);
    if (pUser)
    {
        nRetCode = do_user_kick(pUser->nServerAddr, errLoginRepeated, msg->qwUserID, msg->qwSessionID);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = do_user_login_ack(nSrcAddr, errLoginAlreadyLogin, msg->qwUserID, msg->qwSessionID, msg->qwCoroID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        pUser = pUserModule->new_user(msg->qwUserID);
        LOG_PROCESS_ERROR(pUser);

        pUser->qwUserID = msg->qwUserID;
        pUser->qwSessionID = msg->qwSessionID;
        pUser->nServerAddr = msg->nServerAddr;

        nRetCode = do_user_login_ack(nSrcAddr, errSuccess, msg->qwUserID, msg->qwSessionID, msg->qwCoroID);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void on_user_logout(int32_t nSrcAddr, const char* pBuffer, size_t dwSize)
{
    int32_t nRetCode = 0;
    CUserModule* pUserModule = NULL;
    OS_USER* pUser = NULL;
    G2O_USER_LOGOUT* msg = (G2O_USER_LOGOUT*)pBuffer;

    pUserModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pUserModule);

    pUser = pUserModule->find_user(msg->qwUserID);
    LOG_PROCESS_ERROR(pUser);
    LOG_PROCESS_ERROR(pUser->qwSessionID == msg->qwSessionID);
    LOG_PROCESS_ERROR(pUser->nServerAddr == msg->nServerAddr);

    nRetCode = do_user_logout_ack(nSrcAddr, errSuccess, msg->qwUserID, msg->qwSessionID, msg->qwCoroID);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

BOOL CUserModule::_init_msg_handler(void)
{
    int32_t nRetCode = 0;

    nRetCode |= register_server_msg_handler(g2o_user_login, on_user_login);
    nRetCode |= register_server_msg_handler(g2o_user_logout, on_user_logout);

    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}