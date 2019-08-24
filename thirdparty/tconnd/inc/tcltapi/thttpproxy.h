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
        BASE64_WITH_CRLF = 0x01  /*每76个字符加CRLF*/
    }
    BASE64_FLAG;

    typedef enum
    {
        THTTP_ERROR_NONE                        = 200,
        THTTP_ERROR_CALLOC_HANDLE               = 201,       /*calloc 句柄失败*/
        THTTP_ERROR_INVALID_PROXYINFO           = 202,      /*proxyurl地址非法*/
        THTTP_ERROR_INVALID_DSTINFO             = 203,     /*目的服务器地址非法*/
        THTTP_ERROR_CONNECT_PROXYSVR            = 204,      /*连接代理服务器失败*/
        THTTP_ERROR_SEND_NEGOREQ                = 205,      /*发送验证协商请求包失败 */
        THTTP_ERROR_RECV_NEGORSP                = 206,     /*接收验证协商返回包网络出错*/
        THTTP_ERROR_RECV_NEGORSP_TIMEOUT        = 207,     /*接收验证协商返回包超时*/
        THTTP_ERROR_RECV_NEGORSP_INVALID        =  208,     /*接收验证协商返回包非法*/
        THTTP_ERROR_PROXY_FAIL                  =  210,     /*代理服务器返回状态码非法*/
        THTTP_ERROR_DIGEST_NOTSUPPORT           = 211,     /*Digest验证方式暂时不支持*/
        THTTP_ERROR_NTLM_NOTSUPPORT             = 212,    /*NTLM验证方式暂时不支持*/
        THTTP_ERROR_AUTH_UNKNOWN                = 213,    /*其它验证方式暂时不支持*/
        THTTP_ERROR_USRORPWD_INVALID          =214,     /*用户名或密码非法*/
        THTTP_ERROR_ENCBASE64_BASICSTR          = 215,   /*编码base64 用户名和密码失败*/
        THTTP_ERROR_SEND_BASICREQ               =216,     /*发送basic验证请求包失败 */
        THTTP_ERROR_RECV_BASICRSP               =217,    /*接收basic验证返回包网络出错*/
        THTTP_ERROR_RECV_BASICRSP_TIMEOUT       =218,      /*接收basic验证返回包超时*/
        THTTP_ERROR_RECV_BASICRSP_INVALID       = 219,    /*接收basic验证返回包非法*/
        THTTP_ERROR_BASIC_AUTH_FAIL             = 220,    /*服务器basic验证用户名和密码非法*/
        THTTP_ERROR_COUNT
    } THTTP_ERRORCODE;

    typedef struct
    {
        char szProxyUrl[THTTP_MAX_STRLEN];   /*必填,代理服务器地址:eg:10.0.1.11:1080  */
        unsigned int dwDstIP;                 /*必填,目的服务器IP , 网络序  */
        unsigned short wDstPort;              /*必填,目的服务器Port ,网络序  */
        int    iProxyTimeOut;                 /*必填,代理服务器响应请求超时时间,若设为零,则默认值为 3秒  */
        char szUser[THTTP_MAX_STRLEN];          /*可选,需要用户名和密码方式必选*/
        char szPasswd[THTTP_MAX_STRLEN];      /*可选,需要用户名和密码方式必选*/
        char szDomain[THTTP_MAX_STRLEN];     /*可选, 用户域,NTLM验证方式必选*/
    } THTTPPARAM,*LPTHTTPPARAM;


    struct tagTHTTPPROXY;

    typedef struct tagTHTTPPROXY THTTPPROXY;
    typedef struct tagTHTTPPROXY *LPTHTTPPROXY;


    /**
     *@brief  创建HTTP代理 句柄
     *@param      a_ppHTTP [INOUT]    连接句柄
     *@param      a_pHttpParam[IN]     代理设置参数
     *@retval       >=0      success
     *@retval        <0    返回错误码,错误码参考THTTP_ERRORCODE
     */
    TSF4G_API int thttp_create(INOUT LPTHTTPPROXY *a_ppHTTP,IN LPTHTTPPARAM a_pHttpParam);

    /**
     *@brief  创建HTTP代理 句柄
     *@param      a_phQQClt [IN]    连接句柄
     *@retval
     */
    TSF4G_API void thttp_free(IN LPTHTTPPROXY *a_ppHTTP);

    /**
     *@brief  连接代理服务器
     *@param      a_pHTTP [IN]    连接句柄
     *@param      a_nTimeout[IN]   超时时间,设为零则使用默认代理设置参数中超时时间
     *@retval     >= 0  success ,          返回socket
     *@retval     <0    返回错误码,错误码参考THTTP_ERRORCODE
     */
    TSF4G_API int thttp_connect(IN LPTHTTPPROXY a_pHTTP,IN int a_nTimeout);

    /**
     *@brief  断开代理服务器
     *@param      a_pHTTP [IN]    连接句柄
     *@retval     >= 0  success ,          返回socket
     *@retval     <0    返回错误码,错误码参考THTTP_ERRORCODE
     */
    TSF4G_API void thttp_disconnect(IN LPTHTTPPROXY a_pHTTP);



    /**
     *@brief  到代理服务器验证
     *@param      a_pHTTP [IN]    连接句柄
     *@retval     >= 0  success ,          返回socket
     *@retval     <0    返回错误码,错误码参考THTTP_ERRORCODE
     */
    TSF4G_API int thttp_do_negotiation(IN LPTHTTPPROXY a_pHTTP,IN int a_nTimeout);


    /**
     *@brief  返回 socket
     *@param      a_pHTTP [IN]    连接句柄
     *@retval       返回socket
     */
    TSF4G_API int  thttp_get_socket(IN LPTHTTPPROXY a_pHTTP);


    /**
     *@brief  base64编码
     *@param      pszInput [IN]    编码输入地址
     *@param      a_iInLen [IN]    编码长度
     *@param      pszOutput [IN]  编码输出地址
     *@param      a_iOutLen [INOUT]   输入为编码缓冲区长度,必须为编码长度两倍,输出为实际编码长度
     *@param      a_iFlag [IN]    是否使用MIME base64编码方法,参考BASE64_FLAG定义
     *@retval         >= 0  success
     *@retval        <0   fail
     *@retval        =-1 输出缓冲区长度不够
     */
    TSF4G_API int thttp_base64_encode(IN const char *pszInput,IN int a_iInLen,OUT char *pszOutput,INOUT int *a_iOutLen,IN int a_iFlag);


    /**
     *@brief  base64解码
     *@param      pszInput [IN]    解码输入地址
     *@param      a_iInLen [IN]    解码长度
     *@param      pszOutput [IN]  解码输出地址
     *@param      a_iOutLen [INOUT]   输入为解码缓冲区长度,必须不少于编码长度,输出为实际编码长度
     *@retval         >= 0  success
     *@retval        <0   fail
     *@retval        =-1 输出缓冲区长度不够 或者输入长度异常
     *@retval        =-2 解码字符异常
     *@retval        =-3 解码字符长度异常
     */
    TSF4G_API int thttp_base64_decode(IN const char *pszInput,IN int a_iInLen,OUT  unsigned char *pszOutput,INOUT int *a_iOutLen);


#ifdef __cplusplus
}
#endif

#endif
