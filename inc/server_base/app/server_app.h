#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

#include "module/server_module.h"
#include "define/server_def.h"
#include "define/str_def.h"

#define MG_REGISTER_MODULE(__server__, __module_class__, ...)  \
    __server__->register_module((CServerModule*)__module_class__::instance(#__module_class__, ##__VA_ARGS__)); 

typedef BOOL(*APP_FUNC)(TAPPCTX* pCtx, BOOL bResume);
typedef BOOL(*APP_FRAME_FUNC)();

typedef BOOL(*CONTROL_INIT_FUNC)(TAPPCTX* pCtx, void* pArg);
typedef BOOL(*CONTROL_FINI_FUNC)(TAPPCTX* pCtx, void* pArg);
typedef BOOL(*CONTROL_PRE_PROC_FUNC)(unsigned short argc, const char** argv);
typedef BOOL(*CONTROL_PROC_FUNC)(TAPPCTX *pCtx, void * pArg, unsigned short argc, const char** argv);
typedef const char*(*CONTROL_HELP_FUNC)(void);

typedef BOOL(*USER_MSG_HANDLER)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);

class CMGApp
{
public:
    CMGApp(void) {};
    ~CMGApp(void) {};

    BOOL init(const char* pcszServerName, int32_t argc, char* argv[]);
    void run_mainloop(void);
    void fini();

    inline static CMGApp& instance(void);

    BOOL set_app_func(APP_FUNC pInit, APP_FUNC pFini, APP_FRAME_FUNC pFrame, APP_FUNC pReload, APP_FUNC pStop);
    BOOL set_control_func(CONTROL_INIT_FUNC pInit, CONTROL_FINI_FUNC pFini, CONTROL_PRE_PROC_FUNC pPreProc, CONTROL_PROC_FUNC pProc, CONTROL_HELP_FUNC pHelp);

    inline void set_use_tconnd(BOOL bUseTconnd);
    inline BOOL get_use_tconnd(void);

    inline void set_use_router(BOOL bUseRouter);
    inline BOOL get_use_router(void);

    inline void set_state(int32_t nState);
    inline int32_t get_state(void);

    inline void set_stop_timer(uint64_t qwServerTick);
    inline int32_t get_stop_timer(void);
    
    inline int32_t get_frame(void);
    inline int32_t get_tbus_addr(void);
    inline BOOL is_resume(void);

    BOOL register_module(CServerModule* pModule);
    CServerModule* get_module(const char* pcszModuleName);
    
    BOOL set_user_msg_handler(int32_t nServerType, USER_MSG_HANDLER pMsgHandler);
    BOOL reload(void);

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
    int32_t m_nStopTimer;
    int32_t m_nServerFrame;
    TAPPCTX m_stAppCtx;

    uint32_t m_dwLastReloadTime;

    BOOL m_bNeedExitClean;
    BOOL m_bUseTconnd;
    BOOL m_bUseRouter;

    APP_FUNC pUserInit;
    APP_FUNC pUserFini;
    APP_FRAME_FUNC pUserFrame;
    APP_FUNC pUserReload;
    APP_FUNC pUserStop;

    USER_MSG_HANDLER m_UserMsgHandler[svrTotal + 1];
    CServerModuleContainer m_ModuleCont;

    static CMGApp ms_Instance;
};
    
inline CMGApp& CMGApp::instance()
{
    return ms_Instance;
}

inline void CMGApp::set_use_tconnd(BOOL bUseTconnd)
{
    m_bUseTconnd = bUseTconnd;
}
inline BOOL CMGApp::get_use_tconnd(void)
{
    return m_bUseTconnd;
}

inline void CMGApp::set_use_router(BOOL bUseRouter)
{
    m_bUseRouter = bUseRouter;
}

inline BOOL CMGApp::get_use_router(void)
{
    return m_bUseRouter;
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
    m_nStopTimer = qwServerTick;
}

inline int32_t CMGApp::get_stop_timer(void)
{
    return m_nStopTimer;
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

#endif