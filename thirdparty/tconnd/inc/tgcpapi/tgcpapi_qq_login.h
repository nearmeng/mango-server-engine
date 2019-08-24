/*
 * Copyright (c) 2013 Tencent all rights reserved
 *
 * time:   2014-04-22
 * file:   tgcpapi_qq_login.h
 * desc:   为QQ登陆模式专用的接口
 *
 */
/// 用户在使用QQ登陆模式的时候，分为以下几个步骤：
///         1.初始化；2.连接；3.收发；4.退出
///
///         1.初始化步骤：
///             1.1 tgcpapi_create: 创建句柄
///             1.2 tgcpapi_qq_login_st_init: 初始化
///
///         2.连接：
///           tgcpapi有两种使用模式：同步模式和异步模式。
///             2.1 同步模式涉及接口：
///                 2.1.1 tgcpapi_start_connection：客户端发起与tconnd的连接
///                 2.1.2 tgcpapi_close_connection：关闭与tconnd的连接
///                 2.1.3 tgcpapi_relay_connection：重连通信会话
///                 2.1.4 tgcpapi_query_wait_notify：探询或更新上行的排队通知
///                 2.1.5 tgcpapi_query_stopped：探询服务端是否终止通信会话
///
///             2.2 异步模式涉及接口：
///                 2.2.1 tgcpapi_start：客户端发起与tconnd的连接
///                 2.2.2 tgcpapi_update：刷新tgcpapi内部的状态，包括与后端服务的通信状态
///                 2.2.3 tgcpapi_resume：重连通信会话
///                 2.2.4 tgcpapi_stop：关闭与tconnd的连接
///                 2.2.5 tgcpapi_get_queue：在update获取到TGCP_EVENT_WAITING事件后，获取排队状态
///                 2.2.6 tgcpapi_get_sstop_reason：在update获取到TGCP_EVENT_SSTOPED事件后，获取关闭原因
///                 2.2.7 tgcpapi_recved_pkglen：在update获取到TGCP_EVENT_DATA_IN事件后，获取包长度
///                 2.2.8 tgcpapi_sendable_buflen：在update获取到TGCP_EVENT_DATA_OUT事件后，获取可用空间
///                 2.2.9 tgcpapi_get_acesstoken: 在update获取到TGCP_EVENT_ATK事件后，获取新的AccessToken
///
///         3.收发数据
///           无论是api模式还是Event模式，使用的都是相同的收发接口。区别在于：
///           api模式的收发接口是在应用程序自己的循环中主动调用，(像使用系统socket的send和recv一样）
///           Event模式是在Update之后检测到相关事件后，才被调用。
///
///             3.1 tgcpapi_send：在update检测到TGCP_EVENT_DATA_OUT事件后，如果有待发送的数据包，则调用此接口发送
///             3.2 tgcpapi_recv：在update检测到TGCP_EVENT_DATA_IN事件后，接收网络数据包.与peek的区别是会拷贝数据。
///             3.3 tgcpapi_peek：在update检测到TGCP_EVENT_DATA_IN事件后，接收网络数据包.与recv的区别是不拷贝数据，但需要及时取走，否则数据可能会被冲掉
///             3.4 tgcpapi_flush：当tgcpapi_send返回TGCP_ERR_SEND_UNCOMPLETE时，可以主动调用tgcpapi_flush去冲洗未发完数据包。
///
///           注：当send或flush返回错误，不影响tgcpapi_recv和tgcpapi_peek的接收。应用程序还是应该主动调用recv或peek接口，读取接收缓存中的有效数据包。
///
///         4.退出
///             3.1 tgcpapi_close_connection(如果是同步模式）
///             3.2 tgcpapi_stop（如果是异步模式）
///             3.3 tgcpapi_fini（与init相对）
///             3.4 tgcpapi_destroy（与create相对）
#ifndef TGCPAPI_QQ_LOGIN_H_
#define TGCPAPI_QQ_LOGIN_H_

#include "tgcpapi.h"


#define TGCP_QQ_UNIFIED_MAX_SIGN_LEN 256
#define TGCP_QQ_LOGIN_KEY_LEN 16
#define GSPH_QQDECRYPT_KEY      ("aaaaaaaaaaaaaaaa")


/*
 * CTLogin模式鉴权信息
 */
struct tagTGCPQQUnifiedAuthInfo
{
    unsigned long long Uin;         //Uin
    char szLoginKey[TGCP_QQ_LOGIN_KEY_LEN];     //登陆秘钥，对应集通票据的 STKey
    unsigned short wLen;    //签名长度
    char szSigInfo[TGCP_QQ_UNIFIED_MAX_SIGN_LEN];   //签名信息，对应集通票据的 ST

    tagTGCPQQUnifiedAuthInfo() : Uin(0), wLen(0)
    {

    }
};

typedef struct tagTGCPQQUnifiedAuthInfo TGCPQQUnifiedAuthInfo;


/// @brief 初始化QQ ST票据模式下的句柄
///
/// @param[in]  a_pHandle       TCPAPI句柄
/// @param[in]  a_iServiceID    服务化ID
/// @param[in]  a_iPlatformID   设备信息
/// @param[in]  a_iBuffLen      用户数据缓冲区长度
/// @param[in]  a_iAppID        QQ业务ID
/// @param[in]  a_pstAuthInfo   鉴权信息
/// @param[in]  a_iEncMethod    数据加密方式,请使用 tagEncryptMethod 枚举值
///
/// @return =0, success,
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 当使用CTLogin模式鉴权时，初始化只调用本函数即可，然后调用发起连接的函数
int tgcpapi_qq_login_st_init(HTGCPAPI a_pHandle, int a_iServiceID, ePlatformID a_iPlatformID, int a_iBuffLen,
                             int a_iAppID, TGCPQQUnifiedAuthInfo* a_pstAuthInfo, eEncryptMethod a_iEncMethod);


/// @brief 生成ctlogin鉴权模式下的测试签名
///
/// @param[in] a_pstUnifyAuthInfo 存放签名的结构体指针
/// @param[in] a_uUin   QQ号
/// @param[in] a_pszSvrKey  加解密秘钥
/// @param[in] a_pszSessionKey  会话秘钥
/// @param[in] a_uCltIP     客户端IP
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 此接口生成的签名只对TGCP_AUTH_QQ_CT_LOGIN鉴权模式有效
///         a_pszSvrKey是客户端与TConnd通信，业务包的加密秘钥的秘钥，TConnd也要用该值解密，才可以获取a_pszSessionKey
///         a_pszSessionKey是客户端与gamesvr通信，业务包的加密秘钥
///         通常调用tgcpapi_create_test_signature()函数生成测试签名
int tgcpapi_make_qqunify_authinfo(TGCPQQUnifiedAuthInfo* a_pstUnifyAuthInfo,
                                  unsigned long long a_uUin, char* a_pszSvrKey, char* a_pszSessionKey, unsigned int a_uCltIP);

/// @brief 生成ctlogin鉴权模式下的固定测试签名
///
/// @param[in] a_pstUnifyAuthInfo 存放签名的结构体指针
/// @param[in] a_uUin   QQ号
/// @param[in] a_uCltIP     客户端IP
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错原因
///
/// @note 生成的测试签名，服务器通信秘钥为 GSPH_QQDECRYPT_KEY，TConnd端应该配置相同的值
///         通常调用本函数生成测试签名
int tgcpapi_create_test_signature(TGCPQQUnifiedAuthInfo* a_pstUnifyAuthInfo, unsigned long long a_uUin, unsigned int a_uCltIP);

#endif

