#ifndef _SHM_DEF_H_
#define _SHM_DEF_H_

#define MAX_SHM_TYPE_COUNT 	512

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

	stdTotal = MAX_SHM_TYPE_COUNT,
};

#endif