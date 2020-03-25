/// @file:   tgcpapi.h
/// @brief:  TGCP协议API，主要作为手游API接入使用
/// @author: laynehu
/// @date:   2013-10-15

#ifndef TGCP_API_H_
#define TGCP_API_H_

#include "tgcpapi_error.h"

#define TGCP_MAX_ID_LENGTH 256
#define TGCP_DATA_LEN 16

#if (defined(_WIN32) || defined(_WIN64)) && _MSC_VER < 1300
typedef unsigned __int64 tgcp_ulonglong;
#else
typedef unsigned long long tgcp_ulonglong;
#endif


/*
 * 鉴权方式
 */
typedef enum tagAuthType {
    TGCP_AUTH_NONE = 0,         //不使用鉴权
    TGCP_AUTH_WX_O2_CODE,       //微信 OAuth 2.0 CODE
    TGCP_AUTH_WX_O2_RTK,        //微信 OAuth 2.0 refresh token
    TGCP_AUTH_QQ_O2_ATK,        //手Q  OAuth 2.0 acess token
    TGCP_AUTH_QQ_CT_LOGIN,      //QQ CTLogin鉴权方式
    TGCP_AUTH_ITOP = 0x1000,     //itop鉴权方式
    TGCP_AUTH_PC = 0x1004,     //pc鉴权方式
    TGCP_AUTH_WEGAME = 0x1005,     //wegame auth type

    TGCP_AUTH_VER_2 = 0x7fff,      //新的帐号鉴权方式，2014.03.07
} eAuthType;

/*
 * 加密方法
 */
typedef enum tagEncryptMethod {
    TGCP_ENCRYPT_METHOD_NONE = 0,   //不加密
    TGCP_ENCRYPT_METHOD_TEA  = 1,   //暂不支持
    TGCP_ENCRYPT_METHOD_QQ   = 2,   //暂不支持
    TGCP_ENCRYPT_METHOD_AES  = 3,   //AES加密
    TGCP_ENCRYPT_METHOD_AES2 = 4,   //AES2加密
} eEncryptMethod;

/*
 * 密钥生成方法
 */
typedef enum tagKeyMaking {
    TGCP_KEY_MAKING_NONE = 0,       //不使用秘钥（此时认为通信数据不加密）
    TGCP_KEY_MAKING_INAUTH,         //秘钥包含在鉴权信息
    TGCP_KEY_MAKING_INSVR,          //秘钥由TCONND服务器下发
    TGCP_KEY_MAKING_RAW_DH,         //原始DH算法秘钥
    TGCP_KEY_MAKING_ENC_DH,         //加密过的DH算法秘钥
} eKeyMaking;

/*
 * 平台信息
 */
typedef enum tagPlatformID {
    TGCP_PC = 0,
    TGCP_ANDROID = 101,
    TGCP_IOS = 102,
} ePlatformID;

/*
 * 压缩信息
 */
typedef enum
{
    TGCP_COMPR_NONE = 0,
    TGCP_COMPR_LZ4,
    TGCP_COMPR_ZLIB,
}eCompressMethod;

/*
 * 排队信息
 * */
typedef struct {
    unsigned int iPos;          // 排队位置
    unsigned int iQueueLen;     // 队列长度
    unsigned int iEstimateTime; // 预计排队时长,以秒为单位
} QUEUENOTIFY, *LPQUEUENOTIFY;


/*
 * 账号类型
 */
typedef enum tagAccountType {
    TGCP_ACCOUNT_TYPE_NONE = 0,      //自定义格式
    TGCP_ACCOUNT_TYPE_QQ_UIN,        //QQ号
    TGCP_ACCOUNT_TYPE_QQ_OPENID,     //手QOpenID
    TGCP_ACCOUNT_TYPE_WX_OPENID,     //微信OpenID
	TGCP_ACCOUNT_TYPE_IOS_GUEST = 0x2001,     //游客模式
    TGCP_ACCOUNT_TYPE_FACEBOOK = 0x8001,
    TGCP_ACCOUNT_TYPE_KAKAO = 0x8010,
    TGCP_ACCOUNT_TYPE_GARENA = 0x8011,
} eAccountType;

typedef enum tagAccountFormat {
    TGCP_ACCOUNT_FORMAT_QQ_32 = 0,
    TGCP_ACCOUNT_FORMAT_QQ_64,
    TGCP_ACCOUNT_FORMAT_STRING,
} eAccountFormat;

typedef union tagTGCPAccountValue {
    tgcp_ulonglong uin;         //Uin
    char szID[TGCP_MAX_ID_LENGTH];  //String类型的ID
} TGCPACCOUNTVALUE;

struct tagTGCPAccount {
    unsigned short uType;            //账号类型,itop鉴权模式下,不要使用eAccountType,而是直接使用渠道号
    eAccountFormat bFormat;          //账号格式
    TGCPACCOUNTVALUE stAccountValue; //账号信息
    tgcp_ulonglong llUid;            //帐号映射的uid
    tagTGCPAccount()
        :uType(0)
        ,bFormat(TGCP_ACCOUNT_FORMAT_QQ_32)
        ,llUid(0)
    {
    }
};
typedef struct tagTGCPAccount TGCPACCOUNT;
typedef struct tagTGCPAccount *LPTGCPACCOUNT;

struct tagTGCPApiHandle;
typedef struct tagTGCPApiHandle *HTGCPAPI;

/*
 * 集群化登陆类型
 */
typedef enum
{
    TGCP_ROUTE_NONE = 0,          //非集群化登陆
    TGCP_ROUTE_SPECIFY_ZONE,      //集群化指定区服登陆
    TGCP_ROUTE_SPECIFY_SERVER,    //集群化指定ServerID登陆
    TGCP_ROUTE_LOGIN_POSITION,    //向集群登陆点发送消息
}eTGCPRouteType;

typedef enum
{
    TGCP_NOT_ALLOW_LOST = 0,   //不允许丢失消息，丢失后关闭连接
    TGCP_ALLOW_LOST,           //允许丢失消息，丢失后不关闭连接
}eTGCPAllowLost;


/*
 * 集群化指定区服登陆信息
 */
typedef struct
{
    unsigned int iTypeID;
    unsigned int iZoneID;
}TGCPSPECIFYZONE, *LPTGCPSPECIFYZONE;

/*
 * 集群化指定ServerID登陆信息
 */
typedef struct
{
    tgcp_ulonglong ullServerID;
}TGCPSPECIFYSERVER, *LPTGCPSPECIFYSERVER;

/*
 * 集群化登陆信息
 */
typedef union
{
    TGCPSPECIFYZONE stZone;
    TGCPSPECIFYSERVER stServer;
}TGCPROUTE, *LPTGCPROUTE;

/*
 * 跨服跳转附带参数(集群化断线重连结构和其一样，可直接复用)
 */
typedef struct
{
    unsigned int dwJumpPosition;
    char szConnData[TGCP_DATA_LEN];
}TGCPJUMP, *LPTGCPJUMP;

// windows 环境下初始化socket环境
#if defined(_WIN32) || defined(_WIN64)
int tgcpapi_WSA_startup(int a_iMaj, int a_iMin);
int tgcpapi_WSA_cleanup();
#endif

/// @defgroup GCP_API_MANAGE TGCP_句柄操作
/// @{

/// @brief  创建TGCP句柄
///
/// @param[in, out]  a_ppHandle tgcpapi句柄指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
/// @Warning  与tgcapi_ex.h定义的接口冲突，
///           严禁在调用tgcpapi_create_and_init的地方调用此接口
int tgcpapi_create(HTGCPAPI* a_ppHandle);


/// @brief  销毁TGCP句柄
///
/// @param[in, out]  a_ppHandle tgcpapi句柄指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_destroy(HTGCPAPI* a_ppHandle);

/// @brief  初始化TGCP句柄资源
///
/// @param[in]  a_pHandle       tgcpapi句柄
/// @param[in]  a_iServiceID    业务ID，TConnd3.1.7及以上版本，此字段无意义。其取值会被TConnd配置文件的BusinessID覆盖。
/// @param[in]  a_iPlatformID   平台信息
/// @param[in]  a_iAuthType     鉴权类型,请使用 tagAuthType 枚举值
/// @param[in]  a_iEncMethod    数据加密方式,请使用 tagEncryptMethod 枚举值
/// @param[in]  a_iKeyMode      密钥协商方式,请使用 tagKeyMaking 枚举值
/// @param[in]  a_iBuffLen      用户数据缓冲区长度
///
/// @return =0, success,
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note init接口调用完毕后，需要调用一系列的set接口进行设置，才可以发起连接。
///       a_iAuthType建议使用tagAuthType枚举值，但是支持该枚举值以外的值。对于用户
///       自定义鉴权或TCONND不支持的第三方平台鉴权方式，API会透传该字段的值给用户自定义鉴权
///       服务器，但0~32767是TCONND保留值，用户自定义鉴权类型的值只可以使用32768~65535。
/// @note 这个接口a_iAuthType只支持TGCP_AUTH_NONE、TGCP_AUTH_WX_O2_CODE、TGCP_AUTH_WX_O2_RTK或者TGCP_AUTH_QQ_O2_ATK等。
/// @Warning  与tgcapi_ex.h定义的接口冲突，
///           严禁在调用tgcpapi_create_and_init的地方调用此接口
///
/// @see tgcpapi_set_appid()
/// @see tgcpapi_set_token()
/// @see tgcpapi_set_account()
int tgcpapi_init(HTGCPAPI a_pHandle,
                 int a_iServiceID,
                 ePlatformID a_iPlatformID,
                 unsigned short a_iAuthType,
                 eEncryptMethod a_iEncMethod,
                 eKeyMaking a_iKeyMode,
                 int a_iBuffLen);


/// @brief  清理TGCP句柄资源
/// @param[in]  a_pHandle       tgcpapi句柄
///
/// @return =0, success,
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
int tgcpapi_fini(HTGCPAPI a_pHandle);

/// @}


/// @defgroup GCP_API_PROTOCOL TGCP_选项设置
/// @{

/// @defgroup Common  共有接口
/// @{
///
/// @brief  获得socket文件描述符
/// @param[in]  a_pHandle    tgcpapi句柄
/// @param[out] a_piFd       socket fd句柄指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_get_socket(HTGCPAPI a_pHandle, int* a_piFd);
///
/// @}


/// @defgroup OAuth  使用OAuth方式鉴权方式特有接口
/// @{
///
/// @brief  设置AppID
/// @param[in]  a_pHandle   tgcpapi句柄
/// @param[in]  a_pszOpenID AppID指针
/// @param[in]  a_iLen      AppID长度
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 传入申请的的AppID
/// @Warning  与tgcapi_ex.h定义的接口重复，
///           不建议在调用tgcpapi_create_and_init的地方调用此接口
int tgcpapi_set_appid(HTGCPAPI a_pHandle, const char* a_pszAppID, int a_iLen);

/// @brief  设置鉴权信息
///
/// @param[in]  a_pHandle   tgcpapi句柄
/// @param[in]  a_pszToken  Token指针
/// @param[in]  a_iLen      Token长度
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note a_pszToken参数可以传入OAuth中的Code，也可以传入Token，但使用Code进行鉴权的做法，
///       应逐渐转换为使用Token，因为Token才是与鉴权相关的信息
///       对于手Q的OAuth鉴权方式，需要传入AcessToken
///       对于微信的OAuth鉴权方式，需要传入RefreshToken
///       对于CTLogin鉴权方式，需要传入ST票据
/// @Warning  与tgcapi_ex.h定义的接口冲突，
///           严禁在调用tgcpapi_create_and_init的地方调用此接口
int tgcpapi_set_token(HTGCPAPI a_pHandle, const char* a_pszToken, int a_iLen);

/// @brief  设置Start包扩展字段
///
/// @param[in]  a_pHandle   tgcpapi句柄
/// @param[in]  a_iExt      扩展字段
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
int tgcpapi_set_synreserved(HTGCPAPI a_pHandle, int a_iExt);


/// @brief  设置AcessToken的有效期
/// @param[in]  a_pHandle   tgcpapi句柄
/// @param[in]  a_iExpire   剩余有效时间,以秒为单位
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获得错误原因
///
/// @note 使用QQ鉴权模式时，可以把AcessToken的有效期传给后端作为参考
int tgcpapi_set_acesstoken_expire(HTGCPAPI a_pHandle, unsigned short a_iExpire);


/// @brief  获取AcessToken
///
/// @param[in]       a_pHandle   tgcpapi句柄
/// @param[in, out]  a_pszToken  保存AcessToken缓冲区的指针
/// @param[in, out]  a_piLen     保存AcessToken缓冲区长度的指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 使用WXCode鉴权的方式时，可以调用该接口获得AcessToken
int tgcpapi_get_acesstoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);


/// @brief  获取RefreshToken
///
/// @param[in]       a_pHandle   tgcpapi句柄
/// @param[in, out]  a_pszToken  Token指针
/// @param[in, out]  a_iLen      Token长度
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 使用WXCode鉴权的方式时，可以调用该接口获得RefreshToken
int tgcpapi_get_refreshtoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);


/// @brief  设置账号信息
/// @param[in]  a_pHandle       tgcpapi句柄
/// @param[in]  a_pstAccount    账号结构体指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 对于API，Uin、OpenID都属于账号信息，标识用户的唯一性
/// @Warning  与tgcapi_ex.h定义的接口重复，
///           不建议在调用tgcpapi_create_and_init的地方调用此接口
int tgcpapi_set_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);


/// @brief  获取账号信息
/// @param[in]  a_pHandle       tgcpapi句柄
/// @param[in]  a_pstAccount    账号结构体指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @see tgcpapi_update()
int tgcpapi_get_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);


/// @brief  获取OpenID
/// @param[in]       a_pHandle      tgcpapi句柄
/// @param[in, out]  a_pszOpenID    保存OpenID缓冲区的指针
/// @param[in, out]  a_piLen        保存OpenID缓冲区的长度指针
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 使用WXCode鉴权的方式时，可以调用该接口直接获得OpenID
int tgcpapi_get_openid(HTGCPAPI a_pHandle, char* a_pszOpenID, int* a_piLen);


/// @brief  设置DH交换秘钥算法基本信息
/// @param[in]      a_pHandle       tgcpapi句柄
/// @param[in]      a_DH_p_hex      大素数p的Hex值
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 使用DH算法生成秘钥时，需要调用该接口设置DH秘钥
/// @Warning  与tgcapi_ex.h定义的接口重复，
///           不建议在调用tgcpapi_create_and_init的地方调用此接口
int tgcpapi_set_dh_info(HTGCPAPI a_pHandle, const char* a_DH_p_hex);


/// @brief  设置集群化登陆时的路由信息
/// @param[in]      a_pHandle        tgcpapi句柄
/// @param[in]      a_iRouteType     路由类型
/// @param[in]      a_pstRouteInfo   路由信息
///
/// @return =0, success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 1.默认为TGCP_ROUTE_NONE，非集群化登陆
///       2.当接入集群化TCONND登陆时，可以调用该接口指定集群化登录信息
///       3.该接口在init(初始化)之后、connection(发起连接)之前调用可以设置登集群时选择GAMESVR的方式，
///         可以选择 TGCP_ROUTE_SPECIFY_ZONE、TGCP_ROUTE_SPECIFY_SERVER 这两个值
int tgcpapi_set_route_info(HTGCPAPI a_pHandle, eTGCPRouteType a_iRouteType, const LPTGCPROUTE a_pstRouteInfo);

/// @}


/// @defgroup GCP_API_CONNECTION TGCP_通信管理
/// @{

/// @brief  客户端发起与tconnd的连接、握手(SYN/ACK)、鉴权(加密)，同步等待响应。
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pszUrl   tconnd url(格式示例：tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout 发起连接、握手、认证整个过程最大超时时间值(单位：ms)
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///          其中，返回TGCP_ERR_STAY_IN_QUEUE表明正在排队，通过tgcpapi_query_wait_notify
///          获取或更新排队信息，此接口同时可以确定是否完成排队，完成排队后就可以正常通信
///
///          返回TGCP_ERR_PEER_STOPPED_SESSION时，调用tgcpapi_query_stopped查询原因
///
/// @note 在调用start_connection前，业务必须先调用tgcpapi_init进行相应的初始化操作
///       同时需要调用tgcpapi_set_*等接口设置相关的OAUTH信息
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
/// @see  tgcpapi_query_stopped()
int tgcpapi_start_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);


/// @brief  客户端关闭与tconnd的连接
///
/// @param[in]  a_pHandle tgcpapi句柄
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_close_connection(HTGCPAPI a_pHandle);


/// @brief  客户端重连通信会话
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pszUrl   tconnd url(格式示例：tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout 整个重连过程最大超时时间值(单位：ms)
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///          其中，返回TGCP_ERR_STAY_IN_QUEUE表明正在排队，通过tgcpapi_query_wait_notify
///          获取或更新排队信息，此接口同时可以确定是否完成排队，完成排队后就可以正常通信
///
/// @note 在此之前，业务保证已经调用tgcpapi_init进行相应的初始化操作
///       同时已经调用tgcpapi_set_*等接口设置相关的OAUTH
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
///
/// @note 重要说明：建议当且仅当tgcpapi_recv 或 tgcpapi_peek返回TGCP_ERR_NETWORK_EXCEPTION时调用
int tgcpapi_relay_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);


/// @biref 跨服跳转
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pszUrl   tconnd url(格式示例：tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout 整个重连过程最大超时时间值(单位：ms)
/// @param[in]  a_pExtData 跨服跳转额外带信息 
/// @param[in]  a_pServerId 指定要跳转服务器id(非集群模式为空指针即可)
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///          其中，返回TGCP_ERR_STAY_IN_QUEUE表明正在排队，通过tgcpapi_query_wait_notify
///          获取或更新排队信息，此接口同时可以确定是否完成排队，完成排队后就可以正常通信
///
/// @note 在此之前，业务保证已经调用tgcpapi_init进行相应的初始化操作
///       同时已经调用tgcpapi_set_*等接口设置相关的OAUTH
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
///
int tgcpapi_jump_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout,
        const LPTGCPJUMP a_pExtData, const tgcp_ulonglong* a_pServerId);
///  @}


/// @brief 在tgcpapi_jump_connection或tgcpapi_start_connection调用成功后获得pos，ident相关信息
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///
/// @see  tgcpapi_jump_connection()或tgcpapi_start_connection()
///
/// @note 在TGCP_ERR_NONE != tgcpapi_jump_connection()
///   或  在TGCP_ERR_NONE != tgcpapi_start_connection()时调用
int tgcpapi_get_iden_info(HTGCPAPI a_pHandle, LPTGCPJUMP a_pIdentInfo);

// 异步通信模式
//
// 单线程模式下，这种模式完全异步，同时如果多线程模式下需要使用这种异步通信模式的话，
// 一定要保证网络收发只在一个线程中完成
enum {
    TGCP_EVENT_SSTOPED      = 0x01,     // 服务端主动关闭会话
    TGCP_EVENT_WAITING      = 0x02,     // 正在排队中
    TGCP_EVENT_DATA_IN      = 0x04,     // 接收缓冲区包可读（收到至少一个完整包）
    TGCP_EVENT_DATA_OUT     = 0x08,     // 发送缓冲区可写
    TGCP_EVENT_ATK          = 0x10,     // ATK 续期通知
    TGCP_EVENT_SVR_IS_FULL  = 0x20,     // 服务器爆满通知
    TGCP_EVENT_ROUTE_CHANGE = 0x40,     // 路由跳转通知
};

typedef struct tagGcpEvent {
    unsigned int iEvtNum;
    unsigned int iEvents;
} GCPEVENT, *LPGCPEVENT;


/// @defgroup GCP_API_CONNECTION_ASYNC TGCP_异步模式通信管理
/// @{

/// @brief 启动与后端服务的通信会话
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pszUrl   tconnd url(格式示例：tcp://127.0.0.1:8090)
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 在调用tgcpapi_start前，业务必须先调用tgcpapi_init进行相应的初始化操作
///       同时需要调用tgcpapi_set_*等接口设置相关的OAUTH信息
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_update()
/// @see  tgcpapi_stop()
int tgcpapi_start(HTGCPAPI a_pHandle, const char* a_pszUrl);

/// @brief 刷新与后端服务的通信会话状态
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pstEvent 向上层反馈事件（SSTOP、WAITING、DATAIN、DATAOUT、ATK）
///                        如果a_pstEvent->iEvtNum值大于0时，通过检查a_pstEvent->iEvents来确定事件源
///                        比如判断 a_pstEvent->iEvents & TGCP_EVENT_SSTOPED是否为true来确定是否有SSTOP事件
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 需要上层不断调用去驱动整个通信会话
/// @note 当返回TGCP_ERR_PEER_CLOSED_CONNECTION或TGCP_ERR_NETWORK_EXCEPTION时，仍然需要通过（iEvents & TGCP_EVENT_DATAIN）来判断是否可读
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_start()
/// @see  tgcpapi_resume()
int tgcpapi_update(HTGCPAPI a_pHandle, LPGCPEVENT a_pstEvent);

/// @brief 恢复（重连）与后端服务的通信会话，一般在正常通信过程中网络出现异常后调用
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_pszUrl   tconnd url(格式示例：tcp://127.0.0.1:8090)
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @see  tgcpapi_start()
/// @see  tgcpapi_update()
/// @see  tgcpapi_stop()
int tgcpapi_resume(HTGCPAPI a_pHandle, const char* a_pszUrl);

/// @brief 终止与后端服务的通信会话
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @see  tgcpapi_start()
/// @see  tgcpapi_update()
/// @see  tgcpapi_resume()
int tgcpapi_stop(HTGCPAPI a_pHandle);

/// @brief 在update获取到排队通知事件后，获取排队状态
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  !NULL, success
///          NULL, fail
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_WAITING)为true时调用
const QUEUENOTIFY* tgcpapi_get_queue(HTGCPAPI a_pHandle);

/// @brief 在update获取到服务端关闭会话事件后，获取关闭原因
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_SSTOPED)为true时调用
int tgcpapi_get_sstop_reason(HTGCPAPI a_pHandle);

/// @brief 在update获取到socket异常断开时，通过此接口获取socket错误码
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note 在tgcpapi_update返回TGCP_ERR_NETWORK_EXCEPTION时调用
int tgcpapi_get_last_socket_error(HTGCPAPI a_pHandle);

/// @brief 在update获取到服务端关闭会话事件后，获取tconnd对应的ip、port和id
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[out] a_pszTconndIp  获取得到的ip字符串头指针
/// @param[in, out] a_pszTconndIpSize  传入时必须是TconndIp空间长度(一般为16),
///  输出是获得的真实内容长度
/// @param[out] a_pszTconndId  获取得到的id字符串头指针
/// @param[in, out] a_pszTconndIdSize  传入时必须是TconndId空间长度(一般为32)，
///  输出是获得的真实内容长度
/// @param[out] a_wTconndPort 获取得到的Tconnd端口
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_SSTOPED)为true时调用
int tgcpapi_get_sstop_ip_port_id(HTGCPAPI a_pHandle, char* a_pszTconndIp,
        int& a_iTconndIpSize, char* a_pszTconndId, int& a_iTconndIdSize, unsigned short& a_wTconndPort);

/// @brief 在成功建立连接或update获取到路由切换的通知后，获取重定向后的GAMESVR的ServerID
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_ROUTE_CHANGE)为true时调用
tgcp_ulonglong tgcpapi_get_route_serverid(HTGCPAPI a_pHandle);

/// @brief 在update获取到服务端关闭会话事件后，获取关闭的详细原因
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_SSTOPED)为true时调用
/// @note 当stopreason=7(gamesvr主动关闭连接)时，excode表示gamesvr的stopreason
/// @note 当stopreason=8(鉴权失败时),excode表示具体鉴权失败的原因，其中
/// @note     0x10000001，表示鉴权代理服务不可用
/// @note     0x10000002，表示鉴权类型为0，TCONND不支持
/// @note     其它值：表示当前票据已经失效，客户端需要重新拉取票据
/// @note 当stopreason为其他值是，excode无意义
int tgcpapi_get_sstop_excode(HTGCPAPI a_pHandle);

/// @brief 在update获取到可收完整包事件后，获取包长度
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///          else, 包长
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_DATA_IN)为true时调用
int tgcpapi_recved_pkglen(HTGCPAPI a_pHandle);

/// @brief 在update获取到可发送包事件后，获取可用空间
///
/// @param[in]  a_pHandle  tgcpapi句柄
///
/// @return  -1, invalid
///          else, 可用缓冲区空间大小
///
/// @see  tgcpapi_update()
///
/// @note 在(events & TGCP_EVENT_DATA_OUT)为true时调用
int tgcpapi_sendable_buflen(HTGCPAPI a_pHandle);

///  @}

/// @defgroup TGCP_API_SEND_RECV TGCP_数据收发
/// @{

/// @brief  发送业务数据，保证整个业务数据包发完为止
///
/// @param[in]  a_pHandle    tgcpapi句柄
/// @param[in]  a_pszBuffIn  业务数据缓冲区指针
/// @param[in]  a_iSize      业务数据大小
/// @param[in]  a_iTimeout   发送业务数据包最大超时值(单位：ms), 建议设置为0
///
/// @return  =0, 发送成功
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 返回值为TGCP_ERR_SEND_UNCOMPLETE时，说明可能网络状况不佳，剩余未发送值socket的数据
///       保存在句柄的缓冲区中，随着下一次调用tgcpapi_send时一起copy至socket缓存中，
///       也可以选择再次调用tgcpapi_flush尝试尽量发完。一般情况下，建议客户端此时不要关闭连接。
/// @note 此接口即使返回错误，也不影响tgcpapi_recv和tgcpapi_peek的接收
///
/// @see tgcpapi_flush
int tgcpapi_send(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout);

/// @brief  向指定路由信息发送业务数据，保证整个业务数据包发完为止
///
/// @param[in]  a_pHandle         tgcpapi句柄
/// @param[in]  a_pszBuffIn       业务数据缓冲区指针
/// @param[in]  a_iSize           业务数据大小
/// @param[in]  a_iTimeout        发送业务数据包最大超时值(单位：ms), 建议设置为0
/// @param[in]  a_iRouteType      路由类型
/// @param[in]  a_pstRouteInfo    路由信息
/// @param[in]  a_iAllowLost  是否允许丢失消息
///
/// @return  =0, 发送成功
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 返回值为TGCP_ERR_SEND_UNCOMPLETE时，说明可能网络状况不佳，剩余未发送值socket的数据
///       保存在句柄的缓冲区中，随着下一次调用tgcpapi_send时一起copy至socket缓存中，
///       也可以选择再次调用tgcpapi_flush尝试尽量发完。一般情况下，建议客户端此时不要关闭连接。
/// @note 此接口即使返回错误，也不影响tgcpapi_recv和tgcpapi_peek的接收
/// @note 当a_iAllowLost设置为 TGCP_NOT_ALLOW_LOST，后端找不到指定Gamesvr时，连接会关闭，
///       当a_iAllowLost设置为 TGCP_ALLOW_LOST，后端找不到指定Gamesvr时，连接不会关闭。
///
/// @see tgcpapi_flush
int tgcpapi_send_with_route(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout,
                            eTGCPRouteType a_iRouteType, const LPTGCPROUTE a_pstRouteInfo, eTGCPAllowLost a_iAllowLost);

/// @brief  flush句柄发送缓冲区中剩余数据至网络缓冲区
///
/// @param[in]   a_pHandle   tgcpapi句柄
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///          返回值为TGCP_ERR_FLUSH_UNCOMPLETED时，表明本次flush未完全冲洗干净
///
/// @note 返回值为TGCP_ERR_SEND_UNCOMPLETE时，说明数据包没有完全发送至网络，可以调用tgcpapi_flush或者
///       等待tgcpapi_send触发与下一个包一起发送。
/// @note 建议如果发包频率不高的情况下，tgcpapi_send返回TGCP_ERR_SEND_UNCOMPLETE时，主动调用tgcpapi_flush
///       去冲洗未发完数据包。尤其需要注意的是，通信最后一个包在未完全发送时，需要调用此接口保证发送完整。
/// @note 此接口即使返回错误，也不影响tgcpapi_recv和tgcpapi_peek的接收
///
/// @see tgcpapi_send
int tgcpapi_flush(HTGCPAPI a_pHandle);


/// @brief  接收业务数据，保证接收一个完整业务包
///
/// @param[in]   a_pHandle     tgcpapi句柄
/// @param[out]  a_pszBuffOut  接收业务数据缓冲区空间指针
/// @param[in]   a_piSize      接收业务数据缓冲区空间大小（指针）
/// @param[out]  a_piSize      实际接收到的业务数据大小（指针）
/// @param[in]   a_iTimeout    接收业务数据包最大超时时间值(单位：ms), 建议设置为0
///
/// @return  =0, 接收成功
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 返回值为TGCP_ERR_PEER_STOPPED_SESSION时，表明服务端主动终止会话
/// @note 返回值为TGCP_ERR_PKG_NOT_COMPLETE时，表明收包不完整，建议上层调用此时不要关闭连接
/// @note 返回值为TGCP_HAVE_RECV_GCP_INNER_MSG时，表明收到gcp内部消息，已经在内部处理掉，但是后续可能还有业务包，要继续尝试接受
/// @note 即使tgcpapi_send或tgcpapi_flush返回失败，调用此接口仍然可以接收数据。
///
/// @see tgcpapi_query_stopped
int tgcpapi_recv(HTGCPAPI a_pHandle, char* a_pszBuffOut, int* a_piSize, int a_iTimeout);


/// @brief  从tgcpapi句柄中获取完整业务包的地址及长度
///
/// @param[in]   a_pHandle   tgcpapi句柄
/// @param[out]  a_ppszBuff  保存完整业务包地址的指针值
/// @param[out]  a_piSize    保存完整业务包长度的指针值
/// @param[in]   a_iTimeout  接收业务数据包最大超时时间值(单位：ms), 建议设置为0
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 特别注意：调用tgcpapi_peek可以避免不必要的拷贝，但是必须在peek取到数据指针后马上处理，
///       因为再次调用tgcpapi_peek 或 tgcpapi_recv会把指针指向的内容覆盖掉
/// @note 返回值为TGCP_ERR_PEER_STOPPED_SESSION时，表明服务端主动终止会话
/// @note 返回值为TGCP_ERR_PKG_NOT_COMPLETE时，表明收包不完整，一般建议上层调用收到这个时不要关闭连接
/// @note 即使tgcpapi_send或tgcpapi_flush返回失败，调用此接口仍然可以接收数据。
int tgcpapi_peek(HTGCPAPI a_pHandle, const char** a_ppszBuff, int* a_piSize, int a_iTimeout);


/// @brief  客户端向tconnd主动终止通信会话
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[in]  a_iTimeout 发送终止通信会话的最大超时值(单位：ms)
///
/// @return =0, 接收成功
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
int tgcpapi_stop_session(HTGCPAPI a_pHandle, int a_iTimeout);

///  @}

/// @defgroup TGCP_API_EVENT TGCP_事件探询
/// @{

/// @brief  探询服务端是否终止通信会话
///
/// @param[in]  a_pHandle  tgcpapi句柄
/// @param[out] a_piResult 查询结果，0(对端未终止), 非0(对端终止)
/// @param[out] a_piReason 终止会话原因，如果非NULL返回
///
/// @return =0 success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 推荐当返回值为TGCP_ERR_PEER_STOPPED_SESSION时，上层调用此接口查询原因，
///       也可以在其它地方调用此接口查询是否终止以及终止原因。上行终止通信会话时，建议对通信进行
///       收尾工作后，调用tgcpapi_close_connection关闭socket连接。
///       一般来说，在返回TGCP_ERR_PEER_STOPPED_SESSION时就可以确定服务器已经主动终止会话，
///       如果不需要相关清理或收尾工作，建议检查到这个返回值后直接关闭socket连接。
///
/// @see tgcpapi_start_connection()
///      tgcpapi_recv()
///      tgcpapi_peek()
///      tgcpapi_close_connection()
int tgcpapi_query_stopped(HTGCPAPI a_pHandle, int* a_piResult, int* a_piReason);

/// @brief 探询或更新上行的排队通知
///
/// @param[in]  a_pHandle      tgcpapi句柄
/// @param[out] a_piBingo      是否完成排队的标志，!0(完成排队，可以正常通信), 0(仍然处于排队状态)
/// @param[out] a_pstQueNotify 最新排队信息，当*a_piBingo返回0时有意义
/// @param[in]  a_iTimeout     收取排队及完成排队通知的最大超时时间值(单位：ms)
///
/// @return =0 success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @see tgcpapi_start_connection
///
/// @note 此接口只有在tgcpapi_start_connection返回TGCP_ERR_STAY_IN_QUEUE后调用才有意义，
///       用以获取最新的排队信息，以及是否已经完成排队(完成排队后才可以进行正常的数据收发)
int tgcpapi_query_wait_notify(HTGCPAPI a_pHandle, int* a_piBingo,
                              LPQUEUENOTIFY a_pstQueNotify, int a_iTimeout);
///  @}

/// @defgroup TGCP_LAST_ERROR  TGCP-错误信息相关
/// @{

/// @brief  获取最近一次TDR打解包等接口错误描述信息（定位问题时辅助信息）
///
/// @param[in]  a_pHandle tgcpapi句柄
///
/// @return  错误描述字符串指针，如果为NULL，表明没有相关tdr错误描述信息
///
/// @note 重要说明：当且仅当返回值为TGCP_ERR_PACK_FAILED 或 TGCP_ERR_UNPACK_FAILED时调用有意义
const char* tgcpapi_get_tdr_last_error_string(HTGCPAPI a_pHandle);


/// @brief   获取最近一次非预期命令字值（定位问题时辅助信息）
///
/// @param[in]  a_pHandle
///
/// @return  非预期command命令字值
///          -1, error
///
/// @note 重要说明：当前仅当返回值为TGCP_ERR_UNEXPECTED_COMMAND时调用有意义
int tgcpapi_get_last_unexpected_cmd(HTGCPAPI a_pHandle);

///  @}


/// @defgroup TGCP_OPERATE  TGCP-操作
/// @{
///
/// @brief 请求续期AcessToken
///
/// @param[in] a_pHandle    tgcpapi句柄
/// @param[in] a_iTimeout   超时时间(单位：ms)
///
/// @return =0 success
///         !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 此接口是为微信OAuth鉴权设计，微信鉴权中的AccessToken有效期约为2个小时，在AcessToken失效前，
///       可调用本接口为AcessToken续期，手Q鉴权中的AcessToken过期后，需要拉取新的AcessToken
int tgcpapi_refresh_acesstoken(HTGCPAPI a_pHandle, int a_iTimeout);



/// @brief 是否需要用户重新授权
///
/// @param[in] a_pHandle   tgcpapi句柄
///
/// @return >0, 需要用户重新授权
///         =0, 不需要用户重新授权
///         <0, fail, 调用tgcpapi_error_string()获取错误原因
///
///
/// @note 在以下场景下需要调用该接口确认是否需要用户重新授权
///         同步模式调用 tgcpapi_start_connection 发起连接后，返回值为 TGCP_ERR_PEER_STOPPED_SESSION
///         异步模式调用 tgcpapi_update ，触发了 TGCP_EVENT_SSTOPED 事件
///
/// @note 通常通过调用 MSDK 的 WGLogin 接口去引导用户重新授权
///
int tgcpapi_need_user_reauth(HTGCPAPI a_pHandle);
/// @}

int tgcpapi_report_access_token(HTGCPAPI a_pHandle, const char* a_pszAccessToken, unsigned int expire);


/// @brief  设置GCP协议打包的版本号
///
/// @param[in]   a_pHandle   tgcpapi句柄
/// @param[in]   a_iHeadVersion   协议Head Version，由于GCP协议分两段打包，因此需要提供两个版本号
/// @param[in]   a_iBodyVersion   协议Body Version
/// @note 说明 对端tconnd版本在3.1.9-3.1.16时，请设置HeadVersion=5,BodyVersion=8
///
/// @return  =0, success
///          !0, fail, 调用tgcpapi_error_string()获取错误原因
///
/// @note 说明：当TCONND版本比客户端高或者相同时，不需要调用此接口，在客户端版本比TCONND版本高时，才需要调用
/// @note 说明：GCP默认会使用当前TDR协议的最高版本号，在调用该接口设置版本号后，系统将使用设置的版本号进行TDR协议的打解包
/// 
/// @note 调用时机在tgcpapi_init或tgcpapi_create_and_init之后
///
int tgcpapi_set_gcp_protocol_version(HTGCPAPI a_pHandle, int a_iHeadVersion, int a_iBodyVersion);

/// @brief  获取客户端的版本号
const char* tgcpapi_get_version();

#endif
