#ifndef  _SHM_POOL_H_
#define  _SHM_POOL_H_

#include "shm_pool_def.h"

class CShmMgr
{
public:
	CShmMgr() { pool_mgr = NULL; };
	~CShmMgr() {};

	template<class T> 
	friend class CShmObject;
	template<class T, class N> 
	friend class CShmObjectPool;

	inline static CShmMgr& get_instance(void);

	BOOL init(int32_t shm_key, int32_t shm_size, BOOL is_resume);
	BOOL uninit(void);

	SHM_POOL* init_pool(int32_t shm_type, int32_t pool_size);
	inline SHM_POOL* get_pool(int32_t shm_type);

	template<class T>
	uint64_t ptr2mid(T* object);
	template<class T>
	T* mid2ptr(uint64_t mid);

private:
	int32_t _shm_create(int32_t shm_key, int32_t shm_size);
	int32_t _init_normal(int32_t shm_key, int32_t shm_size);
	int32_t _init_resume(int32_t shm_key, int32_t shm_size);
	inline SHM_POOL_MGR* _get_pool_mgr(void) { return pool_mgr; };

private:
	static CShmMgr m_Instance;

	SHM_POOL_MGR* pool_mgr;
};


template<class T>
class CShmObject
{
public:
	CShmObject() { m_shm_type = 0; }
	~CShmObject() {};

	BOOL init(int32_t shm_type, BOOL is_resume);
	T* get_obj(void);

private:
	int32_t m_shm_type;
};

template<class T, class N>
class CShmObjectPool
{
public:
	CShmObjectPool() { m_shm_type = 0; };
	~CShmObjectPool() {};

	BOOL init(int32_t shm_type, int32_t unit_count, BOOL is_resume);

	inline int32_t get_count(void);
	inline int32_t get_free_count(void);
	inline int32_t get_used_count(void);

	T* new_object(N id);
	BOOL delete_object(T* pObject);
	T* find_object(N id);

	template<class Func>
	inline int32_t traverse(Func& rFunc);

    T* get_first_object();
    T* get_next_object();

private:
	inline int32_t _get_hash_map_count(int32_t unit_count);
	inline int32_t _get_hash_index(N id, int32_t hash_map_count);
    inline SHM_UNIT_DATA<T>* _get_unit_data(SHM_POOL* pool, SHM_UNIT_INDEX<N>* index);
    inline SHM_UNIT_INDEX<N>* _get_unit_index(SHM_POOL* pool, SHM_UNIT_DATA<T>* data);

private:
	int32_t m_shm_type;
    int32_t m_traverse_map_index;
    int64_t m_traverse_offset;
};

#include "shm_pool_inl.h"

#endif   /* ----- #ifndef _SHM_POOL_H_  ----- */

