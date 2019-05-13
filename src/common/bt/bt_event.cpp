#include "stdafx.h"
#include "bt/bt_event.h"
#include "bt/bt_mgr.h"

#include "define/error_def.h"

#include "game_data/global_config.h"

int32_t CBTEventList::ms_nTriggerLayer;
int32_t CBTEventList::ms_nTriggerCounter;

CEventMgr CEventMgr::ms_Instance;
CGlobalEventListMgr CGlobalEventListMgr::ms_Instance;

inline uint64_t MAKE_EVENT_LIST_ID(uint64_t qwStartType, uint64_t qwTemplateID)
{
	return ((qwStartType << 32) | qwTemplateID);
}

BOOL CEventMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_EventPool.init(stdBtEvent, g_ServerConfig.Common.nInitBtEventCount, bResume);
	LOG_PROCESS_ERROR(nRetCode == 0);

	if (bResume)
	{
		
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CEventMgr::uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CEventMgr::create_event(const char* pcszEventName, int32_t nEventType, int32_t nTemplateID, int32_t nEventParam,
	int32_t nTreeID, CLuaScript* pScript, uint64_t qwSourceID, int64_t llVar0, int64_t llVar1)
{
	int32_t nRetCode = 0;
	int32_t nEventID = 0;
	BT_EVENT* pEvent = NULL;
	char szEventName[BT_EVENT_NAME_LEN];

	snprintf(szEventName, BT_EVENT_NAME_LEN, "%s_%s_%d_%d", pcszEventName, pScript->get_script_name(), nTemplateID, nEventParam);
	nEventID = hash_string(szEventName);

	pEvent = m_EventPool.find_object(nEventID);
	LOG_PROCESS_ERROR(pEvent == NULL);

	pEvent = m_EventPool.new_object(nEventID);
	LOG_PROCESS_ERROR(pEvent);

	pEvent->nEventID = nEventID;
	pEvent->nEventType = nEventType;
	pEvent->nEventTemplateID = nTemplateID;
	pEvent->nEventParam = nEventParam;
	pEvent->nTreeID = nTreeID;
	pEvent->qwScriptNameHash = pScript->get_name_hash();
	pEvent->pScript = pScript;			
	pEvent->qwSourceID = qwSourceID;
	pEvent->llEventVar[0] = llVar0;
	pEvent->llEventVar[1] = llVar1;

	return nEventID;
Exit0:
	return 0;
}

BOOL CEventMgr::destroy_event(int32_t nEventID)
{
	int32_t nRetCode = 0;
	BT_EVENT* pEvent = NULL;

	pEvent = m_EventPool.find_object(nEventID);
	LOG_PROCESS_ERROR(pEvent);

	nRetCode = m_EventPool.delete_object(pEvent);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BT_EVENT* CEventMgr::find_event(int32_t nEventID)
{
	return m_EventPool.find_object(nEventID);
}
	
int32_t CEventMgr::get_event_owner(int32_t nEventType)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(nEventType > evtInvalid && nEventType < evtTotal);

	return ms_EventDefList[nEventType].nOwnerType;
Exit0:
	return 0;
}
	
int32_t CEventMgr::get_event_start_type(int32_t nEventType)
{
	LOG_PROCESS_ERROR(nEventType > evtInvalid && nEventType < evtTotal);

	return ms_EventDefList[nEventType].nStartEventType;
Exit0:
	return 0;
}

int32_t CEventMgr::get_event_end_type(int32_t nEventType)
{
	LOG_PROCESS_ERROR(nEventType > evtInvalid && nEventType < evtTotal);

	return ms_EventDefList[nEventType].nEndEventType;
Exit0:
	return 0;
}

BOOL CEventMgr::TRAVERSE_BT_EVENT::operator()(int32_t nEventID, BT_EVENT* pEvent)
{
	pEvent->pScript = CScriptMgr::instance().find_script(pEvent->qwScriptNameHash);
	LOG_PROCESS_ERROR(pEvent->pScript);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBTEventList::init(int32_t nStartEventType, int32_t nEndEventType)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(nEndEventType - nStartEventType < MAX_BT_EVENT_PER_OWNER);

	m_nEventCount = 0;
	memset(m_EventInfo, 0, sizeof(m_EventInfo));

	memset(m_byTypeCountList, 0, sizeof(m_byTypeCountList));
	m_nStartEventType = nStartEventType;
	m_nEndEventType = nEndEventType;

	m_nTriggerLayer = 0;
	m_bPendingDelete = FALSE;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBTEventList::uninit(void)
{
	return TRUE;
}

BOOL CBTEventList::register_event(int32_t nEventID)
{
	int32_t nRetCode = 0;
	BT_EVENT* pEvent = NULL;

	LOG_PROCESS_ERROR(m_nEventCount < MAX_BT_EVENT_COUNT);

	pEvent = CEventMgr::instance().find_event(nEventID);
	LOG_PROCESS_ERROR(pEvent);
	LOG_PROCESS_ERROR(pEvent->nEventType >= m_nStartEventType);
	LOG_PROCESS_ERROR(pEvent->nEventType < m_nEndEventType);

	m_EventInfo[m_nEventCount].nEventID = nEventID;
	m_EventInfo[m_nEventCount].pEvent = pEvent;
	m_EventInfo[m_nEventCount].bDeleted = FALSE;
	m_nEventCount++;

	m_byTypeCountList[pEvent->nEventType - m_nStartEventType]++;

	return TRUE;
Exit0:
	return FALSE;
}
	
BOOL CBTEventList::unregister_event(int32_t nEventID)
{
	int32_t nRetCode = 0;

	for (int32_t i = 0; i < m_nEventCount; i++)
	{
		if (m_EventInfo[i].nEventID == nEventID)
		{
			int32_t nEventType = m_EventInfo[i].pEvent->nEventType;

			if (m_nTriggerLayer <= 0)
			{
				for (int32_t j = i; j < m_nEventCount; j++)
					m_EventInfo[j] = m_EventInfo[j + 1];

				m_nEventCount--;
				m_byTypeCountList[nEventType - m_nStartEventType]--;
			}
			else
			{
				m_EventInfo[i].bDeleted = TRUE;
				m_bPendingDelete = TRUE;
			}

			return TRUE; 
		}
	}

Exit0:
	return FALSE;
}

int32_t CBTEventList::trigger_event(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam,
	void* pOwner, uint64_t qwOwnerID, int64_t llTriggerVar0, int64_t llTriggerVar1, BOOL bRollBack, BOOL bBreakOnFail)
{
	int32_t nRetCode = 0;
	int32_t nOwnerType = 0;
	int32_t nResult = errUnknown;
	BT_CTX Ctx;
		
	LOG_PROCESS_ERROR(nEventType >= m_nStartEventType & nEventType < m_nEndEventType);

	if (m_nEventCount <= 0 || m_byTypeCountList[nEventType - m_nStartEventType] <= 0)
		return brvSuccess;

	LOG_PROCESS_ERROR(ms_nTriggerLayer < MAX_BT_EVENT_RECURSIVE_COUNT);

	ms_nTriggerLayer++;
	m_nTriggerLayer++;

	for (int32_t i = 0; i < m_nEventCount; i++)
	{
		if (m_EventInfo[i].pEvent == NULL)
			continue;

		if (m_EventInfo[i].bDeleted)
			continue;

		if (m_EventInfo[i].pEvent->nEventType != nEventType)
			continue;

		if (m_EventInfo[i].pEvent->nEventParam != nEventParam)
			continue;

		if (m_EventInfo[i].pEvent->nEventTemplateID != nEventTemplateID)
			continue;

		nOwnerType = CEventMgr::instance().get_event_owner(m_EventInfo[i].pEvent->nEventType);

		nRetCode = CBTMgr::instance().start_run(Ctx, m_EventInfo[i].pEvent->nTreeID, m_EventInfo[i].nEventID,
			nOwnerType, pOwner, qwOwnerID, m_EventInfo[i].pEvent->pScript, llTriggerVar0, llTriggerVar1,
			m_EventInfo[i].pEvent->llEventVar[0], m_EventInfo[i].pEvent->llEventVar[1], bRollBack);
		LOG_CHECK_ERROR(nRetCode != brvError);

		if (bBreakOnFail)
		{
			nResult = Ctx.nResult;
			if (nRetCode != brvSuccess)
				break;
		}
	}

	LOG_PROCESS_ERROR(ms_nTriggerLayer > 0);
	ms_nTriggerLayer--;
	m_nTriggerLayer--;

	if (m_nTriggerLayer <= 0 && m_bPendingDelete)
	{
		for (int32_t i = m_nEventCount - 1; i >= 0; i--)
		{
			if (m_EventInfo[i].bDeleted)
			{
				int32_t nEventType = m_EventInfo[i].pEvent->nEventType;

				for (int32_t j = i; j < m_nEventCount; j++)
					m_EventInfo[j] = m_EventInfo[j + 1];

				m_nEventCount--;
				m_byTypeCountList[nEventType - m_nStartEventType]--;
			}
		}

		m_bPendingDelete = FALSE;
	}

	return nResult;

Exit0:
	CRI("failed to trigger event %d", nEventType);
	return errUnknown;
}

BOOL CBTEventList::is_event_registed(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(nEventType >= m_nStartEventType && nEventType < m_nEndEventType);

	if (m_nEventCount <= 0 || m_byTypeCountList[nEventType - m_nStartEventType] <= 0)
		return FALSE;

	for (int32_t i = 0; i < m_nEventCount; i++)
	{
		BT_EVENT* pEvent = m_EventInfo[i].pEvent;
		LOG_PROCESS_ERROR(pEvent);

		if (pEvent->nEventType != nEventType)
			continue;

		if (pEvent->nEventTemplateID != nEventTemplateID)
			continue;

		if (pEvent->nEventParam != nEventParam)
			continue;

		return TRUE;
	}

Exit0:
	return FALSE;
}


BOOL CGlobalEventListMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_GlobalEventListPool.init(stdBtGlobalEventList, g_ServerConfig.Common.nInitBtGlobalEventListCount, bResume);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGlobalEventListMgr::uninit(void)
{
	return TRUE;
}

BOOL CGlobalEventListMgr::register_global_event(int32_t nEventID)
{
	int32_t nRetCode = 0;
	uint64_t qwEventListID = 0;
	CBTEventList* pEventList = NULL;
	BT_EVENT* pEvent = NULL;
	int32_t nStartType = 0;
	int32_t nEndType = 0;

	pEvent = CEventMgr::instance().find_event(nEventID);
	LOG_PROCESS_ERROR(pEvent);

	nStartType = CEventMgr::instance().get_event_start_type(pEvent->nEventType);
	nEndType = CEventMgr::instance().get_event_end_type(pEvent->nEventType);

	qwEventListID = MAKE_EVENT_LIST_ID(nStartType, pEvent->nEventTemplateID);

	pEventList = m_GlobalEventListPool.find_object(qwEventListID);
	LOG_PROCESS_ERROR(pEventList == NULL);

	pEventList = m_GlobalEventListPool.new_object(qwEventListID);
	LOG_PROCESS_ERROR(pEventList);

	nRetCode = pEventList->register_event(nEventID);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGlobalEventListMgr::unregister_global_event(int32_t nEventID)
{
	int32_t nRetCode = 0;
	BT_EVENT* pEvent = NULL;
	int32_t nStartType = 0;
	uint64_t qwEventListID = 0;
	CBTEventList* pEventList = NULL;
	
	pEvent = CEventMgr::instance().find_event(nEventID);
	LOG_PROCESS_ERROR(pEvent);

	nStartType = CEventMgr::instance().get_event_start_type(pEvent->nEventType);
	qwEventListID = MAKE_EVENT_LIST_ID(nStartType, pEvent->nEventTemplateID);

	pEventList = m_GlobalEventListPool.find_object(qwEventListID);
	LOG_PROCESS_ERROR(pEventList);

	nRetCode = pEventList->unregister_event(nEventID);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CGlobalEventListMgr::trigger_global_event(int32_t nEventType, int32_t nEventTemplateID, int32_t nEventParam, void* pOwner, uint64_t qwOwnerID,
	int64_t llTriggerVar0, int64_t llTriggerVar1, BOOL bRollBack, BOOL bBreakOnFail)
{
	int32_t nRetCode = 0;
	int32_t nResult = errUnknown;
	int32_t nStartType = 0;
	uint64_t qwEventListID = 0;
	CBTEventList* pEventList = NULL;

	LOG_PROCESS_ERROR(nEventType > evtInvalid && nEventType < evtTotal);

	nStartType = CEventMgr::instance().get_event_start_type(nEventType);
	qwEventListID = MAKE_EVENT_LIST_ID(nStartType, nEventTemplateID);

	pEventList = m_GlobalEventListPool.find_object(qwEventListID);
	LOG_PROCESS_ERROR(pEventList);

	return pEventList->trigger_event(nEventType, nEventTemplateID, nEventParam, pOwner, qwOwnerID, 
				llTriggerVar0, llTriggerVar1, bRollBack, bBreakOnFail);
	
Exit0:
	return nResult;
}


BT_EVENT_DEF CEventMgr::ms_EventDefList[evtTotal] = 
{
	{evtInvalid,		"Invalid",		botInvalid},

	{evtRoleCreate,		"RoleCreate",	botRole, evtStaticRoleBegin, evtStaticRoleEnd},
	
	{evtRoleKillNpc,	"RoleKillNpc",	botRole, evtDynamicBegin, evtDynamicEnd},
};