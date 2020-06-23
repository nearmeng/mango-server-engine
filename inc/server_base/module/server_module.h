#ifndef _SERVER_MODULE_H_
#define _SERVER_MODULE_H_

#include "define/str_def.h"

class CServerModule
{
public:
    CServerModule() {};
    virtual ~CServerModule() {};

    inline const char* get_name(void);
    inline void set_name(const char* pcszModuleName);
    
    inline int32_t get_cont_index(void);
    inline void set_cont_index(int32_t nContIndex);

    virtual BOOL init(BOOL bResume) = 0;
    virtual BOOL uninit(void) = 0;

    virtual void on_frame(void) {};
    virtual void on_proc(void) {};

    virtual BOOL on_reload(void) { return TRUE; };
    virtual BOOL on_stop(void) { return TRUE; };

    virtual BOOL msg_handler_init(void) { return TRUE; };

private:
    char        m_szName[MAX_MODULE_NAME_LEN];
    int32_t     m_nContIndex;
};

class CServerModuleContainer
{
public:
    CServerModuleContainer() {};
    ~CServerModuleContainer() {};

    BOOL init(void);
    BOOL uninit(void);

    BOOL add_module(CServerModule* pModule);
    BOOL del_module(CServerModule* pModule);

    CServerModule* get_module_by_idx(int32_t nIndex);
    CServerModule* get_module_by_name(const char* pcszName);

    inline int32_t get_module_count(void);

private:
    int32_t         m_nModuleCount;
    CServerModule*  m_pModules[MAX_MODULE_COUNT];
    std::map<std::string, CServerModule*> m_Name2Module;
};

inline const char* CServerModule::get_name(void)
{
    return m_szName;
}
    
inline void CServerModule::set_name(const char* pcszModuleName)
{
    strxcpy(m_szName, pcszModuleName, sizeof(m_szName)); 
}
    
inline int32_t CServerModule::get_cont_index(void)
{
    return m_nContIndex;
}

inline void CServerModule::set_cont_index(int32_t nContIndex)
{
    m_nContIndex = nContIndex;
}

inline int32_t CServerModuleContainer::get_module_count(void)
{
    return m_nModuleCount;
}

#endif