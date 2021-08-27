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
inline SHM_UNIT_DATA* CShmObjectPool<T, N>::_get_unit_data(SHM_POOL* pool, SHM_UNIT_INDEX<N>* index)
{
    ptrdiff_t diff;
    SHM_UNIT_INDEX<N>* base_index = NULL;

    LOG_PROCESS_ERROR(pool);

    base_index = (SHM_UNIT_INDEX<N>*)(OFFSET2PTR(pool->index_offset));
    diff = (ptrdiff_t)(index - base_index);

	return (SHM_UNIT_DATA*)(OFFSET2PTR(pool->data_offset) + pool->unit_data_size * diff);
Exit0:
    return NULL;
}

template<class T, class N>
inline SHM_UNIT_INDEX<N>* CShmObjectPool<T, N>::_get_unit_index(SHM_POOL* pool, SHM_UNIT_DATA* data)
{
    ptrdiff_t diff;
    SHM_UNIT_DATA* base_data = NULL;

    LOG_PROCESS_ERROR(pool);
	LOG_PROCESS_ERROR(pool->unit_data_size > 0);

    base_data = (SHM_UNIT_DATA*)(OFFSET2PTR(pool->data_offset));
    diff = (ptrdiff_t)(((char*)data - (char*)base_data) / pool->unit_data_size);

	return (SHM_UNIT_INDEX<N>*)OFFSET2PTR(pool->index_offset) + diff;
Exit0:
    return NULL;
}

template<class T, class N>
BOOL CShmObjectPool<T, N>::init(int32_t shm_type, int32_t unit_count, BOOL is_resume)
{
	SHM_POOL* pool = NULL;
	int32_t hash_map_size = ALIGN8(sizeof(int64_t) * _get_hash_map_count(unit_count));
	int32_t unit_size = ALIGN8(sizeof(SHM_UNIT_DATA) + sizeof(T));
	uint64_t pool_size = ALIGN8(sizeof(SHM_POOL) + hash_map_size + (sizeof(SHM_UNIT_INDEX<N>) + unit_size) * unit_count);

	LOG_PROCESS_ERROR(unit_count > 0);
	LOG_PROCESS_ERROR_DETAIL(shm_type > stdInvalid, "shm_type is %d", shm_type);
	LOG_PROCESS_ERROR_DETAIL(shm_type < stdTotal, "shm type is %d", shm_type);
	
	m_shm_type = shm_type;
	
	if(!is_resume)
	{
		pool = CShmMgr::instance().init_pool(shm_type, pool_size);
		LOG_PROCESS_ERROR(pool);

		memset(pool, 0, pool_size);
		pool->shm_type = shm_type;
		pool->unit_data_size = unit_size;
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

			SHM_UNIT_DATA* unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(pool->data_offset + unit_size * i);
			unit_data->fence = FENCE_NUM;

			new (unit_data->data) T;
		}
	}
	else
	{
		for(int32_t i = 0; i < unit_count; i++)
		{
			SHM_UNIT_DATA* unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(pool->data_offset + unit_size * i);
			assert(unit_data->fence == FENCE_NUM);

			new (unit_data->data) T;
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}
    
template<class T, class N>
inline BOOL CShmObjectPool<T, N>::uninit(void)
{
    return TRUE;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_count(void)
{
	SHM_POOL* pool = NULL;

	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_free_count(void)
{
	SHM_POOL* pool = NULL;
	
	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_used_count(void)
{
	SHM_POOL* pool = NULL;
	
	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count - pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
T* CShmObjectPool<T, N>::new_object(N id)
{
	SHM_POOL* pool = NULL;
	int32_t hash_map_index = 0;
	int64_t curr_offset = 0;
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	SHM_UNIT_DATA* unit_data = NULL;
	
	pool = CShmMgr::instance().get_pool(m_shm_type);
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
    unit_data = _get_unit_data(pool, unit_index);
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);

	unit_data->unit_serial = (++pool->serial_generator) & UNIT_SERIAL_MASK;
	if (unit_data->unit_serial == 0)
	{
		unit_data->unit_serial++;
		pool->serial_generator++;
	}

	pool->free_unit_count--;

	return (T*)(unit_data->data);
Exit0:
	return NULL;
}

template<class T, class N>
BOOL CShmObjectPool<T, N>::delete_object(T* object)
{
	SHM_POOL* pool = NULL;
	SHM_UNIT_DATA* unit_data = (SHM_UNIT_DATA*)((char*)object - sizeof(SHM_UNIT_DATA));
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	int32_t hash_map_index = 0; 
	int64_t* curr_offset = NULL;

	LOG_PROCESS_ERROR(object);

	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);
	
    unit_index = _get_unit_index(pool, unit_data);
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
	SHM_UNIT_DATA* unit_data = NULL;
	int64_t curr_offset = 0;

	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	hash_map_index = _get_hash_index(id, pool->hash_map_count);

	//to keep key uniq
	curr_offset = *((int64_t*)pool->hash_map_root + hash_map_index);
	while(curr_offset != 0)
	{
		SHM_UNIT_INDEX<N>* curr_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
		if(curr_index->id == id)
		{
            unit_data = _get_unit_data(pool, curr_index);
			return (T*)(unit_data->data);
		}
		curr_offset = curr_index->next_unit_offset;
	}

Exit0:
	return NULL;
}

template <class T, class N>
inline T* CShmObjectPool<T, N>::get_first_object()
{
    int32_t ret_code = 0;
    SHM_POOL* pool = NULL;
    
    m_traverse_map_index = 0;
    m_traverse_offset = 0;

    pool = CShmMgr::instance().get_pool(m_shm_type);
    LOG_PROCESS_ERROR(pool);

    for (int32_t i = 0; i < pool->hash_map_count; i++)
    {
        int64_t curr_offset = *((int64_t*)pool->hash_map_root + i);
        if (curr_offset == 0)
            continue;

        m_traverse_map_index = i;
        m_traverse_offset = curr_offset;

		SHM_UNIT_INDEX<N>* unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
        SHM_UNIT_DATA* unit_data = _get_unit_data(pool, unit_index);
        return (T*)(unit_data->data);
    }

Exit0:
    return NULL;
}

template <class T, class N>
inline T* CShmObjectPool<T, N>::get_next_object()
{
    int32_t ret_code = 0;
    SHM_POOL* pool = NULL;
    int64_t next_offset = 0;
    SHM_UNIT_INDEX<N>* unit_index = NULL;
    SHM_UNIT_DATA* unit_data = NULL;
    
    PROCESS_ERROR(m_traverse_offset > 0);

    pool = CShmMgr::instance().get_pool(m_shm_type);
    LOG_PROCESS_ERROR(pool);
    
    unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(m_traverse_offset);
    next_offset = unit_index->next_unit_offset;

    if(next_offset != 0)
    {
        m_traverse_offset = next_offset;
		unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(next_offset);
        unit_data = _get_unit_data(pool, unit_index);
    }
    else
    {
        for (m_traverse_map_index++; m_traverse_map_index < pool->hash_map_count; m_traverse_map_index++)
        {
            next_offset = *((int64_t*)pool->hash_map_root + m_traverse_map_index);
            if (next_offset == 0)
                continue;
            else
                break;
        }
        
        PROCESS_ERROR(next_offset != 0);
        m_traverse_offset = next_offset;

		unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(next_offset);
        unit_data = _get_unit_data(pool, unit_index);
    }

    return (T*)(unit_data->data);
Exit0:
    return NULL;
}

template<class T, class N>
template<class Func>
inline int32_t CShmObjectPool<T, N>::traverse(Func& rFunc)
{
	int32_t ret_code = 0;
	SHM_POOL* pool = NULL;

	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	for(int32_t i = 0; i < pool->hash_map_count; i++)
	{
        int64_t curr_offset = *((int64_t*)pool->hash_map_root + i);

		if(curr_offset == 0)
			continue;

		while(curr_offset != 0)
		{
			SHM_UNIT_INDEX<N>* unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(curr_offset);
            SHM_UNIT_DATA* unit_data = _get_unit_data(pool, unit_index);

			curr_offset = unit_index->next_unit_offset;

			ret_code = rFunc(unit_index->id, (T*)(unit_data->data));
			LOG_CHECK_ERROR(ret_code);
		}
	}

Exit0:
	return 0;
}

template<class T>
BOOL CShmObject<T>::init(int32_t shm_type, BOOL is_resume)
{
	SHM_POOL* pool = NULL;
	int32_t unit_size = ALIGN8(sizeof(SHM_UNIT_DATA) + sizeof(T));
	int32_t pool_size = ALIGN8(sizeof(SHM_POOL) + unit_size);

	LOG_PROCESS_ERROR_DETAIL(shm_type > stdInvalid, "shm_type is %d", shm_type);
	LOG_PROCESS_ERROR_DETAIL(shm_type < stdTotal, "shm type is %d", shm_type);
	
	m_shm_type = shm_type;
	
	if(!is_resume)
	{
		pool = CShmMgr::instance().init_pool(shm_type, pool_size);
		LOG_PROCESS_ERROR(pool);

		memset(pool, 0, pool_size);
		pool->shm_type = shm_type;
		pool->unit_data_size = unit_size;
		pool->total_unit_count = 1;
		pool->hash_map_count = 0;
		pool->index_offset = 0;
		pool->data_offset = PTR2OFFSET(pool->hash_map_root);

		pool->free_unit_count = 1;
		pool->free_unit_head_offset = pool->index_offset;

		{
			SHM_UNIT_DATA* unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(pool->data_offset);
			unit_data->fence = FENCE_NUM;
			unit_data->unit_serial = 1;
			
			new (unit_data->data) T;
		}
	}
	else
	{
		SHM_UNIT_DATA* unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(pool->data_offset);
		assert(unit_data->fence == FENCE_NUM);
		assert(unit_data->unit_serial == 1);
		
		new (unit_data->data) T;
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
BOOL CShmObject<T>::uninit(void)
{
    return TRUE;
}

template<class T>
T* CShmObject<T>::get_obj(void)
{
	SHM_POOL* pool = NULL;
	SHM_UNIT_DATA* unit_data = NULL;

	pool = CShmMgr::instance().get_pool(m_shm_type);
	LOG_PROCESS_ERROR(pool);

	unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(pool->data_offset);
	LOG_PROCESS_ERROR(unit_data);

	return (T*)(unit_data->data);

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

inline CShmMgr& CShmMgr::instance(void)
{
	return m_Instance;
}
	
inline uint64_t CShmMgr::ptr2mid(void* object)
{
	int64_t unit_offset = 0;
	SHM_UNIT_MID mid = { 0 };
	SHM_UNIT_DATA* unit_data = NULL;
	
	LOG_PROCESS_ERROR(object);

	unit_data = (SHM_UNIT_DATA*)((char*)object - sizeof(SHM_UNIT_DATA));
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);
	LOG_PROCESS_ERROR(unit_data->unit_serial > 0);

	unit_offset = PTR2OFFSET(unit_data);
	LOG_PROCESS_ERROR(unit_offset > 0);
	LOG_PROCESS_ERROR(unit_offset < pool_mgr->curr_offset);

	mid.unit_offset = unit_offset;
	mid.unit_serial = unit_data->unit_serial;

	return *(uint64_t*)&mid;
Exit0:
	return 0;
}

inline void* CShmMgr::mid2ptr(uint64_t mid)
{
	SHM_UNIT_MID* unit_mid = (SHM_UNIT_MID*)&mid;
	SHM_UNIT_DATA* unit_data = NULL;

	PROCESS_ERROR(mid > 0);

	unit_data = (SHM_UNIT_DATA*)OFFSET2PTR(unit_mid->unit_offset);
	LOG_PROCESS_ERROR(unit_data->fence == FENCE_NUM);
	PROCESS_ERROR(unit_data->unit_serial == unit_mid->unit_serial);

	return unit_data->data;
Exit0:
	return NULL;
}

#endif   /* ----- #ifndef _SHM_POOL_INL_H_  ----- */

