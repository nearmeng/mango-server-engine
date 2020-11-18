#include "stdafx.h"
#include "module/role_module.h"
#include "module/gs_user_module.h"
#include "module/server_default_session_module.h"

#include "protocol/proto_head.pb.h"
#include "protocol/external_message.pb.h"

BOOL CRoleModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = m_RolePool.init(stdRole, g_ServerConfig.GS.nRoleCount, bResume);
    LOG_PROCESS_ERROR(nRetCode);
    
    nRetCode = CRole::module_init(bResume);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CRole::init_msg_handler();
    LOG_PROCESS_ERROR(nRetCode);

    if (bResume)
    {
        TRAVERSE_ROLE_RESUME TraverseRoleResume;
        m_RolePool.traverse(TraverseRoleResume);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleModule::uninit(void)
{
    int32_t nRetCode = 0;
    
    nRetCode = CRole::module_uninit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_RolePool.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

void CRoleModule::mainloop()
{

}

BOOL CRoleModule::TRAVERSE_ROLE_RESUME::operator()(uint64_t qwRoleID, CRole * pRole)
{
    pRole->on_resume();

    return TRUE;
}

void _on_recv_client_msg_for_role(CLIENT_SESSION* pSession, const CS_HEAD* pHead, const google::protobuf::Message* pMsg)
{
    CRole* pRole = NULL;
    USER* pUser = NULL;
    CRoleModule* pRoleModule = NULL;
    CUserModule* pUserModule = NULL;
    ROLE_MSG_HANDLER pMsgHandler = NULL;

    LOG_PROCESS_ERROR(pSession->qwUserID > 0);

    pUserModule = MG_GET_MODULE(CUserModule);
    LOG_PROCESS_ERROR(pUserModule);
    
    pUser = pUserModule->find_user(pSession->qwUserID);
    LOG_PROCESS_ERROR(pUser);

    pRoleModule = MG_GET_MODULE(CRoleModule);
    LOG_PROCESS_ERROR(pRoleModule);

    pRole = pRoleModule->find_role(pUser->qwCurrPlayingRole);
    LOG_PROCESS_ERROR(pRole);

    pMsgHandler = CRoleModule::get_msg_handler(pHead->msgid());
    if(pMsgHandler != NULL)
    {
        pMsgHandler(pRole, pHead, pMsg);
    }
    else
    {
        CRI("role msgid %d is not set callback", pHead->msgid());
    }

Exit0:
    return;
}

BOOL CRoleModule::reg_msg_handler(int32_t nMsgID, ROLE_MSG_HANDLER pHandler)
{
    int32_t nRetCode = 0;
    CRoleModule* pModule = MG_GET_MODULE(CRoleModule);

    nRetCode = REG_SESSION_CLI_MSG_HANDLER(nMsgID, _on_recv_client_msg_for_role);
    LOG_PROCESS_ERROR(nRetCode);

    pModule->m_MsgHandler[nMsgID] = pHandler;

    return TRUE;
Exit0:
    return FALSE;
}
    
ROLE_MSG_HANDLER CRoleModule::get_msg_handler(int32_t nMsgID)
{
    CRoleModule* pModule = MG_GET_MODULE(CRoleModule);

    if (pModule->m_MsgHandler.find(nMsgID) == pModule->m_MsgHandler.end())
        return NULL;
    else
        return pModule->m_MsgHandler[nMsgID];
}
    