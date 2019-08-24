/*
 **  @file $RCSfile: tcltapi.h,v $
 **  general description of this module
 **  $Id: tcltapi.h,v 1.4 2009-05-20 08:34:22 hardway Exp $
 **  @author $Author: hardway $
 **  @date $Date: 2009-05-20 08:34:22 $
 **  @version $Revision: 1.4 $
 **  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
 **  @note Platform: Linux
 */

#ifndef TCLTAPI_H
#define TCLTAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup tcltapi
     * @{
     */

#include "pal/pal.h"
#include "tdr/tdr.h"

#ifndef IN
#define  IN
#endif

#ifndef OUT
#define  OUT
#endif

#ifndef INOUT
#define INOUT
#endif


#ifdef WIN32
#define  TCLTAPI_WSOCKET_DEFAULT_BUFF  1024*1024
#define  TCLTAPI_RSOCKET_DEFAULT_BUFF  1024*1024
#else
#define  TCLTAPI_WSOCKET_DEFAULT_BUFF   128*1024
#define  TCLTAPI_RSOCKET_DEFAULT_BUFF   128*1024
#endif

#define  TCLTAPI_DEFAULT_LOGSIZE            8*1024*1024

#define TCLTAPI_DEFAULT_LOGPATTEN   "./tcltapi_log/tcltapi_%Y-%m-%d.log"



    typedef enum
    {
        TCLTAPI_LOG_CRIT = 200,          /*Critical Level*/
        TCLTAPI_LOG_ERROR = 300,       /*Error Level */
        TCLTAPI_LOG_INFO = 600,         /*Info Level */
        TCLTAPI_LOG_DEBUG = 700,      /*Debug Level */
        TCLTAPI_LOG_NULL = -1,           /*ALL Level*/
    } TCLTAPI_LOG_PRIORITY;



    typedef enum
    {
        TCLTAPI_ERR_NET                        =1,        /**< �������*/
        TCLTAPI_ERR_TDR                        =2,        /**< TDR����ʧ��*/
        TCLTAPI_ERR_BUF                        =3,       /**< ���ջ���������*/
        TCLTAPI_ERR_INVALID_INPUT              =4,       /**< ��������Ƿ�*/
        TCLTAPI_ERR_INVALID_PKG_LENGTH         =5,    /**< �����ȷǷ�*/
        TCLTAPI_ERR_COUNT
    }TCLTAPI_ERR;


    struct tagTCltapiHandle;
    typedef struct tagTCltapiHandle     TCLTAPIHANDLE;
    typedef struct tagTCltapiHandle     *HTCLTAPI;

    /**
     *@brief: ��־�ص���������,ֻ����a_pArgΪ�ص�����,a_iLogLevel�趨��־�������,һ����ҪӦ�ý���Ϣ���浽�ص�����
     */

    typedef int (*TCLTAPI_PFN_LOG)(IN HTCLTAPI a_phClt, IN int a_iLogLevel,IN void * a_pArg,const char* a_pszFmt, ...);



    /**
     *@brief     �������.���ӶԶ˲����ؾ��socket = tcltapi_new + tcltapi_make_connect+tcltapi_attach
     *@param   a_phClt[IN]            �Զ�URL
     *@param   a_iTimeout[IN]        ��ʱʱ��
     *@param   a_phClt[IN]            API ���

     *@retval    = 0   �ɹ�
     *@retval   < 0    ʧ��
     *@retval   -1     ����ʧ��
     *@retval   -2      �������ʧ��
     *@retval   -3      WSAStartup����ʧ��,WSAGetLastError

    */
    TSF4G_API int tcltapi_open(IN const char* a_pszUri, IN int a_iTimeout, IN HTCLTAPI* a_phClt);

    /**
     *@brief     �������.���ӶԶ˲����ؾ��socket = tcltapi_new + tcltapi_make_connect+tcltapi_attach
     *@param   a_phClt[IN]            �Զ�URL
     *@param   a_iTimeout[IN]        ��ʱʱ��
     *@param   a_phClt[IN]            API ���
     *@param   a_iIsTlvMode[IN]      �Ƿ�tdr2.0 ��Ϊtrue

     *@retval    = 0   �ɹ�
     *@retval   < 0    ʧ��
     *@retval   -1     ����ʧ��
     *@retval   -2      �������ʧ��
     *@retval   -3      WSAStartup����ʧ��,WSAGetLastError

    */
    TSF4G_API int tcltapi_open_tlv_mode(IN const char* a_pszUri, IN int a_iTimeout, IN HTCLTAPI* a_phClt, IN int a_iIsTlvMode);

    /**
     *@brief     ע��tdrģʽ
     *@param   a_phClt[IN]        API ���
     *@param   a_bIsTlvMode[IN]   �Ƿ�tdr2.0 ��Ϊtrue
     *@retval
     *@retval      =0   �ɹ�
     *@retval      < 0   ʧ��
     *@retval      =-1  �����������
     */
    TSF4G_API int tcltapi_set_mode(IN HTCLTAPI* a_phClt, IN int a_iIsTlvMode);


    /**
     *@brief     �ͷž��
     *@param   a_phClt[IN]        API ���
     *@retval
     *@retval      =0   �ɹ�
     *@retval      < 0   ʧ��
     *@retval      =-1  �����������
     */
    TSF4G_API int tcltapi_free(IN HTCLTAPI* a_phClt);



    /**
     *@brief     ���þ��Ĭ����־ϵͳ��־������־·��
     *@param   a_phClt[IN]            API ���
     *@param   a_iLogLevel[IN]        ��־����,���嶨��ο�
     *@param   a_phClt[IN]            ��־�ļ�·��,��Ϊ��ʹ��Ĭ����־�ļ�·����TCLTAPI_DEFAULT_LOGPATTEN:��./tcltapi_log/tcltapi_%Y-%m-%d.log.%F��ʾ�����ڼ�¼��־����format�����ο�strftime
     *@retval    = 0      �ɹ�
     *@retval    < 0      ʧ��
     *@retval    = -1   �����Ƿ�
     *@retval    =-2    ��ʼ��Logʧ��
     *@retval    =-3    ��ȡLogʵ��ʧ��
     */
    TSF4G_API int tcltapi_set_log_default(IN HTCLTAPI a_hClt, IN int a_iLogLevel, IN const char *a_pszLogPattern);



    /**
     *@brief ���þ��Ӧ���Զ�����־ϵͳ�ص�����
     *@param       a_hClt[IN]               ���
     *@param       a_pfnLogCallBack[IN]       ��־�ص�����
     *@param       a_iLogLevel[IN]                �趨��־�������,��Ҫ�����ֶα��浽�ص�������,�Ա�֧�ֲ�ͬ��־�����������
     *@param       a_pArg                            ��־�����ص�����
     *@retval            =0 success
     *@retval        <0  fail
     */
    TSF4G_API int     tcltapi_set_log_callback(IN HTCLTAPI a_hClt, IN TCLTAPI_PFN_LOG a_pfnLogCallBack,IN int a_iLogLevel,IN void* a_pArg);


    /**
     *@brief     ���þ��Ԫ��������
     *@param   a_phClt[IN]                API ���
     *@param   a_pstRecvMeta[IN]        ����meta
     *@param   a_pstSendMeta[IN]        ����meta
     *@retval    = 0      �ɹ�
     *@retval    < 0      ʧ��
     *@retval    -1        �����ڴ�ʧ��
     */
    TSF4G_API int tcltapi_set_pdu_meta(IN HTCLTAPI a_hClt,IN  LPTDRMETA a_pstRecvMeta,IN LPTDRMETA a_pstSendMeta);


    /**
     *@brief     ���þ������Ԫ���ݳ���������Ա·��,��Ҫ�ȵ���tcltapi_set_pdu_meta
     *@param   a_phClt[IN]                API ���
     *@param   a_szHeadLenPath[IN]        head meta·��
     *@param   a_szBodyLenPath[IN]        body meta·��,NULL��ʾֻ��a_szHeadLenPath���Ա�ʾpkglen����

     *@retval    = 0    �ɹ�
     *@retval    < 0    ʧ��
     */
    TSF4G_API int tcltapi_set_pkglen_indicator(IN HTCLTAPI a_hClt,IN  const char * a_szHeadLenPath,IN const char *  a_szBodyLenPath);



    /**
     *@brief     ���ó���Э��Զ˰汾,һ�㲻����
     *@param   a_phClt[IN]                API ���
     *@param   a_iSelfVersion[IN]        �ͻ��˰汾
     *@param   a_iPeerVersion[IN]        �������汾
     *@retval
     */
    TSF4G_API void tcltapi_set_version(IN HTCLTAPI a_hClt, IN int a_iSelfVersion,IN int a_iPeerVersion);


    /**
     *@brief     ���������
     *@param   a_phClt[IN]                API ���
     *@param   a_pszBuff[IN]            ���绺������ַ
     *@param   a_iBuff[IN]                ���ݳ���
     *@param   a_iTimeout[IN]            ���ͳ�ʱʱ��,-1��������,��λΪ����,һ����Ϊ��

     *@retval    = 0     �ɹ�
     *@retval    <0      ʧ��
     *@see    ʹ��tcltapi_get_error��tcltapi_get_errorstring��ô�����ʾ��Ϣ
     */
    TSF4G_API int tcltapi_send_buff(IN HTCLTAPI a_hClt,IN const char* a_pszBuff,IN int a_iBuff,IN int a_iTimeout);



    /**
     *@brief     ���������
     *@param   a_phClt[IN]                API ���
     *@param   a_pszBuff[IN]            ���ݻ�������ַ
     *@param   a_piBuff[IN,OUT]            [IN]���ݻ���������,[OUT]��Ϣ������
     *@param   a_iTimeout[IN]            �հ���ʱʱ��,-1�����հ�,��λΪ����
     *@retval    =0    û���յ���
     *@retval    =1    �յ���
     *@retval    <0    �հ�ʧ��
     *@see    ʹ��tcltapi_get_error��tcltapi_get_errorstring��ô�����ʾ��Ϣ
     */
    TSF4G_API int tcltapi_recv_buff(IN HTCLTAPI a_hClt,IN  char* a_pszBuff,INOUT  int *a_piBuff,IN int a_iTimeout);


    /**
     *@brief     �ͷž��socket
     *@param   a_phClt[IN]        API ���
     *@retval      ���ص�ǰsocket
     */
    TSF4G_API int tcltapi_detach(IN HTCLTAPI a_hClt);


    /**
     *@brief     ���ؾ��socket
     *@param   a_phClt[IN]        API ���
     *@retval
     */
    TSF4G_API void tcltapi_attach(IN HTCLTAPI a_hClt,IN int a_s);


    /**
     *@brief     ���ͱ������ݽṹ
     *@param   a_phClt[IN]                API ���
     *@param   a_pszMsg[IN]            �������ݵ�ַ
     *@param   a_iMsg[IN]                ���ݳ���
     *@param   a_iTimeout[IN]            ���ͳ�ʱʱ��,-1��������,��λΪ����,һ����Ϊ��

     *@retval        = 0    �ɹ�
     *@retval        <0     ʧ��
     *@see           tcltapi_get_error/tcltapi_get_errorstring�õ�����ֵ/��ʾ��Ϣ
     */
    TSF4G_API int tcltapi_send_msg(IN HTCLTAPI a_hClt, IN const char* a_pszMsg,IN int a_iMsg,IN  int a_iTimeout);


    /**
     *@brief     ���ͱ������ݽṹ
     *@param   a_phClt[IN]                API ���
     *@param   a_pszMsg[IN]            �������ݻ����ַ
     *@param   a_piMsg[INOUT]            [IN]���ݻ��峤��,[OUT]�յ� ���ݰ�����
     *@param   a_iTimeout[IN]            ���ճ�ʱʱ��,-1��������,��λΪ����,һ����Ϊ��

     *@retval    = 0    û���յ���
     *@retval    =1     �յ���
     *@retval    <0     ʧ��
     *@see        ��tcltapi_get_error/tcltapi_get_errorstring�õ�����ֵ/��ʾ��Ϣ
     */
    TSF4G_API int tcltapi_recv_msg(IN HTCLTAPI a_hClt, IN char* a_pszMsg,INOUT  int* a_piMsg, IN int a_iTimeout);


    /**
     *@brief     ��þ�� socket
     *@param   a_phClt[IN]                API ���
     *@retval   ����socket
     */
    TSF4G_API int tcltapi_get_sock(IN HTCLTAPI a_hClt);



    /**
     *@brief     ����socket ��������
     *@param   a_phClt[IN]                API ���
     *@param   a_iSize[IN]                          ��������С
     *@retval    =0 �ɹ�
     *@retval    !=0 ʧ��,ȡerror�õ�������ʾ��Ϣ
     */
    TSF4G_API int tcltapi_set_recvbuff(IN HTCLTAPI a_hClt,IN int a_iSize);


    /**
     *@brief     ����socket д������
     *@param   a_phClt[IN]                API ���
     *@param   a_iSize[IN]                          ��������С

     *@retval        =0    �ɹ�
     *@retval        !=0   ʧ��,ȡerror�õ�������ʾ��Ϣ
     */
    TSF4G_API int tcltapi_set_sendbuff(IN HTCLTAPI a_hClt,IN int a_iSize);


    /**
     *@brief      ��ô�����
     *@param   a_phClt[IN]                API ���
     *@retval    ���ش�����
     */
    TSF4G_API int tcltapi_get_err(IN HTCLTAPI a_hClt);


    /**
     *@brief     ��ô�����ʾ��Ϣ
     *@param   a_phClt[IN]                API ���
     *@retval    ���ش�����ʾ��Ϣ
     */
    TSF4G_API const char *tcltapi_get_errstring(IN HTCLTAPI a_hClt);



    /** @} */

#ifdef __cplusplus
}
#endif

#endif /* TCLTAPI_H */


