#ifndef _ACHIEVE_DEF_H_
#define _ACHIEVE_DEF_H_

#include "module/server_component.h"

class CAchieve : public CComponent
{
public:
    CAchieve() {};
    virtual ~CAchieve() {};

    virtual BOOL init(void* pOwner);
    virtual BOOL uninit(void);

    virtual void on_event_sync_data();

    virtual void mainloop(void) {};
    virtual void on_resume(void) {};

    virtual BOOL save_data(google::protobuf::Message* data) { return TRUE;  }
    virtual BOOL load_data(const google::protobuf::Message* data) { return TRUE;  }

    int32_t get_achieve_id(void) { return m_nAchieveID; }

private:
    int32_t     m_nAchieveID;
};

#endif
