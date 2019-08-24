#ifndef _THTTPPROXY_H
#define _THTTPPROXY_H

#include "pal/pal.h"
#include "tcltapi/thttpdef.h"


#ifdef __cplusplus
extern "C"
{
#endif



#define THTTP_MAX_STRLEN                    64
#define THTTP_DEFAULT_RSPTIMEOUT  3000

    typedef enum
    {
        BASE64_WITH_NONE =0x00,
        BASE64_WITH_CRLF = 0x01  /*ÿ76���ַ���CRLF*/
    }
    BASE64_FLAG;

    typedef enum
    {
        THTTP_ERROR_NONE                        = 200,
        THTTP_ERROR_CALLOC_HANDLE               = 201,       /*calloc ���ʧ��*/
        THTTP_ERROR_INVALID_PROXYINFO           = 202,      /*proxyurl��ַ�Ƿ�*/
        THTTP_ERROR_INVALID_DSTINFO             = 203,     /*Ŀ�ķ�������ַ�Ƿ�*/
        THTTP_ERROR_CONNECT_PROXYSVR            = 204,      /*���Ӵ��������ʧ��*/
        THTTP_ERROR_SEND_NEGOREQ                = 205,      /*������֤Э�������ʧ�� */
        THTTP_ERROR_RECV_NEGORSP                = 206,     /*������֤Э�̷��ذ��������*/
        THTTP_ERROR_RECV_NEGORSP_TIMEOUT        = 207,     /*������֤Э�̷��ذ���ʱ*/
        THTTP_ERROR_RECV_NEGORSP_INVALID        =  208,     /*������֤Э�̷��ذ��Ƿ�*/
        THTTP_ERROR_PROXY_FAIL                  =  210,     /*�������������״̬��Ƿ�*/
        THTTP_ERROR_DIGEST_NOTSUPPORT           = 211,     /*Digest��֤��ʽ��ʱ��֧��*/
        THTTP_ERROR_NTLM_NOTSUPPORT             = 212,    /*NTLM��֤��ʽ��ʱ��֧��*/
        THTTP_ERROR_AUTH_UNKNOWN                = 213,    /*������֤��ʽ��ʱ��֧��*/
        THTTP_ERROR_USRORPWD_INVALID          =214,     /*�û���������Ƿ�*/
        THTTP_ERROR_ENCBASE64_BASICSTR          = 215,   /*����base64 �û���������ʧ��*/
        THTTP_ERROR_SEND_BASICREQ               =216,     /*����basic��֤�����ʧ�� */
        THTTP_ERROR_RECV_BASICRSP               =217,    /*����basic��֤���ذ��������*/
        THTTP_ERROR_RECV_BASICRSP_TIMEOUT       =218,      /*����basic��֤���ذ���ʱ*/
        THTTP_ERROR_RECV_BASICRSP_INVALID       = 219,    /*����basic��֤���ذ��Ƿ�*/
        THTTP_ERROR_BASIC_AUTH_FAIL             = 220,    /*������basic��֤�û���������Ƿ�*/
        THTTP_ERROR_COUNT
    } THTTP_ERRORCODE;

    typedef struct
    {
        char szProxyUrl[THTTP_MAX_STRLEN];   /*����,�����������ַ:eg:10.0.1.11:1080  */
        unsigned int dwDstIP;                 /*����,Ŀ�ķ�����IP , ������  */
        unsigned short wDstPort;              /*����,Ŀ�ķ�����Port ,������  */
        int    iProxyTimeOut;                 /*����,�����������Ӧ����ʱʱ��,����Ϊ��,��Ĭ��ֵΪ 3��  */
        char szUser[THTTP_MAX_STRLEN];          /*��ѡ,��Ҫ�û��������뷽ʽ��ѡ*/
        char szPasswd[THTTP_MAX_STRLEN];      /*��ѡ,��Ҫ�û��������뷽ʽ��ѡ*/
        char szDomain[THTTP_MAX_STRLEN];     /*��ѡ, �û���,NTLM��֤��ʽ��ѡ*/
    } THTTPPARAM,*LPTHTTPPARAM;


    struct tagTHTTPPROXY;

    typedef struct tagTHTTPPROXY THTTPPROXY;
    typedef struct tagTHTTPPROXY *LPTHTTPPROXY;


    /**
     *@brief  ����HTTP���� ���
     *@param      a_ppHTTP [INOUT]    ���Ӿ��
     *@param      a_pHttpParam[IN]     �������ò���
     *@retval       >=0      success
     *@retval        <0    ���ش�����,������ο�THTTP_ERRORCODE
     */
    TSF4G_API int thttp_create(INOUT LPTHTTPPROXY *a_ppHTTP,IN LPTHTTPPARAM a_pHttpParam);

    /**
     *@brief  ����HTTP���� ���
     *@param      a_phQQClt [IN]    ���Ӿ��
     *@retval
     */
    TSF4G_API void thttp_free(IN LPTHTTPPROXY *a_ppHTTP);

    /**
     *@brief  ���Ӵ��������
     *@param      a_pHTTP [IN]    ���Ӿ��
     *@param      a_nTimeout[IN]   ��ʱʱ��,��Ϊ����ʹ��Ĭ�ϴ������ò����г�ʱʱ��
     *@retval     >= 0  success ,          ����socket
     *@retval     <0    ���ش�����,������ο�THTTP_ERRORCODE
     */
    TSF4G_API int thttp_connect(IN LPTHTTPPROXY a_pHTTP,IN int a_nTimeout);

    /**
     *@brief  �Ͽ����������
     *@param      a_pHTTP [IN]    ���Ӿ��
     *@retval     >= 0  success ,          ����socket
     *@retval     <0    ���ش�����,������ο�THTTP_ERRORCODE
     */
    TSF4G_API void thttp_disconnect(IN LPTHTTPPROXY a_pHTTP);



    /**
     *@brief  �������������֤
     *@param      a_pHTTP [IN]    ���Ӿ��
     *@retval     >= 0  success ,          ����socket
     *@retval     <0    ���ش�����,������ο�THTTP_ERRORCODE
     */
    TSF4G_API int thttp_do_negotiation(IN LPTHTTPPROXY a_pHTTP,IN int a_nTimeout);


    /**
     *@brief  ���� socket
     *@param      a_pHTTP [IN]    ���Ӿ��
     *@retval       ����socket
     */
    TSF4G_API int  thttp_get_socket(IN LPTHTTPPROXY a_pHTTP);


    /**
     *@brief  base64����
     *@param      pszInput [IN]    ���������ַ
     *@param      a_iInLen [IN]    ���볤��
     *@param      pszOutput [IN]  ���������ַ
     *@param      a_iOutLen [INOUT]   ����Ϊ���뻺��������,����Ϊ���볤������,���Ϊʵ�ʱ��볤��
     *@param      a_iFlag [IN]    �Ƿ�ʹ��MIME base64���뷽��,�ο�BASE64_FLAG����
     *@retval         >= 0  success
     *@retval        <0   fail
     *@retval        =-1 ������������Ȳ���
     */
    TSF4G_API int thttp_base64_encode(IN const char *pszInput,IN int a_iInLen,OUT char *pszOutput,INOUT int *a_iOutLen,IN int a_iFlag);


    /**
     *@brief  base64����
     *@param      pszInput [IN]    ���������ַ
     *@param      a_iInLen [IN]    ���볤��
     *@param      pszOutput [IN]  ���������ַ
     *@param      a_iOutLen [INOUT]   ����Ϊ���뻺��������,���벻���ڱ��볤��,���Ϊʵ�ʱ��볤��
     *@retval         >= 0  success
     *@retval        <0   fail
     *@retval        =-1 ������������Ȳ��� �������볤���쳣
     *@retval        =-2 �����ַ��쳣
     *@retval        =-3 �����ַ������쳣
     */
    TSF4G_API int thttp_base64_decode(IN const char *pszInput,IN int a_iInLen,OUT  unsigned char *pszOutput,INOUT int *a_iOutLen);


#ifdef __cplusplus
}
#endif

#endif
