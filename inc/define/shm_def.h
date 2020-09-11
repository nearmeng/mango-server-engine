#ifndef _SHM_DEF_H_
#define _SHM_DEF_H_

#define MAX_SHM_TYPE_COUNT 	1024

enum SHM_TYPE_DEF
{
    stdInvalid = -1,

    stdType1,
    stdType2,
    stdBtCtx,
    stdBtMgrData,
    stdBtEvent,
    stdBtGlobalEventList,
    stdTimeMgr,
    stdTimerPool,
    stdGuidMgr,
    stdRouterAliveData,
    stdRouterObj,
    stdRouterServiceMgr,
    stdRouterServiceData,
    stdMsgQueue,
    stdSMMsgData,
    stdSMSObj,
    stdSMSServiceData,
    stdSMSRouterMgr,
    stdSMSTrans,
    stdSession,

    stdStacklessGlobalMgr,
    stdStacklessCoreBegin,
    stdStacklessCoreEnd = stdStacklessCoreBegin + 256,
    
    stdGlobalResMgr,
    stdResBegin,
    stdResEnd = stdResBegin + 256,

    stdServerSessionDefault,
    stdUser,

    stdOnlineUser,
    stdRole,

	stdTotal = MAX_SHM_TYPE_COUNT,
};

#endif