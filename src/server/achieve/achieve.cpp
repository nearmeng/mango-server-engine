#include "stdafx.h"
#include "role_sub_module/achieve.h"

#include "module/role_module.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"

BOOL CAchieve::init(CRole* pRole)
{
    LOG_PROCESS_ERROR(pRole);

    m_pRole = pRole;
    m_nAchieveID = 0;

    DBG("role sub module achieve is init");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAchieve::uninit(void)
{
    INF("role sub module achieve is uninit");

    return TRUE;
}

BOOL CAchieve::init_msg_handler(void)
{
    INF("achieve msg handler is inited");

    int32_t nRetCode = 0;

    //nRetCode = REG_ROLE_MSG_HANDLER(cs_test, test_func);
    //LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAchieve::module_init(BOOL bResume)
{
    INF("achieve sub module init");
    return TRUE;
}
    
BOOL CAchieve::module_uninit()
{
    INF("achieve sub module uninit");
    return TRUE;
}
    
void CAchieve::on_event_sync_data()
{
    INF("achieve on event sync data");
}