#ifndef _ACHIEVE_MODULE_H_
#define _ACHIEVE_MODULE_H_

#include "module/server_component.h"
#include "achieve/achieve.h"

class CAchieveModule : public CComponentModule 
{
public:
    CAchieveModule() {};
    virtual ~CAchieveModule() {};

    BOOL init(BOOL bResume);
    BOOL uninit(void);

    CComponent* new_component(uint64_t qwComponentID);
    CComponent* find_component(uint64_t qwComponentID);
    BOOL del_component(CComponent* pComponent);

private:
    CShmObjectPool<CAchieve, uint64_t>  m_AchievePool;
};

#endif
