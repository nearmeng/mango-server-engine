#ifndef _SMS_OBJECT_MGR_H_
#define _SMS_OBJECT_MGR_H_

#include "shm/shm_pool.h"

struct SMS_OBJ
{
	int32_t			nTbusAddr;
	uint32_t		dwUpdateTime;
};

class CSMSObjMgr
{
public:
	CSMSObjMgr() {};
	~CSMSObjMgr() {};

	BOOL init(BOOL bResume);
	BOOL uninit(void);
	inline static CSMSObjMgr& get_instance(void);

	template<class TFunc>
	inline BOOL traverse_session(TFunc& rFunc);

	SMS_OBJ* new_object(uint64_t qwObjID);
	BOOL del_object(SMS_OBJ* pObj);
	SMS_OBJ* find_object(uint64_t qwObjID);

private:
	static CSMSObjMgr	ms_Instance;
	CShmObjectPool<uint64_t, SMS_OBJ>		m_SmsObjMgr;
};

inline CSMSObjMgr& CSMSObjMgr::get_instance()
{
	return ms_Instance;
}
	
template<class TFunc>
inline BOOL CSMSObjMgr::traverse_session(TFunc& rFunc)
{
	int32_t nRetCode = 0;

	nRetCode = m_SmsObjMgr.traverse(rFunc);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;

}

#endif