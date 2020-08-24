#ifndef _CORO_STACKLESS_H_
#define _CORO_STACKLESS_H_

#include "linked_list/linked_list.h"
#include "define/str_def.h"

#include "time/time_mgr.h"
#include "config/global_config.h"

#define MAX_CORO_TYPE_COUNT     (32)
#define MAKE_CORO_ID(__server_addr__, __coro_mgr_index__, __coro_index__)       ((((uint64_t)__server_addr__) << 32) + (((uint64_t)__coro_index__) << 10) + __coro_mgr_index__)
#define GET_CORO_SERVER_ADDR(__coro_id__)                   (__coro_id__ >> 32)
#define GET_CORO_MGR_INDEX(__coro_id__)                     (__coro_id__ & 0x3FF)

enum CORO_STATE
{
    crsInvalid,

    crsReady,       // ready for execute
    crsStart,       // begin run
    crsRunning,     // yield, wait for resume
    crsStop,        // terminal for normal exit 
    crsFailed,      // execute failed for some reason, can see coro_return_code for detail

    crsTotal
};

enum CORO_RETURN_CODE
{
    crcInvalid          =       -1, 

    crcSuccess          =       0,
    crcTimeout          =       1,
    crcInternalError    =       2,   

    crcTotal
};

enum CORO_REPLY_TYPE
{
    crtInvalid,

    crtMsg,
    crtDB,

    crtTotal
};

#define CORO_BEGIN() switch(get_yield_id()) { case 0:
#define CORO_END() return crsStop; }

#ifdef __linux__
#define CORO_YIELD()								\
  do {											\
    return _yield_func(__COUNTER__ + 1);		\
    case __COUNTER__:; 							\
  } while(0);

#else 
#define CORO_YIELD()	         					\
  do {											\
    return _yield_func(__LINE__);	            \
    case __LINE__:; 							\
  } while(0);
#endif

struct CORO_REPLY
{
    int32_t     nReplyType;
    void*       pReplyData;
    int32_t     nReplyLen;
};

class CCoroStackless : public LINK_NODE
{
public: 
    CCoroStackless() {};
    virtual ~CCoroStackless() {};

    BOOL init(uint64_t qwCoroID);
    BOOL uninit();

    inline uint64_t get_coro_id(void);
    inline int32_t get_yield_id(void);
    inline uint32_t get_create_time(void);
    inline uint32_t get_yield_time(void);
    inline int32_t get_state(void);
    inline CORO_REPLY& get_coro_reply(void);
    inline int32_t get_coro_ret_code();

    virtual CORO_STATE coro_process() = 0;

    inline void set_coro_ret_code(int32_t nRetCode);
    inline void set_state(int32_t nState);
    inline void set_coro_reply(int32_t nReplyType, void* pReplyData, uint32_t dwDataLen);

protected:
    CORO_STATE _yield_func(int32_t nYieldID);

private:
    uint64_t    m_qwCoroID;
    int32_t     m_nYieldID;
    
    uint32_t    m_dwCreateTime;
    uint32_t    m_dwYieldTime;

    int32_t     m_nState;

    CORO_REPLY  m_Reply;
    int32_t     m_nCoroRetCode;         // 协程的状态码, 描述当前状态的具体信息
};

template<class T>
class CCoroStacklessMgr
{
public:
    CCoroStacklessMgr() ;
    ~CCoroStacklessMgr() {};

    virtual BOOL init(int32_t nShmType, BOOL nInitCoroCount, BOOL bResume);
    virtual BOOL uninit(void);
    virtual void mainloop(void);

    inline static CCoroStacklessMgr instance(void);
    
    T* new_coro(void);
    virtual T* find_coro(uint64_t qwCoroID);
    BOOL del_coro(T* pCoro);

    BOOL start_coro(CCoroStackless* pCoro, BOOL bImmediate = TRUE);
    virtual BOOL resume_coro(CCoroStackless* pCoro);

private:
    BOOL _on_coro_process_run(CCoroStackless* pCoro, int32_t nReturnState);

private:
    static CCoroStacklessMgr<T>     ms_Instance;
    CShmObjectPool<T, uint64_t>     m_CoroPool;
    int32_t                         m_nMgrIndex;

    LINK_HEAD                       m_ReadyLinkHead;         //初始化的协程列表
    LINK_HEAD                       m_RunLinkHead;           //按照上次yeild时间顺序的运行列表
    LINK_HEAD                       m_DeleteLinkHead;        //按照时间顺序的删除列表
};

struct SHM_TYPE_INFO
{
    char    szMgrName[COMMON_NAME_LEN];
    int32_t nShmType;
};

struct CORO_MGR_DATA
{
    char    szMgrName[COMMON_NAME_LEN];
    void*   pMgr;
};

struct GLOBAL_STACKLESS_MGR_DATA
{
    int32_t         nShmTypeInfoCount;
    SHM_TYPE_INFO   stShmTypeInfo[MAX_CORO_TYPE_COUNT];
    int32_t         nCoroIDGenerator;
    int32_t         nShmTypeGenerator;
};

class CGlobalStacklessMgr
{
public:
    CGlobalStacklessMgr() {};
    ~CGlobalStacklessMgr() {};

    BOOL init(int32_t nServerAddr, BOOL bResume);
    BOOL uninit(void);
    void mainloop(void);

    inline static CGlobalStacklessMgr& instance(void);

    inline void set_curr_coro(CCoroStackless* pCoro);
    inline CCoroStackless* get_curr_coro(void);

    BOOL add_coro_stackless_mgr(const char* pcszMgrName, void* pMgr);
    uint64_t generate_coro_id(int32_t nMgrIndex);

    CCoroStackless* get_coro(uint64_t qwCoroID);
    BOOL resume_coro(CCoroStackless* pCoro);
    
private:
    int32_t _get_shm_type_by_mgr_name(const char* pcszMgrName);

private:
    static CGlobalStacklessMgr*             ms_Instance;

    int32_t                                 m_nServerAddr;
    CCoroStackless*                         m_pCurrCoro;
    CShmObject<GLOBAL_STACKLESS_MGR_DATA>   m_MgrData;

    std::vector<CORO_MGR_DATA>              m_CoroStacklessMgrList;
};

#include "coro_stackless_inl.h"

#endif
