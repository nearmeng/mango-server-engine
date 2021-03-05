#ifndef _EVENT_DEF_H_
#define _EVNET_DEF_H_

#include "str_def.h"
#include "object_def.h"

#define MAX_EVENT_COUNT		(32)
#define MAX_EVENT_PER_OWNER	(64)
#define MAX_EVENT_TRIGGER_COUNT	(256)
#define EVENT_NAME_LEN		(128)
#define MAX_EVENT_RECURSIVE_COUNT  (8)

#define REG_EVENT_DEF(__type__, __owner__, __start_type__, __end_type__)  \
	nRetCode |= CEventMgr::instance().register_event_def(__type__, #__type__, __owner__, __start_type__, __end_type__); \
	LOG_PROCESS_ERROR(nRetCode);

struct EVENT_PARAM
{
    int32_t     nOwnerType;
    void*       pOwner;
    uint64_t    qwOwnerID;
    int64_t     llTriggerVar0;
    int64_t     llTriggerVar1;
};

struct EVENT_INFO;
typedef void(*EVENT_CALLBACK)(EVENT_INFO* pEvent, EVENT_PARAM& stEventParam);

//tolua_begin

enum EVENT_TYPE
{
	evtInvalid,

	evtStaticBegin,

	evtStaticRoleBegin = evtStaticBegin,
	
	evtRoleSyncData,

	evtStaticRoleEnd,
	
	evtStaticEnd,

	evtDynamicBegin,

	evtDynamicRoleBegin,

	evtRoleKillNpc,

	evtDynamicRoleEnd,

	evtDynamicEnd,

	evtTotal
};

//事件基础信息
struct EVENT_INFO
{
	int32_t			nEventID;			// 事件ID
	int32_t			nEventType;			// 事件类型
	int32_t			nEventTemplateID;	// 事件的模板ID
	int32_t			nEventParam;		// 事件参数
	int32_t			nTreeID;			// 触发的行为树ID
	uint64_t		qwSourceID;			// 事件源ID
	int64_t			llEventVar[2];		// 事件注册参数
    EVENT_CALLBACK  pEventCallback;     // C回调函数
};
	
//事件基本定义
struct EVENT_DEF
{
	int32_t			nEventType;
	char			szName[COMMON_NAME_LEN];
	int32_t			nOwnerType;
	int32_t			nStartEventType;
	int32_t			nEndEventType;
};

//tolua_end

#endif
