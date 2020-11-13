#ifndef _ROLE_MODULE_H_
#define _ROLE_MODULE_H_

#include "object/role.h"
#include "module/server_module.h"
#include "shm/shm_pool.h"

#define MAX_ROLE_DATA_SIZE       (500 * 1024)

typedef void(*ROLE_MSG_HANDLER)(CRole* pRole, const CS_HEAD* pHead, const google::protobuf::Message* pMsg);

class CRoleModule : public CServerModule
{
public:
    CRoleModule() {};
    ~CRoleModule() {};

    virtual BOOL init(BOOL bResume);
    virtual BOOL uninit(void);
    virtual void mainloop(void);

    inline CRole* create_role(uint64_t qwRoleID);
    inline BOOL destroy_role(CRole* pRole);
    inline CRole* find_role(uint64_t qwRoleID);
    inline int32_t get_role_count(void);

    static BOOL reg_msg_handler(int32_t nMsgID, ROLE_MSG_HANDLER pHandler);
    static ROLE_MSG_HANDLER get_msg_handler(int32_t nMsgID);

private:
    struct TRAVERSE_ROLE_RESUME
    {
        BOOL operator()(uint64_t qwRoleID, CRole* pRole);
    };

private:
    CShmObjectPool<CRole, uint64_t>     m_RolePool;
    std::map<int32_t, ROLE_MSG_HANDLER> m_MsgHandler;
};

inline CRole* CRoleModule::create_role(uint64_t qwRoleID)
{
    return m_RolePool.new_object(qwRoleID);
}

inline BOOL CRoleModule::destroy_role(CRole* pRole)
{
    return m_RolePool.delete_object(pRole);
}

inline CRole* CRoleModule::find_role(uint64_t qwRoleID)
{
    return m_RolePool.find_object(qwRoleID);
}

inline int32_t CRoleModule::get_role_count()
{
    return m_RolePool.get_used_count();
}

#define REG_ROLE_MSG_HANDLER(__msgid__, __handler__)             (CRoleModule::reg_msg_handler(__msgid__, __handler__))

#endif
