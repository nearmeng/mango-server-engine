/*
 **  @file $RCSfile: tqqapi.h,v $
 **  general description of this module
 **  $Id: tqqapi.h,v 1.27 2009/09/28 07:52:15 hardway Exp $
 **  @author $Author: hardway $
 **  @date $Date: 2009/09/28 07:52:15 $
 **  @version $Revision: 1.27 $
 **  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
 **  @note Platform: Linux
 */

#ifndef TQQAPI_H
#define TQQAPI_H

#include "pal/pal.h"
#include "tdr/tdr.h"

#include "tqqdef.h"
#include "tpdudef.h"

#include "tcltapi/tsock5proxy.h"
#include "tcltapi/thttpproxy.h"


#ifdef __cplusplus
extern "C"
{
#endif


    /** @defgroup tqqapi
     * @{
     */


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
#define  TQQAPI_WSOCKET_DEFAULT_BUFF  1024*1024
#define  TQQAPI_RSOCKET_DEFAULT_BUFF  1024*1024
#else
#define TQQAPI_WSOCKET_DEFAULT_BUFF   128*1024
#define  TQQAPI_RSOCKET_DEFAULT_BUFF   128*1024
#endif

#define TQQAPI_DEFAULT_LOGPATTEN   "./tqqapi_log/tqqapi_%Y-%m-%d.log"
#define TQQAPI_DEFAULT_LOGSIZE         8*1024*1024


    typedef enum
    {
        TQQAPI_LOG_CRIT = 200,          /*Critical Level*/
        TQQAPI_LOG_ERROR = 300,       /*Error Level */
        TQQAPI_LOG_INFO = 600,         /*Info Level */
        TQQAPI_LOG_DEBUG = 700,      /*Debug Level */
        TQQAPI_LOG_NULL = -1,           /*ALL Level*/
    } TQQAPI_LOG_PRIORITY;


    /** ������ */
    typedef enum
    {
        TQQAPI_ERR_NETWORK_FAILURE          =1,    	    /**< �հ�ʧ��,�������*/
        TQQAPI_ERR_TDR_NTOH_HEAD             =2,    	    /**< ��ͷ����ת������ʧ��*/
        TQQAPI_ERR_NOT_ENOUGHBUF             =3,  	        /**< ���ջ���������*/
        TQQAPI_ERR_DECRYPTION_HEAD          =4,   	        /**< ��ͷ����ʧ��*/
        TQQAPI_ERR_TDR_NTOH_BODY              =5, 	        /**< ��������ת������ʧ��*/
        TQQAPI_ERR_TDR_HTON_HEAD             =6,  	        /**< ��ͷ����ת������ʧ��*/
        TQQAPI_ERR_TDR_HTON_BODY             =7,  	        /**< ���屾��ת������ʧ��*/
        TQQAPI_ERR_RECV_TIMEOUT                =8,  	    /**< ���а��հ���ʱ*/
        TQQAPI_ERR_DECRYPTION_BODY          =9,  	        /**< ���ܰ���ʧ��*/
        TQQAPI_ERR_INVALID_CMD                  =10,        	 /**< �Ƿ���ͷ����*/
        TQQAPI_ERR_TDR_HTON_USERIDENT   =11,	        /**< �ڲ�����*/
        TQQAPI_ERR_BUILD_AUTH                    =12, 	        /**< �ڲ�����*/
        TQQAPI_ERR_BUILD_SYNACK                 =13,	        /**< �ڲ�����*/
        TQQAPI_ERR_HANDLE_INVALID             =14,	        /**<���ָ��Ƿ�*/
        TQQAPI_ERR_ENCPYT_MSG	                 =15,           /**<������Ϣʧ��*/
        TQQAPI_ERR_SENDBUF_LIMITED          =16,             /**<���ͻ�����ʣ��ռ䲻��������Ϣ�ĳ���*/
        TQQAPI_ERR_RECVBUFF_LIMITED          =17,            /**<���ջ������ռ䲻������һ��������Ϣ*/
        TQQAPI_ERR_SYSTEM                             =18,             /* *< ϵͳ�ڲ�����*/
        TQQAPI_ERR_SOCKSEND_FAIL                   =19,             /* *<���Ͱ�ʧ��,�������*/
        TQQAPI_ERR_SOCKSENDBUF_FULL             =20,             /* *<���Ͱ�ʧ��,socket ����������*/
        TQQAPI_ERR_SOCKSEND_UNCOMPLETED    =21,             /* *<���Ͱ�ʧ��,socket������ֻ�����˲�������*/
        TQQAPI_ERR_RECV_STOP                              =22,         /* *<�ͻ����յ��������Ͽ�����֪ͨ*/
        TQQAPI_ERR_COUNT
    }TQQAPI_ERR;


    struct tagTQQApiHandle;

    typedef struct tagTQQApiHandle		TQQAPIHANDLE;

    typedef struct tagTQQApiHandle		*HTQQAPI;

    extern LPTDRMETA g_pstqqapiPduHead;

#define TQQAPI_PDUHEAD_HTON(net, host, ver)		tdr_hton(g_pstqqapiPduHead, net,host, ver)

#define TQQAPI_PDUHEAD_NTOH(host,net, ver)		tdr_ntoh(g_pstqqapiPduHead, host, net, ver)


    /**
     *@brief: ��־�ص���������,a_pArgΪ�ص�����,a_iLogLevel�趨��־�������
     */

    typedef int (*TQQAPI_PFN_LOG)(IN HTQQAPI a_phQQClt,IN int a_iLogLevel,IN void *a_pArg,const char* a_pszFmt, ...);


    /**
     *@brief  ���tconnd �����汾
     *@retval   ���ذ汾������Ϣ
     */
    TSF4G_API char* tqqapi_get_version(void);

    /**
     *@brief: ʹ��tqqapi֮ǰ��Ҫ�ȵ���tqqapi_create��ʼ��
     *@param:
     *@retval   <0     fail
     *@retval   =0     success
     */
    TSF4G_API int  tqqapi_create(void);


    /**
     *@brief:���:
     *@param         pszBuff[IN]:		���ջ�������ʼ��ַ
     *@param         iBuff[IN]:     		����
     *@param         pstHead[OUT]:	��ͷ��Ϣ
     *@param         piHeadLen[OUT] 	�������
     *@retval         = 0      for success
     *@retval         <0       failed,����tdr������
     *@see tdr_error_string(iRet)
     */
    TSF4G_API int tqqapi_decode(IN const char* pszBuff,IN int iBuff,OUT TPDUHEAD* pstHead,OUT int* piHeadLen);


    /**
     *@brief:���
     *@param        pszBuff[IN,OUT]: 	���������
     *@param        iBuff[IN,OUT]:     	[IN]����������
     *                                               [OUT]�������
     *@param        pstHead[IN]:         ��ͷ
     *@retval           =0    for success
     *@retval           <0    failed,����tdr������
     *@see tdr_error_string(iRet)
     */
    TSF4G_API int tqqapi_encode(INOUT char* pszBuff,INOUT int* piBuff, IN TPDUHEAD* pstHead);





    /**
     *@brief  ���ӷ��������ɹ��������Ӿ��
     *@param      a_pszUri�{[IN]       URL ��ַeg:172.25.40.97:9088
     *@param      a_iTimeout [IN]       ����������ʱʱ��
     *@param      a_phQQClt [OUT]    ���Ӿ��
     *@retval       >=0      success,          ����socket
     *@retval       =-1       connect ʧ��,telnet url �Ƿ�����
     *@retval       =-2        �����ͳ�ʼ�����ʧ��
     *@retval       =-3       WSAStartup����ʧ��,WSAGetLastError
     */
    TSF4G_API int tqqapi_open(IN const char* a_pszUri,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);

    /**
     *@brief  ͨ��Sock5�������ӷ�����,���ɹ��������Ӿ��
     *@param      a_pHTSock5Param�{[IN]       sock5�������
     *@param      a_iTimeout [IN]       ����Sock5����ʱʱ��
     *@param      a_phQQClt [OUT]    ���Ӿ��
     *@retval       >=0      success,          ����socket
     *@retval       =-1        WSAStartup����ʧ��,WSAGetLastError
     *@retval       =-2        �����ͳ�ʼ�����ʧ��
     *@retval       <-100        ����sock5������,��������뺬��ο�tsock5proxy.h ��TSOCK5_ERRORCODE����
     */
    TSF4G_API int tqqapi_open_sock5(IN  LPTSOCK5PARAM a_pHTSock5Param,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);


    /**
     *@brief  ͨ��Sock5�������ӷ�����,���ɹ��������Ӿ��
     *@param      a_pHTSock5Param�{[IN]       sock5�������
     *@param      a_iTimeout [IN]       ����Sock5����ʱʱ��
     *@param      a_phQQClt [OUT]    ���Ӿ��
     *@retval       >=0      success,          ����socket
     *@retval       =-1        WSAStartup����ʧ��,WSAGetLastError
     *@retval       =-2        �����ͳ�ʼ�����ʧ��
     *@retval       <-200        ����http���������,��������뺬��ο�thttpproxy.h ��THTTP_ERRORCODE����
     */
    TSF4G_API int tqqapi_open_http(IN  LPTHTTPPARAM a_pHttpParam,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);



    /**
     *@brief ����APIĬ����־ϵͳ��־������־·��,���������ɾ�������ȵ��øú���ע�ⲻͬ���ʵ�����ʹ�ò�ͬa_pszLogPattern,������־д��,
     *@param        a_hQQClt[IN]    		���
     *@param        a_iLogLevel[IN]            ��־�������,���嶨��ο�TQQAPI_LOG_PRIORITY����
     *@param        a_pszLogPattern[IN]     ��־�ļ�·��,��Ϊ��ʹ��Ĭ����־�ļ�·����TQQAPI_DEFAULT_LOGPATTEN:��./tqqapi_log/tqqapi_%Y-%m-%d.log.%F��ʾ�����ڼ�¼��־����format�����ο�strftime
     *@retval 		  =0       success
     *@retval           <0       fail
     *@retval           = -1    �����Ƿ�
     *@retval           =-2     ��ʼ��Logʧ��
     *@retval           =-3     ��ȡLogʵ��ʧ��
     */
    TSF4G_API int 	tqqapi_set_log_default(IN HTQQAPI a_phQQClt, IN int a_iLogLevel, IN const char *a_pszLogPattern);




    /**
     *@brief ���þ��Ӧ���Զ�����־ϵͳ�ص�����
     *@param       a_hQQClt[IN]    		   ���
     *@param       a_pfnLogCallBack[IN]       ��־�ص�����
     *@param       a_iLogLevel[IN]                �趨��־�������,��Ҫ�����ֶα��浽�ص�������,�Ա�֧�ֲ�ͬ��־�����������
     *@param       a_pArg                            ��־�����ص�����
     *@retval 	       =0 success
     *@retval        <0  fail
     */
    TSF4G_API int 	tqqapi_set_log_callback(IN HTQQAPI a_phQQClt, IN TQQAPI_PFN_LOG a_pfnLogCallBack,IN int a_iLogLevel,IN void* a_pArg);





    /**
     *@brief ���ݰ���meta���䷢��/���ջ�������С
     *@param      a_hQQClt[IN] 	     ���Ӿ��
     *@param      a_pstSendMeta[IN]    ����tdr �ṹmeta
     *@param      a_pstRecvMeta[IN]    ����tdr �ṹmeta,��һ��ͬ����meta
     *@retval	      =0       for success
     *@retval       < 0      for fail
     *@retval       = -1    �����������
     *@retval       =-2     calloc failed,����ڴ��Ƿ񲻹�
     */
    TSF4G_API int tqqapi_set_pdu_meta(IN HTQQAPI a_hQQClt,IN LPTDRMETA a_pstSendMeta,IN  LPTDRMETA a_pstRecvMeta);



    /**
     *@brief   ����tconnd,���ӿ�Ϊ����ʹ��,��ʽ���������ͻ���ʹ��ͳһ��¼ϵͳ����ϵ�����������ȡ���SDK
     *@param   a_phQQClt[IN] 	      ���Ӿ��
     *@param   uin[IN]			      QQ��
     *@param   a_pszSvrkey[IN]	      16�ֽ�ǩ����Կ
     *@param   a_iAuthType[IN]        QQǩ������,��Ҫ��Ϊ3,�����tconnsecdef.h��tagTCONN_SEC_AUTH
     *@param   a_iTimeOut[IN]         �հ�������ʱʱ��
     *@param   a_iEncMethod[IN]       һ����Ϊ3,�����tconnsecdef.h��tagTCONN_SEC_ENC
     *@param   a_iServiceID[IN]       ��ϷID

     *@note        a_iAuthType=3      	0xdeͳһǩ������
     *@note	    a_iEncMethod=3		taes�ӽ����㷨


     *@retval             0:for success
     *@retval            <0:for fail
     *@retval             use tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_create_initial_connection(IN HTQQAPI a_phQQClt,IN unsigned int a_uin, IN const char *a_pszSvrkey,IN int a_iAuthType,IN int a_iTimeOut,IN int a_iEncMethod,IN int a_iServiceID);



    /**
     *@brief ��������tconnd,���ӿ�Ϊ���Խӿ�,��ʽ���������ͻ���ʹ��ͳһ��¼ϵͳ����ϵ�����������ȡ���SDK
     *@param  	   a_phQQClt[IN] 		���Ӿ��
     *@param          uin[IN]				QQ��
     *@param          nPos[IN]			����ռλ����
     *@param          szConnectKey[IN]	16 �ֽ� ��Կ
     *@param           szIdentity[IN]		16 �ֽ���֤��
     *@param          iRelayType[IN]           ��������,Ŀǰһ��Ϊ1
     *@param          a_iTimeOut[IN]           �հ���ʱʱ��
     *@param          iEncMethod[IN]            �ӽ��ܷ���,һ����3,,�����tconnsecdef.h��tagTCONN_SEC_ENC

     *@note            iRelayType =1             �����ת
     *@note            iRelayType =2             ��������
     *@note            iEncMethod = 3           taes�ӽ����㷨


     *@retval             0:for success
     *@retval            <0:for fail
     *@retval             use tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_create_relay_connection(IN HTQQAPI a_phQQClt,IN unsigned int a_uin,IN int nPos,IN const char * szConnectKey,IN const char *szIdentity,IN int a_iTimeOut,IN int iRelayType,IN int iEncMethod);



    /**
     *@brief ������Ϣ
     *@param    a_hQQClt[IN] 		���Ӿ��
     *@param    a_pstHead[IN] 		PDU ��ͷ, ����ͨ����Ϣ,��ͷ������ΪNULL
     *@param    a_pszMsg[IN]		Ӧ����Ϣ���ͻ�����
     *@param    a_iMsg[IN]                ��Ϣ��С
     *@param    a_iTimeout[IN]	       �������ͳ�ʱʱ��: ��Ϊ-1�ȴ����ͳɹ�,��Ϊ���ȴ���λ/ms

     *@retval		 =0:for success
     *@retval          <0:for fail
     *@retval          use tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_send_msg(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN const char* a_pszMsg,IN int a_iMsg,IN int a_iTimeout);



    /**
     *@brief ������Ϣ
     *@param      a_hQQClt[IN] 		 	���Ӿ��
     *@param      a_pstHead[IN]			������Ϣ��PDU��ͷ,�������İ�ͷ������ΪNULL
     *@param      a_pszMsg[IN,OUT]		Ӧ����Ϣ����ջ�����
     *@param      a_iMsg[IN,OUT]		���ջ�������С(IN)
     *                                                      ��Ϣ���С(OUT)
     *@param      a_iTimeout[IN]			���ճ�ʱʱ��: ��������Ϊ-1,��Ϊ���ȴ���λ/ms

     *@retval       =1:receive one package
     *@retval       =0:receive NULL
     *@retval       <0:for fail
     *@retval       use tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_recv_msg(IN HTQQAPI a_hQQClt,OUT TPDUHEAD* a_pstHead,INOUT char* a_pszMsg,INOUT int* a_piMsg,IN int a_iTimeout);



    /**
     *@brief ������Ϣ�����ֽ���
     *@param    a_hQQClt[IN] 		���Ӿ��
     *@param    a_pstHead[IN] 		PDU ��ͷ, ����ͨ����Ϣ,��ͷ������ΪNULL
     *@param    a_pszBuffer[IN]		Ӧ����Ϣ���ͻ�����
     *@param    iBufferLen[IN]           ��Ϣ�����ֽ�����С
     *@param    a_iTimeout		      �������ͳ�ʱʱ��: ��Ϊ-1�ȴ����ͳɹ�,��Ϊ���ȴ���λ/ms

     *@retval		 =0   for success
     *@retval          <0   for fail
     *@retval          use tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_send_buffer(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN const char* a_pszBuffer,IN int iBufferLen,IN int a_iTimeout);



    /**
     *@brief ������Ϣ�����ֽ���
     *@param      a_hQQClt[IN] 		 	���Ӿ��
     *@param      a_pstHead[IN]			������Ϣ��PDU��ͷ,�������İ�ͷ������ΪNULL
     *@param      a_pszBuffer[IN,OUT]	Ӧ����Ϣ����ջ�����
     *@param      a_iMsg[IN,OUT]		���ջ�������С(IN)
     *	                                                 ��Ϣ�����ֽ�����С(OUT)
     *@param      a_iTimeout[IN]			���ճ�ʱʱ��: ��������Ϊ-1,��Ϊ���ȴ���λ/ms

     *@retval       =1      receive one package
     *@retval       =0      receive NULL
     *@retval       <0       for fail
     *@retval       use     tqqapi_get_err/tqqapi_get_errorstring��ô����������ʾ��Ϣ
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_recv_buffer(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN  char* a_pszBuffer,INOUT int * piBufferLen,IN int a_iTimeout);


    /**
     *@brief   ��þ��������
     *@param  a_hQQClt[IN]  ���Ӿ��
     *@retval  ���ش�����
     *@see    TQQAPI_ERR
     */
    TSF4G_API int tqqapi_get_err(IN HTQQAPI a_hQQClt);



    /**
     *@brief   ��þ��������Ϣ
     *@param  a_hQQClt[IN]  ���Ӿ��
     *@retval  �����ַ���
     */
    TSF4G_API const char * tqqapi_get_errstring(IN HTQQAPI a_hQQClt);


    /**
     *@brief  �ͷž��
     *@param  a_phQQClt[IN]  ���Ӿ��ָ��
     *@retval  =0     success
     *@retval  <0     fail
     */
    TSF4G_API int tqqapi_free(IN HTQQAPI* a_phQQClt);



    /**
     *@brief ����socket ���ͻ�����
     *@param    a_hQQClt[IN]    		���
     *@param    a_hQQClt[IN]    		��������С
     *@retval
     *@retval 		0        success
     *@retval         <0     fail
     */
    TSF4G_API int 	tqqapi_set_sendbuff(IN HTQQAPI a_phQQClt,IN  int a_iSize);


    /**
     *@brief ����socket ���ջ�����
     *@param    a_hQQClt[IN]    		���
     *@param    a_hQQClt[IN]    		��������С
     *@retval
     *@retva 		0       success
     *@retva         <0     fail
     */
    TSF4G_API int 	tqqapi_set_recvbuff(IN HTQQAPI a_phQQClt, IN int a_iSize);



    /**
     *@brief  ��ȡ���յ�ԭʼ���ݣ�����֮ǰ������ʵ�ʴ�������ݣ����ܴ�С
     *@param    a_hQQClt   ���
     *@retval  ���յ�ԭʼ���ݣ�����֮ǰ������ʵ�ʴ�������ݣ����ܴ�С
     */
    TSF4G_API uint64_t tqqapi_getTotalRecvLen(HTQQAPI a_hQQClt);

    /**
     *@brief  ��ȡ���͵��������ݣ�����֮�������ʵ�ʴ�������ݣ����ܴ�С
     *@param    a_hQQClt   ���
     *@retval  ���͵��������ݣ�����֮�������ʵ�ʴ�������ݣ����ܴ�С
     */
    TSF4G_API uint64_t tqqapi_getTotalSendLen(HTQQAPI a_hQQClt);




    /*
     * @name  ---------------�ڲ��ӿ�-------------
     * @{
     */



    /**
     *@brief ��ʼ�����
     *@param    a_hQQClt   		���

     *@retva 		   =0      success
     *@retva            <0      fail
     *@retva            =-1    �����Ƿ�
     *@retva            =-2    ��ȡTPDUBase Meta����
     *@retva            =-3    ��ȡTPDUHead Meta����
     * @deprecated
     */
    TSF4G_API int 	tqqapi_init(HTQQAPI a_hQQClt);


    /**
     *@brief ������ ��ʼ�����
     *@param    a_hQQClt   		���

     *@retval  	     =0   success
     *@retval             <0    fail
     *@retval             =-1  �����Ƿ�
     *@retval             =-2  ��ʼ���������
     * @deprecated
     */
    TSF4G_API int 	tqqapi_new(HTQQAPI* a_phQQClt);

    /**
     *@brief ��ʼ��ͷ��������Ϣ
     *@param    a_pstBase   		ͷ��������Ϣ�ṹ��
     *@retval
     */
    TSF4G_API void      tqqapi_init_base(TPDUBASE* a_pstBase);


    /**
     *@brief ���þ��ͨ��Socket
     *@param    a_hQQClt   		���
     *@param    a_s   		       ͨ��Socket
     *@retval
     */
    TSF4G_API void 	tqqapi_attach(HTQQAPI a_hQQClt,int a_s);

    /**
     *@brief ���ؾ��ͨ��Socket
     *@param    a_hQQClt   		���
     *@retval     ���ͨ��Socket
     *@retval                >=0  ����socket
     *@retval                <0    �쳣socket
     */
    TSF4G_API int		tqqapi_get_sock(HTQQAPI a_phQQClt);

    /**
     *@brief ���ؾ��ͨ��ƾ��
     *@param    a_hQQClt   		���
     *@param    a_iPos   		λ������ָ��
     *@param    a_pszIdentity   	λ����֤�ַ���
     *@retval
     */
    TSF4G_API void 	tqqapi_get_identity(HTQQAPI a_hQQClt, int *a_iPos, char * a_pszIdentity);

    /**
     *@brief ���þ��ͨ��ƾ��
     *@param    a_hQQClt   		���
     *@param    a_iPos   		λ��������Ϣ
     *@param    a_pszIdentity   	λ����֤�ַ���
     *@retval
     */
    TSF4G_API void	tqqapi_set_identity(HTQQAPI a_hQQClt, int a_iPos,const char * a_pszIdentity);

    /**
     *@brief ����ͨ��˫���汾,Ŀǰ�Ѳ���
     *@param    a_hQQClt   		���
     *@param    a_iSelfVersion   	����ͨ�Ű汾
     *@param    a_iPeerVersion   �Զ˰汾
     *@retval
     *@deprecated
     */
    TSF4G_API void 	tqqapi_set_version(HTQQAPI a_hQQClt,int a_iSelfVersion, int a_iPeerVersion);

    /**
     *@brief ����ͨ��˫��ͨ����Կ
     *@param    a_hQQClt   		���
     *@param    pszGameKey   	16�ֽ�ͨ����Կ
     *@param    iEncMethod         ͨ�ż��ܷ���
     *@retval     ����ֵ
     *@retval      = 0      success
     *@retval      <0       fail
     *@retval       =-1     ���ܷ�������
     *@retval      =-2     ������Կ����
     */
    TSF4G_API int 	tqqapi_set_gamekey(HTQQAPI a_hQQClt, const char* pszGameKey,int iEncMethod);


    /**
     *@brief ��ȡͨ��˫��ͨ����Կ
     *@param    a_hQQClt   		���
     *@param    pszGameKey   	16�ֽ�ͨ����Կ
     *@retval     ����ֵ
     *@retval     = 0  success
     *@retval     <0   fail
     *@retval     =-1  û����Կ
     */
    TSF4G_API int  	tqqapi_get_gamekey(HTQQAPI a_hQQClt, char* pszGameKey);

    /**
     *@brief ��ȡtqqapi MetaLib
     *@param    a_hQQClt   		���
     *@retval     ����MetaLib ��ַ
     */
    TSF4G_API const char*   tqqapi_get_meta_data(void);


    /**
     *@brief ж�ؾ��ͨ��Socket
     *@param    a_hQQClt   		���
     *@retval    ��ǰ�� ͨ��Socket
     *@retval    >=0     ����socket
     *@retval    <0       �쳣socket
     */
    TSF4G_API int          tqqapi_detach(HTQQAPI a_hQQClt);


    /**
     *@brief��ȡ���ͨ�ŵ�ǰ�������к�
     *@param    a_hQQClt   		���
     *@retval   ��ǰ�������к�
     */
    TSF4G_API unsigned int    tqqapi_get_sequence(HTQQAPI a_hQQClt);

    /**
     *@brief���þ��ͨ���������к�
     *@param    a_hQQClt   		���
     *@param    iSequence          �������к�
     *@retval
     */
    TSF4G_API void        tqqapi_set_sequence(HTQQAPI a_hQQClt,unsigned int iSequence);

    /**
     *@brief  ��TPDUHEAD ����ȡͨ��ƾ����Ϣ
     *@param    a_hQQClt   		���
     *@param    a_pstHead          TPDUHEAD
     *@param    pstIdentInfo         ƾ����Ϣ

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD Э�����Ͳ���
     *@retval     =-2    ��ȡЭ������Metaʧ��
     *@retval     =-3    ������Ϣʧ��
     *@retval     =-4    ���ƾ����Ϣʧ��
     */
    TSF4G_API int          tqqapi_extract_identinfo(HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead, TPDUIDENTINFO* pstIdentInfo);

    /**
     *@brief  ��TPDUHEAD ����ȡͨ����Կ
     *@param    a_hQQClt   		���
     *@param    a_pstHead          TPDUHEAD
     *@param    pszSessionkey     16�ֽ���Կ,����Ԥ��16 �ֽڿռ�

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD Э�����Ͳ���
     *@retval     =-2    ������Կ��Ϣʧ��
     */
    TSF4G_API int           tqqapi_extract_Sessionkey(HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead,char *pszSessionkey);

    /**
     *@brief  ��TPDUHEAD ����ȡ��������֤��Ϣ
     *@param    a_hQQClt   		���
     *@param    a_pstHead          TPDUHEAD
     l
     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD Э�����Ͳ���
     *@retval     =-2    ������Կ��Ϣʧ��
     *@retval     =-3    ��ȡЭ��Metaʧ��
     *@retval     =-4    ���Syn ��Ϣʧ��
     */
    TSF4G_API int           tqqapi_extract_syninfo(HTQQAPI a_hQQClt,TPDUHEAD* a_pstHead);

    /**
     *@brief  ����0x82 ����ǩ��TQQAUTHINFO ��Ϣ
     *@param    pstAuthInfo   	 ǩ���ṹ��
     *@param    pstGameSig        �����0x82 ǩ����һ�νṹ
     *@param    pstSigForS2        �����0x82 ǩ���ڶ��νṹ
     *@param    uin                     QQ��
     *@param    pszSvrSKey        16�ֽڼ�����Կ��Ϣ

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TDR ���0x82 ǩ����һ�νṹʧ��
     *@retval     =-2    TDR ���0x82 ǩ���ڶ��νṹʧ��
     */
    TSF4G_API int           tqqapi_make_authinfo(TQQAUTHINFO* pstAuthInfo, TQQGAMESIG* pstGameSig, TQQSIGFORS2* pstSigForS2, unsigned int uin, char* pszSvrSKey);

    /**
     *@brief  ��֤0x82 ����ǩ��TQQAUTHINFO ��Ϣ�Ƿ���ȷ
     *@param    pstGameSig        �����0x82 ǩ����һ�νṹ
     *@param    pstSigForS2        �����0x82 ǩ���ڶ��νṹ
     *@param    uin                     QQ��
     *@param    pstAuthInfo         ǩ���ṹ��
     *@param    pszSvrSKey        16�ֽڽ�����Կ��Ϣ

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    ����0x82 ǩ����һ�νṹʧ��
     *@retval     =-2    TDR ���0x82 ǩ����һ�νṹʧ��
     *@retval     =-3    ����0x82 ǩ���ڶ��νṹʧ��
     *@retval     =-4   TDR ���0x82 ǩ���ڶ��νṹʧ��
     */
    TSF4G_API int          tqqapi_extract_authinfo(TQQGAMESIG* pstGameSig, TQQSIGFORS2* pstSigForS2, unsigned int uin, TQQAUTHINFO* pstAuthInfo,char* pszSvrSKey);


    /**
     *@brief  ����0xDE ����ǩ��TQQUNIFIEDAUTHINFO ��Ϣ
     *@param    pstUnifyAuthInfo   ǩ���ṹ��
     *@param    uin                     QQ��
     *@param    pszSvrSKey        16�ֽڼ�����Կ��Ϣ
     *@param    pszSessionkey    16�ֽ�ͨ�ų�ʼ��Կ��Ϣ
     *@param    uCltIP                 �ͻ���IP

     *@retval
     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   TDR ���0xDE ǩ���ṹʧ��
     *@retval     =-2   TDR ���0xDE ǩ��Э��ʧ��
     */
    TSF4G_API int         tqqapi_make_QQUnify_Authinfo(TQQUNIFIEDAUTHINFO *pstUnifyAuthInfo,unsigned int uin ,char* pszSvrSKey,char *pszSessionkey,unsigned int uCltIP);


    /**
     *@brief  ��֤0xDE����ǩ��TQQUNIFIEDAUTHINFO ��Ϣ�Ƿ���ȷ
     *@param    pstUnifyAuthInfo  ǩ����Ϣ
     *@param    pszSvrSKey        16�ֽڽ�����Կ��Ϣ
     *@retval
     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   TDR ���ǩ����Ϣʧ��
     *@retval     =-2   ����0xDE ǩ����Ϣʧ��
     *@retval     =-3   TDR ���ǩ���ṹʧ��
     */
    TSF4G_API int         tqqapi_extract_QQUnify_Authinfo(TQQUNIFIEDAUTHINFO *pstUnifyAuthInfo,char* pszSvrSKey);

    /**
     *@brief  ���ɲ�����֤��
     *@param    a_hQQClt         ���
     *@param    a_pstHead        TPDUHEAD
     *@param    a_uin                QQ��
     *@param    pszSvrkey         16�ֽڼ�����Կ
     *@param    iAuthType          ǩ������
     *@param    uCltIP               �ͻ���IP
     *@param    iEncMethod        ���ܷ���
     *@param    iServiceID        ҵ��APPID

     *@retval     =0  success
     *@retval     <0  fail
     *@retval     =-1    �����Ƿ�
     *@retval     =-2    ����ͨ����Կ����
     *@retval     =-3    ���ɲ���ǩ������
     *@retval     =-4    ��֤����ǩ������
     *@retval     =-5    ǩ�����ͷǷ�
     */
    TSF4G_API int		tqqapi_build_auth_msg( HTQQAPI a_hQQClt, TPDUHEAD * a_pstHead, unsigned int a_uin, const char *pszSvrkey,int iAuthType,unsigned int uCltIP,int iEncMethod,int iServiceID);

    /**
     *@brief  �����������������
     *@param  a_hQQClt    ���
     *@param    a_pstHead        TPDUHEAD

     *@retval      =0      success
     *@retval      <0      fail
     *@retval      =-1    ��ȡЭ��Meta��
     *@retval      =-2    ������������ʧ��
     *@retval      =-3    ���ܳ���

*/
    TSF4G_API int 	tqqapi_build_synack_msg(HTQQAPI a_hQQClt,TPDUHEAD* a_pstHead);


    /**
     *@brief  �������������
     *@param    a_hQQClt         ���
     *@param    a_pstHead        TPDUHEAD
     *@param    a_uin                QQ��
     *@param    nPos                 λ��������Ϣ
     *@param    szConnectKey    16�ֽ�ͨ����Կ
     *@param    szIdentity          16�ֽ�������֤��Ϣ
     *@param    iRelayType         ��������
     *@param    iEncMethod        ���ܷ���

     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   �����Ƿ�
     *@retval     =-2   ��ȡЭ��Meta����
     *@retval     =-3   ����ͨ����Կ����
     *@retval     =-4   TDR�������ƾ֤��Ϣ����
     *@retval     =-5   ���ܳ���

*/
    TSF4G_API int       tqqapi_build_relay_msg( HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead,unsigned int uin,int nPos,const char * szConnectKey,const char *szIdentity,int iRelayType,int iEncMethod);

    /**
     *@brief  ����������URL,���ɹ�����socket
     *@param a_pszUri�{[IN]:URL ��ַeg:172.25.40.97:9088
     *@retval       >=0    success,����socket
     *@retval       <0      ����url����
     */
    TSF4G_API int 	tqqapi_open2(const char* a_pszUri);

    /**
     *@brief  ���ص�ǰ�������Ӧ�����ݴ�С
     *@param    a_hQQClt         ���
     *@retval  ����Ӧ�����ݴ�С
     */
    TSF4G_API int		tqqapi_getBufferdLen(HTQQAPI a_hQQClt);

    /**
     *@brief  ������һ�ξ����socket ��ȡ���ݴ�С
     *@param    a_hQQClt         ���
     *@retval    ��һ����ȡ���ݴ�С
     */
    TSF4G_API int		tqqapi_getRecvdLen(HTQQAPI a_hQQClt);

    /**
     *@brief  ������һ�ξ�����͵�socket ���ݴ�С
     *@param    a_hQQClt         ���
     *@retval     ���͵�socket ���ݴ�С
     */
    TSF4G_API int		tqqapi_sendLen(HTQQAPI a_hQQClt);

    /**���ܽӿ�,
     *@param[in] a_iNeedCopy �����������Ϣ�����ֶ����ڿ��� �Ƿ���Ϣ����������������������޸�ָ�룬��ֵΪ0������������ֵΪ��0������Ҫ����
     */
    TSF4G_API  int       tqqapi_encrypt_i(HTQQAPI a_hQQClt, const char *a_pszInBuf, int a_iInLen, char **a_pszOutBuf,int *a_piOutLen, int a_iNeedCopy);

    /**���ܽӿ�,
     *@param[in] a_iIsPlain �����Ϣͷ��������ΪTPDU_CMD_PLAIN����Ϣ������
     *@param[in] a_iNeedCopy �����������Ϣ�����ֶ����ڿ��� �Ƿ���Ϣ����������������������޸�ָ�룬��ֵΪ0������������ֵΪ��0������Ҫ����
     *@retval    �ɹ�������
     */
    TSF4G_API  int       tqqapi_decrypt_i(HTQQAPI a_hQQClt, const char *a_pszInBuf, int a_iInLen, char **a_pszOutBuf,int *a_piOutLen, int a_iIsPlain, int a_iNeedCopy);



    /** @}*/ // ---------------�ڲ��ӿ�-------------





    /** @} */



#ifdef __cplusplus
}
#endif

#endif


