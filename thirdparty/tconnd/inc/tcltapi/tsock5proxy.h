/************************************************************
 *  File Module:    TSock5Proxy.h
 *  File Description: 头文件
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
#define TSOCK5_DEFAULT_RSPTIMEOUT  3000     /*默认proxy响应超时时间*/



    typedef enum
    {
        TSOCK5_ERROR_NONE                               = 100,
        TSOCK5_ERROR_CALLOC_HANDLE         = 101,       /*calloc 句柄失败*/
        TSOCK5_ERROR_INVALID_PROXYINFO      = 102,      /*proxyurl地址非法*/
        TSOCK5_ERROR_INVALID_DSTINFO          =  103,     /*目的服务器地址非法*/
        TSOCK5_ERROR_CONNECT_PROXYSVR       = 104,      /*连接代理服务器失败*/
        TSOCK5_ERROR_SEND_NEGOREQ               = 105,      /*发送验证协商请求包失败 */
        TSOCK5_ERROR_RECV_NEGORSP                 = 106,     /*接收验证协商返回包网络出错*/
        TSOCK5_ERROR_RECV_NEGORSP_TIMEOUT= 107,     /*接收验证协商返回包超时*/
        TSOCK5_ERROR_RECV_NEGORSP_INVALID =  108,     /*接收验证协商返回包非法*/
        TSOCK5_ERROR_NOMETHOD_ACCEPT         = 109,       /*代理服务器没有可支持的协商方法*/
        TSOCK5_ERROR_USRPWD_INVALID            = 110,       /*用户名或密码空*/
        TSOCK5_ERROR_SEND_USRPWDREQ           = 111,       /*发送用户名密码验证包失败*/
        TSOCK5_ERROR_RECV_USRPWDRSP            = 112,       /*接收用户名密码返回包网络出错*/
        TSOCK5_ERROR_RECV_USRPWDRSP_TIMEOUT=113,    /*接收用户名密码返回包超时*/
        TSOCK5_ERROR_RECV_USRPWDRSP_INVALID =114,   /*接收用户名密码返回包非法*/
        TSOCK5_ERROR_AUTH_USRPWD_INVALID    =115,       /*用户名密码验证失败*/
        TSOCK5_ERROR_SEND_TCPCONNREQ            = 116,       /*发送连接目的服务器请求失败*/
        TSOCK5_ERROR_RECV_TCPCONNRSP            = 117,       /*接收连接目的服务器应答网络出错*/
        TSOCK5_ERROR_RECV_TCPCONNRSP_TIMEOUT=118,    /*接收连接目的服务器应答返回包超时*/
        TSOCK5_ERROR_RECV_TCPCONNRSP_INVALID =119,   /*接收用户名密码返回包非法*/
        TSOCK5_ERROR_TCPCONN_FAIL                       =120,   /*代理服务器连接目的服务器失败*/
        TSOCK5_ERROR_COUNT
    }
    TSOCK5_ERRORCODE;

    typedef struct
    {
        char szProxyUrl[TSOCK5_MAX_STRLEN];   /*必填,代理服务器地址:eg:10.0.1.11:1080*/
        unsigned int dwDstIP;                               /*必填,目的服务器IP , 网络序*/
        unsigned short wDstPort;                         /*必填,目的服务器Port ,网络序*/
        int    iProxyTimeOut;                              /*必填,代理服务器响应请求超时时间,若设为零,则默认值为 3秒*/
        char szUser[TSOCK5_MAX_STRLEN];         /*可选,如果需要用户名和密码验证则必填*/
        char szPasswd[TSOCK5_MAX_STRLEN];    /*可选,如果需要用户名和密码验证则必填*/
    } TSOCK5PARAM,*LPTSOCK5PARAM;


    struct tagTSOCK5PROXY;
    typedef struct tagTSOCK5PROXY  TSOCK5PROXY;
    typedef struct tagTSOCK5PROXY *LPHTSOCK5PROXY;


    /**
     *@brief  创建Sock5 句柄
     *@param      a_pszUri[IN]       URL 地址eg:172.25.40.97:9088
     *@param      a_iTimeout [IN]       连接阻塞超时时间
     *@param      a_phQQClt [OUT]    连接句柄
     *@retval       >=0      success,          返回socket
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
