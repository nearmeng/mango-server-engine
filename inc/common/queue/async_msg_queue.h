#ifndef __ASYNC_MSG_QUEUE_H__
#define __ASYNC_MSG_QUEUE_H__

#ifdef __linux__
#define MAX_RING_BUFFER_SIZE	(4 * 1024 * 1024)
#else
#define MAX_RING_BUFFER_SIZE	(128 * 1024)
#endif

#include "shm/shm_pool.h"

class CRingBuffer
{
public:
	CRingBuffer() {};
	~CRingBuffer() {};

	BOOL init(void);
	BOOL uninit(void);
	
	BOOL clear(void);
	BOOL put_data(const char* pBuffer, size_t dwSize);
	BOOL get_data(char* pBuffer, size_t dwSize);

	inline int32_t get_used_space();
	inline int32_t get_left_space();
	inline int32_t get_size();

private:
	BOOL _check_is_power_of_2(uint32_t dwSize);

private:	
	char		m_szBuffer[MAX_RING_BUFFER_SIZE];
	int32_t		m_nSize;
	int32_t		m_nReadPos;				// not turn back
	int32_t		m_nWritePos;			// not turn back
};


inline int32_t CRingBuffer::get_used_space()
{
	return m_nWritePos - m_nReadPos;
}

inline int32_t CRingBuffer::get_left_space()
{
	return m_nSize - get_used_space();
}

inline int32_t CRingBuffer::get_size()
{
	return m_nSize;
}

class CShmMsgQueue 
{
public:
	CShmMsgQueue() {}
	~CShmMsgQueue() {}

	BOOL init(BOOL bResume);
	BOOL uninit(void);

	BOOL has_msg(void);
	BOOL push_msg(int32_t nMsgAddr, const char* pBuffer, size_t dwSize);
	BOOL pop_msg(int32_t& nMsgAddr, char* pBuffer, size_t &dwSize);

private:
	CRingBuffer* _shm_data() { return m_Buffer.get_obj(); }

private:
	CShmObject<CRingBuffer>		m_Buffer;
};

#endif

