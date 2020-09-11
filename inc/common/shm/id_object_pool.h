#ifndef _ID_OBJECT_POOL_H_
#define _ID_OBJECT_POOL_H_

#include "rb_tree.h"
#include <errno.h>
#include <typeinfo>
#include <vector>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>

#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef WIN32
	#pragma warning(push)
	#pragma warning(disable : 4312)
#endif  // WIN32

#define FENCE_NUMBER			(0xABCDEF0123456789)
#define ID_POOL_MAGIC_FREE_TAG	(0xFEEDBAC9)
#define ID_POOL_HEAD_MAGIC		(0xDEADBAC9)
#define HASH_TABLE_SIZE			(1024 * 64)
#define ID_POOL_NAME_LEN		(128)
#define MAX_CHUNK_COUNT			(64)

#if defined(_DEBUG)
	#define _DEBUG_FENCE
#endif	// _DEBUG

///static uint64_t __pool_profile_syscall_cnt = 0;
///static uint64_t __pool_profile_rbtree_cnt = 0;
///static uint64_t __pool_profile_rdtsc_syscall = 0;
///static uint64_t __pool_profile_rdtsc_rbtree = 0;

template<class T, class N>
class CIDObjectPool
{
public:
	CIDObjectPool();
	virtual ~CIDObjectPool();

	BOOL init(int32_t nInitSize, const char* pcszInstanceTag);
	BOOL restore(const char* szInstanceTag);
	BOOL uninit(void);
	BOOL clear(void);

	T* new_object(N ID);
	BOOL delete_object(T* pObject);
	T* find(N ID);

	template<class TFunc>
	inline BOOL traverse(TFunc& rFunc);

	T* get_first_obj(void);
	T* get_next_obj(void);

	inline int32_t get_size(void);
	inline int32_t get_free_size(void);
	inline int32_t get_used_size(void);
	inline int32_t get_obj_size(void);

	N get_obj_id(T* pObject);

	void check_fence(void);

private:
	char m_szPoolName[ID_POOL_NAME_LEN];

	struct T_NODE
	{
		T           Data;
#ifdef _DEBUG_FENCE
		uint64_t    qwFence;
#endif  // _DEBUG_FENCE
	};

	struct CHUNK_NODE
	{
		uint32_t			dwTag;
		int32_t				nIndex;
		int32_t				nHandle;
		RB_TREE_NODE<N>*	pRbNode;
		T_NODE*				pTNode;
		int32_t				nSize;
		CHUNK_NODE*			pNext;
	};

	struct CHUNK_HEAD
	{
		uint32_t			m_dwMagic;
		uint32_t			m_dwPadding;
		int32_t				m_nSize;
		int32_t				m_nChunkCount;
		RB_TREE_NODE<N>*	m_pDataMapRoot[HASH_TABLE_SIZE];
		RB_TREE_NODE<N>		m_FreeListHead;
		RB_TREE_NODE<N>		m_FreeListRear;
		int32_t				m_nFreeListSize;
		int32_t				m_nObjSize;
		int32_t				m_nInitSize;
		int32_t				m_nCurMapRootIdx;
		RB_TREE_NODE<N>*	m_pCurTreeNode;
		CHUNK_NODE*			m_pChunkNode;
		void*				m_pThisAddr;
		void*				m_pChunkAddr[MAX_CHUNK_COUNT];
	};

	CHUNK_HEAD*				m_pChunkHead;

	BOOL _create_chunk_head(void);
	BOOL _restore_chunk_head(void);
	BOOL _destroy_chunk_head(void);

	BOOL _create_chunk(int32_t nSize);
	BOOL _find_chunk(int32_t nChunkIndex);
	BOOL _restore_chunk(int32_t nChunkIndex);
	BOOL _destroy_chunk(int32_t nChunkIndex);
	RB_TREE_NODE<N>* _get_data_node(T* pData);
	T* _get_data(RB_TREE_NODE<N>* pDataNode);

	inline void _push_into_free_list(RB_TREE_NODE<N>* pDataNode);
	inline RB_TREE_NODE<N>* _pop_from_free_list(void);
	inline BOOL _is_free_list_empty(void);
};

template<class T, class N>
T* CIDObjectPool<T, N>::get_first_obj(void)
{
	LOG_PROCESS_ERROR(m_pChunkHead);
	m_pChunkHead->m_nCurMapRootIdx = 0;

	while (m_pChunkHead->m_nCurMapRootIdx < HASH_TABLE_SIZE)
	{
		if (m_pChunkHead->m_pDataMapRoot[m_pChunkHead->m_nCurMapRootIdx])
		{
			m_pChunkHead->m_pCurTreeNode = rb_first(m_pChunkHead->m_pDataMapRoot[m_pChunkHead->m_nCurMapRootIdx]);
			return _get_data(m_pChunkHead->m_pCurTreeNode);
		}

		++m_pChunkHead->m_nCurMapRootIdx;
	}

Exit0:
	return NULL;
}

template<class T, class N>
T* CIDObjectPool<T, N>::get_next_obj(void)
{
	RB_TREE_NODE<N>* pNextNode = NULL;
	LOG_PROCESS_ERROR(m_pChunkHead);

	pNextNode = rb_next(m_pChunkHead->m_pCurTreeNode);

	while (pNextNode == NULL && ++m_pChunkHead->m_nCurMapRootIdx < HASH_TABLE_SIZE)
	{
		if (m_pChunkHead->m_pDataMapRoot[m_pChunkHead->m_nCurMapRootIdx])
		{
			pNextNode = rb_first(m_pChunkHead->m_pDataMapRoot[m_pChunkHead->m_nCurMapRootIdx]);
			break;
		}
	}

	m_pChunkHead->m_pCurTreeNode = pNextNode;

	return _get_data(m_pChunkHead->m_pCurTreeNode);
Exit0:
	return NULL;
}

template<class T, class N>
CIDObjectPool<T, N>::CIDObjectPool()
{
	m_szPoolName[0] = 0;
	m_pChunkHead = NULL;
}

template<class T, class N>
CIDObjectPool<T, N>::~CIDObjectPool()
{
//	INF("CIDObjectPool::~ [%s]", m_szPoolName);
	clear();
	uninit();
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_create_chunk_head(void)
{
	int32_t nResult = -1;
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	char szChunkHeadName[ID_POOL_NAME_LEN] = { 0 };

	// alloc memory
	snprintf(szChunkHeadName, ID_POOL_NAME_LEN, "CH_%s", m_szPoolName);
#if defined(WIN32) || defined(WIN64)
	m_pChunkHead = (CHUNK_HEAD*)malloc(sizeof(CHUNK_HEAD));
#else

	nHandle = shm_open(szChunkHeadName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	LOG_PROCESS_ERROR(nHandle != -1);

	//INF("creating chunk head: %s", szChunkHeadName);

	nRetCode = ftruncate(nHandle, sizeof(CHUNK_HEAD));
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == 0, errno);

	m_pChunkHead = (CHUNK_HEAD*)mmap(NULL, sizeof(CHUNK_HEAD), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(m_pChunkHead != MAP_FAILED);
#endif	//WIN32 || WIN64
	LOG_PROCESS_ERROR(m_pChunkHead);

	memset(m_pChunkHead, 0, sizeof(CHUNK_HEAD));

	m_pChunkHead->m_dwMagic = ID_POOL_HEAD_MAGIC;
	m_pChunkHead->m_pThisAddr = m_pChunkHead;

	m_pChunkHead->m_FreeListHead.pLeft = NULL;
	m_pChunkHead->m_FreeListHead.pRight = &m_pChunkHead->m_FreeListRear;
	m_pChunkHead->m_FreeListRear.pLeft = &m_pChunkHead->m_FreeListHead;
	m_pChunkHead->m_FreeListRear.pRight = NULL;

	return TRUE;
Exit0:
	return FALSE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_restore_chunk_head(void)
{

#if defined(WIN32) || defined(WIN64)
	return TRUE;
#else
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	char szChunkHeadName[ID_POOL_NAME_LEN] = { 0 };
	void* pHead = NULL;
	struct stat st;

	snprintf(szChunkHeadName, ID_POOL_NAME_LEN, "CH_%s", m_szPoolName);

	nHandle = shm_open(szChunkHeadName, O_RDWR, S_IRUSR | S_IWUSR);
	LOG_PROCESS_ERROR(nHandle != -1);

	//INF("restoring chunk head: %s", m_szPoolName);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(nRetCode == 0);
	LOG_PROCESS_ERROR(st.st_size == sizeof(CHUNK_HEAD));

	m_pChunkHead = (CHUNK_HEAD*)mmap(NULL, sizeof(CHUNK_HEAD), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(m_pChunkHead != MAP_FAILED);
	LOG_PROCESS_ERROR(m_pChunkHead->m_dwMagic == ID_POOL_HEAD_MAGIC);

	pHead = m_pChunkHead->m_pThisAddr;
	nRetCode = munmap(pHead, sizeof(CHUNK_HEAD));
	LOG_PROCESS_ERROR(nRetCode == 0);

	m_pChunkHead = (CHUNK_HEAD*)mmap(pHead, sizeof(CHUNK_HEAD), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE | MAP_FIXED, nHandle, 0);
	LOG_PROCESS_ERROR(m_pChunkHead != MAP_FAILED);
	LOG_PROCESS_ERROR(m_pChunkHead == pHead);
	LOG_PROCESS_ERROR(m_pChunkHead->m_dwMagic == ID_POOL_HEAD_MAGIC);
	LOG_PROCESS_ERROR(m_pChunkHead->m_nObjSize == sizeof(T));

	return TRUE;
Exit0:
	INF("restore chunk head: %s failed.", szChunkHeadName);

	if (m_pChunkHead)
	{
		munmap(m_pChunkHead, sizeof(CHUNK_HEAD));
		m_pChunkHead = NULL;
	}

	if (nHandle > 0)
	{
		shm_unlink(szChunkHeadName);
	}

	return FALSE;
#endif // win32 || win64
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_destroy_chunk_head(void)
{
#if defined(WIN32) || defined(WIN64)
	return TRUE;
#else
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	struct stat st;
	CHUNK_HEAD* pHead = NULL;
	char szChunkHeadName[ID_POOL_NAME_LEN] = { 0 };

	snprintf(szChunkHeadName, ID_POOL_NAME_LEN, "CH_%s", m_szPoolName);

	nHandle = shm_open(szChunkHeadName, O_RDWR, S_IRUSR | S_IWUSR);
	PROCESS_SUCCESS(nHandle < 0);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(st.st_size == sizeof(CHUNK_HEAD));

	pHead = (CHUNK_HEAD*)mmap(NULL, sizeof(CHUNK_HEAD), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(pHead != MAP_FAILED);
	LOG_PROCESS_ERROR(pHead->m_dwMagic == ID_POOL_HEAD_MAGIC);

	nRetCode = munmap(pHead, sizeof(CHUNK_HEAD));
	LOG_PROCESS_ERROR(nRetCode == 0);
	pHead = NULL;

	nRetCode = shm_unlink(szChunkHeadName);
	LOG_PROCESS_ERROR(nRetCode == 0);

	m_pChunkHead = NULL;

	close(nHandle);

//	INF("destroyed data chunk %s", szChunkHeadName);

Exit1:
	return TRUE;

Exit0:
	CRI("failed to destroy header chunk %s", szChunkHeadName);

	if (pHead)
		munmap(pHead, sizeof(CHUNK_HEAD));
	if (nHandle != -1)
	{
		shm_unlink(szChunkHeadName);
		close(nHandle);
	}

	return FALSE;
#endif	//WIN32 || WIN64
}


template<class T, class N>
BOOL CIDObjectPool<T, N>::_create_chunk(int32_t nSize)
{
	int32_t nResult = 0;
	int32_t nRetCode = 0;
	RB_TREE_NODE<N>* pNewChunk = NULL;
	CHUNK_NODE* pNewChunkNode = NULL;
	int32_t nHandle = -1;
	size_t qwChunkSize = sizeof(CHUNK_NODE) + (sizeof(RB_TREE_NODE<N>) + sizeof(T_NODE)) * nSize;
	char szPoolIndexName[ID_POOL_NAME_LEN] = { 0 };

	LOG_PROCESS_ERROR(nSize > 0);
	LOG_PROCESS_ERROR(m_pChunkHead);
	LOG_PROCESS_ERROR(m_pChunkHead->m_nChunkCount < MAX_CHUNK_COUNT);

	// alloc memory
	snprintf(szPoolIndexName, ID_POOL_NAME_LEN, "%s_%d", m_szPoolName, m_pChunkHead->m_nChunkCount);
#if defined(WIN32) || defined(WIN64)
	pNewChunkNode = (CHUNK_NODE*)malloc(qwChunkSize);
#else
	nHandle = shm_open(szPoolIndexName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	LOG_PROCESS_ERROR(nHandle != -1);

	//INF("creating chunk: %s", m_szPoolName);

	nRetCode = ftruncate(nHandle, qwChunkSize);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode == 0, errno);

	pNewChunkNode = (CHUNK_NODE*)mmap(NULL, qwChunkSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(pNewChunkNode != MAP_FAILED);
#endif	//WIN32 || WIN64
	LOG_PROCESS_ERROR(pNewChunkNode);

	pNewChunk = (RB_TREE_NODE<N>*)(pNewChunkNode + 1);
	LOG_PROCESS_ERROR(((int64_t)pNewChunk & 3) == 0);       // check 4 bytes aligned

	// link chunk list
	pNewChunkNode->dwTag = ID_POOL_MAGIC_FREE_TAG;
	pNewChunkNode->nIndex = m_pChunkHead->m_nChunkCount;
	pNewChunkNode->nHandle = nHandle;
	pNewChunkNode->pRbNode = pNewChunk;
	pNewChunkNode->pTNode = (T_NODE*)(pNewChunk + nSize);
	pNewChunkNode->nSize = nSize;
	pNewChunkNode->pNext = m_pChunkHead->m_pChunkNode;

	m_pChunkHead->m_pChunkNode = pNewChunkNode;
	m_pChunkHead->m_pChunkAddr[m_pChunkHead->m_nChunkCount] = pNewChunkNode;
	m_pChunkHead->m_nChunkCount++;

	// call constructor
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		T_NODE* pTNode = pNewChunkNode->pTNode + nIndex;

#ifdef _DEBUG_FENCE
		pTNode->qwFence = FENCE_NUMBER;
#endif  // _DEBUG_FENCE

		new (&pTNode->Data) T;
	}

	for (int32_t i = 0; i < nSize; i++)
		_push_into_free_list(pNewChunk + i);

	m_pChunkHead->m_nSize += nSize;

	//INF("created data chunk %s size %d memory %.2fMB", szPoolIndexName, nSize, nSize * sizeof(T) / 1024.0 / 1024);

	return TRUE;
Exit0:
	CRI("failed to create data chunk %s size %d by reason %d", szPoolIndexName, nSize, result);

#if defined(WIN32) || defined(WIN64)
	SAFE_FREE(pNewChunkNode);
#else
	if (pNewChunkNode)
		munmap(pNewChunkNode, qwChunkSize);
	if (nHandle != -1)
	{
		shm_unlink(szPoolIndexName);
		close(nHandle);
	}
#endif	// WIN32 || WIN64

	return FALSE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_find_chunk(int32_t nChunkIndex)
{
#if defined(WIN32) || defined(WIN64)
	return FALSE;
#else
	int32_t nResult = FALSE;
	int32_t nRetCode = 0;
	CHUNK_NODE* pNewChunkNode = NULL;
	struct stat st;
	int32_t nHandle = -1;
	size_t qwChunkSize = 0;
	char szPoolIndexName[ID_POOL_NAME_LEN] = { 0 };

	LOG_PROCESS_ERROR(m_pChunkHead);

	snprintf(szPoolIndexName, ID_POOL_NAME_LEN, "%s_%d", m_szPoolName, nChunkIndex);
	nHandle = shm_open(szPoolIndexName, O_RDWR, S_IRUSR | S_IWUSR);
	PROCESS_ERROR(nHandle != -1);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(nRetCode == 0);
	qwChunkSize = st.st_size;

	pNewChunkNode = (CHUNK_NODE*)mmap(NULL, qwChunkSize, PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(pNewChunkNode != MAP_FAILED);
	LOG_PROCESS_ERROR(pNewChunkNode->dwTag == ID_POOL_MAGIC_FREE_TAG);
	LOG_PROCESS_ERROR(pNewChunkNode->nIndex == nChunkIndex);

	nResult = TRUE;

Exit0:

	//if(!nResult)
		//WRN("find chunk failed: %s", szPoolIndexName);
	//else
		//INF("find chunk: %s", szPoolIndexName);

	if (pNewChunkNode)
		munmap(pNewChunkNode, qwChunkSize);
	if (nHandle != -1)
	{
		close(nHandle);
	}

	return nResult;
#endif	//WIN32 || WIN64
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_restore_chunk(int32_t nChunkIndex)
{
#if defined(WIN32) || defined(WIN64)
	return TRUE;
#else
	int32_t nRetCode = 0;
	RB_TREE_NODE<N>* pNewChunk = NULL;
	CHUNK_NODE* pNewChunkNode = NULL;
	RB_TREE_NODE<N>* pNode = NULL;
	struct stat st;
	int32_t nHandle = -1;
	size_t qwChunkSize = 0;
	char szPoolIndexName[ID_POOL_NAME_LEN] = { 0 };
	int32_t nFreeSize = 0;
	void* pChunkAddr = NULL;

	uint64_t rdtsc[3] = { 0 };


	LOG_PROCESS_ERROR(m_pChunkHead);
	LOG_PROCESS_ERROR(nChunkIndex < MAX_CHUNK_COUNT && nChunkIndex < m_pChunkHead->m_nChunkCount);
	LOG_PROCESS_ERROR(m_pChunkHead->m_pChunkAddr[nChunkIndex]);

	snprintf(szPoolIndexName, ID_POOL_NAME_LEN, "%s_%d", m_szPoolName, nChunkIndex);
	nHandle = shm_open(szPoolIndexName, O_RDWR, S_IRUSR | S_IWUSR);
	PROCESS_ERROR(nHandle != -1);

	//INF("restoring chunk: %s", m_szPoolName);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(nRetCode == 0);
	qwChunkSize = st.st_size;

	pNewChunkNode = (CHUNK_NODE*)mmap(m_pChunkHead->m_pChunkAddr[nChunkIndex], qwChunkSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE | MAP_FIXED, nHandle, 0);
	LOG_PROCESS_ERROR(pNewChunkNode != MAP_FAILED);
	LOG_PROCESS_ERROR(pNewChunkNode == m_pChunkHead->m_pChunkAddr[nChunkIndex]);
	LOG_PROCESS_ERROR(pNewChunkNode->dwTag == ID_POOL_MAGIC_FREE_TAG);
	LOG_CHECK_ERROR(pNewChunkNode->nIndex == nChunkIndex);
	LOG_PROCESS_ERROR(qwChunkSize == (sizeof(CHUNK_NODE) + (sizeof(RB_TREE_NODE<N>) + sizeof(T_NODE)) * pNewChunkNode->nSize));


	pNewChunk = (RB_TREE_NODE<N>*)(pNewChunkNode + 1);
	LOG_PROCESS_ERROR(((uint64_t)pNewChunk & 3) == 0);

	pNewChunkNode->nHandle = nHandle;
	pNewChunkNode->pRbNode = pNewChunk;
	pNewChunkNode->pTNode = (T_NODE*)(pNewChunk + pNewChunkNode->nSize);

	// call constructor
	for (int nIndex = 0; nIndex < pNewChunkNode->nSize; nIndex++)
	{
		T_NODE* pTNode = pNewChunkNode->pTNode + nIndex;

#ifdef _DEBUG_FENCE
		LOG_CHECK_ERROR(pTNode->qwFence == FENCE_NUMBER);
#endif  // _DEBUG_FENCE

		new (&pTNode->Data) T;
	}

	return TRUE;
Exit0:
	CRI("failed to restore data chunk %s, errno: %d", szPoolIndexName, errno);
	if (pNewChunkNode)
	{
		CRI("new chunk addr: 0x%p, old chunk addr: 0x%p", pNewChunkNode, m_pChunkHead->m_pChunkAddr[nChunkIndex]);
		munmap(pNewChunkNode, qwChunkSize);
	}
	if (nHandle != -1)
	{
		shm_unlink(szPoolIndexName);
		close(nHandle);
	}

	return FALSE;
#endif	//WIN32 || WIN64
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::_destroy_chunk(int32_t nChunkIndex)
{
#if defined(WIN32) || defined(WIN64)
	return TRUE;
#else
	int32_t nRetCode = 0;
	CHUNK_NODE* pNewChunkNode = NULL;
	struct stat st;
	int32_t nHandle = -1;
	size_t qwChunkSize = 0;
	char szPoolIndexName[ID_POOL_NAME_LEN] = { 0 };

	snprintf(szPoolIndexName, ID_POOL_NAME_LEN, "%s_%d", m_szPoolName, nChunkIndex);
	nHandle = shm_open(szPoolIndexName, O_RDWR, S_IRUSR | S_IWUSR);
	PROCESS_SUCCESS(nHandle < 0);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(nRetCode == 0);
	qwChunkSize = st.st_size;

	pNewChunkNode = (CHUNK_NODE*)mmap(NULL, qwChunkSize, PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(pNewChunkNode != MAP_FAILED);
	LOG_PROCESS_ERROR(pNewChunkNode->dwTag == ID_POOL_MAGIC_FREE_TAG);

	nRetCode = munmap(pNewChunkNode, qwChunkSize);
	LOG_PROCESS_ERROR(nRetCode == 0);
	pNewChunkNode = NULL;

	nRetCode = shm_unlink(szPoolIndexName);
	LOG_PROCESS_ERROR(nRetCode == 0);

	close(nHandle);

	//INF("destroyed data chunk %s", szPoolIndexName);

Exit1:
	return TRUE;
Exit0:
	CRI("failed to destroy data chunk %s", szPoolIndexName);

	if (pNewChunkNode)
		munmap(pNewChunkNode, qwChunkSize);
	if (nHandle != -1)
	{
		shm_unlink(szPoolIndexName);
		close(nHandle);
	}

	return FALSE;
#endif	//WIN32 || WIN64
}

template<class T, class N>
RB_TREE_NODE<N>* CIDObjectPool<T, N>::_get_data_node(T* pData)
{
	T_NODE* pTNode = (T_NODE*)pData;
	CHUNK_NODE* pChunkNode = NULL;

	LOG_PROCESS_ERROR(m_pChunkHead);
	pChunkNode = m_pChunkHead->m_pChunkNode;

	while (pChunkNode)
	{
		ptrdiff_t nOffset = pTNode - pChunkNode->pTNode;
		if (nOffset >= 0 && nOffset < pChunkNode->nSize)
		{
			return pChunkNode->pRbNode + nOffset;
		}
		pChunkNode = pChunkNode->pNext;
	}

Exit0:
	return NULL;
}

template<class T, class N>
T* CIDObjectPool<T, N>::_get_data(RB_TREE_NODE<N>* pRbNode)
{
	CHUNK_NODE* pChunkNode = NULL;

	LOG_PROCESS_ERROR(m_pChunkHead);
	pChunkNode = m_pChunkHead->m_pChunkNode;

	while (pChunkNode)
	{
		ptrdiff_t nOffset = pRbNode - pChunkNode->pRbNode;
		if (nOffset >= 0 && nOffset < pChunkNode->nSize)
		{
			return (T*)(pChunkNode->pTNode + nOffset);
		}
		pChunkNode = pChunkNode->pNext;
	}

Exit0:
	return NULL;
}

template<class T, class N>
inline void CIDObjectPool<T, N>::_push_into_free_list(RB_TREE_NODE<N>* pDataNode)
{
	LOG_PROCESS_ERROR(pDataNode->uColor != ID_POOL_MAGIC_FREE_TAG);

	pDataNode->pLeft = &m_pChunkHead->m_FreeListHead;
	pDataNode->pRight = m_pChunkHead->m_FreeListHead.pRight;
	pDataNode->pRight->pLeft = pDataNode;
	m_pChunkHead->m_FreeListHead.pRight = pDataNode;

	pDataNode->uColor = ID_POOL_MAGIC_FREE_TAG;
	m_pChunkHead->m_nFreeListSize++;

	return;
Exit0:
	CRI("ok");
	return;
}

template<class T, class N>
inline RB_TREE_NODE<N>* CIDObjectPool<T, N>::_pop_from_free_list(void)
{
	RB_TREE_NODE<N>* pResult = NULL;

	PROCESS_ERROR(!_is_free_list_empty());

	pResult = m_pChunkHead->m_FreeListRear.pLeft;
	LOG_PROCESS_ERROR(pResult->uColor == ID_POOL_MAGIC_FREE_TAG);

	m_pChunkHead->m_FreeListRear.pLeft = pResult->pLeft;
	pResult->pLeft->pRight = &m_pChunkHead->m_FreeListRear;
	m_pChunkHead->m_nFreeListSize--;

	pResult->pLeft = pResult->pRight = NULL;
	pResult->uColor = 0;

	return pResult;
Exit0:
	return NULL;
}

template<class T, class N>
inline BOOL CIDObjectPool<T, N>::_is_free_list_empty(void)
{
	return m_pChunkHead->m_nFreeListSize == 0;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::init(int32_t nInitSize, const char* pcszInstanceTag)
{
	int32_t nRetCode = 0;

	if (pcszInstanceTag)
		snprintf(m_szPoolName, ID_POOL_NAME_LEN, "%s_%s", typeid(T).name(), pcszInstanceTag);
	else
		snprintf(m_szPoolName, ID_POOL_NAME_LEN, "%s_null", typeid(T).name());

	_destroy_chunk_head();

	nRetCode = _create_chunk_head();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _find_chunk(0);
	if (nRetCode)
	{
		int32_t nChunkIndex = 0;
		do
		{
			nRetCode = _destroy_chunk(nChunkIndex);
			LOG_CHECK_ERROR(nRetCode);

			nChunkIndex++;
		} while (_find_chunk(nChunkIndex));
	}

	nRetCode = _create_chunk(nInitSize);
	LOG_PROCESS_ERROR(nRetCode);

	m_pChunkHead->m_nInitSize = nInitSize;
	m_pChunkHead->m_nObjSize = sizeof(T);

	nRetCode = m_pChunkHead->m_nObjSize % 4;
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::restore(const char* pcszInstanceTag)
{
	int32_t nRetCode = 0;
	int32_t nChunkCount = 0;

	if (pcszInstanceTag)
		snprintf(m_szPoolName, ID_POOL_NAME_LEN, "%s_%s", typeid(T).name(), pcszInstanceTag);
	else
		snprintf(m_szPoolName, ID_POOL_NAME_LEN, "%s_null", typeid(T).name());

	nRetCode = _restore_chunk_head();
	LOG_PROCESS_ERROR(nRetCode);

	nChunkCount = m_pChunkHead->m_nChunkCount;

	for (int32_t i = 0; i < nChunkCount; ++i)
	{
		nRetCode = _find_chunk(i);
		if (nRetCode)
		{
			nRetCode = _restore_chunk(i);
			LOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			nRetCode = _create_chunk(m_pChunkHead->m_nInitSize);
			LOG_PROCESS_ERROR(nRetCode);
		}
	}

	nRetCode = m_pChunkHead->m_nObjSize % 4;
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	if(m_pChunkHead)
		_destroy_chunk_head();
	return FALSE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::uninit(void)
{
	CHUNK_NODE* pChunkNode = NULL;

	PROCESS_ERROR(m_pChunkHead);

	//DBG("CIDObjectPool::uninit [%s]", m_szPoolName);

	// Check memory leak
	if (m_pChunkHead->m_nFreeListSize != m_pChunkHead->m_nSize)
		CRI("detected memory leak in data chunk %s size %d free size %d", m_szPoolName, m_pChunkHead->m_nSize, m_pChunkHead->m_nFreeListSize);

	check_fence();

	pChunkNode = m_pChunkHead->m_pChunkNode;
	m_pChunkHead->m_pChunkNode = NULL;

	while (pChunkNode)
	{
		CHUNK_NODE* pNextChunk = pChunkNode->pNext;
		pChunkNode->pNext = NULL;

		// call destructor
		for (int nIndex = 0; nIndex < pChunkNode->nSize; nIndex++)
		{
			T_NODE* pTNode = pChunkNode->pTNode + nIndex;
#ifdef _DEBUG_FENCE
			LOG_CHECK_ERROR(pTNode->qwFence == FENCE_NUMBER);
#endif //_DEBUG_FENCE

			(&pTNode->Data)->T::~T();
		}

#if defined(WIN32) || defined(WIN64)
		SAFE_FREE(pChunkNode);
#else
		{
			int32_t nHandle = pChunkNode->nHandle;
			char szPoolIndexName[ID_POOL_NAME_LEN];

			snprintf(szPoolIndexName, ID_POOL_NAME_LEN, "%s_%d", m_szPoolName, pChunkNode->nIndex);
			pChunkNode->nHandle = -1;

			munmap(pChunkNode, sizeof(CHUNK_NODE) + (sizeof(RB_TREE_NODE<N>) + sizeof(T_NODE)) * pChunkNode->nSize);
			if (nHandle != -1)
			{
				shm_unlink(szPoolIndexName);
				close(nHandle);
			}
		}
#endif	// WIN32 || WIN64

		pChunkNode = pNextChunk;
	}

	_destroy_chunk_head();

	m_pChunkHead = NULL;
Exit0:
	return TRUE;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::clear(void)
{
	CHUNK_NODE* pChunkNode = NULL;

	PROCESS_ERROR(m_pChunkHead);

	DBG("CIDObjectPool::clear [%s]", m_szPoolName);

	m_pChunkHead->m_nSize = 0;
	m_pChunkHead->m_nFreeListSize = 0;
	memset(m_pChunkHead->m_pDataMapRoot, 0, sizeof(m_pChunkHead->m_pDataMapRoot));
	m_pChunkHead->m_FreeListHead.pLeft = NULL;
	m_pChunkHead->m_FreeListHead.pRight = &m_pChunkHead->m_FreeListRear;
	m_pChunkHead->m_FreeListRear.pLeft = &m_pChunkHead->m_FreeListHead;
	m_pChunkHead->m_FreeListRear.pRight = NULL;

	pChunkNode = m_pChunkHead->m_pChunkNode;
	while (pChunkNode)
	{
		CHUNK_NODE* pNextChunk = pChunkNode->pNext;

		RB_TREE_NODE<N>* pRbData = pChunkNode->pRbNode;
		//RB_TREE_NODE<N>* pNode = pRbData;

		memset(pRbData, 0, sizeof(*pRbData) * pChunkNode->nSize);
		for (int32_t nIndex = 0; nIndex < pChunkNode->nSize; ++nIndex)
			_push_into_free_list(pRbData + nIndex);

		m_pChunkHead->m_nSize += pChunkNode->nSize;

		pChunkNode = pNextChunk;
	}

Exit0:
	return TRUE;
}

template<class T, class N>
T* CIDObjectPool<T, N>::new_object(N ID)
{
	int32_t nRetCode = 0;
	RB_TREE_NODE<N>* pRbNode = NULL;

	LOG_PROCESS_ERROR(m_pChunkHead);

	if (_is_free_list_empty())
	{
		nRetCode = _create_chunk(m_pChunkHead->m_nInitSize);
		LOG_PROCESS_ERROR(nRetCode);
	}

	pRbNode = _pop_from_free_list();
	LOG_PROCESS_ERROR(pRbNode);

	pRbNode->ID = ID;
	nRetCode = rb_insert(pRbNode, m_pChunkHead->m_pDataMapRoot + (abs(ID) % HASH_TABLE_SIZE));
	LOG_PROCESS_ERROR(nRetCode);

	return _get_data(pRbNode);
Exit0:
	if (pRbNode)
		_push_into_free_list(pRbNode);

	return NULL;
}

template<class T, class N>
BOOL CIDObjectPool<T, N>::delete_object(T* pObject)
{
	RB_TREE_NODE<N>* pRbNode = NULL;
	LOG_PROCESS_ERROR(m_pChunkHead);
	LOG_PROCESS_ERROR(pObject);

	pRbNode = _get_data_node(pObject);
	LOG_PROCESS_ERROR(pRbNode);
	LOG_PROCESS_ERROR(pRbNode->uColor != ID_POOL_MAGIC_FREE_TAG);

	rb_erase(pRbNode, m_pChunkHead->m_pDataMapRoot + (abs(pRbNode->ID) % HASH_TABLE_SIZE));
	_push_into_free_list(pRbNode);

	return TRUE;
Exit0:

	return FALSE;
}

template<class T, class N>
T* CIDObjectPool<T, N>::find(N ID)
{
	RB_TREE_NODE<N>* pDataNode = NULL;
	LOG_PROCESS_ERROR(m_pChunkHead);
	
	pDataNode = rb_search(ID, m_pChunkHead->m_pDataMapRoot[abs(ID) % HASH_TABLE_SIZE]);
	PROCESS_ERROR(pDataNode);

	return _get_data(pDataNode);
Exit0:

	return NULL;
}

template<class T, class N>
template<class TFunc>
inline BOOL CIDObjectPool<T, N>::traverse(TFunc& rFunc)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pChunkHead);

	for (int32_t nIndex = 0; nIndex < HASH_TABLE_SIZE; nIndex++)
	{
		RB_TREE_NODE<N>* pRootNode = m_pChunkHead->m_pDataMapRoot[nIndex];
		if (pRootNode)
		{
			RB_TREE_NODE<N>* pDataNode = rb_first(pRootNode);
			while (pDataNode)
			{
				RB_TREE_NODE<N>* pNextNode = rb_next(pDataNode);

				nRetCode = rFunc(pDataNode->ID, _get_data(pDataNode));
				PROCESS_ERROR(nRetCode);

				pDataNode = pNextNode;
			}
		}
	}

	return TRUE;
Exit0:

	return FALSE;
}

template<class T, class N>
inline int32_t CIDObjectPool<T, N>::get_size(void)
{
	return m_pChunkHead->m_nSize;
}

template<class T, class N>
inline int32_t CIDObjectPool<T, N>::get_free_size(void)
{
	return m_pChunkHead->m_nFreeListSize;
}

template<class T, class N>
inline int32_t CIDObjectPool<T, N>::get_used_size(void)
{
	return m_pChunkHead->m_nSize - m_pChunkHead->m_nFreeListSize;
}

template<class T, class N>
inline int32_t CIDObjectPool<T, N>::get_obj_size(void)
{
	return m_pChunkHead->m_nObjSize;
}

template<class T, class N>
N CIDObjectPool<T, N>::get_obj_id(T* pObject)
{
	RB_TREE_NODE<N>* pDataNode = _get_data_node(pObject);
	PROCESS_ERROR(pDataNode);

	return pDataNode->ID;
Exit0:
	return 0;
}

template<class T, class N>
void CIDObjectPool<T, N>::check_fence(void)
{
#ifdef _DEBUG_FENCE
	CHUNK_NODE* pChunkNode = m_pChunkHead->m_pChunkNode;
	while (pChunkNode)
	{
		for (int32_t nIndex = 0; nIndex < pChunkNode->nSize; nIndex++)
		{
			LOG_CHECK_ERROR(pChunkNode->pTNode[nIndex].qwFence == FENCE_NUMBER);
		}
		pChunkNode = pChunkNode->pNext;
	}
#endif
	return;
}

#ifdef WIN32
	#pragma warning(pop)
#endif  // WIN32

#ifdef _DEBUG_FENCE
	#undef _DEBUG_FENCE
#endif  // _DEBUG_FENCE

#endif	// _ID_OBJECT_POOL_H_
