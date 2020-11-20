#ifndef _ACHIEVE_DEF_H_
#define _ACHIEVE_DEF_H_

#include "module/role_sub_module.h"

class CAchieve : public CRoleSubModule
{
public:
    CAchieve() {};
    virtual ~CAchieve() {};

    static BOOL module_init(BOOL bResume);
    static BOOL module_uninit(void);
    static BOOL init_msg_handler(void);

    virtual BOOL init(CRole* pRole);
    virtual BOOL uninit(void);

    virtual void on_event_sync_data();

    virtual void mainloop(void) {};
    virtual void on_resume(void) {};

    virtual BOOL save_data(ROLE_DB::ROLE_DATA& data) { return TRUE;  };
    virtual BOOL load_data(const ROLE_DB::ROLE_DATA& data) { return TRUE;  };

    int32_t get_achieve_id(void) { return m_nAchieveID; }

private:
    int32_t     m_nAchieveID;
};

#endif