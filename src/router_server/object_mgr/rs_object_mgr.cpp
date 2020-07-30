#include "stdafx.h"
#include "rs_object_mgr.h"

#include "config/global_config.h"

CRsObjMgr CRsObjMgr::ms_Instance;

BOOL CRsObjMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_RsObjMgr.init(stdRouterObj, g_ServerConfig.SC.nInitObjMgrCount, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRsObjMgr::uninit(void)
{
	return TRUE;
}

RS_OBJ* CRsObjMgr::new_object(uint64_t qwObjID)
{
	return m_RsObjMgr.new_object(qwObjID);
}

RS_OBJ* CRsObjMgr::find_object(uint64_t qwObjID)
{
	return m_RsObjMgr.find_object(qwObjID);
}

BOOL CRsObjMgr::del_object(RS_OBJ* pObj)
{
	return m_RsObjMgr.delete_object(pObj);
}