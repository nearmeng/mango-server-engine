#include "shm_pool.h"
#ifndef  _SHM_POOL_INL_H_
#define  _SHM_POOL_INL_H_

template<class T, class N>
int32_t CShmObjectPool<T, N>::_get_hash_map_count(int32_t unit_count)
{
	int32_t count = 1;

	while(count < unit_count)
	{
		count = count << 1;
	}

	count = count / 4;
	return count;
}

template<class T, class N>
int32_t CShmObjectPool<T, N>::_get_hash_index(N id, int32_t hash_map_count)
{
	uint64_t key = (uint64_t)id;

	key = key | (key >> 16);
	return (key & (hash_map_count - 1));
}

template<class T, class N>
BOOL CShmObjectPool<T, N>::init(int32_t shm_type, int32_t unit_count, BOOL is_resume)
{
	SHM_POOL* pool = NULL;
	int32_t hash_map_size = (sizeof(int64_t) * _get_hash_map_count(unit_count));
	uint64_t pool_size = sizeof(SHM_POOL) + hash_map_size
		+ (sizeof(SHM_UNIT_INDEX<N>) + sizeof(SHM_UNIT_DATA<T>)) * unit_count;

	LOG_PROCESS_ERROR_DETAIL(shm_type > stdInvalid, "shm_type is %d", shm_type);
	LOG_PROCESS_ERROR_DETAIL(shm_type < stdTotal, "shm type is %d", shm_type);
	
	m_shm_type = shm_type;
	
	if(!is_resume)
	{
		pool = CShmMgr::get_instance().init_pool(shm_type, pool_size);
		LOG_PROCESS_ERROR(pool);

		memset(pool, 0, pool_size);
		pool->shm_type = shm_type;
		pool->unit_data_size = sizeof(SHM_UNIT_DATA<T>);
		pool->total_unit_count = unit_count;
		pool->hash_map_count = _get_hash_map_count(unit_count);
		pool->index_offset = PTR2OFFSET(pool->hash_map_root) + hash_map_size;
		pool->data_offset = PTR2OFFSET(pool->hash_map_root) + hash_map_size + sizeof(SHM_UNIT_INDEX<N>) * unit_count;
		pool->serial_generator = (uint32_t)time(NULL) & UNIT_SERIAL_MASK;

		pool->free_unit_count = unit_count;
		pool->free_unit_head_offset = pool->index_offset;
		
		for(int32_t i = 0; i < unit_count; i++)
		{
			SHM_UNIT_INDEX<N>* unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(pool->index_offset + sizeof(SHM_UNIT_INDEX<N>) * i);
			unit_index->id = 0;
			
			if(i != unit_count - 1)
				unit_index->next_unit_offset = PTR2OFFSET(unit_index) + sizeof(SHM_UNIT_INDEX<N>);
			else
				unit_index->next_unit_offset = 0;

			SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + sizeof(SHM_UNIT_DATA<T>) * i);
			unit_data->fence = FENCE_NUM;

			new (&(unit_data->data)) T;
		}
	}
	else
	{
		for(int32_t i = 0; i < unit_count; i++)
		{
			SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + sizeof(SHM_UNIT_DATA<T>) * i);
			assert(unit_data->fence == FENCE_NUM);

			new (&(unit_data->data)) T;
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_count(void)
{
	SHM_POOL* pool = NULL;

	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_free_count(void)
{
	SHM_POOL* pool = NULL;
	
	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_used_count(void)
{
	SHM_POOL* pool = NULL;
	
	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count - pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
T* CShmObjectPool<T, N>::new_object(N id)
{
	SHM_POOL* pool = NULL;
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	int32_t hash_map_index = 0;
	SHM_UNIT_DATA<T>* unit_data = NULL;
	int64_t curr_offset = 0;
	
	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);
	LOG_PROCESS_ERROR(pool->free_unit_head_offset != 0);

	hash_map_index = _get_hash_index(id, pool->hash_map_count);

	//to keep key uniq
	curr_offset = *((int64_t*)pool->hash_map_root + hash_map_index);
	while(curr_offset != 0)
	{
		SHM_UNIT_INDEX<N>* curr_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
		LOG_PROCESS_ERROR(curr_index->id != id);

		curr_offset = curr_index->next_unit_offset;
	}

	//pop from free list
	unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(pool->free_unit_head_offset);
	pool->free_unit_head_offset = unit_index->next_unit_offset;

	//insert into hash map used list
	unit_index->id = id;
	unit_index->next_unit_offset = *((int64_t*)pool->hash_map_root + hash_map_index);
	*((int64_t*)pool->hash_map_root + hash_map_index) = PTR2OFFSET(unit_index);

	//get data
	unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + PTR2OFFSET(unit_index) - pool->index_offset);
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);

	unit_data->unit_serial = (++pool->serial_generator) & UNIT_SERIAL_MASK;
	if (unit_data->unit_serial == 0)
	{
		unit_data->unit_serial++;
		pool->serial_generator++;
	}

	pool->free_unit_count--;

	return (T*)(unit_data);

Exit0:
	return NULL;
}

template<class T, class N>
BOOL CShmObjectPool<T, N>::delete_object(T* object)
{
	SHM_POOL* pool = NULL;
	SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)object;
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	int32_t hash_map_index = 0; 
	int64_t* curr_offset = NULL;

	LOG_PROCESS_ERROR(object);

	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);
		
	unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(pool->index_offset + PTR2OFFSET(unit_data) - pool->data_offset);
	hash_map_index = _get_hash_index(unit_index->id, pool->hash_map_count);

	//reset data
	unit_data->unit_serial = 0;
	
	//remove from hash map used list
	curr_offset = (int64_t*)pool->hash_map_root + hash_map_index;
	while(*curr_offset != 0)
	{
		SHM_UNIT_INDEX<N>* curr_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(*curr_offset);
		if(curr_index == unit_index)
		{
			*curr_offset = curr_index->next_unit_offset;
			break;
		}
		else 
			curr_offset = &(curr_index->next_unit_offset);
	}

	//insert into free list
	unit_index->id = 0;
	unit_index->next_unit_offset = pool->free_unit_head_offset;
	pool->free_unit_head_offset = PTR2OFFSET(unit_index);

	pool->free_unit_count++;

	return TRUE;
Exit0:
	return FALSE;
}

template<class T, class N>
T* CShmObjectPool<T, N>::find_object(N id)
{
	SHM_POOL* pool = NULL;
	int32_t hash_map_index = 0;
	SHM_UNIT_DATA<T>* unit_data = NULL;
	int64_t curr_offset = 0;

	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	hash_map_index = _get_hash_index(id, pool->hash_map_count);

	//to keep key uniq
	curr_offset = *((int64_t*)pool->hash_map_root + hash_map_index);
	while(curr_offset != 0)
	{
		SHM_UNIT_INDEX<N>* curr_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
		if(curr_index->id == id)
		{
			unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + curr_offset - pool->index_offset);
			return (T*)(unit_data);
		}
		curr_offset = curr_index->next_unit_offset;
	}

Exit0:
	return NULL;
}

template<class T, class N>
template<class Func>
inline int32_t CShmObjectPool<T, N>::traverse(Func& rFunc)
{
	int32_t ret_code = 0;
	SHM_POOL* pool = NULL;

	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	for(int32_t i = 0; i < pool->hash_map_count; i++)
	{
		int64_t curr_offset = 0;

		if(*((int64_t*)pool->hash_map_root + i) == 0)
			continue;

		curr_offset = *((int64_t*)pool->hash_map_root + i);
		while(curr_offset != 0)
		{
			SHM_UNIT_INDEX<N>* unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
			SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + curr_offset - pool->index_offset);

			ret_code = rFunc(unit_index->id, (T*)unit_data);
			LOG_CHECK_ERROR(ret_code == 0);

			curr_offset = unit_index->next_unit_offset;
		}
	}

Exit0:
	return 0;
}

template<class T>
BOOL CShmObject<T>::init(int32_t shm_type, BOOL is_resume)
{
	SHM_POOL* pool = NULL;
	uint64_t pool_size = sizeof(SHM_POOL) + sizeof(SHM_UNIT_DATA<T>);

	LOG_PROCESS_ERROR_DETAIL(shm_type > stdInvalid, "shm_type is %d", shm_type);
	LOG_PROCESS_ERROR_DETAIL(shm_type < stdTotal, "shm type is %d", shm_type);
	
	m_shm_type = shm_type;
	
	if(!is_resume)
	{
		pool = CShmMgr::get_instance().init_pool(shm_type, pool_size);
		LOG_PROCESS_ERROR(pool);

		memset(pool, 0, pool_size);
		pool->shm_type = shm_type;
		pool->unit_data_size = sizeof(SHM_UNIT_DATA<T>);
		pool->total_unit_count = 1;
		pool->hash_map_count = 0;
		pool->index_offset = 0;
		pool->data_offset = PTR2OFFSET(pool->hash_map_root);

		pool->free_unit_count = 1;
		pool->free_unit_head_offset = pool->index_offset;

		{
			SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset);
			unit_data->fence = FENCE_NUM;
			
			new (&(unit_data->data)) T;
		}
	}
	else
	{
		SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset);
		assert(unit_data->fence == FENCE_NUM);
		
		new (&(unit_data->data)) T;
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
T* CShmObject<T>::get_obj(void)
{
	SHM_POOL* pool = NULL;
	SHM_UNIT_DATA<T>* unit_data = NULL;

	pool = CShmMgr::get_instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset);
	LOG_PROCESS_ERROR(unit_data);

	return &(unit_data->data);

Exit0:
	return NULL;
}

inline SHM_POOL* CShmMgr::get_pool(int32_t shm_type)
{
	SHM_POOL* pool = NULL;
	
	LOG_PROCESS_ERROR(pool_mgr);
	LOG_PROCESS_ERROR(shm_type > stdInvalid && shm_type < stdTotal);

	pool = pool_mgr->pool_address[shm_type];
	LOG_PROCESS_ERROR(pool);

	return pool;
Exit0:
	return NULL;
}

inline CShmMgr& CShmMgr::get_instance(void)
{
	return m_Instance;
}
	
template<class T>
inline uint64_t CShmMgr::ptr2mid(T * object)
{
	SHM_POOL* pool = NULL;
	int64_t unit_offset = 0;
	int32_t shm_type = 0;
	SHM_UNIT_MID mid = { 0 };
	SHM_UNIT_DATA<T>* unit_data = NULL;
	
	LOG_PROCESS_ERROR(object);

	unit_offset = PTR2OFFSET(object);
	
	for (int32_t i = 0; i < MAX_SHM_TYPE_COUNT; i++)
	{
		if (unit_offset >= pool_mgr->pool_offset[i] &&
			unit_offset <= pool_mgr->pool_offset[i + 1])
		{
			shm_type = i;
			pool = pool_mgr->pool_address[i];
			break;
		}
	}
	
	LOG_PROCESS_ERROR(pool);
	LOG_PROCESS_ERROR(unit_offset >= pool->data_offset);
	LOG_PROCESS_ERROR(unit_offset <= (pool->data_offset + pool->unit_data_size * pool->total_unit_count));

	unit_data = (SHM_UNIT_DATA<T>*)object;
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);
	LOG_PROCESS_ERROR(unit_data->unit_serial > 0);

	mid.shm_type = shm_type;
	mid.unit_offset = unit_offset;
	mid.unit_serial = unit_data->unit_serial;

	return *(uint64_t*)&mid;
Exit0:
	return 0;
}

template<class T>
inline T * CShmMgr::mid2ptr(uint64_t mid)
{
	SHM_UNIT_MID* unit_mid = (SHM_UNIT_MID*)&mid;
	SHM_UNIT_DATA<T>* unit_data = NULL;

	PROCESS_ERROR(mid > 0);
	LOG_PROCESS_ERROR(unit_mid->shm_type > stdInvalid && unit_mid->shm_type < stdTotal);

	unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(unit_mid->unit_offset);
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);
	PROCESS_ERROR(unit_data->unit_serial == unit_mid->unit_serial);

	return (T*)unit_data;
Exit0:
	return NULL;
}

#endif   /* ----- #ifndef _SHM_POOL_INL_H_  ----- */

