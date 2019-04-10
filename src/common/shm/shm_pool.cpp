#include "stdafx.h"
#include "shm/shm_pool.h"

#ifdef __linux__
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#define SHM_PERM (0666)

CShmMgr CShmMgr::m_Instance;

#ifdef __linux__
int32_t CShmMgr::_shm_create(int32_t shm_key, int32_t shm_size)
{
	int32_t ret = 0;
	int shm_id = 0; 
	
	shm_id = shmget(shm_key, 0, SHM_PERM);
	if(shm_id > 0)
	{
		ret = shmctl(shm_id, IPC_RMID, 0);
		LOG_PROCESS_ERROR_DETAIL(ret == 0, "shm_id %d", shm_id);
	}

	shm_id = shmget(shm_key, shm_size, SHM_PERM | IPC_CREAT | IPC_EXCL);
	LOG_PROCESS_ERROR_DETAIL(shm_id >= 0, "shm_id %d", shm_id);

	return shm_id;
Exit0:
	return -1;
}
#endif

int32_t CShmMgr::_init_normal(int32_t shm_key, int32_t shm_size)
{
	int32_t shm_id = 0;

#ifdef __linux__
	shm_id = _shm_create(shm_key, shm_size);
	LOG_PROCESS_ERROR_DETAIL(shm_id > 0, "shm_key %d", shm_key);

	pool_mgr = (SHM_POOL_MGR*)shmat(shm_id, NULL, 0);
	LOG_PROCESS_ERROR_DETAIL((int64_t)pool_mgr != -1, "shm_key %d", shm_key);
#else
	pool_mgr = (SHM_POOL_MGR*)malloc(shm_size);
	LOG_PROCESS_ERROR_DETAIL(pool_mgr, "shm_size %d", shm_size);
#endif

	memset(pool_mgr, 0, sizeof(SHM_POOL_MGR));
	pool_mgr->shm_id = shm_id;
	pool_mgr->shm_size = shm_size;
	pool_mgr->curr_offset = sizeof(SHM_POOL_MGR);

	return 0;
Exit0:
	return -1;
}

int32_t CShmMgr::_init_resume(int32_t shm_key, int32_t shm_size)
{
#ifdef __linux__
	int32_t shm_id = 0;

	shm_id = shmget(shm_key, 0, SHM_PERM);
	LOG_PROCESS_ERROR_DETAIL(shm_id >= 0, "shm_key %d", shm_key);

	pool_mgr = (SHM_POOL_MGR*)shmat(shm_id, NULL, 0);
	LOG_PROCESS_ERROR((int64_t)pool_mgr != -1);
	LOG_PROCESS_ERROR(pool_mgr->shm_id == shm_id);
	LOG_PROCESS_ERROR(pool_mgr->shm_size == (uint32_t)shm_size);

	for(int32_t i = 0; i < MAX_SHM_TYPE_COUNT; i++)
	{
		if(pool_mgr->pool_offset[i] <= 0)
			continue;

		pool_mgr->pool_address[i] = (SHM_POOL*)OFFSET2PTR(pool_mgr->pool_offset[i]);
	}
#else
	LOG_PROCESS_ERROR(FALSE);
#endif

	return 0;
Exit0:
	return -1;
}

int32_t CShmMgr::init(int32_t shm_key, int32_t shm_size, bool is_resume)
{
	int32_t ret_code = 0;

	if(!is_resume)
	{
		ret_code = _init_normal(shm_key, shm_size);
		LOG_PROCESS_ERROR_DETAIL(ret_code == 0, "shm_key %d shm_size %d", shm_key, shm_size);
	}
	else
	{
		ret_code = _init_resume(shm_key, shm_size);
		LOG_PROCESS_ERROR_DETAIL(ret_code == 0, "shm_key %d shm_size %d", shm_key, shm_size);
	}

	return 0;
Exit0:
	return -1;
}

int32_t CShmMgr::uninit(void)
{
	if(pool_mgr != NULL)
	{
#ifdef __linux__
		shmctl(pool_mgr->shm_id, IPC_RMID, 0);
#else
		free(pool_mgr);
#endif
	}

	return 0;
}

