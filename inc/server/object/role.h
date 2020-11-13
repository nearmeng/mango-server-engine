#ifndef _ROLE_H_
#define _ROLE_H_

#include "define/str_def.h"
#include "module/role_sub_module.h"
#include "role_sub_module/achieve.h"

namespace ROLE_DB
{
    class ROLE_BASE_DATA;
}

#define MAX_ROLE_SUB_MODULE_COUNT   (50)

typedef BOOL(*INIT_MSG_HANDLER_FUNC)(void);

enum ROLE_SUB_MODULE_TYPE
{
    rsmtInvalid,

    rsmtAchieve,

    rsmtTotal
};

class CRole
{
public:
    CRole() {};
    ~CRole() {};

    BOOL init(uint64_t qwObjID);
    BOOL uninit(void);
    
    static BOOL init_msg_handler(void);

    void mainloop(void);
    void on_resume(void);
    
    BOOL save(char* pData, uint32_t &dwSize, char* pBaseData, uint32_t &dwBaseDataSize);
    BOOL load(const char* pData, uint32_t dwSize);

    inline uint64_t get_obj_id(void);

    inline void set_name(const char* pcszName);
    inline const char* get_name(void);

    inline void set_level(int32_t nLevel);
    inline int32_t get_level(void);

    inline void set_create_time(uint32_t dwCreateTime);
    inline uint32_t get_create_time(void);

    inline void set_save_time(uint32_t dwSaveTime);
    inline uint32_t get_save_time(void);

    inline void set_login_time(uint32_t dwLoginTime);
    inline uint32_t get_login_time(void);

    inline void set_session_id(uint64_t qwSessionID);
    inline uint64_t get_session_id(void);

    inline void set_user_id(uint64_t qwUserID);
    inline uint64_t get_user_id(void);

private:
    BOOL _save_base_data(ROLE_DB::ROLE_BASE_DATA& BaseData);
    BOOL _load_base_data(const ROLE_DB::ROLE_BASE_DATA& BaseData);

private:
    uint64_t    m_qwObjID;
    uint64_t    m_qwUserID;
    uint64_t    m_qwSessionID;

    uint32_t    m_dwCreateTime;
    uint32_t    m_dwSaveTime;
    uint32_t    m_dwLoginTime;

    char        m_szName[COMMON_NAME_LEN];
    int32_t     m_nLevel;
    
    static int32_t m_nSubModuleOffset[rsmtTotal];
    static INIT_MSG_HANDLER_FUNC m_pSubModuleMsg[rsmtTotal];

    REG_ROLE_SUB_MODULE(CAchieve, achieve, rsmtAchieve);
};

#include "object/role_inl.h"

#endif
