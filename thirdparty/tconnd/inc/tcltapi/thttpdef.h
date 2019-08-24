/************************************************************
 *  File Module:    THttpDef.h
 *  File Description: 头文件,RFC2617,NTLM创建
 *  File History:
 *  <author>  <time>            <version >   <desc>
 *  Hardway    2010/10/27         1.0           create this moudle
 ***********************************************************/

/***********************************************************

 *Negotiation Http Request:client->proxy
 *       CONNECT %s:%d HTTP/1.1\r\n
 *       Accept: * / *\r\n
 *          Content-Type: text/html\r\n
 *       Proxy-Connection: Keep-Alive\r\n
 *       Content-length: 0\r\n\r\n

 *Negotiation Http Response:proxy->client
 *            HTTP/1.1 200  OK (不需要验证)
 *            HTTP/1.1 407  Proxy Authentication Required(需要验证)
 *                   Proxy-Authenticate: Basic
 *                   Proxy-Authenticate: Digest *
 *                   Proxy-Authenticate: NTLM
 ***********************************************************/

#ifndef _THTTPDEF_H
#define _THTTPDEF_H


#define THTTP_STATUSCODE_LEN              3
#define THTTP_AUTHSTR_LEN                    8
#define THTTP_PROXYAUTH_HEAD             "Proxy-Authenticate:"
#define THTTP_BASICAUTH_STR                "Basic"
#define THTTP_DIGESTAUTH_STR              "Digest"
#define THTTP_NTLMAUTH_STR                  "NTLM"
#define THTTP_BASE64_BLOCK                     76

typedef enum
{
    HTTP_STATUS_OK                 =   200,
    HTTP_STATUS_PROXY_REQAUTH     =   407,
    HTTP_STATUS_COUNT
} HTTP_STATUSCODE;

typedef enum
{
    HTTPPROXY_NONE         =0,
    HTTPPROXY_BASIC        =1,
    HTTPPROXY_DIGEST      =2,
    HTTPPROXY_NTLM          =3,
    HTTPPROXY_UNKNOWN  =4,
    HTTPPROXY_COUNT
} HTTPPROXY_METHOD;


typedef enum
{

    ASCII_CR          = 13,
    ASCII_LF          = 10,
    ASCII_SP          = 32,
    ASCII_COLON    =58,



} ASCII_CODE;


#endif




