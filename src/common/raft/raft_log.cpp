#include "stdafx.h"
#include "raft_log.h"

struct RAFT_LOG
{
	int32_t nSize;			// total size of entry
	int32_t nCount;			// curr count of entry

	int32_t nFront;			// position of queue head
	int32_t nBack;			// position of queue tail

	int32_t nBase;			// compact log base index

	RAFT_ENTRY*	pEntry;		// entry pointer

	RAFT_CALLBACK_MGR* pCallBackMgr;	// log callback
	RAFT_MGR* pRaftMgr;						
};

static BOOL _raft_log_entry_memory_expand(RAFT_LOG* pRaftLog)
{
	int32_t nRetCode = 0;
	int32_t nNewCount = 0;
	RAFT_ENTRY* pNewEntry = NULL;

	LOG_PROCESS_ERROR(pRaftLog);

	pNewEntry = (RAFT_ENTRY*)malloc(sizeof(RAFT_ENTRY) * pRaftLog->nSize * 2);
	LOG_PROCESS_ERROR(pNewEntry);
	memset(pNewEntry, 0, sizeof(RAFT_ENTRY) * pRaftLog->nSize * 2);

	for (int32_t i = pRaftLog->nFront; nNewCount < pRaftLog->nCount; i++)
	{
		if (i == pRaftLog->nSize)
			i = 0;

		memcpy(&(pNewEntry[nNewCount]), &(pRaftLog->pEntry[i]), sizeof(RAFT_ENTRY));
		nNewCount++;
	}

	SAFE_FREE(pRaftLog->pEntry);

	pRaftLog->nFront = 0;
	pRaftLog->nBack = pRaftLog->nCount;
	pRaftLog->pEntry = pNewEntry;
	pRaftLog->nSize *= 2;
	
	return TRUE;
Exit0:
	return FALSE;
}

RAFT_LOG* raft_log_new(int32_t nInitSize)
{
	RAFT_LOG* pRaftLog = NULL;

	pRaftLog = (RAFT_LOG*)malloc(sizeof(RAFT_LOG));
	LOG_PROCESS_ERROR(pRaftLog);

	memset(pRaftLog, 0, sizeof(RAFT_LOG));

	pRaftLog->nSize = nInitSize;
	pRaftLog->pEntry = (RAFT_ENTRY*)malloc(sizeof(RAFT_ENTRY) * nInitSize);
	LOG_PROCESS_ERROR(pRaftLog->pEntry);

	memset(pRaftLog->pEntry, 0, sizeof(RAFT_ENTRY) * nInitSize);
	return pRaftLog;

Exit0:
	raft_log_free(pRaftLog);
	return NULL;
}

void raft_log_free(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	for (int32_t i = 0; i < pRaftLog->nSize; i++)
	{
		if (pRaftLog->pEntry[i].stData.pBuff)
			SAFE_FREE(pRaftLog->pEntry[i].stData.pBuff);
	}

	if (pRaftLog->pEntry)
		SAFE_FREE(pRaftLog->pEntry);

	SAFE_FREE(pRaftLog);
Exit0:
	return;
}

void raft_log_clear(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	pRaftLog->nFront = 0;
	pRaftLog->nBack = 0;
	pRaftLog->nCount = 0;
	pRaftLog->nBase = 0;

Exit0:
	return;
}

void raft_log_clear_entry(RAFT_LOG* pRaftLog)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftLog);
	LOG_PROCESS_ERROR(pRaftLog->nCount > 0);
	LOG_PROCESS_ERROR(pRaftLog->pCallBackMgr);
	
	PROCESS_ERROR(pRaftLog->pCallBackMgr->pLogClear);

	for (int32_t i = pRaftLog->nBase; i < pRaftLog->nBase + pRaftLog->nCount; i++)
	{
		int32_t nSlot = (i - pRaftLog->nBase + pRaftLog->nFront) % pRaftLog->nSize;
		nRetCode = pRaftLog->pCallBackMgr->pLogClear(pRaftLog->pRaftMgr, &(pRaftLog->pEntry[nSlot]), i);
		LOG_CHECK_ERROR(nRetCode);
	}

Exit0:
	return;
}

void raft_log_set_callback(RAFT_LOG* pRaftLog, RAFT_CALLBACK_MGR* pCallBackMgr, RAFT_MGR* pRaftMgr)
{
	LOG_PROCESS_ERROR(pRaftLog);
	LOG_PROCESS_ERROR(pCallBackMgr);
	LOG_PROCESS_ERROR(pRaftMgr);

	pRaftLog->pCallBackMgr = pCallBackMgr;
	pRaftLog->pRaftMgr = pRaftMgr;

Exit0:
	return;
}

void raft_log_append_entry(RAFT_LOG* pRaftLog, RAFT_ENTRY* pEntry)
{
	int32_t nRetCode = 0;
	void* pData = NULL;
	RAFT_ENTRY* pRaftEntry = NULL;
	
	LOG_PROCESS_ERROR(pRaftLog);
	LOG_PROCESS_ERROR(pEntry);

	if (pRaftLog->nCount >= pRaftLog->nSize)
	{
		nRetCode = _raft_log_entry_memory_expand(pRaftLog);
		LOG_PROCESS_ERROR(nRetCode);
	}

	pRaftEntry = &pRaftLog->pEntry[pRaftLog->nBack];
	pData = malloc(pEntry->stData.dwSize);
	memcpy(pData, pEntry->stData.pBuff, pEntry->stData.dwSize);
	memcpy(pRaftEntry, pEntry, sizeof(RAFT_ENTRY));
	pRaftEntry->stData.pBuff = pData;

	if (pRaftLog->pCallBackMgr && pRaftLog->pCallBackMgr->pLogAppend)
	{
		nRetCode = pRaftLog->pCallBackMgr->pLogAppend(pRaftLog->pRaftMgr, &(pRaftLog->pEntry[pRaftLog->nBack]), pRaftLog->nBase + pRaftLog->nCount + 1);
		LOG_CHECK_ERROR(nRetCode);
	}

	pRaftLog->nBack = (pRaftLog->nBack + 1) % pRaftLog->nSize;
	pRaftLog->nCount++;

Exit0:
	return;
}

BOOL raft_log_delete_entry_from_index(RAFT_LOG* pRaftLog, int32_t nIndex)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pRaftLog);
	LOG_PROCESS_ERROR(nIndex > 0);

	nIndex = (nIndex >= pRaftLog->nBase) ? nIndex : pRaftLog->nBase;
	while (nIndex < pRaftLog->nBase + pRaftLog->nCount)
	{
		int32_t nSlot = (pRaftLog->nBack - 1) % pRaftLog->nSize;
		if (nSlot < 0)
			nSlot += pRaftLog->nSize;

		if (pRaftLog->pCallBackMgr && pRaftLog->pCallBackMgr->pLogDeleteLast)
		{
			nRetCode = pRaftLog->pCallBackMgr->pLogDeleteLast(pRaftLog->pRaftMgr, &pRaftLog->pEntry[nSlot], pRaftLog->nBase + pRaftLog->nCount);
			LOG_CHECK_ERROR(nRetCode);
		}

		SAFE_FREE(pRaftLog->pEntry[nSlot].stData.pBuff);

		pRaftLog->nBack = nSlot;
		pRaftLog->nCount--;
	}

	return TRUE;
Exit0:
	return FALSE;
}

void raft_log_empety_queue(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	pRaftLog->nFront = 0;
	pRaftLog->nBack = 0;
	pRaftLog->nCount = 0;

Exit0:
	return;
}

RAFT_ENTRY* raft_log_get_entry(RAFT_LOG* pRaftLog, int32_t nIndex)
{
	int32_t nSlot = 0;
	LOG_PROCESS_ERROR(pRaftLog);

	LOG_PROCESS_ERROR(nIndex > 0);
	LOG_PROCESS_ERROR(nIndex > pRaftLog->nBase);
	PROCESS_ERROR(nIndex <= pRaftLog->nBase + pRaftLog->nCount);

	nIndex -= 1;
	nSlot = (nIndex - pRaftLog->nBase + pRaftLog->nFront) % pRaftLog->nSize;

	return &pRaftLog->pEntry[nSlot];
Exit0:
	return NULL;
}

RAFT_ENTRY* raft_log_get_from_entry(RAFT_LOG * pRaftLog, int32_t nIndex, int32_t& nCount)
{
	int32_t nRetCode = 0;
	int32_t nSlot = 0;

	LOG_PROCESS_ERROR(pRaftLog);
	PROCESS_ERROR(nIndex <= pRaftLog->nBase + pRaftLog->nCount);
	LOG_PROCESS_ERROR(nIndex > pRaftLog->nBase);
	
	nCount = 0;
	nIndex -= 1;
	nSlot = (pRaftLog->nFront + nIndex - pRaftLog->nBase) % pRaftLog->nSize;
	
	if (nSlot < pRaftLog->nBack)
		nCount = pRaftLog->nBack - nSlot;
	else
		nCount = pRaftLog->nSize - nSlot;

	return &pRaftLog->pEntry[nSlot];

Exit0:
	return NULL;
}

RAFT_ENTRY* raft_log_get_entry_tail(RAFT_LOG* pRaftLog)
{
	int32_t nSlot = 0; 

	LOG_PROCESS_ERROR(pRaftLog);
	LOG_PROCESS_ERROR(pRaftLog->nCount > 0);
		
	nSlot = (pRaftLog->nBack - 1) % pRaftLog->nSize;
	if (nSlot < 0)
		nSlot += pRaftLog->nSize;

	return &pRaftLog->pEntry[nSlot];
Exit0:
	return NULL;
}

int32_t raft_log_get_entry_count(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	return pRaftLog->nCount;
Exit0:
	return 0;
}

int32_t raft_log_get_curr_index(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	return pRaftLog->nBase + pRaftLog->nCount;
Exit0:
	return 0;
}

int32_t raft_log_get_base_index(RAFT_LOG* pRaftLog)
{
	LOG_PROCESS_ERROR(pRaftLog);

	return pRaftLog->nBase;
Exit0:
	return 0;
}

