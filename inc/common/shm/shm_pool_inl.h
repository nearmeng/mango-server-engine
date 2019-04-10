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
int32_t CShmObjectPool<T, N>::init(int32_t shm_type, int32_t unit_count, bool is_resume)
{
	SHM_POOL* pool = NULL;
	SHM_POOL_MGR* pool_mgr = NULL;
	int32_t hash_map_size = (sizeof(int64_t) * _get_hash_map_count(unit_count));
	uint64_t pool_size = sizeof(SHM_POOL) + hash_map_size
		+ (sizeof(SHM_UNIT_INDEX<N>) + sizeof(SHM_UNIT_DATA<T>)) * unit_count;

	LOG_PROCESS_ERROR_DETAIL(shm_type > stdInvalid, "shm_type is %d", shm_type);
	LOG_PROCESS_ERROR_DETAIL(shm_type < stdTotal, "shm type is %d", shm_type);
	
	m_shm_type = shm_type;
	
	if(!is_resume)
	{
		pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
		LOG_PROCESS_ERROR(pool_mgr != NULL);

		LOG_PROCESS_ERROR(unit_count > 0);
		LOG_PROCESS_ERROR(pool_mgr->curr_offset + pool_size <= pool_mgr->shm_size);

		pool = (SHM_POOL*)OFFSET2PTR(pool_mgr->curr_offset);
		LOG_PROCESS_ERROR(pool != NULL);
		LOG_PROCESS_ERROR(pool_mgr->pool_offset[shm_type] == 0);
		LOG_PROCESS_ERROR(pool_mgr->pool_address[shm_type] == 0);

		pool_mgr->pool_offset[shm_type] = pool_mgr->curr_offset;
		pool_mgr->pool_address[shm_type] = pool;
		pool_mgr->curr_offset += pool_size;

		memset(pool, 0, pool_size);
		pool->shm_type = shm_type;
		pool->unit_data_size = sizeof(SHM_UNIT_DATA<T>);
		pool->total_unit_count = unit_count;
		pool->hash_map_count = _get_hash_map_count(unit_count);
		pool->index_offset = PTR2OFFSET(pool->hash_map_root) + hash_map_size;
		pool->data_offset = PTR2OFFSET(pool->hash_map_root) + hash_map_size + sizeof(SHM_UNIT_INDEX<N>) * unit_count;

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
#if 	defined(_DEBUG)
			unit_data->fence = FENCE_NUM;
#endif
			new (&(unit_data->data)) T;
		}
	}
	else
	{
		for(int32_t i = 0; i < unit_count; i++)
		{
			SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)OFFSET2PTR(pool->data_offset + sizeof(SHM_UNIT_DATA<T>) * i);
#if 	defined(_DEBUG)
			assert(unit_data->fence == FENCE_NUM);
#endif
			new (&(unit_data->data)) T;
		}
	}

	return 0;
Exit0:
	return -1;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_count(void)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_free_count(void)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
	LOG_PROCESS_ERROR(pool);

	return pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
inline int32_t CShmObjectPool<T, N>::get_used_count(void)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
	LOG_PROCESS_ERROR(pool);

	return pool->total_unit_count - pool->free_unit_count;
Exit0:
	return 0;
}

template<class T, class N>
T* CShmObjectPool<T, N>::new_object(N id)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	int32_t hash_map_index = 0;
	SHM_UNIT_DATA<T>* unit_data = NULL;
	int64_t curr_offset = 0;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
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

	pool->free_unit_count--;

	return (T*)(unit_data);

Exit0:
	return NULL;
}

template<class T, class N>
int32_t CShmObjectPool<T, N>::delete_object(T* object)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;
	SHM_UNIT_DATA<T>* unit_data = (SHM_UNIT_DATA<T>*)object;
	SHM_UNIT_INDEX<N>* unit_index = NULL;
	int32_t hash_map_index = 0; 
	int64_t* curr_offset = NULL;

	LOG_PROCESS_ERROR(object);

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
	LOG_PROCESS_ERROR(pool);
		
	unit_index = (SHM_UNIT_INDEX<N>*)OFFSET2PTR(pool->index_offset + PTR2OFFSET(unit_data) - pool->data_offset);
	hash_map_index = _get_hash_index(unit_index->id, pool->hash_map_count);
	
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

	return 0;
Exit0:
	return -1;
}

template<class T, class N>
T* CShmObjectPool<T, N>::find_object(N id)
{
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;
	int32_t hash_map_index = 0;
	SHM_UNIT_DATA<T>* unit_data = NULL;
	int64_t curr_offset = 0;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
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
	SHM_POOL_MGR* pool_mgr = NULL;
	SHM_POOL* pool = NULL;

	pool_mgr = CShmMgr::get_instance().get_shm_pool_mgr();
	LOG_PROCESS_ERROR(pool_mgr);

	pool = pool_mgr->pool_address[m_shm_type];
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

#endif   /* ----- #ifndef _SHM_POOL_INL_H_  ----- */
