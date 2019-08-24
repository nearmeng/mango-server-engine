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
        TCLTAPI_ERR_NET                        =1,        /**< 网络错误*/
        TCLTAPI_ERR_TDR                        =2,        /**< TDR打解包失败*/
        TCLTAPI_ERR_BUF                        =3,       /**< 接收缓冲区不够*/
        TCLTAPI_ERR_INVALID_INPUT              =4,       /**< 输入参数非法*/
        TCLTAPI_ERR_INVALID_PKG_LENGTH         =5,    /**< 包长度非法*/
        TCLTAPI_ERR_COUNT
    }TCLTAPI_ERR;


    struct tagTCltapiHandle;
    typedef struct tagTCltapiHandle     TCLTAPIHANDLE;
    typedef struct tagTCltapiHandle     *HTCLTAPI;

    /**
     *@brief: 日志回调函数定义,只保存a_pArg为回调数据,a_iLogLevel设定日志输出级别,一般需要应用将信息保存到回调数据
     */

    typedef int (*TCLTAPI_PFN_LOG)(IN HTCLTAPI a_phClt, IN int a_iLogLevel,IN void * a_pArg,const char* a_pszFmt, ...);



    /**
     *@brief     创建句柄.连接对端并加载句柄socket = tcltapi_new + tcltapi_make_connect+tcltapi_attach
     *@param   a_phClt[IN]            对端URL
     *@param   a_iTimeout[IN]        超时时间
     *@param   a_phClt[IN]            API 句柄

     *@retval    = 0   成功
     *@retval   < 0    失败
     *@retval   -1     连接失败
     *@retval   -2      创建句柄失败
     *@retval   -3      WSAStartup返回失败,WSAGetLastError

    */
    TSF4G_API int tcltapi_open(IN const char* a_pszUri, IN int a_iTimeout, IN HTCLTAPI* a_phClt);

    /**
     *@brief     创建句柄.连接对端并加载句柄socket = tcltapi_new + tcltapi_make_connect+tcltapi_attach
     *@param   a_phClt[IN]            对端URL
     *@param   a_iTimeout[IN]        超时时间
     *@param   a_phClt[IN]            API 句柄
     *@param   a_iIsTlvMode[IN]      是否tdr2.0 是为true

     *@retval    = 0   成功
     *@retval   < 0    失败
     *@retval   -1     连接失败
     *@retval   -2      创建句柄失败
     *@retval   -3      WSAStartup返回失败,WSAGetLastError

    */
    TSF4G_API int tcltapi_open_tlv_mode(IN const char* a_pszUri, IN int a_iTimeout, IN HTCLTAPI* a_phClt, IN int a_iIsTlvMode);

    /**
     *@brief     注册tdr模式
     *@param   a_phClt[IN]        API 句柄
     *@param   a_bIsTlvMode[IN]   是否tdr2.0 是为true
     *@retval
     *@retval      =0   成功
     *@retval      < 0   失败
     *@retval      =-1  输入参数错误
     */
    TSF4G_API int tcltapi_set_mode(IN HTCLTAPI* a_phClt, IN int a_iIsTlvMode);


    /**
     *@brief     释放句柄
     *@param   a_phClt[IN]        API 句柄
     *@retval
     *@retval      =0   成功
     *@retval      < 0   失败
     *@retval      =-1  输入参数错误
     */
    TSF4G_API int tcltapi_free(IN HTCLTAPI* a_phClt);



    /**
     *@brief     设置句柄默认日志系统日志级别及日志路径
     *@param   a_phClt[IN]            API 句柄
     *@param   a_iLogLevel[IN]        日志级别,具体定义参考
     *@param   a_phClt[IN]            日志文件路径,若为空使用默认日志文件路径宏TCLTAPI_DEFAULT_LOGPATTEN:即./tcltapi_log/tcltapi_%Y-%m-%d.log.%F表示按日期记录日志更多format参数参考strftime
     *@retval    = 0      成功
     *@retval    < 0      失败
     *@retval    = -1   参数非法
     *@retval    =-2    初始化Log失败
     *@retval    =-3    获取Log实例失败
     */
    TSF4G_API int tcltapi_set_log_default(IN HTCLTAPI a_hClt, IN int a_iLogLevel, IN const char *a_pszLogPattern);



    /**
     *@brief 设置句柄应用自定义日志系统回调函数
     *@param       a_hClt[IN]               句柄
     *@param       a_pfnLogCallBack[IN]       日志回调函数
     *@param       a_iLogLevel[IN]                设定日志输出级别,需要将该字段保存到回调数据中,以便支持不同日志级别输出过滤
     *@param       a_pArg                            日志函数回调数据
     *@retval            =0 success
     *@retval        <0  fail
     */
    TSF4G_API int     tcltapi_set_log_callback(IN HTCLTAPI a_hClt, IN TCLTAPI_PFN_LOG a_pfnLogCallBack,IN int a_iLogLevel,IN void* a_pArg);


    /**
     *@brief     设置句柄元数据描述
     *@param   a_phClt[IN]                API 句柄
     *@param   a_pstRecvMeta[IN]        下行meta
     *@param   a_pstSendMeta[IN]        上行meta
     *@retval    = 0      成功
     *@retval    < 0      失败
     *@retval    -1        分配内存失败
     */
    TSF4G_API int tcltapi_set_pdu_meta(IN HTCLTAPI a_hClt,IN  LPTDRMETA a_pstRecvMeta,IN LPTDRMETA a_pstSendMeta);


    /**
     *@brief     设置句柄接收元数据长度描述成员路径,需要先调用tcltapi_set_pdu_meta
     *@param   a_phClt[IN]                API 句柄
     *@param   a_szHeadLenPath[IN]        head meta路径
     *@param   a_szBodyLenPath[IN]        body meta路径,NULL表示只用a_szHeadLenPath可以表示pkglen长度

     *@retval    = 0    成功
     *@retval    < 0    失败
     */
    TSF4G_API int tcltapi_set_pkglen_indicator(IN HTCLTAPI a_hClt,IN  const char * a_szHeadLenPath,IN const char *  a_szBodyLenPath);



    /**
     *@brief     设置承载协议对端版本,一般不用设
     *@param   a_phClt[IN]                API 句柄
     *@param   a_iSelfVersion[IN]        客户端版本
     *@param   a_iPeerVersion[IN]        服务器版本
     *@retval
     */
    TSF4G_API void tcltapi_set_version(IN HTCLTAPI a_hClt, IN int a_iSelfVersion,IN int a_iPeerVersion);


    /**
     *@brief     发送网络包
     *@param   a_phClt[IN]                API 句柄
     *@param   a_pszBuff[IN]            网络缓冲区地址
     *@param   a_iBuff[IN]                数据长度
     *@param   a_iTimeout[IN]            发送超时时间,-1阻塞发送,单位为毫秒,一般设为零

     *@retval    = 0     成功
     *@retval    <0      失败
     *@see    使用tcltapi_get_error和tcltapi_get_errorstring获得错误提示信息
     */
    TSF4G_API int tcltapi_send_buff(IN HTCLTAPI a_hClt,IN const char* a_pszBuff,IN int a_iBuff,IN int a_iTimeout);



    /**
     *@brief     接收网络包
     *@param   a_phClt[IN]                API 句柄
     *@param   a_pszBuff[IN]            数据缓冲区地址
     *@param   a_piBuff[IN,OUT]            [IN]数据缓冲区长度,[OUT]消息包长度
     *@param   a_iTimeout[IN]            收包超时时间,-1阻塞收包,单位为毫秒
     *@retval    =0    没有收到包
     *@retval    =1    收到包
     *@retval    <0    收包失败
     *@see    使用tcltapi_get_error和tcltapi_get_errorstring获得错误提示信息
     */
    TSF4G_API int tcltapi_recv_buff(IN HTCLTAPI a_hClt,IN  char* a_pszBuff,INOUT  int *a_piBuff,IN int a_iTimeout);


    /**
     *@brief     释放句柄socket
     *@param   a_phClt[IN]        API 句柄
     *@retval      返回当前socket
     */
    TSF4G_API int tcltapi_detach(IN HTCLTAPI a_hClt);


    /**
     *@brief     加载句柄socket
     *@param   a_phClt[IN]        API 句柄
     *@retval
     */
    TSF4G_API void tcltapi_attach(IN HTCLTAPI a_hClt,IN int a_s);


    /**
     *@brief     发送本地数据结构
     *@param   a_phClt[IN]                API 句柄
     *@param   a_pszMsg[IN]            本地数据地址
     *@param   a_iMsg[IN]                数据长度
     *@param   a_iTimeout[IN]            发送超时时间,-1阻塞发送,单位为毫秒,一般设为零

     *@retval        = 0    成功
     *@retval        <0     失败
     *@see           tcltapi_get_error/tcltapi_get_errorstring得到返回值/提示信息
     */
    TSF4G_API int tcltapi_send_msg(IN HTCLTAPI a_hClt, IN const char* a_pszMsg,IN int a_iMsg,IN  int a_iTimeout);


    /**
     *@brief     发送本地数据结构
     *@param   a_phClt[IN]                API 句柄
     *@param   a_pszMsg[IN]            本地数据缓冲地址
     *@param   a_piMsg[INOUT]            [IN]数据缓冲长度,[OUT]收到 数据包长度
     *@param   a_iTimeout[IN]            接收超时时间,-1阻塞发送,单位为毫秒,一般设为零

     *@retval    = 0    没有收到包
     *@retval    =1     收到包
     *@retval    <0     失败
     *@see        用tcltapi_get_error/tcltapi_get_errorstring得到返回值/提示信息
     */
    TSF4G_API int tcltapi_recv_msg(IN HTCLTAPI a_hClt, IN char* a_pszMsg,INOUT  int* a_piMsg, IN int a_iTimeout);


    /**
     *@brief     获得句柄 socket
     *@param   a_phClt[IN]                API 句柄
     *@retval   返回socket
     */
    TSF4G_API int tcltapi_get_sock(IN HTCLTAPI a_hClt);



    /**
     *@brief     设置socket 读缓冲区
     *@param   a_phClt[IN]                API 句柄
     *@param   a_iSize[IN]                          缓冲区大小
     *@retval    =0 成功
     *@retval    !=0 失败,取error得到错误提示信息
     */
    TSF4G_API int tcltapi_set_recvbuff(IN HTCLTAPI a_hClt,IN int a_iSize);


    /**
     *@brief     设置socket 写缓冲区
     *@param   a_phClt[IN]                API 句柄
     *@param   a_iSize[IN]                          缓冲区大小

     *@retval        =0    成功
     *@retval        !=0   失败,取error得到错误提示信息
     */
    TSF4G_API int tcltapi_set_sendbuff(IN HTCLTAPI a_hClt,IN int a_iSize);


    /**
     *@brief      获得错误码
     *@param   a_phClt[IN]                API 句柄
     *@retval    返回错误码
     */
    TSF4G_API int tcltapi_get_err(IN HTCLTAPI a_hClt);


    /**
     *@brief     获得错误提示信息
     *@param   a_phClt[IN]                API 句柄
     *@retval    返回错误提示信息
     */
    TSF4G_API const char *tcltapi_get_errstring(IN HTCLTAPI a_hClt);



    /** @} */

#ifdef __cplusplus
}
#endif

#endif /* TCLTAPI_H */


