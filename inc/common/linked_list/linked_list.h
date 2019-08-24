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

// 这里是的模板参数是指当前链表的数据类型 由于需要从mid查找到指针 需要验证mid的合理性 
// 而验证合理性就需要知道共享内存数据的结构 所以就需要知道数据的类型
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
	
	pNewNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(qwObjMid);
	LOG_PROCESS_ERROR(pNewNode);

	pHeadNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(pHead->qwHeadObjMid);
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

	pNewNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(qwObjMid);
	LOG_PROCESS_ERROR(pNewNode);

	pTailNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(pHead->qwTailObjMid);
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

	pDelNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(qwObjMid);
	LOG_PROCESS_ERROR(pDelNode);

	pPrevNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(pDelNode->qwPrevObjMid);
	pNextNode = (LINK_NODE*)CShmMgr::get_instance().mid2ptr<T>(pDelNode->qwNextObjMid);

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