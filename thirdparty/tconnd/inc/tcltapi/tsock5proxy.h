/************************************************************
 *  File Module:    TSock5Proxy.h
 *  File Description: ͷ�ļ�
 *  File History:
 *  <author>  <time>            <version >   <desc>
 *  Hardway    2010/10/27         1.0           create this moudle
 ***********************************************************/




#ifndef _TSOCK5PROXY_H
#define _TSOCK5PROXY_H

#include "pal/pal.h"
#include "tcltapi/tsock5def.h"

#ifdef __cplusplus
extern "C"
{
#endif





#define TSOCK5_MAX_STRLEN  64
#define TSOCK5_DEFAULT_RSPTIMEOUT  3000     /*Ĭ��proxy��Ӧ��ʱʱ��*/



    typedef enum
    {
        TSOCK5_ERROR_NONE                               = 100,
        TSOCK5_ERROR_CALLOC_HANDLE         = 101,       /*calloc ���ʧ��*/
        TSOCK5_ERROR_INVALID_PROXYINFO      = 102,      /*proxyurl��ַ�Ƿ�*/
        TSOCK5_ERROR_INVALID_DSTINFO          =  103,     /*Ŀ�ķ�������ַ�Ƿ�*/
        TSOCK5_ERROR_CONNECT_PROXYSVR       = 104,      /*���Ӵ��������ʧ��*/
        TSOCK5_ERROR_SEND_NEGOREQ               = 105,      /*������֤Э�������ʧ�� */
        TSOCK5_ERROR_RECV_NEGORSP                 = 106,     /*������֤Э�̷��ذ��������*/
        TSOCK5_ERROR_RECV_NEGORSP_TIMEOUT= 107,     /*������֤Э�̷��ذ���ʱ*/
        TSOCK5_ERROR_RECV_NEGORSP_INVALID =  108,     /*������֤Э�̷��ذ��Ƿ�*/
        TSOCK5_ERROR_NOMETHOD_ACCEPT         = 109,       /*���������û�п�֧�ֵ�Э�̷���*/
        TSOCK5_ERROR_USRPWD_INVALID            = 110,       /*�û����������*/
        TSOCK5_ERROR_SEND_USRPWDREQ           = 111,       /*�����û���������֤��ʧ��*/
        TSOCK5_ERROR_RECV_USRPWDRSP            = 112,       /*�����û������뷵�ذ��������*/
        TSOCK5_ERROR_RECV_USRPWDRSP_TIMEOUT=113,    /*�����û������뷵�ذ���ʱ*/
        TSOCK5_ERROR_RECV_USRPWDRSP_INVALID =114,   /*�����û������뷵�ذ��Ƿ�*/
        TSOCK5_ERROR_AUTH_USRPWD_INVALID    =115,       /*�û���������֤ʧ��*/
        TSOCK5_ERROR_SEND_TCPCONNREQ            = 116,       /*��������Ŀ�ķ���������ʧ��*/
        TSOCK5_ERROR_RECV_TCPCONNRSP            = 117,       /*��������Ŀ�ķ�����Ӧ���������*/
        TSOCK5_ERROR_RECV_TCPCONNRSP_TIMEOUT=118,    /*��������Ŀ�ķ�����Ӧ�𷵻ذ���ʱ*/
        TSOCK5_ERROR_RECV_TCPCONNRSP_INVALID =119,   /*�����û������뷵�ذ��Ƿ�*/
        TSOCK5_ERROR_TCPCONN_FAIL                       =120,   /*�������������Ŀ�ķ�����ʧ��*/
        TSOCK5_ERROR_COUNT
    }
    TSOCK5_ERRORCODE;

    typedef struct
    {
        char szProxyUrl[TSOCK5_MAX_STRLEN];   /*����,�����������ַ:eg:10.0.1.11:1080*/
        unsigned int dwDstIP;                               /*����,Ŀ�ķ�����IP , ������*/
        unsigned short wDstPort;                         /*����,Ŀ�ķ�����Port ,������*/
        int    iProxyTimeOut;                              /*����,�����������Ӧ����ʱʱ��,����Ϊ��,��Ĭ��ֵΪ 3��*/
        char szUser[TSOCK5_MAX_STRLEN];         /*��ѡ,�����Ҫ�û�����������֤�����*/
        char szPasswd[TSOCK5_MAX_STRLEN];    /*��ѡ,�����Ҫ�û�����������֤�����*/
    } TSOCK5PARAM,*LPTSOCK5PARAM;


    struct tagTSOCK5PROXY;
    typedef struct tagTSOCK5PROXY  TSOCK5PROXY;
    typedef struct tagTSOCK5PROXY *LPHTSOCK5PROXY;


    /**
     *@brief  ����Sock5 ���
     *@param      a_pszUri�{[IN]       URL ��ַeg:172.25.40.97:9088
     *@param      a_iTimeout [IN]       ����������ʱʱ��
     *@param      a_phQQClt [OUT]    ���Ӿ��
     *@retval       >=0      success,          ����socket
     */
    TSF4G_API int tsock5_create(INOUT LPHTSOCK5PROXY *a_ppHTSock5,IN LPTSOCK5PARAM a_pHTSock5Param);

    TSF4G_API void tsock5_free(IN LPHTSOCK5PROXY *a_ppHTSock5);

    TSF4G_API int tsock5_connect(IN LPHTSOCK5PROXY a_pHTSock5,IN int a_nTimeout);

    TSF4G_API void tsock5_disconnect(IN LPHTSOCK5PROXY a_pHTSock5);

    TSF4G_API int tsock5_do_negotiation(IN LPHTSOCK5PROXY a_pHTSock5,IN int a_nTimeout);

    TSF4G_API int  tsock5_do_tcpconnect(IN LPHTSOCK5PROXY a_pHTSock5,IN int a_nTimeout);

    TSF4G_API int  tsock5_get_socket(IN LPHTSOCK5PROXY a_pHTSock5);




#ifdef __cplusplus
}
#endif

#endif
