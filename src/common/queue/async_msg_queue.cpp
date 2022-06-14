#include "stdafx.h"
#include "queue/async_msg_queue.h"

BOOL CRingBuffer::init(void)
{
	memset(m_szBuffer, 0, sizeof(m_szBuffer));
	m_nSize = MAX_RING_BUFFER_SIZE; 
	m_nReadPos = 0;
	m_nWritePos = 0;

	LOG_PROCESS_ERROR(_check_is_power_of_2(m_nSize));

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRingBuffer::uninit(void)
{
	return TRUE;
}

BOOL CRingBuffer::clear(void)
{
	m_nWritePos = m_nReadPos = 0;
	return TRUE;
}

BOOL CRingBuffer::put_data(const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	int32_t nSplitLen = 0;
	
	if (get_left_space() < dwSize)
		return FALSE;

	nSplitLen = m_nSize - (m_nWritePos & (m_nSize - 1));
	if (nSplitLen < dwSize)
	{
		memcpy(m_szBuffer + (m_nWritePos & (m_nSize - 1)), pBuffer, nSplitLen);
		memcpy(m_szBuffer, pBuffer + nSplitLen, dwSize - nSplitLen);
	}
	else
	{
		memcpy(m_szBuffer + (m_nWritePos & (m_nSize - 1)), pBuffer, dwSize);
	}

	m_nWritePos += dwSize;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRingBuffer::get_data(char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	int32_t nSplitLen = 0;

	LOG_PROCESS_ERROR(pBuffer);
	LOG_PROCESS_ERROR(get_used_space() >= dwSize);

	nSplitLen = m_nSize - (m_nReadPos & (m_nSize - 1));
	if (nSplitLen < dwSize)
	{
		memcpy(pBuffer, m_szBuffer + (m_nReadPos & (m_nSize - 1)), nSplitLen);
		memcpy((char*)(pBuffer + nSplitLen), m_szBuffer, dwSize - nSplitLen);
	}
	else
	{
		memcpy(pBuffer, m_szBuffer + (m_nReadPos & (m_nSize - 1)), dwSize);
	}
	
	m_nReadPos += dwSize;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRingBuffer::_check_is_power_of_2(uint32_t dwSize)
{
	return (dwSize != 0) && ((dwSize & (dwSize - 1)) == 0);
}

BOOL CShmMsgQueue::init(BOOL bResume)
{
	int32_t nRetCode = 0;
	CRingBuffer* pRingBuffer = NULL;
	
	nRetCode = m_Buffer.init(stdMsgQueue, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	pRingBuffer = m_Buffer.get_obj();
	LOG_PROCESS_ERROR(pRingBuffer);
	
	if (!bResume)
	{
		pRingBuffer->init();
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CShmMsgQueue::uninit(void)
{
	int32_t nRetCode = 0;
	CRingBuffer* pRingBuffer = NULL;

	pRingBuffer = _shm_data();
	LOG_PROCESS_ERROR(pRingBuffer);

	pRingBuffer->uninit();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CShmMsgQueue::has_msg(void)
{
	CRingBuffer* pRingBuffer = NULL;

	pRingBuffer = _shm_data();
	LOG_PROCESS_ERROR(pRingBuffer);

	PROCESS_ERROR(pRingBuffer->get_used_space() > 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CShmMsgQueue::push_msg(int32_t nMsgAddr, const char* pBuffer, size_t dwSize)
{
	int32_t nRetCode = 0;
	CRingBuffer* pRingBuffer = NULL;
	
	pRingBuffer = _shm_data();
	LOG_PROCESS_ERROR(pRingBuffer);
	
	LOG_PROCESS_ERROR(pRingBuffer->get_left_space() >= (dwSize + 2 * sizeof(int32_t)));

	//can be optimized
	pRingBuffer->put_data((char*)&nMsgAddr, sizeof(nMsgAddr));
	pRingBuffer->put_data((char*)&dwSize, sizeof(dwSize));
	pRingBuffer->put_data(pBuffer, dwSize);

	return TRUE;
Exit0:
	INF("left space %d size %d", pRingBuffer->get_left_space(), dwSize);
	return FALSE;
}

BOOL CShmMsgQueue::pop_msg(int32_t& nMsgAddr, char* pBuffer, size_t &dwSize)
{
	int32_t nRetCode = 0;
	CRingBuffer* pRingBuffer = NULL;
	
	pRingBuffer = _shm_data();
	LOG_PROCESS_ERROR(pRingBuffer);

	LOG_PROCESS_ERROR(pRingBuffer->get_used_space() > (2 * sizeof(int32_t)));

	pRingBuffer->get_data((char*)&nMsgAddr, sizeof(nMsgAddr));
	pRingBuffer->get_data((char*)&dwSize, sizeof(dwSize));
	pRingBuffer->get_data(pBuffer, dwSize);

	return TRUE;
Exit0:
	return FALSE;
}
