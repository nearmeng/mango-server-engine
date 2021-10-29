#pragma once

#include "shm_log.h"
class ShmConfig
{
    bool m_Enabled = false;
    bool m_AlreadySet = false;
    ShmConfig()
    {
        SHMLOG_INFO("ShmConfig ctor at address %p",  this);
    }
    void* Addr()
    {
        return this;
    }
    ShmConfig(const ShmConfig& other) = delete;
public:
    
    static ShmConfig& Instance()
    {
        static ShmConfig the_one;
        return the_one;
    }
    static bool IsEnabled()
    {
        return Instance().m_Enabled;
    }
    static bool SetEnable(bool enable)
    {
        ShmConfig& cur_instance = Instance();
        if(cur_instance.m_AlreadySet)
        {
            return false;
        }
        SHMLOG_INFO("SetEnable with flag %d at address %p", int(enable), cur_instance.Addr());
        cur_instance.m_Enabled = enable;
        cur_instance.m_AlreadySet = true;
        return true;
    }
};