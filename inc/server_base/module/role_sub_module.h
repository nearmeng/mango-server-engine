#ifndef _ROLE_SUB_MODULE_H_
#define _ROLE_SUB_MODULE_H_

class CRole;
namespace ROLE_DB
{
    class ROLE_DATA;
}

template<int N>
class CRoleSubModuleParam
{
};


#define  REG_ROLE_SUB_MODULE(__module_class__, __module_name__, __module_enum__)        \
    public:                                                                             \
        inline __module_class__&  ##__module_name__()                                   \
        { return m_##__module_name__; };                                               \
    private:                                                                            \
        __module_class__    m_##__module_name__;                                        \
        class __module_class__##Register {                                              \
            public:                                                                     \
                __module_class__##Register()                                            \
                {                                                                       \
                    if(CRole::m_nSubModuleOffset[__module_enum__] == 0)                 \
                    {                                                                   \
                        CRole::m_nSubModuleOffset[__module_enum__] = offsetof(CRole, m_##__module_name__);   \
                        CRole::m_pSubModuleMsg[__module_enum__] = &__module_class__::init_msg_handler;        \
                        CRole::m_pSubModuleInit[__module_enum__] = &__module_class__::module_init;        \
                        CRole::m_pSubModuleUnInit[__module_enum__] = &__module_class__::module_uninit;        \
                    }                                                                   \
                };                                                                      \
        };                                                                              \
        __module_class__##Register m_##__module_class__##Register;               


class CRoleSubModule
{
public:
    CRoleSubModule() {};
    virtual ~CRoleSubModule() {};

    virtual BOOL init(CRole* pRole) = 0;
    virtual BOOL uninit(void) = 0;

    virtual void on_event_sync_data(void) = 0;

    virtual void mainloop(void) = 0;
    virtual void on_resume(void) = 0;

    virtual BOOL save_data(ROLE_DB::ROLE_DATA& data) = 0;
    virtual BOOL load_data(const ROLE_DB::ROLE_DATA& data) = 0;

protected:
    CRole*      m_pRole;
};

#endif