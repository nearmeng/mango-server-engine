#ifndef  _SHM_POOL_DEF_H_
#define  _SHM_POOL_DEF_H_

#define MAX_SHM_TYPE_COUNT 	512
#define UNIT_MAGIC          0xabcddcba
#define FENCE_NUM			0xABCDEF0123456789

#define OFFSET2PTR(offset) ((int64_t)pool_mgr + (int64_t)(offset))
#define PTR2OFFSET(ptr) ((int64_t)(ptr) - (int64_t)pool_mgr)

enum SHM_TYPE_DEF
{
	stdInvalid = -1,

	stdType1,
	stdType2,
	stdBtCtx,
	stdBtMgrData,
	stdBtEvent,
	stdBtGlobalEventList,

	stdTotal = MAX_SHM_TYPE_COUNT,
};

template<class N>
struct SHM_UNIT_INDEX
{
	N id;
	int64_t next_unit_offset;
};

template<class T>
struct SHM_UNIT_DATA
{
	T        data;
#if defined(_DEBUG)
	uint64_t fence;
#endif
};

struct SHM_POOL 
{
	int32_t  shm_type;
	int32_t  unit_data_size;
	int32_t  total_unit_count;
	int32_t  free_unit_count;
	int64_t  free_unit_head_offset;
	int64_t  index_offset;
	int64_t  data_offset;
	int32_t  hash_map_count;
	char     hash_map_root[0];
};

struct SHM_POOL_MGR
{
	int32_t     shm_id;
	uint64_t    shm_size;
	int64_t     curr_offset;
	int64_t     pool_offset[MAX_SHM_TYPE_COUNT];
	SHM_POOL*   pool_address[MAX_SHM_TYPE_COUNT];
};

#endif   /* ----- #ifndef _SHM_POOL_DEF_H_  ----- */

