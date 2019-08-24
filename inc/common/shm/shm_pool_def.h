#ifndef  _SHM_POOL_DEF_H_
#define  _SHM_POOL_DEF_H_

#include "define/shm_def.h"

#define FENCE_NUM          0xabcddcba
#define UNIT_SERIAL_MASK	0x3FFFF

#define OFFSET2PTR(offset) ((int64_t)CShmMgr::get_instance()._get_pool_mgr() + (int64_t)(offset))
#define PTR2OFFSET(ptr) ((int64_t)(ptr) - (int64_t)CShmMgr::get_instance()._get_pool_mgr())

struct SHM_UNIT_MID
{
	uint64_t shm_type		: 10;		// shm type
	uint64_t unit_serial	: 18;		// unit serial
	uint64_t unit_offset	: 36;		// unit offset
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
	uint32_t fence;					// check overflow
	uint32_t unit_serial;			// check mid valid
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
	uint32_t serial_generator;
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

