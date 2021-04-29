#ifndef _SERVER_COMPONENT_H_
#define _SERVER_COMPONENT_H_

#define MAX_COMPONENT_COUNT (128)

#include "server_module.h"

#define MG_REGISTER_COMPONENT_MODULE(__module_class__, __owner_type__, __component_type__, ...)         \
    class __module_class__##_auto_register                                                              \
    {                                                                                                   \
    public:                                                                                             \
        __module_class__##_auto_register()                                                              \
        {                                                                                               \
            CComponentModule* pCompModule = (CComponentModule*)__module_class__::create_instance<__module_class__>(#__module_class__, ##__VA_ARGS__); \
            pCompModule->set_owner_type(__owner_type__);                                                \
            pCompModule->set_component_type(__component_type__);                                        \
            CMGApp::instance().register_module(pCompModule);                                            \
        };                                                                                              \
    };                                                                                                  \
    static __module_class__##_auto_register __module_class__##Auto;


namespace google
{
	namespace protobuf {
		class Message;
	}
}

class CComponent
{
public:
    CComponent() {};
    virtual ~CComponent() {};

    virtual BOOL init(void* pOwner) = 0;
    virtual BOOL uninit(void) = 0;

    virtual void on_event_sync_data(void) = 0;

    virtual void mainloop(void) = 0;
    virtual void on_resume(void) = 0;

    virtual BOOL save_data(google::protobuf::Message* data) = 0;
    virtual BOOL load_data(const google::protobuf::Message* data) = 0;

protected:
    void*      m_pOwner;
};


class CComponentModule : public CServerModule
{
public:
    CComponentModule() {};
    virtual ~CComponentModule() {};

    virtual CComponent* new_component(uint64_t qwComponentID) = 0;
    virtual CComponent* find_component(uint64_t qwComponentID) = 0;
    virtual BOOL del_component(CComponent* pComponent) = 0;

    void set_owner_type(int32_t nOwnerType) { m_nOwnerType = nOwnerType; }
    int32_t get_owner_type(void) { return m_nOwnerType; }

    void set_component_type(int32_t nComponentType) { m_nComponentType = nComponentType; }
    int32_t get_component_type(void) { return m_nComponentType; }

private:
    int32_t     m_nOwnerType;
    int32_t     m_nComponentType;
};

class CComponentContainer
{
public:
    CComponentContainer() {};
    ~CComponentContainer() {};

    BOOL init();
    BOOL uninit();
    void mainloop();

    BOOL add_component(int32_t nComponentType, uint64_t qwComponentID, CComponent* pComponent);
    BOOL del_component(CComponent* pComponent);
    BOOL set_component(int32_t nComponentType, CComponent* pComponent);
    CComponent* get_component(int32_t nComponentType);
    uint64_t    get_component_id(int32_t nComponentType);

    int32_t get_component_count(void);

private:
    int32_t         m_nComponentCount;
    CComponent*     m_pComponent[MAX_COMPONENT_COUNT];
    uint64_t        m_qwComponentID[MAX_COMPONENT_COUNT];
};

#endif
