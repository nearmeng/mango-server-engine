#ifndef _BT_EVENT_H_
#define _BT_EVENT_H_

#include "define/bt_def.h"
#include "shm/shm_pool.h"

//事件基础信息
struct BT_EVENT
{
	int32_t			nEventID;			// 事件ID
	int32_t			nEventType;			// 事件类型
	int32_t			nEventTemplateID;	// 事件的模板ID
	int32_t			nEventParam;		// 事件参数
	int32_t			nTreeID;			// 触发的行为树ID
	uint64_t		qwScriptNameHash;	// lua script hash name
	CLuaScript*		pScript;			// lua script
	uint64_t		qwSourceID;			// 事件源ID
	int64_t			llEventVar[2];		// 事件注册参数
};
	
//事件基本定义
struct BT_EVENT_DEF
{
	int32_t			nEventType;
	char			szName[BT_ACTION_NAME_LEN];
	int32_t			nOwnerType;
	int32_t			nStartEventType;
	int32_t			nEndEventType;
};

class CEventMgr
{
public:
	CEventMgr() {};
	~CEventMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);

	inline static CEventMgr& instance(void);

	int32_t create_event(const char* pcszEventName, int32_t nEventType, int32_t nTemplateID, int32_t nEventParam, 
		int32_t nTreeID, CLuaScript* pScript, uint64_t qwSourceID, int64_t llVar0 = 0, int64_t llVar1 = 0);
	BOOL destroy_event(int32_t nEventID);
	BT_EVENT* find_event(int32_t nEventID);

	int32_t get_event_owner(int32_t nEventType);
	int32_t get_event_start_type(int32_t nEventType);
	int32_t get_event_end_type(int32_t nEventType);

private:
	struct TRAVERSE_BT_EVENT
	{
		BOOL operator()(int32_t nEventID, BT_EVENT* pEvent);
	};

private:

	CShmObjectPool<BT_EVENT, int32_t>  m_EventPool;
	static BT_EVENT_DEF ms_EventDefList[evtTotal];
	static CEventMgr ms_Instance;
};

class CBTEventList
{
public:
	CBTEventList() {};
	~CBTEventList() {};

	BOOL init(int32_t nStartEventType, int32_t nEndEventType);
	BOOL uninit(void);
	static inline void clear(void);

	BOOL register_event(int32_t nEventID);
	BOOL unregister_event(int32_t nEventID);

	int32_t trigger_event(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam, 
		void* pOwner, uint64_t qwOwnerID, int64_t llTriggerVar0 = 0, int64_t llTriggerVar1 = 0, BOOL bRollBack = FALSE, BOOL bBreakOnFail = FALSE);
	BOOL is_event_registed(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam);

private:
	int32_t						m_nEventCount;
	struct EVENT_INFO
	{
		int32_t		nEventID;
		BT_EVENT*	pEvent;
		BOOL		bDeleted;
	};
	EVENT_INFO m_EventInfo[MAX_BT_EVENT_COUNT];

	uint8_t						m_byTypeCountList[MAX_BT_EVENT_PER_OWNER];
	int32_t						m_nStartEventType;
	int32_t						m_nEndEventType;

	int32_t						m_nTriggerLayer;
	BOOL						m_bPendingDelete;

	static int32_t				ms_nTriggerLayer;
	static int32_t				ms_nTriggerCounter;
};

class CGlobalEventListMgr
{
public:
	CGlobalEventListMgr() {};
	~CGlobalEventListMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CGlobalEventListMgr& instance(void);

	BOOL register_global_event(int32_t nEventID);
	BOOL unregister_global_event(int32_t nEventID);
	int32_t trigger_global_event(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam, void* pOwner, uint64_t qwOwnerID, 
		int64_t llTriggerVar0 = 0, int64_t llTriggerVar1 = 0, BOOL bRollBack = FALSE, BOOL bBreakOnFail = FALSE);

private:
	static CGlobalEventListMgr ms_Instance;
	CShmObjectPool<CBTEventList, uint64_t>	m_GlobalEventListPool;
};

inline void CBTEventList::clear(void)
{
	ms_nTriggerCounter = 0;
	ms_nTriggerLayer = 0;
}

inline CEventMgr& CEventMgr::instance(void)
{
	return ms_Instance;
}

inline CGlobalEventListMgr& CGlobalEventListMgr::instance(void)
{
	return ms_Instance;
}

#endif