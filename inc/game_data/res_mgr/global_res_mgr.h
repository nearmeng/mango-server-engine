#ifndef _GLOBAL_RES_MGR_H_
#define _GLOBAL_RES_MGR_H_

#include "define/res_def.h"

class CGlobalResMgr
{
public:
    CGlobalResMgr();
    virtual ~CGlobalResMgr() {};

    inline static CGlobalResMgr& instance(void);

    BOOL init(int32_t nResMode, BOOL bResume);
    BOOL uninit(void);

    BOOL add_res_mgr(const char* pcszResName, void* pResMgr);
    BOOL reload(BOOL bForce);

    inline int32_t get_res_mode(void);
    inline int32_t get_res_mgr_count(void);
    inline CLuaScript& get_lua_script();

private:
    static CGlobalResMgr*            ms_Instance;
    int32_t                         m_nResMode;
    CLuaScript                      m_LuaScript;

    struct RES_INFO
    {
        std::string     ResObjName;
        void*           pResMgr;
    };
    std::map<std::string, RES_INFO>    m_ResMgr;
};

inline CGlobalResMgr& CGlobalResMgr::instance(void)
{
    if (ms_Instance == NULL)
        ms_Instance = new CGlobalResMgr;

    return *ms_Instance;
}

inline int32_t CGlobalResMgr::get_res_mode(void)
{
    return m_nResMode;
}
    
inline int32_t CGlobalResMgr::get_res_mgr_count(void)
{
    return (int32_t)m_ResMgr.size();
}
    
inline CLuaScript& CGlobalResMgr::get_lua_script()
{
    return m_LuaScript;
}

#endif
