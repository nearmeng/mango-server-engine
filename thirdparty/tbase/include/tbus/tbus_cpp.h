/*
 * Copyright (c) 2013 Tencent all rights reserved
 *
 * date:   2015��1��4��
 * file:   tbus_cpp.h
 * desc:   tbus api��c++��װ
 */

#ifndef TBUS_CPP_H_
#define TBUS_CPP_H_
#include <string>
#include "tbus.h"


/** @defgroup CPP_API C++�ӿ�
 * @{
 */


namespace tsf4g_tbus
{

/**
 * @brief �ַ�����ʽ�������ַ
 */
const char BUS_ADDR_ALL[] = "all";          //ȫ��
const char BUS_ADDR_DEFAULT[] = "default";  //Ĭ��

/**
 * @brief busȫ�ֻ������ܰ�װ�࣬�����ķ������Ǿ�̬����
 */
class BusEnv
{
public:

/**
 * @brief ��ʼ����ͨģʽ��tbus
 * @see tbus_init_ex
 * @param[in] shm_key GCIM�����ڴ��key
 * @param[in] flag ���ܱ�ǣ���tbus_init_ex
 * @param[in] business_id ҵ��id
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int InitNormalModeBusEnv(IN const char* shm_key, IN int flag, IN int business_id=0);

/**
 * @brief ��ʼ���Զ���ģʽ��tbus
 * @see tbus_automatic_init
 * @param[in] tbusd_url tbusd�ı��ع���url
 * @param[in] flag ���ܱ��
 * @param[in] timeout �� tbusd�Ľ�����ʱ����λ����
 * @param[in] business_id ҵ��id
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int InitAutomaticModeBusEnv(IN const char* tbusd_url, IN int flag, IN int time_out,
                            IN int business_id=0);

/**
 * @brief ����tbus�������ͷ���Դ
 * @see tbus_fini
 */
static
void FiniBusEnv(void);

/**
 * @brief �����Զ���ģʽ�º�tbusd�Ľ�����ʱ
 * @see tbus_set_opt
 * @param[in] timeout ��ʱʱ�䣬��λ����
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int SetOptAutomaticTbusdTimeout(int timeout);

/**
 * @brief ������־���
 * @see tbus_set_logcat
 * @param[in] logcat tlog�ľ��ʵ��
 */
static
void SetLogCat(IN LPTLOGCATEGORYINST logcat);

/**
 * @brief ������־����
 * @see tbus_set_logpriority
 * @param[in] priority tlog��־����
 */
static
void SetLogPriority(IN int priority);

/**
 * @brief ����zookeeper apiʹ�õ���־�ļ�
 * @see tbus_automatic_set_zookeeper_log_file
 * @param[in] log_file ��־�ļ�·��
 * @retval 0  �ɹ�
 * @retval !0 ʧ��
 */
static
int SetZookeeperLogFile(IN const char* log_file);

/**
 * @brief �Զ���ģʽ��ע��ͨ��
 * @see tbus_automatic_regist_relations
 * @param[in] relations Ҫע���ͨ����Ϣ(���˵�ַ�ʹ�С)
 * @param[in] relations_num Ҫע���ͨ������
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int AutomaticRegistRelations(IN const AddrRelation* relations, IN int relations_num);

/**
 * @brief �Զ���ģʽ��ע��ͨ��
 * @see tbus_automatic_regist_relations_by_str
 * @param[in] relations Ҫע���ͨ����Ϣ(���˵�ַ�ʹ�С)
 * @param[in] relations_num Ҫע���ͨ������
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int AutomaticRegistRelations(IN const AddrRelationStr* relations, IN int relations_num);

/**
 * @brief ���ַ�����ʽ��tbus��ַת��Ϊ������ʽ
 * @see tbus_addr_aton
 * @param[in] str_addr  Ҫת����tbus��ַ
 * @param[out] bus_addr ת��Ϊ������tbus��ַ
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int AddrAToN(IN const char* str_addr, OUT TBUSADDR* bus_addr);

/**
 * @brief ��������ʽ��tbus��ַת��Ϊ�ַ�����ʽ
 * @see tbus_addr_ntoa
 * @param[in] bus_addr Ҫת����tbus��ַ
 * @retval �ַ�����ʽ��tbus��ַ, ʧ�ܻ᷵��"", Ҳ���ǿմ�
 */
static
const char* AddrNToA(IN TBUSADDR bus_addr);

/**
 * @brief ��������ʽ��tbus��ַת��Ϊ�ַ�����ʽ���ָ����buffer��
 * @see tbus_addr_ntop
 * @param[in] bus_addr       Ҫת����tbus��ַ
 * @param[out] str_addr_buff ���ת�������buffer
 * @param[in] buff_len       str_addr_buff�ĳ���
 * @retval str_addr_buff�ĵ�ַ, ʧ�ܻ᷵��""��Ҳ���ǿմ�
 */
static
char* AddrNToP(IN TBUSADDR bus_addr, OUT char* str_addr_buff, IN unsigned int buff_len);

/**
 * @brief ��ȡ��Ϣ��Ⱦɫid
 * @see tbus_get_dyedmsgid
 * @retval Ⱦɫid
 */
static
unsigned int GetDyedMsgId(void);

/**
 * @brief ����tbusϵͳʹ�õĵ�ǰʱ��
 * @see tbus_set_current_time
 * @param[in] current_time ��ǰʱ��
 * @retval 0  �ɹ�
 * @retval <0 ʧ��
 */
static
int SetCurrentTime(IN const struct timeval* current_time);

/**
 * @brief ��tbusϵͳʹ���û��ṩ��ϵͳʱ��
 * @see tbus_use_saved_time
 * @retval ʼ��Ϊ0
 */
static
int UseSavedTime(void);

/**
 * @brief ��tbusϵͳʹ��gettimeofday����ȡϵͳʱ��
 * @see tbus_use_sys_time
 * @retval ʼ��Ϊ0
 */
static
int UseSysTime(void);

/**
 * @brief ��ȡ�������Ӧ�Ĵ�����Ϣ
 * @see tbus_error_string
 * @retval ������Ϣ�ַ����ĵ�ַ
 */
static
const char* GetErrorStr(int err_code);

};

/**
 * @brief tbus handle�İ�װ��
 */
class BusHandle
{
public:
    /**
     * @brief Ĭ�Ϲ��캯���������ڲ�����tbus_new����һ���������ownΪtrue,
     *        ����������set_handleʱ�����tbus_delete�ͷ�������
     */
    BusHandle(void);
    /**
     * @brief ʹ���ⲿ�����tbus handle�Ĺ��캯��
     *        own��ʾ�Ƿ��ɱ�BusHandle���ʵ���ʱ��(������������set_handle)����tbus_delete�ͷ����handle
     * @param[in] handle �ⲿ�Ѿ�ʹ��tbus_new��õ�tbus���
     * @param[in] own �Ƿ���ҪBusHandle������ʱ��handle����tbus_delete
     */
    BusHandle(IN int handle, IN bool own);
    virtual ~BusHandle();

public:
    /**
     * @brief ��һ����ַ����Ӧtbus_bind_by_str
     * @note  ��tbus_bind_by_str��ͬ���ǣ�Ϊ�˼򻯸����API������һ��BusHandleֻ����Bindһ����ַ,
     *        �����ҪBind�����ַ����ô���Կ���ʹ�ö��BusHandle������ʹ��c�汾��tbus api
     * @see tbus_bind_by_str
     * @param[in] addr Ҫ�󶨵ĵ�ַ
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int Bind(IN const char* addr);
    /**
     * @brief ��һ����ַ����Ӧtbus_bind
     * @note  ��tbus_bind��ͬ���ǣ�Ϊ�˼򻯸����API������һ��BusHandleֻ����Bindһ����ַ,
     *        �����ҪBind�����ַ����ô���Կ���ʹ�ö��BusHandle������ʹ��c�汾��tbus api
     * @see tbus_bind
     * @param[in] addr Ҫ�󶨵ĵ�ַ
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int Bind(IN TBUSADDR addr);
    /**
     * @brief ��ռģʽ��ָ��һ��Ҫͨ�ŵĶԶ˵�ַ, �Ƕ�ռģʽ�������壬��Ӧtbus_connect_by_str
     * @see tbus_connect_by_str
     * @param[in] peer_addr �Զ˵�ַ
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int Connect(IN const char* peer_addr);
    /**
     * @brief ��ռģʽ��ָ��һ��Ҫͨ�ŵĶԶ˵�ַ, �Ƕ�ռģʽ�������壬��Ӧtbus_connect
     * @see tbus_connect
     * @param[in] peer_addr �Զ˵�ַ
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int Connect(IN TBUSADDR peer_addr);
    /**
     * @brief ��ָ��Ŀ���ַ����һ����Ϣ, ��Ӧtbus_send
     * @note  ���tbus_send, ����ӿ�û�д�src������ΪBusHandle��ֻ����Bindһ����ַ��src����Bind�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_send
     * @param[in] msg Ҫ���͵���Ϣ
     * @param[in] msg_len msg�ĳ���
     * @param[in] peer_addr Ŀ�ĵ�ַ���ַ�����ʽ
     * @param[in] flag ���ܱ�ǣ���tbus_send��˵��
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int SendTo(IN const void* msg, IN size_t msg_len, IN const char* peer_addr, int flag=0);
    /**
     * @brief ��ָ��Ŀ���ַ����һ����Ϣ, ��Ӧtbus_send
     * @note  ���tbus_send, ����ӿ�û�д�src������ΪBusHandle��ֻ����Bindһ����ַ��src����Bind�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_send
     * @param[in] msg Ҫ���͵���Ϣ
     * @param[in] msg_len msg�ĳ���
     * @param[in] peer_addr Ŀ�ĵ�ַ��������ʽ
     * @param[in] flag ���ܱ�ǣ���tbus_send��˵��
     * @retval 0  �ɹ�
     * @retval <0 ʧ�ܣ�ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int SendTo(IN const void* msg, IN size_t msg_len, IN TBUSADDR peer_addr, int flag=0);
    /**
     * @brief �����buffer�е�������Ϊһ����Ϣ���ͳ�ȥ
     * @note  ���tbus_sendv, ����ӿ�û�д�src������ΪBusHandle��ֻ����Bindһ����ַ��src����Bind�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_sendv
     */
    int SendVTo(IN const struct iovec* vec, IN const int vec_num, IN const char* peer_addr,
                IN int flag=0);
    /**
     * @brief �����buffer�е�������Ϊһ����Ϣ���ͳ�ȥ
     * @note  ���tbus_sendv, ����ӿ�û�д�src������ΪBusHandle��ֻ����Bindһ����ַ��src����Bind�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_sendv
     */
    int SendVTo(IN const struct iovec* vec, IN const int vec_num, IN TBUSADDR peer_addr,
                IN int flag=0);
    /**
     * @brief ��ָ��Ŀ�ĵ�ַ����һ����Ϣ����Ӧtbus_recv
     * @note  ���tbus_recv, ����ӿڲ���ֻ��һ��peer_addr������ΪBusHandle��ֻ����Bindһ����ַ��Bind�ĵ�ַ������Ϣ��Ŀ�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_recv
     * @param[out]      recv_buffer ���������յ���Ϣ���ڴ�buffer
     * @param[in,out]   recv_buffer_len ����-recv_buffer�ĳ���
     *                                  ���-�յ���Ϣ�ĳ���
     * @param[in,out]   peer_addr ����-ָ��ͨ�ŶԶ˵ĵ�ַ, ֧�������ַ(�㲥��Ĭ��)
     *                            ���-��Ϣ�ľ�����Դ��ַ
     * @param[in]       peer_addr_len peer_addr�Ļ���������
     * @param[in]       flag ���ܱ�ǣ���tbus_recv
     * @retval 0 �յ�����Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int RecvFrom(OUT void* recv_buffer, INOUT size_t* recv_buffer_len,
                 INOUT char* peer_addr, IN size_t peer_addr_len, int flag=0);
    /**
     * @brief ��ָ��Ŀ�ĵ�ַ����һ����Ϣ����Ӧtbus_recv
     * @note  ���tbus_recv, ����ӿڲ���ֻ��һ��peer_addr������ΪBusHandle��ֻ����Bindһ����ַ��Bind�ĵ�ַ������Ϣ��Ŀ�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_recv
     * @param[out]      recv_buffer ���������յ���Ϣ���ڴ�buffer
     * @param[in,out]   recv_buffer_len ����-recv_buffer�ĳ���
     *                                  ���-�յ���Ϣ�ĳ���
     * @param[in,out]   peer_addr ����-ָ��ͨ�ŶԶ˵ĵ�ַ, ֧�������ַ(�㲥��Ĭ��)
     *                            ���-��Ϣ�ľ�����Դ��ַ
     * @param[in]       flag ���ܱ�ǣ���tbus_recv
     * @retval 0 �յ�����Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int RecvFrom(OUT void* recv_buffer, INOUT size_t* recv_buffer_len,
                 INOUT TBUSADDR* peer_addr, int flag=0);
    /**
     * @brief ������ͨ���л�ȡ������ͷ����һ����Ϣ�Ĵ洢λ�úʹ���Ϣ�ĳ�����Ϣ, ��Ҫ�ֹ�����DeleteMsg�Ż�Ӷ�����ɾ��������Ϣ(RecvFrom���Զ�ɾ��)
     * @note  ���tbus_peek_msg, ����ӿڲ���ֻ��һ��peer_addr������ΪBusHandle��ֻ����Bindһ����ַ��Bind�ĵ�ַ������Ϣ��Ŀ�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_peek_msg
     * @retval 0 ȡ������Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int PeekMsg(INOUT const char** data, OUT size_t* data_len, INOUT char* peer_addr, size_t peer_addr_len,
                IN int flag=0);
    /**
     * @brief ������ͨ���л�ȡ������ͷ����һ����Ϣ�Ĵ洢λ�úʹ���Ϣ�ĳ�����Ϣ, ��Ҫ�ֹ�����DeleteMsg�Ż�Ӷ�����ɾ��������Ϣ(RecvFrom���Զ�ɾ��)
     * @note  ���tbus_peek_msg, ����ӿڲ���ֻ��һ��peer_addr������ΪBusHandle��ֻ����Bindһ����ַ��Bind�ĵ�ַ������Ϣ��Ŀ�ĵ�ַ���Ӷ����˽ӿ�
     * @see tbus_peek_msg
     * @retval 0 ȡ������Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int PeekMsg(INOUT const char** data, OUT size_t* data_len, INOUT TBUSADDR* peer_addr, IN int flag=0);
    /**
     * @brief ɾ��"peer_addr->�󶨵�ַ"ͨ���ϵĵ�һ����Ϣ, ͨ����PeekMsg��ʹ��
     * @see tbus_delete_msg
     * @retval 0 ɾ������Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int DeleteMsg(const char* peer_addr);
    /**
     * @brief ɾ��"peer_addr->�󶨵�ַ"ͨ���ϵĵ�һ����Ϣ, ͨ����PeekMsg��ʹ��
     * @see tbus_delete_msg
     * @retval 0 ɾ������Ϣ
     * @retval <0 ����(Ҳ������ͨ����û����Ϣ, ��ʱ����TBUS_ERR_CHANNEL_EMPTY), ʹ��BusEnv::GetErrorStr��ȡ������Ϣ
     */
    int DeleteMsg(TBUSADDR peer_addr);
    /**
     * @brief ���ָ��ͨ��"peer_addr->�󶨵�ַ"����������
     * @see tbus_empty_channel_recvside
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int EmptyRecvSide(const char* peer_addr);
    /**
     * @brief ���ָ��ͨ��"peer_addr->�󶨵�ַ"����������
     * @see tbus_empty_channel_recvside
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int EmptyRecvSide(TBUSADDR peer_addr);
    /**
     * @brief ���ָ��ͨ��"�󶨵�ַ->peer_addr"����������
     * @see tbus_empty_channel_sendside
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int EmptySendSide(const char* peer_addr);
    /**
     * @brief ���ָ��ͨ��"�󶨵�ַ->peer_addr"����������
     * @see tbus_empty_channel_sendside
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int EmptySendSide(TBUSADDR peer_addr);
    /**
     * @brief ˢ�¾��, ��Ӧtbus_refresh_handle
     * @see tbus_refresh_handle
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int Refresh();

public:
    /**
     * @brief ��handle����Ϊ��ռģʽ,��Ϊ��ѡ����ֻ�ܴ򿪣��޷��رյģ������������û�в���
     * @see tbus_set_handle_opt
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int SetOptTurnOnExclusiveMode();
    /**
     * @brief ��/�ر���Ϣʱ������ܣ���tbus.h-TBUS_HANDLE_OPT_NAME_CHANNELS_TIMESTAMP
     * @see tbus_set_handle_opt
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int SetOptWithTimeStamp(IN bool turn_on);
    /**
     * @brief �Ѿ������Ϊ����Ӧģʽ����tbus.h-TBUS_HANDLE_OPT_NAME_ADAPTIVE_BINDING
     * @see tbus_set_handle_opt
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int SetOptAdaptiveMode(IN bool turn_on);

public:
    /**
     * @brief ��ȡ�յ�����һ����Ϣ��ʱ���
     * @see tbus_get_msg_timestamp
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int GetMsgTimeStamp(OUT LPTBUSTIMESTAMP time_stamp);

public:
    /**
     * @brief ��ȡԭʼbus handle
     * @retval tbus handle��ԭʼֵ
     */
    inline
    int handle(){ return handle_; }
    /**
     * @brief ����ԭʼbus handle
     * @note  �����ǰown_��true, ��ô���ȵ���tbus_delete�ͷ�handle_, �ٰ�handle_����Ϊ�µ�handle
     * @param[in] handle �и�BusHandle�����ԭʼtbus���
     * @param[in] own    �Ƿ���BusHandle�ฺ��handle���ͷ�
     * @retval 0  �ɹ�
     * @retval <0 ʧ��
     */
    int set_handle(IN int handle, IN bool own);

private:
    void SetLocalAddr(int handle);

private:
    int handle_;
    bool own_;
    TBUSADDR local_addr_;
    std::string local_addr_str_;
};


}

#endif /* TBUS_CPP_H_ */
