#ifndef _RS_ROLE_MGR_H_
#define _RS_ROLE_MGR_H_

#include "shm/shm_pool.h"

struct RS_OBJ
{
	int32_t nTbusAddr;
};

class CRsObjMgr
{
public:
	CRsObjMgr() {};
	~CRsObjMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CRsObjMgr& get_instance();

	RS_OBJ* new_object(uint64_t qwObjID);
	BOOL del_object(RS_OBJ* pObj);
	RS_OBJ* find_object(uint64_t qwObjID);

private:
	static CRsObjMgr							ms_Instance;
	CShmObjectPool<RS_OBJ, uint64_t>			m_RsObjMgr;
};

inline CRsObjMgr& CRsObjMgr::get_instance()
{
	return ms_Instance;
}

#endif