#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

#include "module/server_module.h"
#include "module/server_component.h"
#include "define/server_def.h"
#include "define/str_def.h"

typedef BOOL(*APP_FUNC)(TAPPCTX* pCtx, BOOL bResume);
typedef BOOL(*APP_FRAME_FUNC)();

typedef int (*CONTROL_INIT_FUNC)(TAPPCTX* pCtx, void* pArg);
typedef int (*CONTROL_FINI_FUNC)(TAPPCTX* pCtx, void* pArg);
typedef int (*CONTROL_PRE_PROC_FUNC)(unsigned short argc, const char** argv);
typedef int (*CONTROL_PROC_FUNC)(TAPPCTX *pCtx, void * pArg, unsigned short argc, const char** argv);
typedef const char*(*CONTROL_HELP_FUNC)(void);

typedef BOOL(*USER_MSG_HANDLER)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

struct MG_CONFIG
{
    MG_CONFIG()
    {
        bUseTconnd = FALSE;
        bUseRouter = FALSE;
        bUseConn = FALSE;
        bUseDBProxy = FALSE;

        pAppInit = NULL;
        pAppFini = NULL;
        pAppFrame = NULL;
        pAppReload = NULL;
        pAppStop = NULL;
            
        pControlInit = NULL;
        pControlFini = NULL;
        pControlPreProc = NULL;
        pControlProc = NULL;
        pControlHelp = NULL;

        nInitShmSize = 0;
        nResMode = 0;
    }

    //sys control
    BOOL bUseTconnd;
    BOOL bUseRouter;
    BOOL bUseConn;
    BOOL bUseDBProxy;

    //func 
    APP_FUNC pAppInit; 
    APP_FUNC pAppFini; 
    APP_FRAME_FUNC pAppFrame; 
    APP_FUNC pAppReload; 
    APP_FUNC pAppStop;
    
    CONTROL_INIT_FUNC pControlInit; 
    CONTROL_FINI_FUNC pControlFini; 
    CONTROL_PRE_PROC_FUNC pControlPreProc; 
    CONTROL_PROC_FUNC pControlProc; 
    CONTROL_HELP_FUNC pControlHelp;

    //shm 
    int nInitShmSize;

    //res
    int nResMode;
};

class CMGApp
{
public:
    CMGApp(void);
    ~CMGApp(void) {};

    BOOL init(const char* pcszServerName, int32_t argc, char* argv[]);
    void run_mainloop(void);
    BOOL reload(void);
    void fini();

    inline static CMGApp& instance(void);

    void set_config(MG_CONFIG& config);

    inline void set_stop_timer(uint64_t qwServerTick);
    inline uint64_t get_stop_timer(void);

    inline void set_state(int32_t nState);
    inline int32_t get_state(void);
    
    inline int32_t get_frame(void);
    inline int32_t get_tbus_addr(void);
    inline BOOL is_resume(void);

    BOOL register_module(CServerModule* pModule);
    CServerModule* get_module(const char* pcszModuleName);
    
    inline std::list<CComponentModule*>& get_component_list(int32_t nOwnerType);
    
    BOOL set_user_msg_handler(int32_t nServerType, USER_MSG_HANDLER pMsgHandler);

private:
    BOOL _init_tlog(void);
    
    static int32_t _app_init(TAPPCTX* pCtx, void* pArg);
    static int32_t _app_fini(TAPPCTX* pCtx, void* pArg);
    static int32_t _app_proc(TAPPCTX* pCtx, void* pArg);
    static int32_t _app_reload(TAPPCTX* pCtx, void* pArg);
    static int32_t _app_stop(TAPPCTX* pCtx, void* pArg);
    static int32_t _app_quit(TAPPCTX* pCtx, void* pArg);

    static void _frame_timeout(uint64_t qwTimerID, void* pCbData, int32_t nCbDataLen);
    static BOOL _default_msg_recv_proc(const char* pBuff, size_t nSize, int32_t nSrcAddr);

private:
    char    m_szServerName[MAX_SERVER_NAME_LEN];
    int32_t m_nState;
    int64_t m_qwStopTimer;
    int32_t m_nServerFrame;
    TAPPCTX m_stAppCtx;

    uint32_t m_dwLastReloadTime;

    BOOL m_bNeedExitClean;
    MG_CONFIG m_Config;

    USER_MSG_HANDLER m_UserMsgHandler[svrTotal + 1];

    CServerModuleContainer m_ModuleCont;
    std::map<int32_t, std::list<CComponentModule*> > m_ComponentModuleList;

    static CMGApp* ms_Instance;  // to avoid construct behind other static auto register class 
};
    
inline CMGApp& CMGApp::instance()
{
    if (ms_Instance == NULL)
        ms_Instance = new CMGApp();

    return *ms_Instance;
}

inline void CMGApp::set_state(int32_t nState)
{
    m_nState = nState;
}

inline int32_t CMGApp::get_state(void)
{
    return m_nState;
}

inline void CMGApp::set_stop_timer(uint64_t qwServerTick)
{
    m_qwStopTimer = qwServerTick;
}

inline uint64_t CMGApp::get_stop_timer(void)
{
    return m_qwStopTimer;
}

inline int32_t CMGApp::get_frame(void)
{
    return m_nServerFrame;
}

inline int32_t CMGApp::get_tbus_addr(void)
{
	return m_stAppCtx.iId;
}

inline BOOL CMGApp::is_resume(void)
{
	return (m_stAppCtx.iStartMode == TAPP_START_MODE_RESUME);
}
    
inline std::list<CComponentModule*>& CMGApp::get_component_list(int32_t nOwnerType)
{
    return m_ComponentModuleList[nOwnerType];
}

#endif
