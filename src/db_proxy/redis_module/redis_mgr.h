#ifndef _REDIS_MGR_H_
#define _REDIS_MGR_H_

#include "hiredis.h"
#include "async.h"
#include "define/redis_def.h"

class CRedisCli
{
public:
    CRedisCli() {};
    ~CRedisCli() {};

    BOOL init(const char* pcszName, const char* pcszHostName, 
            int32_t nPort, const char* pcszPassWord);
    BOOL uninit(void);

    void mainloop(void);

    BOOL async_connect(void);
    BOOL sync_connect(void);
    BOOL async_disconnect(void);
    BOOL sync_disconnect(void);

    BOOL command(int32_t nExecuteAddr, int32_t nCmdID, const char* pUserData, size_t dwDataLen, const char* format, ...);
    BOOL eval(int32_t nExecuteAddr, int32_t nCmdID, const char* pUserData, size_t dwDataLen, const char* format, ...);

    BOOL command_arg(int32_t nExecuteAddr, int32_t nCmdID, const char* pUserData, size_t dwDataLen, int32_t nArgc, const char** pArgv, const size_t * nArgvLen);
    BOOL eval_arg(int32_t nExecuteAddr, int32_t nCmdID, const char* pUserData, size_t dwDataLen, int32_t nArgc, const char** pArgv, const size_t* nArgvLen);

    inline std::string get_name(void);
    inline std::string get_host(void);
    inline int32_t get_port(void);
    inline int32_t get_state(void);
    inline int32_t get_connect_time(void);
    inline int32_t get_disconnect_time(void);
    
    BOOL reg_handler(int32_t nCmdID, REDIS_CMD_CALLBACK pCallBack);
    
public:
    BOOL on_connect(int32_t nStatus);
    BOOL on_disconnect(int32_t nStatus);
    BOOL on_auth(redisReply* pReply);
    BOOL on_pong(redisReply* pReply);
    BOOL on_command(redisReply* pReply, void* pCallBackData);

private:
    BOOL _pack_redis_reply(redisReply* pReply, char*& pPackedBuffer, int32_t& nPackedSize);
    
private:
    struct CALLBACK_DATA
    {
        int32_t     nFence;
        int32_t     nCmdID;
        int32_t     nTbusAddr;
        uint32_t    dwExpireTime;
        uint32_t    dwUserDataLen;
        char        szUserData[MAX_REDIS_CALLBACK_BUFFER_LEN];
    };

    BOOL _auth(void);
    BOOL _ping(void);
    CALLBACK_DATA* _prepare_callback_data(int32_t nExecuteAddr, int32_t nCmdID, const char* pUserData, size_t dwDataLen);
    BOOL _command_varlist(redisCallbackFn* pCallback, void* pPriData, const char* format, va_list valist);
    BOOL _eval_varlist(redisCallbackFn* pCallback, void* pPriData, const char* format, va_list valist);

private:
    std::string m_sClientName;
    std::string m_sHostName;
    int32_t     m_nPort;
    std::string m_sPassword;
    int32_t     m_nState;
    std::list<CALLBACK_DATA*>   m_CallBackList;

    redisAsyncContext* m_pCtx;

    int32_t     m_nPingSec;
    int32_t     m_nPongSec;
    int32_t     m_nConnectSec;
    int32_t     m_nDisConnectSec;

    REDIS_CMD_CALLBACK m_Handler[MAX_CMD_HANDLER_COUNT];
};

class CRedisCliMgr
{
public:
    CRedisCliMgr() {};
    ~CRedisCliMgr() {};

    BOOL init(BOOL bResume);
    BOOL uninit(void);
    inline static CRedisCliMgr& instance(void);

    void on_proc(void);
    void on_frame(void);

    CRedisCli* new_client(const char* pcszName, const char* pcszHost, int32_t nPort, const char* pcszPassWord);
    BOOL del_client(CRedisCli* pClient);
    CRedisCli* find_client(const char* pcszName);

    inline void* get_event_base(void);

private:
    static CRedisCliMgr ms_Instance;

    struct event_base*  m_pEventBase;
    std::list<CRedisCli*>   m_ClientList;
};

inline CRedisCliMgr& CRedisCliMgr::instance()
{
    return ms_Instance;
}

inline std::string CRedisCli::get_name(void)
{
    return m_sClientName;
}

inline std::string CRedisCli::get_host(void)
{
    return m_sHostName;
}

inline int32_t CRedisCli::get_port(void)
{
    return m_nPort;
}

inline int32_t CRedisCli::get_state(void)
{
    return m_nState;
}

inline int32_t CRedisCli::get_connect_time(void)
{
    return m_nConnectSec;
}

inline int32_t CRedisCli::get_disconnect_time(void)
{
    return m_nDisConnectSec;
}
    
inline void* CRedisCliMgr::get_event_base(void)
{
    return m_pEventBase;
}

#endif