#include "stdafx.h"
#include "res_mgr/global_res_mgr.h"
#include "res_mgr/res_mgr.h"

#include "config/global_config.h"


CGlobalResMgr* CGlobalResMgr::ms_Instance = NULL;

int tolua_enum_open(lua_State* tolua_S);
int tolua_res_open(lua_State* tolua_S);

struct FAKE_RES : RES_BASE
{
};

CGlobalResMgr::CGlobalResMgr()
{
    m_nResMode = 0;
    m_ResMgr.clear();
}

BOOL CGlobalResMgr::init(int32_t nResMode, BOOL bResume)
{
    int32_t nRetCode = 0;
    char szScriptPath[COMMON_NAME_LEN];
    CResMgr<FAKE_RES>* pResMgr = NULL;

    LOG_PROCESS_ERROR(nResMode > rlmInvalid && nResMode < rlmTotal);

    m_nResMode = nResMode;

    nRetCode = m_LuaScript.init();
    LOG_PROCESS_ERROR(nRetCode);

    tolua_enum_open(m_LuaScript.get_lua_state());
    tolua_res_open(m_LuaScript.get_lua_state());

    snprintf(szScriptPath, COMMON_NAME_LEN, "%s/%s", g_ServerConfig.Common.szScriptPath, "read_res/main.lua");

    nRetCode = m_LuaScript.load_from_file(szScriptPath, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    for (std::map<std::string, RES_INFO>::iterator it = m_ResMgr.begin(); it != m_ResMgr.end(); it++)
    {
        std::string ResName = it->first;
        RES_INFO& rResInfo = it->second;

        pResMgr = (CResMgr<FAKE_RES>*)rResInfo.pResMgr;
        LOG_PROCESS_ERROR(pResMgr);

        nRetCode = pResMgr->init(ResName.c_str(), g_ServerConfig.Common.szResPath, rResInfo.ResObjName.c_str(), bResume);
        LOG_PROCESS_ERROR(nRetCode);
    }

    for (std::map<std::string, RES_INFO>::iterator it = m_ResMgr.begin(); it != m_ResMgr.end(); it++)
    {
        std::string ResName = it->first;
        RES_INFO& rResInfo = it->second;

        pResMgr = (CResMgr<FAKE_RES>*)rResInfo.pResMgr;
        LOG_PROCESS_ERROR(pResMgr);

        nRetCode = pResMgr->post_check();
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGlobalResMgr::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = m_LuaScript.uninit();
    LOG_CHECK_ERROR(nRetCode);
    
    for (std::map<std::string, RES_INFO>::iterator it = m_ResMgr.begin(); it != m_ResMgr.end(); it++)
    {
        std::string ResName = it->first;
        RES_INFO& rResInfo = it->second;

        CResMgr<FAKE_RES>* pResMgr = (CResMgr<FAKE_RES>*)rResInfo.pResMgr;
        LOG_PROCESS_ERROR(pResMgr);

        nRetCode = pResMgr->uninit();
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGlobalResMgr::add_res_mgr(const char* pcszResName, void* pResMgr)
{
    int32_t nRetCode = 0;
    const char* pcszStart = NULL;
    RES_INFO stInfo;
    char szParsedResName[COMMON_NAME_LEN];
    char szParsedObjName[COMMON_NAME_LEN];

    LOG_PROCESS_ERROR(pcszResName);
    LOG_PROCESS_ERROR(pResMgr);

#if defined(WIN32) || defined(WIN64)
	pcszStart = strstr(pcszResName, " ");
	PROCESS_ERROR(pcszStart);

	snprintf(szParsedObjName, COMMON_NAME_LEN, pcszStart + 1);
	snprintf(szParsedResName, COMMON_NAME_LEN, pcszStart + 1);
#else
	pcszStart = pcszResName;
	while (isdigit(*pcszStart))
		pcszStart++;

	snprintf(szParsedObjName, COMMON_NAME_LEN, pcszStart);
	snprintf(szParsedResName, COMMON_NAME_LEN, pcszStart);
#endif	// WIN32 || WIN64
    
	*strrchr(szParsedResName, '_') = 0;
	strxtolower(szParsedResName, COMMON_NAME_LEN);

    stInfo.pResMgr = pResMgr;
    stInfo.ResObjName = szParsedObjName;

    m_ResMgr.insert(std::make_pair(szParsedResName, stInfo));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGlobalResMgr::reload(BOOL bForce)
{
    int32_t nRetCode = 0;

    for (std::map<std::string, RES_INFO>::iterator it = m_ResMgr.begin(); it != m_ResMgr.end(); it++)
    {
        std::string ResName = it->first;
        RES_INFO& rResInfo = it->second;

        CResMgr<FAKE_RES>* pResMgr = (CResMgr<FAKE_RES>*)rResInfo.pResMgr;
        LOG_PROCESS_ERROR(pResMgr);

        nRetCode = pResMgr->reload(bForce);
        LOG_PROCESS_ERROR(nRetCode);
    }
    
    for (std::map<std::string, RES_INFO>::iterator it = m_ResMgr.begin(); it != m_ResMgr.end(); it++)
    {
        std::string ResName = it->first;
        RES_INFO& rResInfo = it->second;

        CResMgr<FAKE_RES>* pResMgr = (CResMgr<FAKE_RES>*)rResInfo.pResMgr;
        LOG_PROCESS_ERROR(pResMgr);

        nRetCode = pResMgr->post_check();
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}
