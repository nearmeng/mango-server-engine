#ifndef _SERVER_MODULE_H_
#define _SERVER_MODULE_H_

#include "define/str_def.h"

#define MG_REGISTER_MODULE_PRE_INIT(__module_class__, ...)                       \
    class __module_class__##_auto_register                                      \
    {                                                                           \
    public:                                                                     \
        __module_class__##_auto_register()                                      \
        {                                                                       \
            CServerModule* pServerModule = (CServerModule*)__module_class__::create_instance<__module_class__>(#__module_class__, ##__VA_ARGS__); \
            pServerModule->set_pre_init(TRUE);                                  \
            CMGApp::instance().register_module(pServerModule);                  \
        };                                                                      \
    };                                                                          \
    static __module_class__##_auto_register __module_class__##Auto;

#define MG_REGISTER_MODULE(__module_class__, ...)                               \
    class __module_class__##_auto_register                                      \
    {                                                                           \
    public:                                                                     \
        __module_class__##_auto_register()                                      \
        {                                                                       \
            CServerModule* pServerModule = (CServerModule*)__module_class__::create_instance<__module_class__>(#__module_class__, ##__VA_ARGS__); \
            CMGApp::instance().register_module(pServerModule);                  \
        };                                                                      \
    };                                                                          \
    static __module_class__##_auto_register __module_class__##Auto;

#define MG_GET_MODULE(__module_class__) \
    (__module_class__*)CMGApp::instance().get_module(#__module_class__);

class CServerModule
{
public:
    CServerModule() {};
    virtual ~CServerModule() {};

    template<class T>
    inline static T* create_instance(const char* pcszModuleName, ...);

    virtual BOOL set_create_arg(const char* pcszModuleName, va_list args) { set_name(pcszModuleName); return TRUE; };

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

    BOOL get_pre_init(void) { return m_bPreInit; };
    void set_pre_init(BOOL bPreInit) { m_bPreInit = bPreInit; };

private:
    char                        m_szName[MAX_MODULE_NAME_LEN];
    int32_t                     m_nContIndex;
    BOOL                        m_bPreInit;
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

template<class T>
inline T* CServerModule::create_instance(const char* pcszModuleName, ...)
{
    int32_t nRetCode = 0;
    va_list args;
    CServerModule* pServerModule = NULL;

    T* pModule = new T();
    LOG_PROCESS_ERROR(pModule);

    pServerModule = (CServerModule*)pModule;
    pServerModule->set_pre_init(FALSE);

    va_start(args, pcszModuleName);
    nRetCode = pServerModule->set_create_arg(pcszModuleName, args);
    va_end(args);

    LOG_PROCESS_ERROR(nRetCode);

    return pModule;
Exit0:
    return NULL;
}

#endif