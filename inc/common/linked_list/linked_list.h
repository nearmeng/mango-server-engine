#ifndef _LINKED_LIST_
#define _LINKED_LIST_

struct LINK_HEAD
{
	uint64_t	qwHeadObjMid;
	uint64_t	qwTailObjMid;
};

struct LINK_NODE
{
	uint64_t	qwPrevObjMid;
	uint64_t	qwNextObjMid;
};

template<class T>
BOOL add_head(LINK_HEAD* pHead, uint64_t qwObjMid);

template<class T>
BOOL add_tail(LINK_HEAD* pHead, uint64_t qwObjMid);

template<class T>
BOOL del_node(LINK_HEAD* pHead, uint64_t qwObjMid);

template<class T>
BOOL add_head(LINK_HEAD* pHead, uint64_t qwObjMid)
{
	LINK_NODE* pHeadNode = NULL;
	LINK_NODE* pNewNode = NULL;
	
	pNewNode = (T*)SHM_MID2PTR(qwObjMid);
	LOG_PROCESS_ERROR(pNewNode);

	pHeadNode = (T*)SHM_MID2PTR(pHead->qwHeadObjMid);
	if (pHeadNode == NULL)
	{
		pNewNode->qwNextObjMid = 0;
		pNewNode->qwPrevObjMid = 0;
		
		pHead->qwHeadObjMid = qwObjMid;
		pHead->qwTailObjMid = qwObjMid;
	}
	else
	{
		pNewNode->qwNextObjMid = pHead->qwHeadObjMid;
		pNewNode->qwPrevObjMid = 0;

		pHead->qwHeadObjMid = qwObjMid;
		pHeadNode->qwPrevObjMid = qwObjMid;
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
BOOL add_tail(LINK_HEAD* pHead, uint64_t qwObjMid)
{
	LINK_NODE* pNewNode = NULL;
	LINK_NODE* pTailNode = NULL;

	pNewNode = (T*)SHM_MID2PTR(qwObjMid);
	LOG_PROCESS_ERROR(pNewNode);

	pTailNode = (T*)SHM_MID2PTR(pHead->qwTailObjMid);
	if (pTailNode == NULL)
	{
		pNewNode->qwNextObjMid = 0;
		pNewNode->qwPrevObjMid = 0;
		
		pHead->qwHeadObjMid = qwObjMid;
		pHead->qwTailObjMid = qwObjMid;
	}
	else
	{
		pNewNode->qwPrevObjMid = pHead->qwTailObjMid;
		pNewNode->qwNextObjMid = 0;

		pHead->qwTailObjMid = qwObjMid;
		pTailNode->qwNextObjMid = qwObjMid;
	}
	
	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
BOOL del_node(LINK_HEAD* pHead, uint64_t qwObjMid)
{
	LINK_NODE* pDelNode = NULL;
	LINK_NODE* pPrevNode = NULL;
	LINK_NODE* pNextNode = NULL;

	pDelNode = (T*)SHM_MID2PTR(qwObjMid);
	LOG_PROCESS_ERROR(pDelNode);

	pPrevNode = (T*)SHM_MID2PTR(pDelNode->qwPrevObjMid);
	pNextNode = (T*)SHM_MID2PTR(pDelNode->qwNextObjMid);

	if (pPrevNode != NULL)
		pPrevNode->qwNextObjMid = pDelNode->qwNextObjMid;

	if (pNextNode != NULL)
		pNextNode->qwPrevObjMid = pDelNode->qwPrevObjMid;

	if (pHead->qwHeadObjMid == qwObjMid)
		pHead->qwHeadObjMid = pDelNode->qwNextObjMid;

	if (pHead->qwTailObjMid == qwObjMid)
		pHead->qwTailObjMid = pDelNode->qwPrevObjMid;

	return TRUE;
Exit0:
	return FALSE;
}

#endif