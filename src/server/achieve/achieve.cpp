#include "stdafx.h"
#include "component/achieve.h"

#include "module/role_module.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"

BOOL CAchieve::init(void* pOwner)
{
    LOG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;
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
    
void CAchieve::on_event_sync_data()
{
    INF("achieve on event sync data");
}