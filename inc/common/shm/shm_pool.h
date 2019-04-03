#ifndef  _SHM_POOL_H_
#define  _SHM_POOL_H_

#include "shm_pool_def.h"

class CShmMgr
{
public:
	CShmMgr() { pool_mgr = NULL; };
	~CShmMgr() {};
	inline static CShmMgr& get_instance(void);

	int32_t init(int32_t shm_key, int32_t shm_size, bool is_resume);
	int32_t uninit(void);

	inline SHM_POOL_MGR* get_shm_pool_mgr();

private:
	int32_t _shm_create(int32_t shm_key, int32_t shm_size);
	int32_t _init_normal(int32_t shm_key, int32_t shm_size);
	int32_t _init_resume(int32_t shm_key, int32_t shm_size);

private:
	static CShmMgr m_Instance;

	SHM_POOL_MGR* pool_mgr;
};

template<class T, class N>
class CShmObjectPool
{
public:
	CShmObjectPool() { m_shm_type = 0; };
	~CShmObjectPool() {};

	int32_t init(int32_t shm_type, int32_t unit_count, bool is_resume);

	inline int32_t get_count(void);
	inline int32_t get_free_count(void);
	inline int32_t get_used_count(void);

	T* new_object(N id);
	int32_t delete_object(T* pObject);
	T* find_object(N id);

	template<class Func>
	inline int32_t traverse(Func& rFunc);

private:
	inline int32_t _get_hash_map_count(int32_t unit_count);
	inline int32_t _get_hash_index(N id, int32_t hash_map_count);

private:
	int32_t m_shm_type;
};

inline CShmMgr& CShmMgr::get_instance(void)
{
	return m_Instance;
}
	
inline SHM_POOL_MGR* CShmMgr::get_shm_pool_mgr()
{
	return pool_mgr;
}

#include "shm_pool_inl.h"

#endif   /* ----- #ifndef _SHM_POOL_H_  ----- */

