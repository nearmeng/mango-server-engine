#include "stdafx.h"
#include "gs_user_module_coro.h"

#include "db_proxy_client/db_proxy_client.h"

#include "module/gs_user_module.h"
#include "module/session_module.h"
#include "module/role_module.h"

#include "protocol/game_online_msg.h"
#include "gs_user_module_msg_handler.h"

#include "event/server_event.h"

static char s_szUserData[MAX_USER_DATA_SIZE];
static char s_szRoleData[MAX_ROLE_DATA_SIZE];
static char s_szRoleBaseData[MAX_ROLE_DATA_SIZE/2];

void CLoginCoro::set_start_arg(const char * pcszOpenID, uint64_t qwSessionID)
{
    strxcpy(m_szOpenID, pcszOpenID, sizeof(m_szOpenID));
    m_qwSessionID = qwSessionID;
    m_qwUserID = 0;
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);
}

BOOL CLoginCoro::on_resume()
{
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);
    return TRUE;
}

CORO_STATE CLoginCoro::coro_process()
{
    int32_t nRetCode = 0;
    redisReply* pReply = NULL;
    USER* pUser = NULL;
    int32_t nResult = errUnknown;
    USER stUser = { 0 };
    uint32_t dwUserDataSize = 0;
    CLIENT_SESSION* pSession = NULL;

    CORO_BEGIN()

    // check is user first login
    nRetCode = CDBProxyClient::instance().redis_command_coro("hmget user_data:%s userid data", m_szOpenID);
    LOG_PROCESS_ERROR(nRetCode);

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    LOG_PROCESS_ERROR(pReply);

    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_ARRAY);
    LOG_PROCESS_ERROR(pReply->elements == 2);

    if (pReply->element[0]->type == REDIS_REPLY_NIL)
    {
        //create user id
        nRetCode = CDBProxyClient::instance().redis_command_coro("INCR user_id_generator");
        LOG_PROCESS_ERROR(nRetCode);

        CORO_YIELD()

        LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
        LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

        pReply = (redisReply*)(get_coro_reply().pReplyData);
        LOG_PROCESS_ERROR(pReply);
        LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_INTEGER);
        
        INF("user %s create user id %d", m_szOpenID, pReply->integer);
        m_qwUserID = pReply->integer;

        //create user data
        memset(&stUser, 0, sizeof(stUser));
        dwUserDataSize = sizeof(s_szUserData);

        stUser.qwUserID = pReply->integer;
        nRetCode = stUser.save_user(s_szUserData, dwUserDataSize);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CDBProxyClient::instance().redis_command_coro("hmset user_data:%s userid %llu data %b", m_szOpenID, m_qwUserID, s_szUserData, (size_t)dwUserDataSize);
        LOG_PROCESS_ERROR(nRetCode);

        CORO_YIELD()

        LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
        LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

        pReply = (redisReply*)(get_coro_reply().pReplyData);
        LOG_PROCESS_ERROR(pReply);
        LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
        LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);

        pUser = m_pUserModule->create_user(m_qwUserID);
        LOG_PROCESS_ERROR(pUser);

        pUser->qwUserID = m_qwUserID;
        pUser->qwSessionID = m_qwSessionID;
        pUser->nState = usLogin;
    }
    else
    {
        //load user data
        LOG_PROCESS_ERROR(pReply->element[0]->type == REDIS_REPLY_STRING);
        LOG_PROCESS_ERROR(pReply->element[1]->type == REDIS_REPLY_STRING);
        LOG_PROCESS_ERROR(pReply->element[0]->str);
        LOG_PROCESS_ERROR(pReply->element[1]->str);

        m_qwUserID = _strtoui64(pReply->element[0]->str, 0, 0);

        pUser = m_pUserModule->find_user(m_qwUserID);
        LOG_PROCESS_ERROR_RET_CODE(pUser == NULL, errLoginAlreadyLogin);

        pUser = m_pUserModule->create_user(m_qwUserID);
        LOG_PROCESS_ERROR(pUser);

        nRetCode = pUser->load_user(pReply->element[1]->str, pReply->element[1]->len);
        LOG_PROCESS_ERROR(nRetCode);

        pUser->qwSessionID = m_qwSessionID;
        pUser->nState = usLogin;
    }

    //check user account valid
    // 1 white list
    nRetCode = CDBProxyClient::instance().redis_command_coro("SISMEMBER user_white_list %s", m_szOpenID);
    LOG_PROCESS_ERROR(nRetCode);

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    LOG_PROCESS_ERROR(pReply);
    //LOG_PROCESS_ERROR_RET_CODE(pReply->integer == 1, errLoginNotInWhiteList);

    // 2 user ban
    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);
    LOG_PROCESS_ERROR_RET_CODE(CTimeMgr::instance().get_time_sec() > pUser->dwBanEndTime, errLoginUserBanned);

    //check user login uniq
    {
        G2O_USER_LOGIN msg;
        msg.qwUserID = m_qwUserID;
        msg.qwSessionID = m_qwSessionID;
        msg.nServerAddr = CMGApp::instance().get_tbus_addr();

        nRetCode = send_server_msg_by_routerid_coro(m_qwUserID, svrOnline, g2o_user_login, &msg, sizeof(msg));
        LOG_PROCESS_ERROR(nRetCode);
    }

    CORO_YIELD()
    
    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtMsg);

    {
        O2G_USER_LOGIN_ACK* msg = (O2G_USER_LOGIN_ACK*)(get_coro_reply().pReplyData);
        LOG_PROCESS_ERROR(msg->qwSessionID == m_qwSessionID);
        LOG_PROCESS_ERROR(msg->qwUserID == m_qwUserID);
        LOG_PROCESS_ERROR_RET_CODE(msg->nErrorCode == errSuccess, msg->nErrorCode);
    }

    // get role list
    nRetCode = CDBProxyClient::instance().redis_command_coro("HVALS user_role_list:%llu", m_qwUserID);
    LOG_PROCESS_ERROR(nRetCode);
    
    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_ARRAY);

    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);

    for (int32_t i = 0; i < pReply->elements; i++)
    {
        LOG_PROCESS_ERROR(pReply->element[i]->type == REDIS_REPLY_STRING);
        LOG_PROCESS_ERROR(pReply->element[i]->str);

        nRetCode = pUser->load_role_base(pReply->element[i]->str, pReply->element[i]->len);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // modify user info of the session 
    pSession = m_pSessionModule->find_session(m_qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    pSession->qwUserID = m_qwUserID;

    // sync role list
    nRetCode = do_g2c_sync_role_list(m_qwSessionID, pUser);
    LOG_PROCESS_ERROR(nRetCode);

    pUser->nState = usFinishLogin;

    CORO_END()

Exit0:
    if (nResult != errUnknown)
    {
        LOG_CHECK_ERROR(m_pUserModule->kick_user(m_qwUserID, nResult, 0));
    }

    return crsFailed;
}

void CCreateRoleCoro::set_start_arg(uint64_t qwSessionID, uint64_t qwUserID, const char* pcszRoleName)
{
    LOG_PROCESS_ERROR(pcszRoleName);
    
    m_qwSessionID = qwSessionID;
    m_qwUserID = qwUserID;
    m_qwRoleID = 0;
    m_pRole = NULL;
    m_dwRoleSize = sizeof(s_szRoleData);
    m_dwRoleBaseSize = sizeof(s_szRoleBaseData);
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    strxcpy(m_szRoleName, pcszRoleName, sizeof(m_szRoleName));
Exit0:
    return;
}
    
BOOL CCreateRoleCoro::on_resume()
{
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    
    if (m_qwRoleID > 0)
    {
        m_pRole = m_pRoleModule->find_role(m_qwRoleID);
    }
    return TRUE;
}

CORO_STATE CCreateRoleCoro::coro_process()
{
    int32_t nRetCode = 0;
    USER* pUser = NULL;
    redisReply* pReply = NULL;

    CORO_BEGIN()

    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);
    LOG_PROCESS_ERROR(pUser->nState == usFinishLogin || pUser->nState == usFinishCreateRole);

    pUser->nState = usCreateRole;

    // get role id
    nRetCode = CDBProxyClient::instance().redis_command_coro("INCR role_id_generator");
    LOG_PROCESS_ERROR(nRetCode);

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_INTEGER);

    m_qwRoleID = pReply->integer;

    // create role and save to role db
    // save role base data to user data db
    m_pRole = m_pRoleModule->create_role(m_qwRoleID);
    LOG_PROCESS_ERROR(m_pRole);

    m_pRole->init(m_qwRoleID);
    m_pRole->set_name(m_szRoleName);
    m_pRole->set_level(1);
    m_pRole->set_user_id(m_qwUserID);
    m_pRole->set_session_id(m_qwSessionID);
    m_pRole->set_create_time(CTimeMgr::instance().get_time_sec());

    {
        nRetCode = m_pRole->save(s_szRoleData, m_dwRoleSize, s_szRoleBaseData, m_dwRoleBaseSize);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CDBProxyClient::instance().redis_command_coro("set role:%llu %b", m_qwRoleID, s_szRoleData, (size_t)m_dwRoleSize);
        LOG_PROCESS_ERROR(nRetCode);
    
        CORO_YIELD()

        LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
        LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

        pReply = (redisReply*)(get_coro_reply().pReplyData);
        LOG_PROCESS_ERROR(pReply);
        LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
        LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);

        nRetCode = CDBProxyClient::instance().redis_command_coro("hmset user_role_list:%llu %llu %b", m_qwUserID, m_qwRoleID, s_szRoleBaseData, (size_t)m_dwRoleBaseSize);
        LOG_PROCESS_ERROR(nRetCode);

        CORO_YIELD()

        LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
        LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

        pReply = (redisReply*)(get_coro_reply().pReplyData);
        LOG_PROCESS_ERROR(pReply);
        LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
        LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);
    }

    // add role to user data
    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);

    m_pRole = m_pRoleModule->find_role(m_qwRoleID);
    LOG_PROCESS_ERROR(m_pRole);

    {
        ROLE_BASE& RoleBase = pUser->stRoleBase[pUser->nRoleCount];
        RoleBase.qwRoleID = m_pRole->get_obj_id();
        strxcpy(RoleBase.szRoleName, m_pRole->get_name(), COMMON_NAME_LEN);
        RoleBase.wLevel = m_pRole->get_level();
        RoleBase.dwCreateTime = m_pRole->get_create_time();
        RoleBase.dwLastLoginTime = m_pRole->get_login_time();
        RoleBase.dwLastSaveTime = m_pRole->get_save_time();

        pUser->nRoleCount++;
    }

    LOG_CHECK_ERROR(m_pRole->uninit());
    nRetCode = m_pRoleModule->destroy_role(m_pRole);
    LOG_PROCESS_ERROR(nRetCode);
    
    // sync role list
    nRetCode = do_g2c_sync_role_list(m_qwSessionID, pUser);
    LOG_PROCESS_ERROR(nRetCode);

    pUser->nState = usFinishCreateRole;

    CORO_END()

Exit0:
    if (m_pRole)
    {
        LOG_CHECK_ERROR(m_pRole->uninit());
        LOG_CHECK_ERROR(m_pRoleModule->destroy_role(m_pRole));
    }
    return crsFailed;
}

BOOL CSelectRoleCoro::on_resume()
{
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);

    return TRUE;
}

void CSelectRoleCoro::set_start_arg(uint64_t qwSessionID, uint64_t qwUserID, uint64_t qwRoleID)
{
    m_qwSessionID = qwSessionID;
    m_qwUserID = qwUserID;
    m_qwRoleID = qwRoleID;

    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);
}

CORO_STATE CSelectRoleCoro::coro_process()
{
    int32_t nRetCode = 0;
    CRole* pRole = NULL;
    USER* pUser = NULL;
    redisReply* pReply = NULL;
    CLIENT_SESSION* pSession = NULL;

    CORO_BEGIN()

    //check is role
    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);
    LOG_PROCESS_ERROR(pUser->is_role(m_qwRoleID));
    LOG_PROCESS_ERROR(pUser->nState == usFinishCreateRole || pUser->nState == usFinishLogin);

    pUser->nState = usSelectRole;

    // load from db
    nRetCode = CDBProxyClient::instance().redis_command_coro("get role:%llu", m_qwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
        
    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
    LOG_PROCESS_ERROR(pReply);
    LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STRING);

    // create role
    pRole = m_pRoleModule->find_role(m_qwRoleID);
    LOG_PROCESS_ERROR(pRole == NULL);

    pRole = m_pRoleModule->create_role(m_qwRoleID);
    LOG_PROCESS_ERROR(pRole);

    nRetCode = pRole->load(pReply->str, pReply->len);
    LOG_PROCESS_ERROR(nRetCode);

    pRole->set_session_id(m_qwSessionID);
    pRole->set_login_time(CTimeMgr::instance().get_time_sec());

    pUser = m_pUserModule->find_user(m_qwUserID);
    LOG_PROCESS_ERROR(pUser);

    pSession = m_pSessionModule->find_session(pUser->qwSessionID);
    LOG_PROCESS_ERROR(pSession);

    pUser->qwCurrPlayingRole = m_qwRoleID;
    pSession->qwCurrRoleID = m_qwRoleID;

    // enter scene to do

    //sync role data
    nRetCode = do_g2c_sync_role_data(m_qwSessionID, pRole);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGlobalEventListMgr::instance().trigger_global_event(evtRoleSyncData, 0, 0, pRole, pRole->get_obj_id(), 1, 2);
    LOG_CHECK_ERROR(nRetCode);

    INF("role sync data finished");
    
    pUser->nState = usPlaying;
    
    CORO_END()

Exit0:
    if (pRole)
        m_pRoleModule->destroy_role(pRole);
    return crsFailed;
}
    
BOOL CKickUserCoro::on_resume()
{
    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);

    return TRUE;
}

void CKickUserCoro::set_start_arg(uint64_t qwUserID, uint64_t qwSessionID, int32_t nKickReason, uint64_t qwKickParam)
{
    m_qwUserID = qwUserID;
    m_qwSessionID = qwSessionID;
    m_nKickReason = nKickReason;
    m_qwKickParam = qwKickParam;

    m_pRole = NULL;
    m_pUser = NULL;

    m_pUserModule = MG_GET_MODULE(CUserModule);
    m_pRoleModule = MG_GET_MODULE(CRoleModule);
    m_pSessionModule = MG_GET_MODULE(CSessionModule);
}
 
CORO_STATE CKickUserCoro::coro_process()
{
    int32_t nRetCode = 0;
    CLIENT_SESSION* pSession = NULL;
    redisReply* pReply = NULL;
    uint32_t dwRoleSize = 0;
    uint32_t dwRoleBaseSize = 0;

    CORO_BEGIN()

    m_pUser = m_pUserModule->find_user(m_qwUserID);
    if (m_pUser)
    {
        LOG_PROCESS_ERROR(m_pUser->nState != usKick);

        m_pUser->nState = usKick;

        if (m_pUser->qwCurrPlayingRole > 0)
        {
            dwRoleSize = sizeof(s_szRoleData);
            dwRoleBaseSize = sizeof(s_szRoleBaseData);

            // save role
            m_pRole = m_pRoleModule->find_role(m_pUser->qwCurrPlayingRole);
            LOG_PROCESS_ERROR(m_pRole);

            nRetCode = m_pRole->save(s_szRoleData, dwRoleSize, s_szRoleBaseData, dwRoleBaseSize);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = CDBProxyClient::instance().redis_command_coro("set role:%llu %b", m_pRole->get_obj_id(), s_szRoleData, (size_t)dwRoleSize);
            LOG_PROCESS_ERROR(nRetCode);

            CORO_YIELD()

            LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
            LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtDB);

            pReply = (redisReply*)(get_coro_reply().pReplyData);
            LOG_PROCESS_ERROR(pReply);
            LOG_PROCESS_ERROR(pReply->type == REDIS_REPLY_STATUS);
            LOG_PROCESS_ERROR(pReply->integer == REDIS_OK);

            m_pUser = m_pUserModule->find_user(m_qwUserID);
            LOG_PROCESS_ERROR(m_pUser);

            m_pRole = m_pRoleModule->find_role(m_pUser->qwCurrPlayingRole);
            LOG_PROCESS_ERROR(m_pRole);

            LOG_CHECK_ERROR(m_pRole->uninit());
            nRetCode = m_pRoleModule->destroy_role(m_pRole);
            LOG_PROCESS_ERROR(nRetCode);

            m_pUser->qwCurrPlayingRole = 0;
        }

        // logout from online server
        {
            G2O_USER_LOGOUT msg;
            msg.qwUserID = m_qwUserID;
            msg.qwSessionID = m_qwSessionID;
            msg.nServerAddr = CMGApp::instance().get_tbus_addr();

            nRetCode = send_server_msg_by_routerid_coro(m_qwUserID, svrOnline, g2o_user_logout, &msg, sizeof(msg));
            LOG_PROCESS_ERROR(nRetCode);
        }
    
        CORO_YIELD()
    
        LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
        LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtMsg);

        {
            O2G_USER_LOGOUT_ACK* msg = (O2G_USER_LOGOUT_ACK*)(get_coro_reply().pReplyData);
            LOG_PROCESS_ERROR(msg->qwSessionID == m_qwSessionID);
            LOG_PROCESS_ERROR(msg->qwUserID == m_qwUserID);
            LOG_CHECK_ERROR(msg->nErrorCode == errSuccess);
        }

        // delete user
        nRetCode = m_pUserModule->destroy_user(m_pUser);
        LOG_PROCESS_ERROR(nRetCode);
    }

    // kick session
    pSession = m_pSessionModule->find_session(m_qwSessionID);
    if (pSession)
    {
        nRetCode = m_pSessionModule->kick_session(pSession, m_nKickReason, m_qwKickParam);
        LOG_PROCESS_ERROR(nRetCode);
    }

    CORO_END()

Exit0:
    return crsFailed;
}