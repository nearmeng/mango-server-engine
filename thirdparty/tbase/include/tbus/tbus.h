/** @file $RCSfile: tbus.h,v $
  Invoked function header file for tsf4g-tbus module
  $Id: tbus.h,v 1.24 2009-09-14 10:53:48 jacky Exp $
  @author $Author: jacky $
  @date $Date: 2009-09-14 10:53:48 $
  @version $Revision: 1.24 $
  @note Editor: Vim 6.3, Gcc 4.0.2, tab=4
  @note Platform: Linux
  */


#ifndef _TBUS_H
#define _TBUS_H

/* automatically include the correct library on windows */
#if defined(_WIN32)|| defined(_WIN64)


#ifdef TSF4G_SMALL_LIB
#undef _TSF4G_LARGE_LIB_
#endif


#ifdef _TSF4G_LARGE_LIB_
#ifdef _DEBUG
#pragma comment(lib, "libtsf4g_d.lib")
#else
#pragma comment(lib, "libtsf4g.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "libtbus_d.lib")
#else
#pragma comment(lib, "libtbus.lib")
#endif
#endif



#endif /*#if defined(_WIN32)|| defined(_WIN64)*/

#include <stdio.h>
#include "pal/pal.h"
#include "tlog/tlog.h"
#include "tbus/tbus_macros.h"
#include "tbus/tbus_error.h"



/** @name ������������/�������
 * @{
 */
/*��ʾ�ò���ֻ���������*/
#ifndef IN
#define IN
#endif


/*��ʾ�ò���ֻ���������*/
#ifndef OUT
#define OUT
#endif


/*��ʾ�ò���������������������������*/
#ifndef INOUT
#define INOUT
#endif

/** @}*/ // ������������/�������


/* TbusFlag*/
enum tagTbusFlag
{
    TBUS_FLAG_START_DYE_MSG = 1,     /* ָʾtbusϵͳ�Դ���Ϣ����Ⱦɫ */
    TBUS_FLAG_KEEP_DYE_MSG = 2,      /* ָʾtbusϵͳ����ǰһ����Ϣ��Ⱦɫ��Ϣ */
    TBUS_FLAG_SET_TEST_MSG = 4,      /* ָʾtbusϵͳ�����ݰ�Ϊ�������ݰ� */
    TBUS_FLAG_WITH_TIMESTAMP = 8,    /* ָʾЯ��ʱ�����Ϣ */
    TBUS_FLAG_USE_GETTIMEOFDAY = 16, /* ָʾʹ��gettimeofdayȡ�õ�ʱ����Ϊʱ���ֵ */
	TBUS_FLAG_USE_NOTIFY = 32,       /* ָʾʹ���¼����Ʒ���һ��֪ͨ��Ϣ���Զ� */
};

/* initialize flags*/
enum tagTbusInitFlag
{
    TBUS_INIT_FLAG_NONE    =    0x00000000,     /*��������*/
    TBUS_INIT_FLAG_USE_SAVED_TIME = 0x00000001, /*��¼ʱ���ֵʱʹ��tbus_set_current_time���õ�ʱ��ֵ*/
};

/*channel mode initialize flags*/
enum tagTbusChannelModeInitFlag
{
    TBUS_CHANNEL_MODE_INIT_FLAG_NONE    =    0x00000000,        /*��������*/

    /*
     * ��ȱʡ��־���ܣ���channel���ý��е��Ը��٣�
     * �˱�־λֻ�ʺϵ��߳���ֻ�ǵ�������ʱʹ�ã�
     * ��ʽ��Ӫ���ܴ򿪴˱�־λ
     */
    TBUS_CHANNEL_MODE_INIT_FLAG_ENABLE_TRACE = 0x00000001,
};

/* tbus global option */
enum tagTbusOptionName
{
    /* ��tbusd����ʱ�ĳ�ʱʱ��  */
    TBUS_OPT_NAME_TBUSD_TIMEOUT     = 1,
    /* ��̬ģʽ�£�ͨ�������ڲ��Զ����� */
    TBUS_OPT_NAME_NONAUTO_CREATED   = 2,
};
typedef enum tagTbusOptionName TBUSOPTIONNAME;
typedef enum tagTbusOptionName *LPTBUSOPTIONNAME;

/* tbus handle option */
enum tagTbusHandleOptionName
{
    /* �����˴�ѡ��󣬾��ֻ�ܽ���tbus_connectָ�������ͨ�� */
    TBUS_HANDLE_OPT_NAME_EXCLUSIVE_CHANNELS = 1,
    /* ͨ�����ô�ѡ�ָʾ����ͨ����������ʱЯ��ʱ�����Ϣ */
    TBUS_HANDLE_OPT_NAME_CHANNELS_TIMESTAMP = 2,
    /* ͨ�����ô�ѡ�������Զ�̬���䣬���贴��ͨ��Ҳ�ɰ󶨳ɹ� */
    TBUS_HANDLE_OPT_NAME_ADAPTIVE_BINDING = 3,
};
typedef enum tagTbusHandleOptionName    TBUSHANDLEOPTIONAME;
typedef enum tagTbusHandleOptionName    *LPTBUSHANDLEOPTIONAME;

enum tagTbusPeerStateType
{
    TBUS_PEER_STATE_UNAVAILABLE = 0x00,
    TBUS_PEER_STATE_AVAILABLE   = 0x01,
};

#define TBUS_HAVE_MESSAGE                            1     /* ��ʾ����Ϣ���Խ���*/

#define TBUS_ADDR_DEFAULT                            0     /*  ��ʾ�����һ����ַ*/
#define TBUS_ADDR_ALL                                -1     /* ��ʾ�������е�ַ */


#define TBUS_LOGCAT_NAME    "tsf4g.tbus"        /*tbus��־������*/

#define TBUS_DEFAULT_GCIM_KEY   "1688"    /*default share memory key for GCIM(Global Channel Infomation Map)*/
#define TBUS_DEFAULT_GRM_KEY    "2688"    /*default share memory key for GRM(Global Route Map)*/
#define TBUS_DEFAULT_MAP_KEY    "3688"    /*default share memory key for tbus address map */

#define TBUS_ROUTE_GLOBAL_SHMKEY "1688"

#define TBUS_CHANNEL_MAGIC    0x42545553        //TBUS

#define TBUS_DEFAULT_TBUSD_URL "udp://127.0.0.1:1599"

#define TBUS_DEFAULT_TBUSD_TIMEOUT (800)   /*default timeout for the oper with tbusd*/

#define TBUS_MAX_ADDR_LEN 64

#define TBUS_MAX_RELATION_COUNT (256)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef  int    TBUSADDR;        /*tbusϵͳͨ�ŵ�ַ*/
    typedef  int    *LPTBUSADDR;     /*tbusϵͳͨ�ŵ�ַ*/


    typedef struct tagTbusChannel                              *LPTBUSCHANNEL;

    /*route info of package */
    struct tagHeadRoute
    {
        TBUSADDR iSrc;
    };
    typedef struct tagHeadRoute                            HEADROUTE;
    typedef struct tagHeadRoute                            *LPHEADROUTE;

    struct tagTbusTimeStamp
    {
        int iIsAvailable;      /* ǰһ����Ϣ�Ƿ���ʱ��� */
        long long llSec;       /* ʱ���ֵ      seconds ���� */
        unsigned int dwUSec;   /* ʱ���ֵ microseconds ���� */
        unsigned int dwCumm;   /* TBUS�������Ϣ���õ���ʱ�� */
    };
    typedef struct tagTbusTimeStamp                                        TBUSTIMESTAMP;
    typedef struct tagTbusTimeStamp                                        *LPTBUSTIMESTAMP;

    struct tagTbusRecv
    {
        IN    int           iHandle;            /* tbus ��� */
        INOUT TBUSADDR      iSrc;               /* tbus Դ��ַ */
        INOUT TBUSADDR      iDst;               /* tbus Ŀ�ĵ�ַ */
        INOUT void          *pvData;            /* �������ݵĻ����� */
        INOUT size_t        tDataLen;           /* ����������ݵĳ��� */
        IN    int           iFlag;              /* ȡtagTbusFlag�е�ֵ����"|"���� */
        INOUT LPTBUSTIMESTAMP pstTimeStamp;     /* �����ΪNULL��������Ϣ��ʱ���ֵ */
    };
    typedef struct tagTbusRecv    TBUSRECV;
    typedef struct tagTbusRecv *LPTBUSRECV;

    struct tagTbusPeerState
    {
        long long llLastBeatTime;
        int iIsAvailable;                 /*�ο�enum����tagTbusPeerStateType*/
        int iTimeOutGap;
    };
    typedef struct tagTbusPeerState       TBUSPEERSTATE;
    typedef struct tagTbusPeerState    *LPTBUSPEERSTATE;

    struct tagAddrRelation
    {
        TBUSADDR iAddr1;
        TBUSADDR iAddr2;
        int iSendQueueSize;           /* iAddr1->iAddr2�ڴ���еĴ�С��<=0��ʾʹ��tbusd���õ�Ĭ��ֵ */
        int iRecvQueueSize;           /* iAddr2->iAddr1�ڴ���еĴ�С,<=0��ʾʹ��tbusd���õ�Ĭ��ֵ */
    };
    typedef struct tagAddrRelation AddrRelation;
    struct tagAddrRelationStr
    {
        char szAddr1[TBUS_MAX_ADDR_LEN];
        char szAddr2[TBUS_MAX_ADDR_LEN];
        int iSendQueueSize;           /* szAddr1->szAddr2�ڴ���еĴ�С��<=0��ʾʹ��tbusd���õ�Ĭ��ֵ */
        int iRecvQueueSize;           /* szAddr2->szAddr1�ڴ���еĴ�С,<=0��ʾʹ��tbusd���õ�Ĭ��ֵ */
    };
    typedef struct tagAddrRelationStr AddrRelationStr;


    /** @defgroup TBUS_INIT TBUS_�������
     * @{
     */

    /**
     * @brief ��ʼ��Tbusϵͳ��
     * ��Ҫְ���ǳ��Թ���(attach)��������tbusͨ��������Ϣ�Ĺ����ڴ�GCIM(Global Channel Infomation Map)
     * ʹ�ñ��������г�ʼ����tbusϵͳ�����ھ�̬ģʽ�£�Ҳ����˵���е�CHANNEL����ҪԤ��ʹ�ù��߻���TCM�����ã�tbus���������й����д��������CHANNEL
     *
     * @param[in] a_pszShmkey  ��������GCIM�����ڴ�key����Ϣ����
     * ����Ϣ���е���Ϣ����Ϊʮ�������ִ���
     * ��Ϊһ���ļ�·��(�����ļ�������ʱ��tbusϵͳ�᳢�����ɴ��ļ�)��
     * ���ݴ���Ϣ������GCIM�����ڴ�key���㷨��:
     * - ʮ�������ִ�  �������ִ�ת����������������ΪGCIM�Ĺ����ڴ��key
     * - �ļ�·��      �����ļ�·����a_iBussId��Ϊ����������ftok�ӿ�����GCIM�����ڴ��key
     * - NULL��մ�    ʹ��ȱʡֵ TBUS_DEFAULT_GCIM_KEY(1688)��ΪGCIM�����ڴ��key
     * @param[in]    a_iFlag ��ʼ��������־λ:
     * - TBUS_INIT_FLAG_USE_SAVED_TIME
     * ����tbusʱ���������������Ҫʹ��ϵͳʱ��ʱ��ʹ��ͨ��tbus_set_current_time�����ʱ�䡣
     *
     * @note ���ӿڱ���������tbus�ӿڵ���֮ǰ����
     * @note ���ӿ���tbus_automatic_init�ӿڻ��⣬��ֻ�ܵ�������һ���ӿڽ��г�ʼ�����������Ľӿڻ᷵��ʧ��
     * @note ��ҵ������˳�ʱ���������tbus_fini�ӿڣ��Ƿ񱾽ӿڷ������Դ
     * @note ��ͬһ�������ڣ����ӿ�ֻ�е�һ�γɹ����û�������ã��������ñ��ӿ�ʱ��ֱ�ӷ���
     * @note ���ӿڷǶ��̰߳�ȫ
     *
     * @pre     ͨ��tbusmgr���߻�TCMϵͳ�ɹ�����GCIM�����ڴ�
     *
     * @retval 0 ����ɹ�
     * @retval <0 ����ʧ�ܣ������tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_fini
     * @see tbus_use_saved_time
     * @see tbus_use_sys_time
     */
    TSF4G_API int tbus_init_ex(IN const char *a_pszShmkey, IN int a_iFlag);

    /**
     * @brief ���Զ���ģʽ�³�ʼ��Tbusϵͳ��
     * ��Ҫְ���ǳ�������tbusd����ʹ��tbusd�ṩ��GCIMKey����(attach)��������tbusͨ��������Ϣ�Ĺ����ڴ�GCIM(Global Channel Infomation Map)
     * ʹ�ñ��������г�ʼ����tbusϵͳ�������Զ���ģʽ�£�Ҳ����˵����tbusd����channel���Զ�����������ʹ�ù��߻�TCM��ǰ����ͨ��
     * ���Զ���ģʽ����Щ�ӿ��ڷ���channel������ʱ���Զ���tbusd���󴴽�ͨ��:
     * tbus_connect/tbus_connect_by_str
     * tbus_send/tbus_sendv
     * tbus_forward/tbus_forwardv
     * tbus_backward/tbus_backwardv
     * tbus_send_heartbeat
     *
     * @param[in] a_pszTbusdUrl tbusd�ĵ�ַurl
     * ֻ֧��udp, ��ʽΪ"udp://127.0.0.1:xxxx"
     * - NULL��մ�("") ʹ��ȱʡֵ TBUS_DEFAULT_TBUSD_URL����tbusd
     * @param[in]    a_iFlag ��ʼ��������־λ:
     * - TBUS_INIT_FLAG_USE_SAVED_TIME
     * ����tbusʱ���������������Ҫʹ��ϵͳʱ��ʱ��ʹ��ͨ��tbus_set_current_time�����ʱ�䡣
     * @param[in] a_iTimeout ��tbusd����ͨ��ʱ�ĳ�ʱʱ��,��λ����
     * ���ֵ����Ϊ�������к�tbusd����ʱ�ĳ�ʱֵ���ں��������п���ʹ��tbus_set_opt/tbus_get_opt(TBUS_OPT_NAME_TBUSD_TIMEOUT)�޸ĺͻ�ȡ���ֵ
     * @note ���ӿڱ���������tbus�ӿڵ���֮ǰ����
     * @note ���ӿ���tbus_init/tbus_init_ex�ӿڻ��⣬��ֻ�ܵ�������һ���ӿڽ��г�ʼ�����������Ľӿڻ᷵��ʧ��
     * @note ��ҵ������˳�ʱ���������tbus_fini�ӿڣ��ͷű��ӿڷ������Դ
     * @note ��ͬһ�������ڣ����ӿ�ֻ�е�һ�γɹ����û�������ã��������ñ��ӿ�ʱ��ֱ�ӷ���
     * @note ���ӿڷǶ��̰߳�ȫ
     *
     * @pre     ��Ӧ��tbusd�Ѿ�����
     *
     * @retval 0 ����ɹ�
     * @retval <0 ����ʧ�ܣ������tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_fini
     * @see tbus_use_saved_time
     * @see tbus_use_sys_time
     */
    TSF4G_API int tbus_automatic_init(IN const char* a_pszTbusdUrl, IN int a_iFlag, IN int a_iTimeout);

    /**
     * @brief �Զ���ģʽ�£�ע�������ͨ�Ź�ϵ
     * ֻ�����Զ���ģʽ�²ſ��Ե��ñ��ӿ�(ʹ��tbus_automatic_init���г�ʼ��)
     * ���ڵ�ǰGCIM�в����ڵ�CHANNEL��������tbusd���д���
     * �����ڽ����������г�ʼ��ʱ���ô˽ӿ���ȷ������Ҫ������CHANNEL���Ѵ�����
     * ��������ñ��ӿڣ��ں�����send,connect,forward,backward,send_heartbeat�����У�
     * ���channel�����ڣ�Ҳ���Զ���tbusd���󴴽�channel�����Լ򻯿������̣�������ͨ���Ĺ�����ͬ����(�뱾����tbusdͨ��)�����ܻ����һ��������
     * ��������ڽ��̳�ʼ��ʱ�͵��ñ��ӿڴ���֤������Ҫ��ͨ����������
     *
     * @param[in] a_pstRelations ����ע���ͨ�Ź�ϵ(���ε�ַ�Ͷ��еĴ�С)
     * @param[in] a_iRelationCount ��ϵ����Ŀ������>0����<=256
     * @note ���ӿڷǶ��̰߳�ȫ
     *
     * @pre �Ѿ��ɹ����ù�tbus_automatic_init, tbusd������
     *
     * @retval 0 ����ɹ�
     * @retval <0 ����ʧ�ܣ������tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    int tbus_automatic_regist_relations(IN const AddrRelation* a_pstRelations, IN int a_iRelationCount);

    /**
     * @brief ����ͬtbus_automatic_regist_relations��ֻ�ǵ�ַʹ���ַ�����ʽ��������TBUSADDR
     *
     * @param[in] a_pstRelations ����ע���ͨ�Ź�ϵ(���ε�ַ�Ͷ��еĴ�С)
     * @param[in] a_iRelationCount ��ϵ����Ŀ������>0����<=256
     * @note ���ӿڷǶ��̰߳�ȫ
     *
     * @pre �Ѿ��ɹ����ù�tbus_automatic_init
     *
     * @retval 0 ����ɹ�
     * @retval <0 ����ʧ�ܣ������tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    TSF4G_API int tbus_automatic_regist_relations_by_str(IN const AddrRelationStr* a_pstRelations, IN int a_iRelationCount);

    /**
     * @brief automaticģʽ�£���ȡ��ʼ��ʱ���ӵ�tbusd url
     *
     * @retval !NULL: automaticģʽ
     * @retval NULL:  ��automaticģʽ
     */
    TSF4G_API const char* tbus_automatic_get_tbusd_url();

    /**
     * @brief ����tbus������
     *
     * @param[in,out] a_piHandle �洢��������ָ��
     *
     * @retval 0 -- successful, get handle ok
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre \e   a_piHandle ����Ϊ NULL
     *
     * @note ���ӿ��Ƿ��̰߳�ȫ�ģ����������ľ��Ҳֻ����ͬһ���߳̿ռ���ʹ��
     * @note ���ӿڴ����ľ������ʹ��tbus_delete���رգ����ͷ���Դ
     */
    TSF4G_API int tbus_new(INOUT int *a_piHandle);

    /**
     * @brief ɾ��һ��tbus������
     *
     * @param[in,out] a_piHandle ����tbus��������ָ�룬tbus���ͨ��tbus_new()��ȡ
     */
    TSF4G_API void tbus_delete(INOUT int *a_piHandle);


    /**
     * @brief �ر�tbusϵͳ
     *
     * @note ֻҪ�����˱��ӿڣ�tbus�����ӿڵĹ��ܽ����ٿ���
     */
    TSF4G_API void tbus_fini(void);

    /**
     * @brief ���� tbus ѡ��
     *
     * @param[in] a_iOptionName    �˲���ָ����Ҫ���õ�tbusѡ�Ŀǰ֧�ֵ�ѡ����:
     * - TBUS_OPT_NAME_TBUSD_TIMEOUT ���Զ���ģʽ�£���tbusd�����ĳ�ʱʱ��,��λΪ����,a_pvOptionValueΪָ��int(��ʱֵ)��ָ��,a_dwOptionLen����Ϊsizeof(int)
     * - TBUS_OPT_NAME_NONAUTO_CREATED ���Զ���ģʽ�£����ͨ�������ڣ��Ƿ���tbusd�����Զ�����ͨ��,a_pvOptionValueΪָ��int��ָ�롣��0��ʾ��������0��ʾ������Ĭ��Ϊ0
     *
     * @param[in] a_pvOptionValue ��Ӧѡ��ľ�������ֵ
     * @param[in] a_dwOptionLen   ָ��ѡ��ֵ�ĳ��ȣ������ֵ��a_pvOptionValue�������;�����
     *
     * @see tbus_set_handle_opt
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    TSF4G_API int tbus_set_opt(IN int a_iOptionName, IN const void *a_pvOptionValue, IN unsigned int a_dwOptionLen);


    /**
     * @brief ��ȡ tbus ѡ��
     *
     * @param[in] a_iOptionName    �˲���ָ����Ҫ���õ�tbusѡ�Ŀǰ֧�ֵ�ѡ����:
     * - TBUS_OPT_NAME_TBUSD_TIMEOUT ���Զ���ģʽ�£���tbusd�����ĳ�ʱʱ��,��λΪ����,a_pvOptionValueΪָ��int(��ʱֵ)��ָ��,a_dwOptionLen����Ϊsizeof(int)
     * @param[in] a_pvOptionValue  ��Ӧѡ��ľ�������ֵ
     *
     * @param[in,out] a_pdwOptionLen  ��ȡa_pvOptionValueֵ�ľ��峤�ȣ�
     * ���a_pdwOptionLen��ָ�ĳ���С��ʵ����Ҫ�ĳ��ȣ��򷵻�ʧ�ܡ��˲����������Լ�����£�
     * - ����    ָ��a_pvOptionValue��ָ�������Ĵ�С
     * - ���    ��ȡa_pvOptionValue��ȡ���ݵ�ʵ�ʳ���
     *
     * @see tbus_set_handle_opt
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    TSF4G_API int tbus_get_opt(IN int a_iOptionName, IN const void* a_pvOptionValue, INOUT unsigned int* a_pdwOptionLen);


    /**
     * @brief ����tbusͨ��ѡ��
     *
     * @param[in] a_pstHandle      �˲���ָ��tbus������������tbus_new��ȡ
     * @param[in] a_iOptionName    �˲���ָ����Ҫ���õ�tbus���ѡ�Ŀǰ֧�ֵ�ѡ���У�
     * - TBUS_HANDLE_OPT_NAME_EXCLUSIVE_CHANNELS    ����ͨ��ѡ�
     *   �����ѡ��򿪣���ͨ��tbus_connectָ����channelֻ������a_pstHandle��
     *   a_pstHandleҲֻ��ͨ��tbus_connect���ͨ����
     * - TBUS_HANDLE_OPT_NAME_CHANNELS_TIMESTAMP    ʱ���ѡ�
     *   ���Щѡ��򿪣���˾��������ͨ���Ϸ��ͻ���ת����Ϣ��Я��ʱ���ֵ��
     * - TBUS_HANDLE_OPT_NAME_ADAPTIVE_BINDING   ��Ӧ�԰�ѡ�
     * - �����ѡ��򿪣���˾���ڰ󶨵�ַʱ������ͨ����Ϊ0����Ȼ�󶨳ɹ���
     * @param[in] a_pvOptionValue   ��Ӧѡ��ľ�������ֵ��ѡ�ͬ����ṹҲ��ͬ��
     * - TBUS_HANDLE_OPT_NAME_EXCLUSIVE_CHANNELS
     *   ��ѡ��ֵ����������Ϊint��booleanѡ�����ֵֻ��Ϊ0��1��
     *   ��ֵΪ1���򿪻���ͨ��ѡ�
     *   ��ֵΪ0�����Դ˵��ã�������ͨ��ѡ��һ���򿪾Ͳ�֧�ֶ�̬�رա�
     * - TBUS_HANDLE_OPT_NAME_CHANNELS_TIMESTAMP
     *   ��ѡ��ֵ����������Ϊint��booleanѡ�����ֵֻ��Ϊ0��1��
     *   ��ֵΪ1����ʾ��ʱ���ѡ���ֵΪ0����ʾ�ر�ʱ���ѡ�
     * - TBUS_HANDLE_OPT_NAME_ADAPTIVE_BINDING
     *   ��ѡ��ֵ����������Ϊint��booleanѡ�����ֵֻ��Ϊ0��1��
     *   ��ֵΪ1����ʾ�������Ͱ�ѡ���ֵΪ0����ʾ�ر������Ͱ�ѡ�
     * @param[in] a_dwOptionLen    ָ��ѡ��ֵ�ĳ��ȣ������ֵ��a_pvOptionValue�������;�����
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_handle_opt
     */
    TSF4G_API int tbus_set_handle_opt(IN int a_iHandle, IN TBUSHANDLEOPTIONAME a_iOptionName,
                                      IN const void *a_pvOptionValue, IN unsigned int a_dwOptionLen);

    /**
     * @brief ��ȡtbus���ѡ��
     *
     * @param[in] a_pstHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in] a_iOptionName    �˲���ָ����Ҫ���õ�tbus���ѡ�Ŀǰ֧�ֵ�ѡ���У�
     * - TBUS_HANDLE_OPT_NAME_EXCLUSIVE_CHANNELS    ����ͨ��ѡ�
     *   �����ѡ��򿪣���ͨ��tbus_connectָ����channelֻ������a_pstChannel��
     *   a_pstChannelҲֻ�������tbus_connectָ����ͨ����
     * - TBUS_HANDLE_OPT_NAME_CHANNELS_TIMESTAMP    ʱ���ѡ�
     *   �����ѡ��򿪣���˾���ڵ�ͨ���Ϸ��ͻ���ת����Ϣ��Я��ʱ���ֵ��
     *
     * @param[in] a_pvOptionValue  ��Ӧѡ��ľ�������ֵ��ѡ�ͬ����ṹҲ��ͬ��
     * - TBUS_OPT_EXCLUSIVE_CHANNELES ��ѡ��ֵ����������Ϊint��booleanѡ�����ֵֻ��Ϊ0��1��
     *   ��ֵΪ1��ʾ����ͨ��ѡ��ڴ�״̬����ֵΪ0��ʾ����ͨ��ѡ��ڹر�״̬��
     * @param[in,out] a_pdwOptionLen    ��ȡa_pvOptionValueֵ�ľ��峤�ȣ�
     * ���a_pdwOptionLen��ָ�ĳ���С��ʵ����Ҫ�ĳ��ȣ��򷵻�ʧ�ܡ�
     * �˲����������Լ�����£�
     * - ����    ָ��a_pvOptionValue��ָ�������Ĵ�С
     * - ���    ��ȡa_pvOptionValue��ȡ���ݵ�ʵ�ʳ���
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_set_handle_opt
     */
    TSF4G_API int tbus_get_handle_opt(IN int a_iHandle, IN int a_iOptionName,
                                      IN const void* a_pvOptionValue, INOUT unsigned int* a_pdwOptionLen);

    /**
     * @deprecated this function has been replaced by tbus_init_ex
     *
     * @see tbus_init_ex
     */
    TSF4G_API int tbus_init(IN const unsigned int a_iMapKey, IN const unsigned int a_iSize);


    /**
     * @deprecated this function has been replaced by tbus_set_bussid and tbus_init_ex
     *
     * @see tbus_init_ex
     */
    TSF4G_API int tbus_initialize(IN const char *a_pszShmkey, IN int a_iBussID);

    /**
     * @brief Set business id
     *
     * @param[in] a_iBussId business id assigned by TCM system, otherwise just set it to 0
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note if business id is not euqal to zero, call this function before tbus_init_ex
     *
     * @see tbus_init_ex
     * @see tbus_ini
     */
    TSF4G_API int tbus_set_bussid(IN const int a_iBussId);

    /**
     * @brief ��ѯ "disable��ͨ�����������ʱ��"
     *
     * @note disable��ͨ�����������ʱ��:
     *       ��ͨ������disable��ʱ����ڴ�ֵʱ�����̻��ͨ��ͨ�������ڴ�detach,
     *
     * @note ������ tbus_init_ex �� tbus_initialize ֮�����
     *
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @retval >=0 -- successful, ����ֵ���� "disable��ͨ�����������ʱ��", ��λ:��
     */
    TSF4G_API int tbus_get_disabled_channel_life(void);

    /**
     * @brief ���� "disable��ͨ�����������ʱ��"
     *
     * @param[in] a_iLife "disable��ͨ�����������ʱ��", ���� >= 0, ��λ:��
     *
     * @note disable��ͨ�����������ʱ��:
     *       ��ͨ������disable��ʱ����ڴ�ֵʱ�����̻��ͨ��ͨ�������ڴ�detach,
     *
     * @note ������ tbus_init_ex �� tbus_initialize ֮�����
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    TSF4G_API int tbus_set_disabled_channel_life(IN int a_iLife);

    /** @} */


    /** @defgroup TBUS_ADDR_CONVERT TBUS_ͨ�ŵ�ַת��
     * @{
     */

    /**
     * @brief �����ʮ����ͨ�ŵ�ַ�ַ���ת����tbusϵͳ�ڴ��ַ
     *
     * @param[in] a_pszAddr ���ʮ����ͨ�ŵ�ַ�ַ���
     * @param[out] a_piAddr ����tbusͨ�ŵ�ַ��ָ��
     *
     * @pre \e a_pszAddr����ΪNULL
     * @pre \e a_piAddr ����ΪNULL
     *
     * @pre    �Ѿ�����tbus_init_ex��ʼ��tbusϵͳ
     *
     * @retval 0 -- successful
     * @retval <0 failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note a_pszAddr�б���ĵ��ʮ����ͨ�ŵ�ַ��������ϵ��ʮ����ͨ�ŵ�ַģ���Լ��
     *
     * @see tbus_init_ex
     *
     */
    TSF4G_API int tbus_addr_aton(IN const char *a_pszAddr, OUT TBUSADDR *a_piAddr);


    /**
     * @brief ��tbusͨ�ŵ�ַת���ɵ��ʮ���Ƶ�ַ��Ϣ��
     *
     * @param[in] a_iAddr tbusͨ�ŵ�ַ
     *
     * @return ����ɹ�ת���򷵻ص��ʮ���Ƶ�ַ�������򷵻ؿմ�""
     *
     * @note ���صĳɵ��ʮ���Ʊ�ʾ�ĵ�ַ��������һ����̬�������У���һ�ε��ûḲ��ǰһ�ε���ʱ��ȡ����Ϣ
     * @note ���ñ��ӿ�֮ǰ�������tbus_init_ex�Ѿ���ʼ����tbusϵͳ
     *
     * @see tbus_init_ex
     */
    TSF4G_API char *tbus_addr_ntoa(IN TBUSADDR a_iAddr);

    /**
     * @brief ��tbusͨ�ŵ�ַת���ɵ��ʮ���Ƶ�ַ��Ϣ�����������ָ���������У�
     *
     * @param[in] a_iAddr tbusͨ�ŵ�ַ
     * @param[in] a_pszDstBuff ���������Ϣ�Ļ�����
     * @param[in] a_dwBuffSize �������Ĵ�С
     *
     * @return ����a_pszDstBuff�ĵ�ַ��
     *
     */
    TSF4G_API char *tbus_addr_ntop(IN TBUSADDR a_iAddr, char *a_pszDstBuff, unsigned int a_dwBuffSize);
    /** @} */

    /** @defgroup TBUS_CHANNEL_MODE TBUS_ֱ�ӻ���ͨ����ͨ��
     * @{
     */

    /**
     * @brief ���ͨ����ؽӿ������ȫ�ֳ�ʼ������������������ֻ�����һ��
     *
     * @param[in] a_iFlag ��ʼ����־��Ԥ�������ã�Ŀǰ����ֵ0
     *
     * @retval 0 -- successful,
     * @retval !0 --failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note �ڵ���tbus_channel_open֮ǰ������ñ��ӿ�
     * @note �˳�ʱ����tbus_channel_mode_fini�ͷ���Դ
     * @note ֱ��ʹ��ͨ����ͨ�ŵ�ģʽ����tbusͨ��ģʽ�ļ򻯡�
     * ��tbus���Ʋ�ͬ����������ͨ��tbus��ع��ߴ���GCIM(�洢�ڹ����ڴ��ȫ��ͨ�����ñ�)��
     * ��ģʽ����ָ��һ�������ڴ�key���Ϳ��Դ���һ��ȫ˫��������ͨ����Ϊ�������̻��߳��ṩͨ��
     *
     * @note ֱ��ʹ��ͨ����ͨ�ŵ�ģʽ�������������£�
     * @par ʾ������:
     * @code
    //step1:ͨ�����ó�ʼ��
    tbus_channel_mode_init(0);

    //step2: ����ͨ��ͨ��
    LPTBUSCHANNEL *pstChannel = NULL;  //���洴���ľ��
    TBUSADDR iLocalAddr = 0x01010101;  //��ʾ��ͨ�Ŷ˵ĵ�ַ
    TBUSADDR iPeerAddr = 0x01010201;   //��ʾ�Զ˵ĵ�ַ
    int iShmkey = 1000;            //ͨ��ͨ����ʹ�õĹ����ڴ�key
    int a_iChannelQueSize = 10240;  //��д���еĴ�С

    tbus_channel_open(&pstChannel,iLocalAddr,iPeerAddr,iShmkey,a_iChannelQueSize);

    //step3: ���ݶ�д
    tbus_channel_send();

    ...
    tbus_channel_peek_msg();
    tbus_channel_delete_msg();


    //step4��ʹ����ϣ��ͷ������Դ
    tbus_channel_close(&pstChannel);
    ......
    tbus_channel_mode_fini();

     * @endcode
     */
    TSF4G_API int tbus_channel_mode_init(IN int a_iFlag);



    /**
     * @brief �������˳�ʱ���ô˽ӿ� �ͷ�ֱ��ͨ��ͨ��ģʽ�����ϵͳ��Դ��ÿ�����̽������һ��
     *
     */
    TSF4G_API void tbus_channel_mode_fini();

#define tbus_open_channel    tbus_channel_open
    /**
     * @brief ͨ��ָ�������ڴ��һ��ͨ��ͨ��
     *
     * @param[in,out] a_ppstChannel ����򿪵�ͨ�������ָ��
     * @param[in] a_iLocalAddr ͨ�����ص�ַ����ֵַͨ��inet_aton�������Ϊ�Զ�������
     * @param[in] a_iPeerAddr ͨ���Զ˵�ַ����ֵַͨ��inet_aton�������Ϊ�Զ�������
     * @param[in] a_iShmkey �����ڴ�key
     * @param[in] a_iSize ͨ�����ݶ��еĴ�С����д���еĴ�С��ͬ
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ͨ���˽ӿڴ򿪵�ͨ��������������tbus_channel_close�ر�
     *
     * @pre a_ppstchannel ����ΪNULL
     * @pre a_iSize �������0
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note tbus_channel_open���صľ���Ƿ��̰߳�ȫ�ģ����жԸú����򿪵ľ���Ĳ������Ƿ��̰߳�ȫ��
     * @see tbus_channel_close
     */
    TSF4G_API int tbus_channel_open(INOUT LPTBUSCHANNEL *a_ppstChannel, IN TBUSADDR a_iLocalAddr,
                                    IN TBUSADDR a_iPeerAddr,IN int a_iShmkey, IN int a_iSize);

#define tbus_open_channel_ex    tbus_channel_open_ex
    /**
     * @brief ͨ��ָ�������ڴ��һ��ͨ��ͨ��
     *
     * @param[in,out] a_ppstChannel ����򿪵�ͨ�������ָ��
     * @param[in] a_iLocalAddr ͨ�����ص�ַ����ֵַͨ��inet_aton�������Ϊ�Զ�������
     * @param[in] a_iPeerAddr ͨ���Զ˵�ַ����ֵַͨ��inet_aton�������Ϊ�Զ�������
     * @param[in] a_iShmkey �����ڴ�key
     * @param[in] a_iSendSize ͨ���������ݶ��еĴ�С
     * @param[in] a_iRecvSize ͨ���������ݶ��еĴ�С
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ͨ���˽ӿڴ򿪵�ͨ��������������tbus_channel_close�ر�
     *
     * @pre a_ppstchannel ����ΪNULL
     * @pre a_iSendSize �������0
     * @pre a_iRecvSize �������0
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note tbus_channel_open���صľ���Ƿ��̰߳�ȫ�ģ����жԸú����򿪵ľ���Ĳ������Ƿ��̰߳�ȫ��
     * @see tbus_channel_close
     */
    TSF4G_API int tbus_channel_open_ex(INOUT LPTBUSCHANNEL *a_ppstChannel, IN TBUSADDR a_iLocalAddr,
                                    IN TBUSADDR a_iPeerAddr,IN int a_iShmkey, IN int a_iSendSize, IN int a_iRecvSize);


#define tbus_open_channel_by_str    tbus_channel_open_by_str
    /**
     * @brief ͨ��ָ�������ڴ��һ��ͨ��ͨ�� �˽ӿ����һ��tbusϵͳ�ļ���ʵ��
     *
     * @param[in,out] a_ppstChannel ����򿪵�ͨ�������ָ��
     * @param[in] a_pszLocalAddr ͨ�����ص�ַ����ַ��ʽΪIPv4�����ַ ���ʮ���Ʊ�ʾ������x.x.x.x
     * @param[in] a_pszPeerAddr ͨ���Զ˵�ַ����ַ��ʽΪIPv4�����ַ ���ʮ���Ʊ�ʾ������x.x.x.x
     * @param[in] a_iShmkey �����ڴ�key
     * @param[in] a_iSize ͨ�����ݶ��еĴ�С����д���еĴ�С��ͬ
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ͨ���˽ӿڴ򿪵�ͨ��������������tbus_channel_close�ر�
     * @see tbus_channel_close
     *
     * @pre a_ppstchannel ����ΪNULL
     * @pre a_iSize �������0
     * @pre a_pszLocalAddr ����ΪNULL
     * @pre a_pszPeerAddr ����ΪNULL
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note tbus_channel_open���صľ���ǷǶ��̰߳�ȫ�ģ����жԸþ���Ĳ������ǷǶ��̰߳�ȫ��
     */
    TSF4G_API int tbus_channel_open_by_str(INOUT LPTBUSCHANNEL *a_ppstChannel, IN const char *a_pszLocalAddr,
                                           IN const char *a_pszPeerAddr,IN int a_iShmkey, IN int a_iSize);

#define tbus_open_channel_by_str_ex    tbus_channel_open_by_str_ex
    /**
     * @brief ͨ��ָ�������ڴ��һ��ͨ��ͨ�� �˽ӿ����һ��tbusϵͳ�ļ���ʵ��
     *
     * @param[in,out] a_ppstChannel ����򿪵�ͨ�������ָ��
     * @param[in] a_pszLocalAddr ͨ�����ص�ַ����ַ��ʽΪIPv4�����ַ ���ʮ���Ʊ�ʾ������x.x.x.x
     * @param[in] a_pszPeerAddr ͨ���Զ˵�ַ����ַ��ʽΪIPv4�����ַ ���ʮ���Ʊ�ʾ������x.x.x.x
     * @param[in] a_iShmkey �����ڴ�key
     * @param[in] a_iSendSize ͨ���������ݶ��еĴ�С
     * @param[in] a_iRecvSize ͨ���������ݶ��еĴ�С
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ͨ���˽ӿڴ򿪵�ͨ��������������tbus_channel_close�ر�
     * @see tbus_channel_close
     *
     * @pre a_ppstchannel ����ΪNULL
     * @pre a_iSize �������0
     * @pre a_pszLocalAddr ����ΪNULL
     * @pre a_pszPeerAddr ����ΪNULL
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note tbus_channel_open���صľ���ǷǶ��̰߳�ȫ�ģ����жԸþ���Ĳ������ǷǶ��̰߳�ȫ��
     */
    TSF4G_API int tbus_channel_open_by_str_ex(INOUT LPTBUSCHANNEL *a_ppstChannel, IN const char *a_pszLocalAddr,
                                           IN const char *a_pszPeerAddr,IN int a_iShmkey, IN int a_iSendSize, IN int a_iRecvSize);

#define tbus_close_channel    tbus_channel_close
    /**
     * @brief �ر�һ��ͨ��ͨ��
     *
     * @param[in,out] a_ppstChannel ����򿪵�ͨ�������ָ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note a_ppstChannel��ָ��ͨ����������ǵ���tbus_channel_open�򿪵�
     *
     * @see tbus_channel_open
     */
    TSF4G_API void tbus_channel_close(INOUT LPTBUSCHANNEL *a_ppstChannel);

#define tbus_get_channel_outmsg_num tbus_channel_get_outmsg_num
    /**
     * @brief ��ѯָ����ͨ���ϵķ��Ͷ����е���Ϣ����
     *
     * @param[in,out] a_iMsgNum    ��ɹ����أ����淢�Ͷ����е���Ϣ������
     * @param[in]     a_pstChannel ͨ�����
     *
     * @retval        0 -- successful
     * @retval        !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    int tbus_channel_get_outmsg_num(IN LPTBUSCHANNEL a_pstChannel, OUT int* a_piMsgNum);


    /**
     * @brief ��ѯָ����ͨ���ϵĽ��ն����е���Ϣ����
     *
     * @param[in,out] a_iMsgNum    ��ɹ����أ����淢�Ͷ����е���Ϣ������
     * @param[in]     a_pstChannel ͨ�����
     *
     * @retval        0 -- successful
     * @retval        !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    int tbus_get_channel_inmsg_num(IN LPTBUSCHANNEL a_pstChannel, OUT int* a_piMsgNum);


    /**
     * @brief ��ָ��ͨ���Ϸ���һ�����ݰ�
     *
     * @param[in] a_pstChannel ͨ�����
     * @param[in] a_pvData �������ݵĻ������׵�ַ
     * @param[in] a_iDataLen ���ݳ���
     * @param[in] a_iFlag �����ֶ�
     *
     * @retval 0 -- successful,
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @pre a_pstChannel ����ΪNULL
     *
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_send(IN LPTBUSCHANNEL a_pstChannel,IN const void *a_pvData,
                                    IN const int a_iDataLen, IN const int a_iFlag);

    /**
     * @brief ��a_ptVector������ָ�������ݻ���������Ϣ ���һ�����ݰ������͵����ݶ����С�
     *
     * @param[in] a_pstChannel ͨ�����
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ���ӿ�֧�ֵı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_send
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_sendv(IN LPTBUSCHANNEL a_pstChannel, IN const struct iovec *a_ptVector,
                                     IN const int a_iVecCnt,        IN const int a_iFlag );

    /**
     * @brief ��ָ��ͨ���Ͻ���һ�����ݰ�
     *
     * @param[in] a_pstChannel ͨ�����
     * @param[in] a_pvData �������ݵĻ������׵�ַ
     * @param[in,out] a_piDataLen �������ݳ��ȵ�ָ��
     *    in    - ���ܻ���������С
     *    out - �������ݵ�ʵ�ʴ�С
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * -  TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     *
     * @retval 0 -- successful,
     * @retval !0 --failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     *
     * @see tbus_get_dyedmsgid
     * @pre a_pstChannel ����ΪNULL
     * @pre a_pvData ����ΪNULL
     * @pre *a_ptDataLen �������0
     */
    TSF4G_API int tbus_channel_recv(IN LPTBUSCHANNEL a_pstChannel, INOUT void *a_pvData,
                                    INOUT size_t *a_ptDataLen, IN const int a_iFlag);

    /**
     * @brief ��ָ����������ͨ���л�ȡ������ͷ����һ����Ϣ�Ĵ洢λ�úʹ���Ϣ�ĳ�����Ϣ
     *
     * @param[in] a_pstChannel ͨ�����
     * @param[in,out] a_ppvData ����ɹ���ȡ����Ϣ����˲���Ϊ���ڱ������ݴ洢��ַ��ָ��
     * @param[out] a_piDataLen ��ȡ���ݳ��ȵ�ָ��
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * -  TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note ���ӿ���tbus_channel_recv�Ĳ�֮ͬ�����ڣ�
     * tbus_channel_recv�����ݶ����ж�ȡ�����ݺ󣬻Ὣ���ݴӶ������Ƴ���
     * �����ӿڽ���ֻ�Ƿ��������ڶ����еĴ洢λ�úͳ��ȣ������Ὣ���ݴӶ������Ƴ���
     * �������ݱ����������ص���tbus_channel_delete_msg�����ݴӶ������Ƴ���
     * ����tbus_channel_peek_msg�����Ƕ�ȡ��ͬһ����Ϣ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_channel_delete_msg
     * @see tbus_channel_recv
     *
     * @pre a_ppvData ����ΪNULL
     * @pre a_ptDataLen ����ΪNULL
     */
    TSF4G_API int tbus_channel_peek_msg(IN LPTBUSCHANNEL a_pstChannel,
                                        INOUT const char **a_ppvData,
                                        OUT size_t *a_ptDataLen,
                                        IN const int a_iFlag);


    /**
     * @brief ��ָ��ͨ���������ݶ���ͷ����һ��Ϣ�Ƴ�
     *
     * @param[in] a_pstChannel ͨ�����
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note ���ӿ���tbus_channel_peek_msg�ӿ����ʹ�ôӶ����ж�ȡ���ݽ��д���
     * ���ڶ�ȡ������ֱ�ӱ��������ݶ����У���˿��Լ���һ�����ݿ�������
     *
     * @see tbus_peek_msg
     */
    TSF4G_API int tbus_channel_delete_msg(IN LPTBUSCHANNEL a_pstChannel);


    /**
     * @brief clean remaining msg from channel
     *
     * @param[in] a_pstChannel handle of channel
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @note ���ӿڷǶ��̰߳�ȫ
     */
    TSF4G_API int tbus_channel_clean(IN LPTBUSCHANNEL a_pstChannel);


    /**
     * @brief ��ȡͨ����ǰһ����Ϣ��tbusͷ�����Ա�����forward/backward��Ϣ
     *
     * @param[in] a_pstChannel ����ͨ�����
     * @param[in,out] a_pvBuffer ����ͷ����Ϣ�Ļ�����
     * @param[in,out] a_piLen ����ͷ��������Ϣ
     * - ����    �������������ֽ���
     * - ���    ����ͷ����Ϣ��ʵ�ʳ���
     *
     * @retval 0 -- successful, get ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_pvBuffer ����ΪNULL
     * @pre a_piLen ����ΪNULL�����䱣�������ֵ�������0
     */
    TSF4G_API int tbus_channel_get_bus_head(IN LPTBUSCHANNEL a_pstChannel, INOUT void *a_pvBuffer, INOUT int *a_piLen);




    /**
     * @brief ��tbusͷ����Ϣд�뵽ͨ������У��Ա�����forward/backward��Ϣ
     *
     * @param[in] a_pstChannel ����ͨ�����
     * @param[in] a_pvBuffer ����ͷ����Ϣ�Ļ�����
     * @param[in] a_piLen ����ͷ��������Ϣ
     *
     * @retval 0 -- successful, set ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note this function should be invoked explicitly
     * @pre a_pvBuffer ����ΪNULL
     */
    TSF4G_API int tbus_channel_set_bus_head(IN LPTBUSCHANNEL a_pstChannel, IN const void *a_pvBuffer, IN const int a_iLen);


    /**
     * @brief ��ǰת����Ϣ���书����tbus_channel_send���ƣ�
     * ��������tbus_channel_forward���¼��Ϣ��������е�·����Ϣ��
     * ���ʹ��tbus_channel_backward��Ϣ����ԭ·����
     *
     * @param[in] a_pstChannel    ͨ�����
     * @param[in] a_pvData ���淢�����ݵĻ�����֧��
     * @param[in] a_iDataLen ���������ݵĳ���
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note ���ñ��ӿ�ֱ�ӣ�����ͨ��tbus_channel_set_bus_head����·����Ϣ
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_forward(IN LPTBUSCHANNEL a_pstChannel, IN const void *a_pvData, IN const size_t a_iDataLen,    IN const int a_iFlag);

    /**
     * @brief ���������������и�buff���������װ��һ����Ϣ��ǰת����Ϣ��
     * �书����tbus_channel_forward���ƣ��������� tbus_channel_forwardת������һ�������������е���Ϣ
     *
     * @param[in] a_pstChannel    ͨ�����
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���ӿڷǶ��̰߳�ȫ
     * @note ���ñ��ӿ�ֱ�ӣ�����ͨ��tbus_channel_set_bus_head����·����Ϣ
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_forwardv(IN LPTBUSCHANNEL a_pstChannel, IN const struct iovec *a_ptVector,
                                        IN const int a_iVecCnt,    IN const int a_iFlag);



    /**
     * @brief ���������ݣ���tbus_channel_send��ͬ���Ƿ�������ʱ��ת��ǰһ����Ϣ�и�����·����Ϣ
     *
     * @param[in] a_pstChannel    ͨ�����
     *
     * @param[in] a_pvData ���淢�����ݵĻ�����֧��
     * @param[in] a_iDataLen ���������ݵĳ���
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���ӿڷǶ��̰߳�ȫ
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_backward(IN LPTBUSCHANNEL a_pstChannel, IN const void *a_pvData,
                                        IN const size_t a_iDataLen,    IN const int a_iFlag);


    /**
     * @brief ������tbus_channel_backward���ƣ��������Ǳ��ӿڽ�iovec�����и����������е���Ϣ ��װ��һ����Ϣ����
     *
     * @param[in] a_pstChannel    ͨ�����
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���ӿڷǶ��̰߳�ȫ
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_channel_backwardv(IN LPTBUSCHANNEL a_pstChannel,IN const struct iovec *a_ptVector,
                                         IN const int a_iVecCnt,  IN const int a_iFlag);



    /** @} */ /*defgroup TBUS_CHANNEL_MODE TBUS_ֱ�ӻ���ͨ����ͨ��*/


    /** @defgroup TBUS_CHANNEL_MNG TBUS_ͨ������
     * @{
     */




    /**
     * @brief ָ��tbusͨ�����ص�ַ����tbus�ӿڵ����߽��̵�ͨ�ŵ�ַ
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_szSrcAddr ���ص�ַ�����ʮ���Ʊ�ʾ��ͨ�ŵ�ַ������ʽ������: xxx.xxx.xxx.xxx.xxx
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @note ���ӿڻ�ͨ����д���������GCIM�����ڴ��е���Ϣ����˱��ӿ�������ʽ��
     * @pre a_szSrcAddr ����ΪNULL
     * @pre a_iHandle Ϊ��Ч���
     * @see tbus_new
     */
    TSF4G_API int tbus_bind_by_str(IN const int a_iHandle, IN const char *a_szSrcAddr);


    /**
     * @brief ָ��tbusͨ�����ص�ַ����tbus�ӿڵ����߽��̵�ͨ�ŵ�ַ
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iSrcAddr ���ص�ַ��ͨ��tbus_addr_aton�ӿڿ��Ի�ȡ
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @note ���ӿڻ�ͨ����д���������GCIM�����ڴ��е���Ϣ����˱��ӿ�������ʽ��
     * @see tbus_new
     */
    TSF4G_API int tbus_bind(IN const int a_iHandle, IN const TBUSADDR a_iSrcAddr);


    /**
     * @brief ����ȫ��GCIM�е�����ˢ��tbus�����������ͨ����
     * ����󶨵�ַ���ͨ����������Զ���ӵ�tbus����У����tbus�����������ͨ���Ѿ�����gcim���������ڻ���
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @note tbus APIʹ���߿��Զ�ʱ���ñ��ӿ���ˢ�¸þ�������ͨ������
     *
     * @pre a_iHandle Ϊ��Ч���
     *
     * @see tbus_new
     * @see tbus_bind
     */
    TSF4G_API int tbus_refresh_handle(IN const int a_iHandle);


    /**
     * @brief ָ��tbusͨ���ĶԶ˵�ַ
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_szDstAddr ���ʮ���Ʊ�ʾ��ͨ�ŵ�ַ������ʽ������: xxx.xxx.xxx.xxx.xxx
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_new
     */
    TSF4G_API int tbus_connect_by_str(IN const int a_iHandle, IN const char *a_szDstAddr);


    /**
     * @brief ָ��tbusͨ���ĶԶ˵�ַ���ӿڹ�����tbus_connect_by_str  ��ͬ
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iDstAddr �Զ˵�ַ��ͨ��tbus_addr_aton()��ȡ
     *
     * @retval 0 -- successful,
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_new
     * @see tbus_addr_aton
     */
    TSF4G_API int tbus_connect(IN const int a_iHandle, IN const TBUSADDR a_iDstAddr);


    /**
     * @brief ͨ���Զ˵�ַ���ƣ���ֹ/���� �շ�ĳ���Զ˵�ַ��ͨ������
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iDstAddr ��Ҫ���ƵĶԶ˵�ַ
     * @param[in] a_iMode ����λ������ȡ��������ֵ:
     * - TBUS_MODE_DISABLE    ��ֹ�Ӵ˶Զ˵�ַ�շ�����
     * - TBUS_MODE_ENABLE    �����Ӵ˶Զ˵�ַ�շ�����
     * @param[in] a_iBatch ͨ��ѡ���ʾλ������ȡ��������ֵ��
     * - TBUS_PEER_CTRL_ONE    �����bus���������ĵ�һ��ͨ�����п���
     * - TBUS_PEER_CTRL_ALL    ���bus��������������ͨ�����п���
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     */
    TSF4G_API int tbus_peer_ctrl(IN const int a_iHandle, IN const TBUSADDR a_iDstAddr, IN const int a_iMode, IN const int a_iBatch);

    /**
     * @brief ����ͨ����ַ��ȡָ��ͨ����ָ��
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[out] a_ppstChannel ����ָ��ͨ��ָ���ָ��
     * @param[in] a_iSrcAddr ͨ���ı��ص�ַ
     * @param[in] a_iDstAddr ͨ���ĶԶ˵�ַ
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @pre a_ppstchannel ����ΪNULL
     */
    TSF4G_API int tbus_get_channel(IN const int a_iHandle, OUT LPTBUSCHANNEL *a_ppstChannel, IN TBUSADDR a_iSrcAddr, IN TBUSADDR a_iDstAddr);

    /**
     * @brief ����ͨ����ַ��ȡָ����õ�ַ��ص�ͨ��������
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iLocalAddr     ͨ���ĵ�ַ
     * @param[out] a_piPeerAddr    ���ͨ��������
     *
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @pre a_piPeerCount ����ΪNULL
     * @note a_iLocalAddr ��֧�ִ��������ַTBUS_ADDR_DEFAULT/TBUS_ADDR_ALL
     */
    TSF4G_API int tbus_get_peer_count(IN int a_iHandle, IN TBUSADDR a_iLocalAddr, OUT int* a_piPeerCount);


    /**
     * @brief ����ͨ����ַ��ȡָ����õ�ַ��ص�ͨ���ĵ�ַ
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iLocalAddr      ͨ���ĵ�ַ
     * @param[out] a_piAddrBuff     ����ͨ����ַ�Ļ���
     * @param[in,out] a_piAddrCount ��������a_piAddrBuff�Ĵ�С; ���ʹ�õ�a_piAddrBuff�����Ԫ�ص�����
     *
     * @retval 0 -- successful
     * @retval <0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_piAddrBuff ����ΪNULL
     * @pre a_piAddrCount ����ΪNULL
     * @note a_iLocalAddr ��֧�ִ��������ַTBUS_ADDR_DEFAULT/TBUS_ADDR_ALL
     */
    TSF4G_API int tbus_get_peer_addr(IN int a_iHandle, IN TBUSADDR a_iLocalAddr, OUT TBUSADDR* a_piAddrBuff, INOUT int* a_piAddrCount);

    /**
     * @brief calc the realsize with the channel size a_iSize
     *
     * @param[in] a_iSize the real size per channel.
     * @retval the size of buffer required by the channle.
     *
     * @note �˽ӿ��ڲ�û�п��ǳ˷�����������������a_iSize��ֵ�ڷ�Χ[1, 0x70000000]��
     * @note ���ӿڵķ���ֵ�����вο����壬���������ƶ�ĳ�鹲���ڴ��Ƿ��Ӧһ��TBUSͨ��
     *
     */
    TSF4G_API size_t tbus_channel_calc_mem_size(IN int a_iSize);


    /**
     * @brief create a channel on the specified memory
     *
     * @note channel created by this api need to be closed by tbus_channel_close_mem
     * @see tbus_channel_open
     * @see tbus_channel_close_mem
     */
    TSF4G_API int tbus_channel_open_mem(INOUT LPTBUSCHANNEL *a_ppstChannel,
                                        IN TBUSADDR a_iLocalAddr, IN TBUSADDR a_iPeerAddr,
                                        IN void *a_pvBuff, IN size_t a_iBuffSize,
                                        int a_iNeedInit, IN int a_iQueueSize);
    /**
     * @brief close channel created by tbus_channel_open_mem
     *
     * @see tbus_channel_open_mem
     * @see tbus_channel_close
     */
    TSF4G_API void tbus_channel_close_mem(INOUT LPTBUSCHANNEL *a_ppstChannel);

    /** @} */




    /** @defgroup TBUS_DATA_TRANSFER TBUS_���ݴ���
     * @{
     */

    /**
     * @brief �������ݣ�����ʵ��ֻ��һ��ͨ���������ݣ�Ҳ���ԶԶ��ͨ�����ʹ����ݣ�
     * ������Щͨ���跢��������Դ��ַ��Ŀ�ĵ�ַ����
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc ָ���������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Դ��ַ���������ʹ��ͨ����Դ��ַ��\n
     * �������룬 ��ֵ��Ϊ�������������
     * - *a_piSrc == 0     ��Ϊȱʡֵ����ʹ�õ�һ��ƥ��Ŀ�ĵ�ַ��ͨ������������
     * - *a_piSrc == -1    ��Ϊ�㲥��ַ����������ƥ��Ŀ�ĵĵ�ַ*a_piDst��ͨ������������
     * - �����ֵַ        ָ������Դ��ַ��ֻ��ƥ���Դ��ַ��ͨ���ſ��ܻᷢ������
     * @param[in,out] a_piDst ָ���������ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Ŀ�ĵ�ַ���������ʹ��ͨ����Ŀ�ĵ�ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - *a_piDst == 0    ��Ϊȱʡֵ����ʹ�õ�һ��ƥ��Դ��ַ��ͨ�����������ݡ�
     *   �����ʱ*a_piSrc == 0, ��ʹ��tbus�������ĵ�һ��ͨ�����������ݡ�
     * - *a_piDst == -1    ��Ϊ�㲥��ַ����������ƥ��Դ��ַ��ͨ�����������ݡ�
     * - �����ֵַ        ָ������Ŀ�ĵ�ַ��ֻ��ƥ���Ŀ�ĵ�ַ��ͨ���ſ��ܻᷢ�����ݡ�
     *
     * @param[in] a_pvData ���淢�����ݵĻ�����֧��
     * @param[in] a_iDataLen ���������ݵĳ���
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_WITH_TIMESTAMP ������ϢʱЯ��ʱ�����Ϣ
     * - TBUS_FLAG_USE_GETTIMEOFDAY ������ָ����TBUS_FLAG_WITH_TIMESTAMPʱ�����壬ָʾʹ��gettimeofdayȡ�õ�ǰ����ϵͳʱ��
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_addr_aton
     * @see tbus_new
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen �������0
     * @see tbus_init_ex
     * @see tbus_use_saved_time
     * @see tbus_use_sys_time
     * @see tbus_set_current_time
     * @see tbus_turn_timestamp_on
     * @see tbus_turn_timestamp_off
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_send(IN const int a_iHandle, INOUT TBUSADDR *a_piSrc,
                            INOUT TBUSADDR *a_piDst, IN const void *a_pvData,
                            IN const size_t a_iDataLen, IN const int a_iFlag);


    /**
     * @brief Send data which stored at multiple buffers
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in,out] a_piSrc -- see tbus_send()
     * @param[in,out] a_piDst -- see tbus_send()
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_WITH_TIMESTAMP ������ϢʱЯ��ʱ�����Ϣ
     * - TBUS_FLAG_USE_GETTIMEOFDAY ������ָ����TBUS_FLAG_WITH_TIMESTAMPʱ�����壬ָʾʹ��gettimeofdayȡ�õ�ǰ����ϵͳʱ��
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_send
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_sendv(IN const int a_iHandle,
                             INOUT TBUSADDR *a_piSrc,
                             INOUT TBUSADDR *a_piDst,
                             IN const struct iovec *a_ptVector,
                             IN const int a_iVecCnt,
                             IN const int a_iFlag);


    /**
     * @brief ��ͨ���ж�ȡ��һ����Ϣ������ֻ��ĳһ��ָ��ͨ���������ݣ�Ҳ�ɳ��ԴӶ��ͨ���ж�ȡ���ݡ�
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc ָ���������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Դ��ַ�����ʵ�ʶ�ȡ���ݵ�ͨ����Դ��ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - �����ֵַ         ָ������Դ��ַ��ֻ��ƥ���Դ��ַ��ͨ���ſ��ܻ��������
     * - �����ַ(0 | -1)   ƥ�������ַ��
     * @param[in,out] a_piDst ָ�����ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Ŀ�ĵ�ַ�� ���ʵ�ʶ�ȡ���ݵ�ͨ����Ŀ�ĵ�ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - �����ֵַ        ָ������Ŀ�ĵ�ַ��ֻ��ƥ���Ŀ�ĵ�ַ��ͨ���ſ��ܻ��������
     * - �����ַ(0 | -1)  ƥ�������ַ��
     *
     * @param[in] a_pvData ����������ݵĻ�����ָ��
     * @param[in,out] a_piDataLen ���������ݵĳ���:
     * - ����    �����������ó���
     * - ���    ʵ�ʽ������ݵĳ���
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     *
     * @retval 0  -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���*a_piSrc��*a_piDst�������tbus��ַ������tbus��������(α����)����������ĸ�ͨ������ȡ���ݡ�
     * - ע�⣬Ϊ�˷�ֹͨ��"����"��������������һ��ͨ����ȡ����Ϣ����������(TBUS_MAX_RECV_CNT����ǰֵΪ10)��
     * @code
     for(i = 0; i < a_iHandle.channelCount; i++)
     {
     channel = getChannel(a_iHandle.recvPos);
     if(channel NOT matches <*a_piSrc, *a_piDst>)
     {
     a_iHandle.recvPos =(a_iHandle.recvPos + 1)% a_iHandle.channelCount;
     a_iHandle.recvedCount = 0;
     continue;
     }
     iRet = recvData(channel);
     if(TBUS_SUCCESS != iRet)
     {
     if(isSpecialAddress(*a_piSrc)|| isSpecialAddress(*a_piDst))
     {
     a_iHandle.recvPos =(a_iHandle.recvPos + 1)% a_iHandle.channelCount;
     a_iHandle.recvedCount = 0;
     continue;
     } else {
     break;
     }
     }
     if(isSpecialAddress(*a_piSrc)|| isSpecialAddress(*a_piDst))
     {
     a_iHandle.recvedCount++;
     if(TBUS_MAX_RECV_CNT <= a_iHandle.recvedCount)
     {
     a_iHandle.recvPos =(a_iHandle.recvPos + 1)% a_iHandle.channelCount;
     a_iHandle.recvedCount = 0;
     }
     }
     break;
     }
     @endcode
     *
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen �������0
     */
    TSF4G_API int tbus_recv(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                            INOUT void *a_pvData,    INOUT size_t *a_ptDataLen,    IN const int a_iFlag);

    /**
     * @brief ��ͨ���ж�ȡ��һ����Ϣ
     *
     * @param[in, out] a_pstRecv ���봫�������ṹ
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @pre a_pstRecv ����ΪNULL
     * @note ����TBUSRECV���͵ı����󣬱����ȵ��� memset ��ʼ���������ȫ����ԱΪ0��Ȼ�������ù�ע���ֶ�ֵ
     * @note a_pstRecvָ��Ľṹ�ĸ�����Ա������ɲο�LPTBUSRECV�Ķ��壬�Լ�tbus_recv��������������
     * @see tbus_recv
     */
    TSF4G_API int tbus_recv_ex(INOUT LPTBUSRECV a_pstRecv);

    /**
     * @brief ������ͨ���л�ȡ������ͷ����һ����Ϣ�Ĵ洢λ�úʹ���Ϣ�ĳ�����Ϣ��
     * ���ӿ�ʵ��ֻ��ĳһ��ָ��ͨ����ȡ���ݣ�Ҳ���ԶԳ��ԴӴӶ��ͨ���ж�ȡ���ݣ�
     * ����ѡ���Ǹ�ͨ��������Ϣ���в�����ָ����Դ��Ŀ�ĵ�ַȷ��
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc ָ���������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Դ��ַ�����ʵ�ʶ�ȡ���ݵ�ͨ����Դ��ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - �����ֵַ         ָ������Դ��ַ��ֻ��ƥ���Դ��ַ��ͨ���ſ��ܻ��������
     * - �����ַ(0 | -1)   ƥ�������ַ��
     * @param[in,out] a_piDst ָ�����ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Ŀ�ĵ�ַ�� ���ʵ�ʶ�ȡ���ݵ�ͨ����Ŀ�ĵ�ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - �����ֵַ        ָ������Ŀ�ĵ�ַ��ֻ��ƥ���Ŀ�ĵ�ַ��ͨ���ſ��ܻ��������
     * - �����ַ(0 | -1)  ƥ�������ַ��
     *
     * @param[in,out] a_ppvData ����ɹ���ȡ����Ϣ����˲���Ϊ���ڱ������ݴ洢��ַ��ָ��
     * @param[out] a_piDataLen ��ȡ���ݳ��ȵ�ָ��
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * -  TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @note ���ӿ���tbus_recv�Ĳ�֮ͬ������
     * - tbus_recv�����ݶ����ж�ȡ�����ݺ󣬻Ὣ���ݴӶ������Ƴ���
     * - ���ӿڽ���ֻ�Ƿ��������ڶ����еĴ洢λ�úͳ��ȣ������Ὣ���ݴӶ������Ƴ���
     *   �������ݱ����������ص���tbus_delete_msg�����ݴӶ������Ƴ���
     *   ����tbus_peek_msg�����Ƕ�ȡ��ͬһ����Ϣ��
     *
     * @see tbus_recv
     * @see tbus_delete_msg
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_ppvData ����ΪNULL
     * @pre a_ptDataLen ����ΪNULL
     */
    TSF4G_API int tbus_peek_msg(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                                INOUT const char **a_ppvData,    OUT size_t *a_ptDataLen,    IN const int a_iFlag);


    /**
     * @brief ��ָ��ͨ���������ݶ���ͷ����һ��Ϣ�Ƴ�
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in] a_iSrc ָ�����ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������
     * @param[in] a_iDst ָ�����ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note ���ӿ���tbus_peek_msg�ӿ����ʹ�ôӶ����ж�ȡ���ݽ��д���
     * ���ڶ�ȡ������ֱ�ӱ��������ݶ����У���˿��Լ���һ�����ݿ������̡�
     * @note ����ʹ�� tbus_peek_msg ��������ַ�����Ĵ���ֵ��Ϊ���ӿڵ�������ַ����ֵ��
     *
     * @see tbus_peek_msg
     */
    TSF4G_API int tbus_delete_msg(IN const int a_iHandle,    IN TBUSADDR a_iSrc,    IN TBUSADDR a_iDst);



    /**
     * @brief ��ǰת����Ϣ���书����tbus_send���ơ�
     * ��������tbus_forward���¼��Ϣ��������е�·����Ϣ��
     * �������ʹ��tbus_backward��Ϣ����ԭ·���ء�
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc ָ���������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Դ��ַ�� �������ʹ��ͨ����Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * �������룬��ֵ��Ϊ�������������
     * - *a_piSrc == 0     ��Ϊȱʡֵ����ʹ�õ�һ��ƥ��Ŀ�ĵ�ַ��ͨ������������
     * - *a_piSrc == -1    ��Ϊ�㲥��ַ����������ƥ��Ŀ�ĵĵ�ַ*a_piDst��ͨ�����������ݡ�
     * - �����ֵַ        ָ������Դ��ַ��ֻ��ƥ���Դ��ַ��ͨ���ſ��ܻᷢ�����ݡ�
     * @param[in,out] a_piDst ָ���������ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Ŀ�ĵ�ַ���������ʹ��ͨ����Ŀ�ĵ�ַ��\n
     * �������룬���������������
     * - *a_piDst == 0    ��Ϊȱʡֵ����ʹ�õ�һ��ƥ��Դ��ַ��ͨ�����������ݡ�
     *   �����ʱ*a_piSrc == 0, ��ʹ��tbus�������ĵ�һ��ͨ�����������ݡ�
     * - *a_piDst == -1    ��Ϊ�㲥��ַ����������ƥ��Դ��ַ��ͨ������������
     * - �����ֵַ        ָ������Ŀ�ĵ�ַ��ֻ��ƥ���Ŀ�ĵ�ַ��ͨ���ſ��ܻᷢ������
     *
     * @param[in] a_pvData ���淢�����ݵĻ�����֧��
     * @param[in] a_iDataLen ���������ݵĳ���
     *
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_backward
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     *
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_forward(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                               IN const void *a_pvData,    IN const size_t a_iDataLen,    IN const int a_iFlag);



    /**
     * @brief �书����tbus_forward���ƣ������Ǳ��ӿڽ�iovec�����������ڴ���װ��һ����Ϣ����
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc �ο� tbus_forward
     * @param[in,out] a_piDst �ο� tbus_forward
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     *
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @see tbus_forward
     * @see tbus_backward
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_forwardv(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                                IN const struct iovec *a_ptVector,  IN const int a_iVecCnt,  IN const int a_iFlag);



    /**
     * @brief ��tbusͷ����¼��·�����ģ����ת����Ϣ
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc ָ���������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Դ��ַ�� �����ʹ��ͨ����Դ��ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - *a_piSrc == 0     ��Ϊȱʡֵ����ʹ�õ�һ��ƥ��Ŀ�ĵ�ַ��ͨ������������
     * - *a_piSrc == -1    ��Ϊ�㲥��ַ����������ƥ��Ŀ�ĵĵ�ַ*a_piDst��ͨ������������
     * - �����ֵַ        ָ������Դ��ַ��ֻ��ƥ���Դ��ַ��ͨ���ſ��ܻᷢ������
     * @param[in,out] a_piDst ָ���������ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������\n
     * ����ָ�����ݵ�Ŀ�ĵ�ַ�� �����ʹ��ͨ����Ŀ�ĵ�ַ��\n
     * �������룬��ֵ��Ϊ�������������
     * - *a_piDst == 0     ��Ϊȱʡֵ���Զ�ȡ��һ��������Ϣ�м�¼��·�ɵ�ַ��Ϣ��ΪĿ�ĵ�ַ
     * - *a_piDst == -1    ��Ϊ�㲥��ַ����������ƥ��Դ��ַ��ͨ������������
     * - �����ֵַ        ָ������Ŀ�ĵ�ַ��ֻ��ƥ���Ŀ�ĵ�ַ��ͨ���ſ��ܻᷢ������
     *
     * @param[in] a_pvData ���淢�����ݵĻ�����֧��
     * @param[in] a_iDataLen ���������ݵĳ���
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @pre \e ���ӿ�Ҫ��������������֮ǰ���յ���һ��ͨ��tbus_forward�����������ݰ�
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_pvData ����ΪNULL
     * @pre a_iDataLen ��Чȡֵ��Χ [1, 0x7FFFFFFF]
     */
    TSF4G_API int tbus_backward(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                                IN const void *a_pvData,    IN const size_t a_iDataLen,    IN const int a_iFlag);


    /**
     * @brief �书����tbus_backward���ƣ������Ǳ��ӿڽ�iovec�и��������е�������װ��һ����Ϣ����
     *
     * @param[in] a_iHandle tbus�����ͨ������tbus_new()��ȡ
     * @param[in,out] a_piSrc �ο� tbus_backward
     * @param[in,out] a_piDst �ο� tbus_backward
     * @param[in] a_ptVector data vector buffers
     * @param[in] a_iVecCnt data vector count
     * @param[in] a_iFlag �����ֶΣ������Ʊ�־֧��'��(|)'��ʽ���ʹ�ã�Ŀǰ�Ѷ���ı�־λ�У�
     * - TBUS_FLAG_START_DYE_MSG    �Է�����Ϣ����Ⱦɫ����
     * - TBUS_FLAG_KEEP_DYE_MSG    �̳���һ����Ϣ��Ⱦɫ���ٵĹ��ܣ��������һ�����յ���Ϣ��Ⱦɫ�ģ���˷�����ϢҲ����Ⱦɫ
     *
     * @retval 0 -- successful, sent ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @see tbus_get_dyedmsgid
     * @pre \e ���ӿ�Ҫ��������������֮ǰ���յ���һ��ͨ��tbus_forward�����������ݰ�
     * @pre a_piSrc ����ΪNULL
     * @pre a_piDst ����ΪNULL
     * @pre a_ptVector ����ΪNULL
     * @pre a_iVecCnt �������0
     * @pre ���������ݵ��ܳ��ȵ���Чȡֵ��Χ [1, 0x7FFFFFFF]
     * @see tbus_backward
     */
    TSF4G_API int tbus_backwardv(IN const int a_iHandle,    INOUT TBUSADDR *a_piSrc,    INOUT TBUSADDR *a_piDst,
                                 IN const struct iovec *a_ptVector,  IN const int a_iVecCnt,     IN const int a_iFlag);



    /**
     * @brief ��ȡ��һ��Ⱦɫ��Ϣ�������롣
     * ������tbusϵͳ�ӿڵ��ϲ�Ӧ�öԷ���/���յ���Ϣ����Ⱦɫ���ٺ��������ô˽ӿڣ�
     * ���Ի�ȡ����������Ⱦɫ��Ϣ�������룬�ϲ�Ӧ�ü�¼�������룬�Ϳɱ��ֶ�Ⱦɫ��Ϣ���١�
     *
     * @retval Ⱦɫ��Ϣ��������
     */
    TSF4G_API unsigned int tbus_get_dyedmsgid();

    /**
     * @brief ��ȡͨ�����˶�����ʣ����Ϣ���ֽ�����
     *
     * @param[in] a_hHandle tbus���
     * @param[in] a_iLocalAddr ��tbusͨ������ڱ��˵ĵ�ַ
     * @param[in] a_iPeerAddr ��tbusͨ������ڶԶ˵ĵ�ַ
     * @param[in] a_piInFlux ��ȡ����ͨ��(a_iLocalAddr <-- a_iPeerAddr)ʣ����Ϣ�ֽ�����ָ��
     * @param[in] a_piOutFlux ��ȡ���ͨ��(a_iLocalAddr --> a_iPeerAddr)ʣ����Ϣ�ֽ�����ָ��
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     */
    TSF4G_API int tbus_get_channel_flux(IN int a_hHandle, IN TBUSADDR a_iLocalAddr, IN TBUSADDR a_iPeerAddr,
                                        OUT int *a_piInFlux, OUT int *a_piOutFlux);

    /** @} */

    /** @defgroup TBUS_TIMESTAMP_AND_HEARTBEAT TBUS_��Ϣʱ������ƺ���������
     * @{
     */
    /**
     * @brief ���õ�ǰʱ��
     *
     * @param[in] a_pstTime
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @pre a_pstTime����ΪNULL
     */
    TSF4G_API int tbus_set_current_time(IN const struct timeval *a_pstTime);

    /**
     * @brief ʹ��ͨ��tbus_set_current_time���õ�ʱ����Ϊʱ����ĳ�ֵ�Լ��������Ƶĵ�ǰʱ��
     *
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @note ���Ӧ�õ����˱��ӿڣ���Ҫ��ʱ����tbus_set_current_timeˢ��tbus����ĵ�ǰʱ�䡣
     * - �����ÿ�η�����ʹ��gettimeofday(����ͨ������tbus_use_sys_time����)���ԣ�
     * - tbus_use_saved_time������ʱ�侫ȷ�ȣ�����˳̶�ִ��Ч�ʡ�
     * @see tbus_use_sys_time
     * @see tbus_init_ex
     */
    TSF4G_API int tbus_use_saved_time(void);

    /**
     * @brief ʹ��ͨ��gettimeofday��ȡ��ʱ����Ϊʱ����ĳ�ֵ�Լ��������Ƶĵ�ǰʱ��
     *
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     * @see tbus_use_saved_time
     * @see tbus_init_ex
     */
    TSF4G_API int tbus_use_sys_time(void);

    /**
     * @brief ��ȡǰһ����ȡ����Ϣ��ʱ�����Ϣ
     *
     * @param[in] a_iHandle       ָ��tbus������������tbus_new��ȡ
     * @param[out] a_pstTimeStamp ���շ��ص�ʱ�����Ϣ
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iHandle������һ����Ч�ľ��
     * @pre a_pstTimeStamp����ΪNULL
     */
    TSF4G_API int tbus_get_msg_timestamp(IN int a_iHandle, OUT LPTBUSTIMESTAMP a_pstTimeStamp);

    /**
     * @brief ��һ��ͨ��������Ϣʱ��¼ʱ����Ĺ���
     *
     * @param[in] a_iHandle       ָ��tbus������������tbus_new��ȡ
     * @param[in] a_iAddr1        ͨ��һ�˵ĵ�ַ
     * @param[in] a_iAddr2        ͨ����һ�˵ĵ�ַ
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iHandle������һ����Ч�ľ��
     * @pre a_iAddr1������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @pre a_iAddr2������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @see tbus_turn_timestamp_off
     * @see tbus_set_handle_opt
     * @see tbus_send
     * @see tbus_sendv
     * @see tbus_forward
     * @see tbus_backward
     */
    TSF4G_API int tbus_turn_timestamp_on(IN int a_iHandle, IN TBUSADDR a_iAddr1, IN TBUSADDR a_iAddr2);

    /**
     * @brief �ر�һ��ͨ��������Ϣʱ��¼ʱ����Ĺ���
     *
     * @param[in] a_iHandle       ָ��tbus������������tbus_new��ȡ
     * @param[in] a_iAddr1        ͨ��һ�˵ĵ�ַ
     * @param[in] a_iAddr2        ͨ����һ�˵ĵ�ַ
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iHandle������һ����Ч�ľ��
     * @pre a_iAddr1������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @pre a_iAddr2������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @see tbus_turn_timestamp_on
     * @see tbus_set_handle_opt
     * @see tbus_send
     * @see tbus_sendv
     * @see tbus_forward
     * @see tbus_backward
     */
    TSF4G_API int tbus_turn_timestamp_off(IN int a_iHandle, IN TBUSADDR a_iAddr1, IN TBUSADDR a_iAddr2);

    /**
     * @brief ȡ�ñ��ر����a_iPeerAddr��Ӧ�Ľ��̵�״̬��Ϣ
     *
     * @param[out] a_pstPeerState ָ�����״̬��Ϣ�ı��ر���
     * @param[in]  a_iHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in]  a_iPeerAddr    ָ��ͨ���Զ˽��̵ĵ�ַ
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_pstPeerState����ΪNULL
     * @pre a_iPeerAddr������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @note ��Ӧ��ͨ��tbus_bind/tbus_bind_by_string��ĳ��ͨ��ʱ��
     * ͨ���Ķ�Ӧ��stPeerState��llLastBeatTime��������Ϊϵͳ�ĵ�ǰʱ��(ͨ��gettimeofdayȡ��)��
     * ͨ���Ķ�Ӧ��stPeerState��iIsAlive������Ϊ1, �����Զ˽����������С�
     * ��Ȼ���ִ�������̫�������ǿ��Ա�����̸ո�����ʱ�Ͳ�ѯ��iIsAlive==0�����������
     * ���Ӧ�ò���Ҫ�ڽ���֮��(ͨ��tbus)����ĳЩ��Ϣ�󣬲���Ϊ�Է������������У�
     * ��ôʹ��TBUSPEERSTATE.iIsAlive��ֵ��Ϊ�����Ƿ��������е��жϱ�׼�Ͳ�̫����
     * ʵ���ϣ�����tbusͨ���Ŀ���ͨ��Ŀǰ��û��ʵ�֣��˽ӿڷ��ص�TBUSPEERSTATE.iIsAliveʼ��==1.
     */
    TSF4G_API int tbus_get_peer_state_detail(OUT LPTBUSPEERSTATE a_pstPeerState, IN int a_iHandle, IN TBUSADDR a_iPeerAddr);

    /**
     * @brief ȡ��a_iHandle�������һ��ͨ���ĶԶ˵�ַ
     *
     * @param[in]  a_iHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in, out] a_piPeerAddr   ����0��ǰһ�ε���tbus_get_next_peer_addr���صĵ�ַ�������һ��ͨ���ĶԶ˵�ַ
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_piPeerAddr����ΪNULL
     * @see tbus_get_first_peer_addr
     * @note ����*a_piPeerAddr == 0, ȡ�õ�һ��peer�ĵ�ַ
     */
    TSF4G_API int tbus_get_next_peer_addr(IN int a_iHandle, INOUT TBUSADDR *a_piPeerAddr);

    /**
     * @brief ���ñ��ر����a_iPeerAddr��Ӧ�Ľ��̵ĳ�ʱʱ��
     *
     * @param[in]  a_iHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in]  a_iPeerAddr    ָ��ͨ���Զ˽��̵ĵ�ַ
     * @param[in]  a_iTimeout     ָ����ʱʱ��
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iPeerAddr������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     */
    TSF4G_API int tbus_set_peer_timeout_gap(IN int a_iHandle, IN TBUSADDR a_iPeerAddr, IN int a_iTimeout);

    /**
     * @brief ��ѯ���ر����a_iPeerAddr��Ӧ�Ľ����Ƿ�ʱ
     *
     * @param[out] a_piPreState   ָ�����ǰһ��״̬��Ϣ�ı��ر���
     * @param[out] a_piCurState   ָ����յ�ǰ״̬��Ϣ�ı��ر���
     * @param[in]  a_iHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in]  a_iPeerAddr    ָ��ͨ���Զ˽��̵ĵ�ַ
     * @param[in]  a_iNeedGetCurTime��ָʾtbus�Ƿ�ʹ��gettimeofdayȡ�õ�ǰʱ��
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iPeerAddr������һ���ض��ĵ�ַ������Ϊ�����ַ(0,-1)
     * @note ȡ�õĽ���״̬��enum����tagTbusPeerStateType��ֵ
     * @note �����ص�*a_piPreState!=*a_piCurStateʱ��
     * - ��tbus_get_peer_states�ĵ��û����peer��״̬��
     * - ��*a_piCurState��ֵ����*a_piPreState��ֵ��
     * @see tbus_init_ex
     * @see tbus_use_saved_time
     * @see tbus_use_sys_time
     */
    TSF4G_API int tbus_get_peer_states(OUT int *a_piPreState, OUT int *a_piCurState, IN int a_iHandle,
                                       IN TBUSADDR a_iPeerAddr, IN int a_iNeedGetCurTime);

    /**
     * @brief ��a_iPeerAddr��Ӧ�Ľ��̵ķ���������Ϣ
     *
     * @param[in]  a_iHandle      ָ��tbus������������tbus_new��ȡ
     * @param[in]  a_iPeerAddr    ָ��ͨ���Զ˽��̵ĵ�ַ
     * @param[in]  a_iFlag        �ӿڲ�����־λ:
     * - TBUS_INIT_FLAG_USE_SAVED_TIME
     * ����tbusʱ���������������Ҫʹ��ϵͳʱ��ʱ��ʹ��ͨ��tbus_set_current_time�����ʱ�䡣
     * - TBUS_FLAG_START_DYE_MSG & TBUS_FLAG_KEEP_DYE_MSG
     * ���͵���Ϣ��Ҫ����Ⱦɫ����
     * - TBUS_FLAG_SET_TEST_MSG
     * ������Ϣ��־λ

     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_iPeerAddr����Ϊ�����ַ0(TBUS_ADDR_DEFAULT)
     */
    TSF4G_API int tbus_send_heartbeat(IN int a_iHandle, IN TBUSADDR a_iPeerAddr, IN int a_iFlag);

    /** @} */



    /** @defgroup TBUS_ROUTE_MNG TBUS_·����Ϣ����
     * @{
     */


    /**
     * @brief get current package's route information
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in,out] a_ptRouteVec vector to be stored route information, the first one is the latest route
     * @param[in,out] a_piCnt route vector count, if get package route ok, this value would be set to real route count
     *
     * @retval 0 -- successful, get route information ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_ptRouteVec ����ΪNULL
     * @pre a_piCnt ����ΪNULL�����䱣�������ֵ�������0
     */
    TSF4G_API int tbus_get_pkg_route(IN const int a_iHandle, INOUT HEADROUTE *a_ptRouteVec, INOUT int *a_piCnt);

    /**
     * @brief set current package's route information
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_ptRouteVec vector to store route information, the first one is the latest route
     * @param[in] a_iCnt route vector count, indicated how many routes should be set into package header
     *
     * @retval 0 -- successful, set route information ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @pre a_ptRouteVec ����ΪNULL
     * @pre a_iCnt �䱣�������ֵ�������0
     */
    TSF4G_API int tbus_set_pkg_route(IN const int a_iHandle, IN const HEADROUTE *a_ptRouteVec, IN const int a_iCnt);




#define tbus_save_pkg_header    tbus_save_bus_head
    /**
     * @brief saved current package's header into internal buffer
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     *
     * @retval 0 -- successful, saved ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function should be invoked explicitly
     */
    TSF4G_API int tbus_save_bus_head(IN const int a_iHandle);

#define tbus_restore_pkg_header    tbus_restore_bus_head
    /**
     * @brief restored package's header
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     *
     * @retval 0 -- successful, restored ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function should be invoked explicitly
     */
    TSF4G_API int tbus_restore_bus_head(IN const int a_iHandle);


#define tbus_get_pkg_header    tbus_get_bus_head
    /**
     * @brief get current package's header and save it into external buffer
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in,out] a_pvBuffer value to be stored return header buffer
     * @param[in,out] a_piLen buffer length as in value, and if get header successfully, this value would be set as real buffer length
     *
     * @retval 0 -- successful, get ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function should be invoked explicitly
     * @pre a_pvBuffer ����ΪNULL
     * @pre a_piLen ����ΪNULL�����䱣�������ֵ�������0
     */
    TSF4G_API int tbus_get_bus_head(IN const int a_iHandle, INOUT void *a_pvBuffer, INOUT int *a_piLen);


#define tbus_set_pkg_header    tbus_set_bus_head
    /**
     * @brief set current package's header via specified external data
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_pvBuffer value to store header buffer
     * @param[in] a_iLen buffer length
     *
     * @retval 0 -- successful, set ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function should be invoked explicitly
     * @pre a_pvBuffer ����ΪNULL
     * @pre a_iLen �䱣�������ֵ�������0
     */
    TSF4G_API int tbus_set_bus_head(IN const int a_iHandle, IN const void *a_pvBuffer, IN const int a_iLen);

    /**
     * @brief get and delete previous node's bus address from bus head of current package
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in,out] a_piAddr pointer to output buffer
     *
     * @retval 0 -- successful, set ok
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function should be invoked explicitly
     * @pre a_piAddr ����ΪNULL
     */
    TSF4G_API int tbus_pop_previous_addr(IN const int a_iHandle, INOUT TBUSADDR* a_piAddr);

    /**
     * @brief get destination address list via specified external data
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in,out] a_piDstList vector buffer to store destination list
     * @param[in,out] a_piVecCnt  vector count as in value, and if get list successfully, this value would be set as real destination address count
     *
     * @retval 0 -- successful
     * @retval !0 -- failed, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     * @note this function is added at 2008/03/27 to meet FO2's needs
     * @pre a_piDstList ����ΪNULL
     * @pre a_piVecCnt ����ΪNULL�����䱣�������ֵ�������0
     */
    TSF4G_API int tbus_get_dst_list(IN const int a_iHandle, INOUT unsigned int *a_piDstList, INOUT int *a_piVecCnt);

    /** @} */


    /** @defgroup TBUS_MONITOR TBUS_ͨ��״̬���Ϳ���
     * @{
     */

    /**
     * ����������Щ�ӿ��ڶ�ͨ�����м�غ��޸�ʱʹ�ã�����ͨ��ʱ����ʹ����Щ�ӿ�
     */

    /**
     * @brief get the used proportion of channel recvside queue
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_iPeerAddr �Է�bus��ַ����ϢԴ��ַ����֧�������ַ
     * @param[in] a_iLocalAddr ����bus��ַ����ϢĿ�ĵ�ַ����֧�������ַ
     * @param[out] a_pfUsed ��ȡ���Ķ��пռ�ʹ�ñ�����0~1֮��ĸ����͡�����ΪNULL
     *
     * @retval 0    SUCCESS
     *         !0   FAILED
     */
    TSF4G_API int tbus_get_channel_recvside_used_proportion(IN const int a_iHandle, IN TBUSADDR a_iPeerAddr, IN TBUSADDR a_iLocalAddr, OUT float* a_pfUsed);


    /**
     * @brief get the used proportion of channel sendside queue
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_iLocalAddr ����bus��ַ����ϢԴ��ַ����֧�������ַ
     * @param[in] a_iPeerAddr �Է�bus��ַ����ϢĿ�ĵ�ַ����֧�������ַ
     * @param[out] a_pfUsed ��ȡ���Ķ��пռ�ʹ�ñ�����0~1֮��ĸ����͡�����ΪNULL
     *
     * @retval 0    SUCCESS
     *         !0   FAILED
     */
    TSF4G_API int tbus_get_channel_sendside_used_proportion(IN const int a_iHandle, IN TBUSADDR a_iLocalAddr, IN TBUSADDR a_iPeerAddr, OUT float* a_pfUsed);

    /**
     * @brief empty the channel recvside queue
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_iPeerAddr �Է�bus��ַ����ϢԴ��ַ����֧�������ַ
     * @param[in] a_iLocalAddr ����bus��ַ����ϢĿ�ĵ�ַ����֧�������ַ
     *
     * @retval 0    SUCCESS
     *         !0   FAILED
     */
    TSF4G_API int tbus_empty_channel_recvside(IN const int a_iHandle, IN TBUSADDR a_iPeerAddr, IN TBUSADDR a_iLocalAddr);


    /**
     * @brief empty the channel sendside queue
     *
     * @param[in] a_iHandle tbus handle, acquired by tbus_new()
     * @param[in] a_iLocalAddr ����bus��ַ����ϢԴ��ַ����֧�������ַ
     * @param[in] a_iPeerAddr �Է�bus��ַ����ϢĿ�ĵ�ַ����֧�������ַ
     *
     * @note �˺������̰߳�ȫ��������ͨ�ŵĶԷ����̵��� tbus_empty_channel_recvside �����Ϣ����
     *
     * @retval 0    SUCCESS
     *         !0   FAILED
     */
    TSF4G_API int tbus_empty_channel_sendside(IN const int a_iHandle, IN TBUSADDR a_iLocalAddr, IN TBUSADDR a_iPeerAddr);
    /** @} */

    /** @defgroup TBUS_LOG TBUS_��־����
     * @{
     */

    /**
     * @brief set log category instance for tbus log system
     *
     * @param[in] a_pstLogCat category instance of tbus log system
     *
     */
    TSF4G_API void tbus_set_logcat(IN LPTLOGCATEGORYINST a_pstLogCat);

    /**
     * @brief set priority  of log category  used by tbus
     *
     * @param[in] a_iPriority Priority  log category  used by tbus
     */
    TSF4G_API void tbus_set_logpriority(IN int a_iPriority);

    /**
     * @brief ����ʹ���Զ���-��Ⱥģʽʱ��zookeeper api����־�ļ����
     * @note ��������ñ��ӿڣ���ôĬ��ʹ��/tmp/tbus_zk.log
     *       ��������libtsf4g_zk.a����libtbus_zk.aʱ����ʹ�ñ��ӿڣ�����ᱨ���Ӵ���
     *       ���ӿ���Ҫ��tbus_automatic_init֮ǰ����, ��ֻ�ܵ���һ��
     * @param[in] a_pszLogFile ��־�ļ����ļ�·��
     * @retval 0  SUCCESS
     *         !0 FAILED
     */
    TSF4G_API int tbus_automatic_set_zookeeper_log_file(IN const char* a_pszLogFile);

    /** @} */


	/** @defgroup TBUS_MSG_NOTIFY TBUS_��Ϣ֪ͨ����
     * @{
     */

    /**
     * @brief ֪ͨ�Զ˼�ʱ������Ϣ
	 *
	 * @note �˺���ֻ�������Զ���ģʽ���ο�tbus_automatic_init��˵��
	 * @note ��(һ�λ��߶��)����tbus_send��tbus_sendv��tbus_forward��
	 *       tbus_forwardv��tbus_backward, tbus_backwardv������Ϣ֮��,
	 *       �����Ҫ֪ͨ�Է���ʱ������Ϣ�����ô˺���֪ͨ��
	 *
	 * @see tbus_proc_events
	 *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     * @param[in] a_iSrc ָ�����ݵ�Դ��ַ���ο�tbus_send�Ĳ���˵��
     * @param[in] a_iDst ָ�����ݵ�Ŀ�ĵ�ַ���ο�tbus_send�Ĳ���˵��
     *
     * @retval 0 -- �ɹ�����֪ͨ��Ϣ
     * @retval !0 -- ʧ��, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     */
    TSF4G_API int tbus_send_notify(IN const int a_iHandle, IN TBUSADDR a_iSrc, IN TBUSADDR a_iDst);

	/**
     * @brief ��ȡ�¼��������ӵ�������
	 *
	 * @note �˺���ֻ�������Զ���ģʽ���ο�tbus_automatic_init��˵��
	 *
     * @retval >=0  -- UDP �׽���������
     * @retval <0 -- ʧ��, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
	 *
	 * @note ���ͨ���˺���ȡ�õ���������ע�ᵽEPOLL�У�ֻ֧��ˮƽ������ʽ��
     *
     */
    TSF4G_API int tbus_get_event_fd();

	/**
     * @brief ��⵽ �¼��������ӵ������� ���¼�����ʱ��Ӧ���ô˺�������
	 * �˺�������¼��������ӽ���֪ͨ���������Ϣ֪ͨ������ȡ��Դ��ַ��Ŀ�ĵ�ַ��
	 * �����������a_piSrc, a_piDst������Ϣ֪ͨ�����ʱ�����������a_piSrc, a_piDstΪͨ���ַ��
	 *
	 * @note �˺���ֻ�������Զ���ģʽ���ο�tbus_automatic_init��˵��
	 * @note �����¼������ṩ���ɿ��ķ��񣬵���������TBUS_HAVE_MESSAGEʱ�����ܲ�û�����ݿɽ��ա�
	 *       ���ڿ����ж���֪ͨ��Ϣ������ѭ�����ô˺�����������TBUS_HAVE_MESSAGEʱ����ѭ��, �������ѭ����
	 *
	 * @param[out] a_piSrc ����������ݵ�Դ��ַ��ͨ��tbus_addr_aton()ת��������
     * @param[out] a_piDst ������ݵ�Ŀ�ĵ�ַ��ͨ��tbus_addr_aton()ת��������
	 * @param[in] events������������Ŀǰ��������Ϊ 0
	 *
     * @retval 0 -- �ɹ�������Ϣ
     * @retval 1 (TBUS_HAVE_MESSAGE) -- �ɹ�������Ϣ
     * @retval <0 || > 1 -- ʧ��, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     */
    TSF4G_API int tbus_proc_events(OUT TBUSADDR* a_piSrc, OUT TBUSADDR* a_piDst, IN int events);

    /**
     * @brief tbusd�洢�˿ͻ��˵�tbus��ַ��udp socket��ַӳ����Ϣ����Ҫ��ʱ���µ�ַӳ���
     * ��ʱ��tbusd���͵�ַ���������� tbusd����������Ǩ�ƶ��¼����Ƶ�Ӱ�죬��֤�¼����Ƶ���Ч�ԡ�
	 *
	 * @note Ϊ�˱�֤�¼����Ƶ���Ч�ԣ���Ҫ���ڵ��ô˺���������ÿ�����ٵ���һ�Ρ�
     *
	 * @note �˺���ֻ�������Զ���ģʽ���ο�tbus_automatic_init��˵��
     *
     * @param[in] a_iHandle tbus��������ͨ������tbus_new()��ȡ
     *
     * @retval 0 -- �ɹ�
     * @retval !0 -- ʧ��, �����tbus_error_string�ӿ�ȡ�ô���ԭ��
     *
     */
    TSF4G_API int tbus_event_keep_alive(IN const int a_iHandle);
    /*
    ��ȡchannle ���һ����Ϣ��·����Ϣ.
    */
    TSF4G_API void tbus_get_channel_route(IN LPTBUSCHANNEL a_pstChannel, char* ptbus_head_buff);
	/** @} */


#ifdef __cplusplus
}
#endif


#endif /**< _TBUS_H */

