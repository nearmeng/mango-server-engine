//////////////////////////////////////////////////////////////////////////
/// @file:   tgcpapi_ex.h
/// @brief:  本文件定义的接口是对tgcpapi.h的一个更高层的封装
/// @author: vforkk
/// @date:   2014-03-03
/// @desc:  用户在使用tgcpapi的时候，分为以下几个步骤：
///         1.初始化；2.连接；3.收发；4.退出
///
///         1.初始化步骤：
///             1.1 tgcpapi_create_and_init:创建句柄，同时初始化（包括设置帐号信息和AccessToken)
///             1.2 tgcpapi_set_security_info:设置加密方式信息
///             1.3 tgcpapi_set_acesstoken_expire：设置AccessToken有效期(如果有的话）
///             1.4 tgcpapi_set_refresh_token：设置RefreshToken（如果有的话）
///             1.5 tgcpapi_set_refresh_token_expire: 设置RefreshToken(如果有的话）
///
///         2.连接：
///           tgcpapi有两种使用模式：api模式和Event模式。
///             2.1 api模式涉及接口：
///                 2.1.1 tgcpapi_start_connection：客户端发起与tconnd的连接、握手(SYN/ACK)、鉴权(加密)，同步等待响应。
///                 2.1.2 tgcpapi_close_connection：关闭与tconnd的连接
///                 2.1.3 tgcpapi_relay_connection：重连通信会话，此接口会重走握手、鉴权操作
///                 2.1.4 tgcpapi_query_wait_notify：探询或更新上行的排队通知
///                 2.1.5 tgcpapi_query_stopped：探询服务端是否终止通信会话
///
///             2.2 Event模式涉及接口：(Event模式的使用过程，见本文件末尾的注释部分）
///                 2.2.1 tgcpapi_start：启动与后端服务的通信会话
///                 2.2.2 tgcpapi_update：刷新tgcpapi内部的状态，包括与后端服务的通信状态
///                 2.2.3 tgcpapi_resume：恢复（重连）与后端服务的通信会话，一般在正常通信过程中网络出现异常后调用（只重连网络连接）
///                 2.2.4 tgcpapi_stop： 终止与后端服务的通信会话
///                 2.2.5 tgcpapi_get_queue：在update获取到TGCP_EVENT_WAITING事件后，获取排队状态
///                 2.2.6 tgcpapi_get_sstop_reason：在update获取到TGCP_EVENT_SSTOPED事件后，获取关闭原因
///                 2.2.7 tgcpapi_recved_pkglen：在update获取到TGCP_EVENT_DATA_IN事件后，获取包长度
///                 2.2.8 tgcpapi_sendable_buflen：在update获取到TGCP_EVENT_DATA_OUT事件后，获取可用空间
///                 2.2.9 tgcpapi_refresh_acesstoken：请求续期AcessToken，在update获取到TGCP_EVENT_ATK事件后，调用tgcpapi_get_acesstoken或获取新的AccessToken
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
///             3.1 tgcpapi_close_connection(如果是Api模式）
///             3.2 tgcpapi_stop（如果是Event模式）
///             3.3 tgcpapi_fini（与init相对）
///             3.4 tgcpapi_destroy（与create相对）
///
///         5.其它接口
///             4.1 tgcpapi_get_socket：获取内部socket的文件描述符（除非迫不得已，否则强烈建议不要调用访问此接口）
///             4.2 tgcpapi_get_acesstoken：获取AccessToken
///             4.3 tgcpapi_get_refreshtoken：获取RefreshToken
///             4.4 tgcpapi_get_account:获取Account信息
///             4.5 tgcpapi_get_openid：获取Openid
///             4.6 tgcpapi_get_tdr_last_error_string：获取最近一次TDR打解包等接口错误描述信息（定位问题时辅助信息）
///             4.7 tgcpapi_get_last_unexpected_cmd：获取最近一次非预期命令字值（定位问题时辅助信息）
///             4.8 tgcpapi_error_string: 获取详细的错误描述
//////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             FBI Warning!!!!
// 本文件定义的接口，是对tgcpapi.h的更高层的封装，其中一部分接口和tgcpapi.h中的部分接口不能同时使用.
// tgcpapi_ex.h和tgcpapi.h不能同时使用的接口有：
//          tgcpapi_ex.h                                        tgcpapi.h
// tgcpapi_create_and_init          VS       冲突接口：tgcpapi_create、tgcpapi_init、tgcpapi_set_token
//                                           重复接口：tgcpapi_set_appid、tgcpapi_set_dh_info、tgcpapi_set_account(tgcpapi.h中定义的函数）
//
// tgcpapi_set_security_info        VS       tgcpapi_init、tgcpapi_set_dh_info
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGCP_API_EX_H_
#define TGCP_API_EX_H_
#include "tgcpapi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 以下接口，用于替换tgcpapi_create、tgcpapi_init、tgcpapi_set_appid接口
/// @brief  创建TGCP句柄同时初始化
///
/// @param[in, out]  a_ppHandle tgcpapi句柄指针
/// @param[in]  iServiceID    业务ID，TConnd3.1.7及以上版本，此字段无意义。其取值会被TConnd配置文件的BusinessID覆盖。
/// @param[in]  pszAppId      AppId，第三开发从QQ或微信申请到的AppId，QQ或微信对第三方应用的身份标识。
/// @param[in]  iMaxMessageBuffSize      用户数据缓冲区大小， 这个值不能小于tgcpapi_send,tgcpapi_recv,tgcpapi_peek数据包，可能的最大长度。
/// @param[in]  pAccount     登录者帐号信息
/// @param[in]  pszAccessToken     从QQ或微信返回的AccessToken
/// @param[in]  iAccessTokenLen     pszAccessToken的有效长度
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_create_and_init(HTGCPAPI* ppHandler,
    int iServiceID, 
    const char* pszAppId, int iAppIdLen,
    int iMaxMessageBuffSize,
    LPTGCPACCOUNT pAccount,
    const char* pszAccessToken, int iAccessTokenLen
    );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置验证相关信息
///
/// @param[in, out]  a_pHandle tgcpapi句柄指针
/// @param[in]  iEncMethod    数据加密方式,请使用 tagEncryptMethod 枚举值
/// @param[in]  iKeyMode      密钥协商方式,请使用 tagKeyMaking 枚举值
/// @param[in]  pszDH_P_Hex   大素数p的Hex值（用大素数生成工具生成）,当iKeyMode为TGCP_KEY_MAKING_RAW_DH时，需要填此参数。
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_security_info(HTGCPAPI pHandler, 
    eEncryptMethod iEncMethod,
    eKeyMaking iKeyMode,
    const char* pszDH_P_HexString
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置AuthType，用于标识AccessToken，RefreshToken和Code三种Token以外的其它验证方式，
///         如果用户使用的是以上三种票据，不需要设置此接口
///
/// @param[in, out]  a_pHandle tgcpapi句柄指针
/// @param[in]  authType   验证类型
/// @note       其中 [0x7000, 0x7FFF] 为内部保留，作为特殊类型
/// @note            0x7001 已经分配, 微信鉴权, 统一 CODE 与 RTK 两种鉴权
/// @note            0x7FFF 已经分配，OAuth 2.0 统一鉴权时表示 "无意义"
/// @note            [0x8000, 0xFFFF] 为外部保留，由项目组使用
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_authtype(HTGCPAPI pHandler, eAuthType authType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置RefreshToken
///
/// @param[in, out]  a_pHandle tgcpapi句柄指针
/// @param[in]  pszRefreshToken   Refresh Token内容
/// @param[in]  iRefreshTokenLen   pszRefreshToken的长度
/// @param[in]  iExpire  有效期，以秒为单位,没有的话就填0
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_refresh_token(HTGCPAPI pHandler, const char* pszRefreshToken, int iRefreshTokenLen, int iExpire = 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置RefreshToken的有效期，没有的话不需要调用此接口
///
/// @param[in, out]  a_pHandle tgcpapi句柄指针
/// @param[in]  iExpire  有效期，以秒为单位
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_refresh_token_expire(HTGCPAPI pHandler, int iExpire);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置Code
///
/// @param[in, out]  a_pHandle tgcpapi句柄指针
/// @param[in]  pszAuthCode   AuthCode值
/// @param[in]  iCodeLen   pszAuthCode的长度
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_auth_code(HTGCPAPI pHandler, const char* pszAuthCode, int iCodeLen);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  设置新帐号（一般用于帐号切换）
///
/// @param[in, out]  a_ppHandle tgcpapi句柄指针
/// @param[in]  pAccount     登录者帐号信息
/// @param[in]  pszAccessToken     从QQ或微信返回的AccessToken
/// @param[in]  iAccessTokenLen     iAccessTokenLen的有效长度
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_set_account_ex(HTGCPAPI pHandler,
    LPTGCPACCOUNT pAccount,
    const char* pszAccessToken,
    int iAccessTokenLen
    );

/*********************************************************************************************************
以下接口使用tgcpapi.h的定义

// API模式
int tgcpapi_start_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);
int tgcpapi_close_connection(HTGCPAPI a_pHandle);
int tgcpapi_relay_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);

int tgcpapi_query_wait_notify(HTGCPAPI a_pHandle, int* a_piBingo,
                              LPQUEUENOTIFY a_pstQueNotify, int a_iTimeout);
int tgcpapi_query_stopped(HTGCPAPI a_pHandle, int* a_piResult, int* a_piReason);


// 事件模式
int tgcpapi_start(HTGCPAPI a_pHandle, const char* a_pszUrl);
int tgcpapi_update(HTGCPAPI a_pHandle, LPGCPEVENT a_pstEvent);
int tgcpapi_resume(HTGCPAPI a_pHandle, const char* a_pszUrl);
int tgcpapi_stop(HTGCPAPI a_pHandle);

const QUEUENOTIFY* tgcpapi_get_queue(HTGCPAPI a_pHandle);
int tgcpapi_get_sstop_reason(HTGCPAPI a_pHandle);
int tgcpapi_recved_pkglen(HTGCPAPI a_pHandle);
int tgcpapi_sendable_buflen(HTGCPAPI a_pHandle);


// 收发接口
int tgcpapi_send(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout);
int tgcpapi_recv(HTGCPAPI a_pHandle, char* a_pszBuffOut, int* a_piSize, int a_iTimeout);
int tgcpapi_peek(HTGCPAPI a_pHandle, const char** a_ppszBuff, int* a_piSize, int a_iTimeout);
int tgcpapi_flush(HTGCPAPI a_pHandle);

// 其它操作
int tgcpapi_refresh_acesstoken(HTGCPAPI a_pHandle, int a_iTimeout);

// Set接口
int tgcpapi_set_acesstoken_expire(HTGCPAPI a_pHandle, unsigned short a_iExpire);
int tgcpapi_set_route_info(HTGCPAPI a_pHandle, eClusterType a_iClusterType, LPCLUSTERINFO a_pstClusterInfo, eDataAllowLost a_iDataAllowLost);

// Get接口
int tgcpapi_get_socket(HTGCPAPI a_pHandle, int* a_piFd);
int tgcpapi_get_acesstoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);
int tgcpapi_get_refreshtoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);
int tgcpapi_get_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);
int tgcpapi_get_openid(HTGCPAPI a_pHandle, char* a_pszOpenID, int* a_piLen);

const char* tgcpapi_get_tdr_last_error_string(HTGCPAPI a_pHandle);
int tgcpapi_get_last_unexpected_cmd(HTGCPAPI a_pHandle);

// 获取错误信息
const char* tgcpapi_error_string(int iErrorCode);

*********************************************************************************************************/

/*********************************************************************************************************
** 以下是Event模式的大概过程

//可以放在一个线程循环中
if (tgcpapi_update(a_pHandle, &stEvent) == TGCP_ERR_NONE)
{

    if (stEvent.iEvtNum > 0)
    {
        if (stEvent.iEvents & TGCP_EVENT_SVR_IS_FULL)
        {
            return;
        }

        if (stEvent.iEvents & TGCP_EVENT_SSTOPED)
        {
            return;
        }

        // 排队信息
        if (stEvent.iEvents & TGCP_EVENT_WAITING)
        {
            //只需要提示用户,但不用退出程序。等成功排上了，就可以走后续正常流程了，不需要重新连接
        }

        // 接收
        if (stEvent.iEvents & TGCP_EVENT_DATA_IN)
        {
            tgcpapi_peek();
        }

        if (stEvent.iEvents & TGCP_EVENT_ATK)
        {
            tgcpapi_get_acesstoken(a_pHandle, acc_token, &atk_size);
        }

        // 发送
        if (stEvent.iEvents & TGCP_EVENT_DATA_OUT)
        {
            a、从发送Buffer中读一个完整的数据包
            b、tgcpapi_send(a_pHandle, buffer, sizeof(buffer), 0);
        }
    }
}
*********************************************************************************************************/

#endif
