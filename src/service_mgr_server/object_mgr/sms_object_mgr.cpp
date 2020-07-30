#include "stdafx.h"
#include "sms_object_mgr.h"

#include "config/global_config.h"

CSMSObjMgr CSMSObjMgr::ms_Instance;
	
BOOL CSMSObjMgr::init(BOOL bResume)
{
	int32_t nRetCode = 0;

	nRetCode = m_SmsObjMgr.init(stdSMSObj, g_ServerConfig.SC.nInitObjMgrCount, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSMSObjMgr::uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE;
}

SMS_OBJ* CSMSObjMgr::new_object(uint64_t qwObjID)
{
	return m_SmsObjMgr.new_object(qwObjID);
}

BOOL CSMSObjMgr::del_object(SMS_OBJ* pObj)
{
	return m_SmsObjMgr.delete_object(pObj);
}

SMS_OBJ* CSMSObjMgr::find_object(uint64_t qwObjID)
{
	return m_SmsObjMgr.find_object(qwObjID);
}