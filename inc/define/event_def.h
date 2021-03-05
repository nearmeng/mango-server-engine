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

//�¼�������Ϣ
struct EVENT_INFO
{
	int32_t			nEventID;			// �¼�ID
	int32_t			nEventType;			// �¼�����
	int32_t			nEventTemplateID;	// �¼���ģ��ID
	int32_t			nEventParam;		// �¼�����
	int32_t			nTreeID;			// ��������Ϊ��ID
	uint64_t		qwSourceID;			// �¼�ԴID
	int64_t			llEventVar[2];		// �¼�ע�����
    EVENT_CALLBACK  pEventCallback;     // C�ص�����
};
	
//�¼���������
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
