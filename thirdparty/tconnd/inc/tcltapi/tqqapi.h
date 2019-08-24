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


    /** 错误码 */
    typedef enum
    {
        TQQAPI_ERR_NETWORK_FAILURE          =1,    	    /**< 收包失败,网络出错*/
        TQQAPI_ERR_TDR_NTOH_HEAD             =2,    	    /**< 包头网络转本地序失败*/
        TQQAPI_ERR_NOT_ENOUGHBUF             =3,  	        /**< 接收缓冲区不够*/
        TQQAPI_ERR_DECRYPTION_HEAD          =4,   	        /**< 包头解密失败*/
        TQQAPI_ERR_TDR_NTOH_BODY              =5, 	        /**< 包体网络转本地序失败*/
        TQQAPI_ERR_TDR_HTON_HEAD             =6,  	        /**< 包头本地转网络序失败*/
        TQQAPI_ERR_TDR_HTON_BODY             =7,  	        /**< 包体本地转网络序失败*/
        TQQAPI_ERR_RECV_TIMEOUT                =8,  	    /**< 下行包收包超时*/
        TQQAPI_ERR_DECRYPTION_BODY          =9,  	        /**< 解密包体失败*/
        TQQAPI_ERR_INVALID_CMD                  =10,        	 /**< 非法包头类型*/
        TQQAPI_ERR_TDR_HTON_USERIDENT   =11,	        /**< 内部定义*/
        TQQAPI_ERR_BUILD_AUTH                    =12, 	        /**< 内部定义*/
        TQQAPI_ERR_BUILD_SYNACK                 =13,	        /**< 内部定义*/
        TQQAPI_ERR_HANDLE_INVALID             =14,	        /**<句柄指针非法*/
        TQQAPI_ERR_ENCPYT_MSG	                 =15,           /**<加密消息失败*/
        TQQAPI_ERR_SENDBUF_LIMITED          =16,             /**<发送缓冲区剩余空间不够发送消息的长度*/
        TQQAPI_ERR_RECVBUFF_LIMITED          =17,            /**<接收缓冲区空间不够接收一个完整消息*/
        TQQAPI_ERR_SYSTEM                             =18,             /* *< 系统内部错误*/
        TQQAPI_ERR_SOCKSEND_FAIL                   =19,             /* *<发送包失败,网络错误*/
        TQQAPI_ERR_SOCKSENDBUF_FULL             =20,             /* *<发送包失败,socket 缓冲区已满*/
        TQQAPI_ERR_SOCKSEND_UNCOMPLETED    =21,             /* *<发送包失败,socket缓冲区只发送了部分数据*/
        TQQAPI_ERR_RECV_STOP                              =22,         /* *<客户端收到服务器断开连接通知*/
        TQQAPI_ERR_COUNT
    }TQQAPI_ERR;


    struct tagTQQApiHandle;

    typedef struct tagTQQApiHandle		TQQAPIHANDLE;

    typedef struct tagTQQApiHandle		*HTQQAPI;

    extern LPTDRMETA g_pstqqapiPduHead;

#define TQQAPI_PDUHEAD_HTON(net, host, ver)		tdr_hton(g_pstqqapiPduHead, net,host, ver)

#define TQQAPI_PDUHEAD_NTOH(host,net, ver)		tdr_ntoh(g_pstqqapiPduHead, host, net, ver)


    /**
     *@brief: 日志回调函数定义,a_pArg为回调数据,a_iLogLevel设定日志输出级别
     */

    typedef int (*TQQAPI_PFN_LOG)(IN HTQQAPI a_phQQClt,IN int a_iLogLevel,IN void *a_pArg,const char* a_pszFmt, ...);


    /**
     *@brief  获得tconnd 构建版本
     *@retval   返回版本构建信息
     */
    TSF4G_API char* tqqapi_get_version(void);

    /**
     *@brief: 使用tqqapi之前需要先调用tqqapi_create初始化
     *@param:
     *@retval   <0     fail
     *@retval   =0     success
     */
    TSF4G_API int  tqqapi_create(void);


    /**
     *@brief:解包:
     *@param         pszBuff[IN]:		接收缓冲区起始地址
     *@param         iBuff[IN]:     		长度
     *@param         pstHead[OUT]:	包头信息
     *@param         piHeadLen[OUT] 	解包长度
     *@retval         = 0      for success
     *@retval         <0       failed,返回tdr错误码
     *@see tdr_error_string(iRet)
     */
    TSF4G_API int tqqapi_decode(IN const char* pszBuff,IN int iBuff,OUT TPDUHEAD* pstHead,OUT int* piHeadLen);


    /**
     *@brief:打包
     *@param        pszBuff[IN,OUT]: 	打包缓冲区
     *@param        iBuff[IN,OUT]:     	[IN]缓冲区长度
     *                                               [OUT]打包长度
     *@param        pstHead[IN]:         包头
     *@retval           =0    for success
     *@retval           <0    failed,返回tdr错误码
     *@see tdr_error_string(iRet)
     */
    TSF4G_API int tqqapi_encode(INOUT char* pszBuff,INOUT int* piBuff, IN TPDUHEAD* pstHead);





    /**
     *@brief  连接服务器若成功分配连接句柄
     *@param      a_pszUri[IN]       URL 地址eg:172.25.40.97:9088
     *@param      a_iTimeout [IN]       连接阻塞超时时间
     *@param      a_phQQClt [OUT]    连接句柄
     *@retval       >=0      success,          返回socket
     *@retval       =-1       connect 失败,telnet url 是否正常
     *@retval       =-2        创建和初始化句柄失败
     *@retval       =-3       WSAStartup返回失败,WSAGetLastError
     */
    TSF4G_API int tqqapi_open(IN const char* a_pszUri,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);

    /**
     *@brief  通过Sock5代理连接服务器,若成功分配连接句柄
     *@param      a_pHTSock5Param[IN]       sock5代理参数
     *@param      a_iTimeout [IN]       连接Sock5代理超时时间
     *@param      a_phQQClt [OUT]    连接句柄
     *@retval       >=0      success,          返回socket
     *@retval       =-1        WSAStartup返回失败,WSAGetLastError
     *@retval       =-2        创建和初始化句柄失败
     *@retval       <-100        返回sock5错误码,具体错误码含义参考tsock5proxy.h 中TSOCK5_ERRORCODE定义
     */
    TSF4G_API int tqqapi_open_sock5(IN  LPTSOCK5PARAM a_pHTSock5Param,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);


    /**
     *@brief  通过Sock5代理连接服务器,若成功分配连接句柄
     *@param      a_pHTSock5Param[IN]       sock5代理参数
     *@param      a_iTimeout [IN]       连接Sock5代理超时时间
     *@param      a_phQQClt [OUT]    连接句柄
     *@retval       >=0      success,          返回socket
     *@retval       =-1        WSAStartup返回失败,WSAGetLastError
     *@retval       =-2        创建和初始化句柄失败
     *@retval       <-200        返回http代理错误码,具体错误码含义参考thttpproxy.h 中THTTP_ERRORCODE定义
     */
    TSF4G_API int tqqapi_open_http(IN  LPTHTTPPARAM a_pHttpParam,IN int a_iTimeout,OUT HTQQAPI* a_phQQClt);



    /**
     *@brief 设置API默认日志系统日志级别及日志路径,可以在生成句柄后首先调用该函数注意不同句柄实例最好使用不同a_pszLogPattern,避免日志写乱,
     *@param        a_hQQClt[IN]    		句柄
     *@param        a_iLogLevel[IN]            日志输出级别,具体定义参考TQQAPI_LOG_PRIORITY定义
     *@param        a_pszLogPattern[IN]     日志文件路径,若为空使用默认日志文件路径宏TQQAPI_DEFAULT_LOGPATTEN:即./tqqapi_log/tqqapi_%Y-%m-%d.log.%F表示按日期记录日志更多format参数参考strftime
     *@retval 		  =0       success
     *@retval           <0       fail
     *@retval           = -1    参数非法
     *@retval           =-2     初始化Log失败
     *@retval           =-3     获取Log实例失败
     */
    TSF4G_API int 	tqqapi_set_log_default(IN HTQQAPI a_phQQClt, IN int a_iLogLevel, IN const char *a_pszLogPattern);




    /**
     *@brief 设置句柄应用自定义日志系统回调函数
     *@param       a_hQQClt[IN]    		   句柄
     *@param       a_pfnLogCallBack[IN]       日志回调函数
     *@param       a_iLogLevel[IN]                设定日志输出级别,需要将该字段保存到回调数据中,以便支持不同日志级别输出过滤
     *@param       a_pArg                            日志函数回调数据
     *@retval 	       =0 success
     *@retval        <0  fail
     */
    TSF4G_API int 	tqqapi_set_log_callback(IN HTQQAPI a_phQQClt, IN TQQAPI_PFN_LOG a_pfnLogCallBack,IN int a_iLogLevel,IN void* a_pArg);





    /**
     *@brief 根据包体meta分配发送/接收缓冲区大小
     *@param      a_hQQClt[IN] 	     连接句柄
     *@param      a_pstSendMeta[IN]    上行tdr 结构meta
     *@param      a_pstRecvMeta[IN]    下行tdr 结构meta,若一致同上行meta
     *@retval	      =0       for success
     *@retval       < 0      for fail
     *@retval       = -1    输入参数出错
     *@retval       =-2     calloc failed,检查内存是否不够
     */
    TSF4G_API int tqqapi_set_pdu_meta(IN HTQQAPI a_hQQClt,IN LPTDRMETA a_pstSendMeta,IN  LPTDRMETA a_pstRecvMeta);



    /**
     *@brief   连接tconnd,本接口为测试使用,正式发布环境客户端使用统一登录系统请联系基础开发组获取相关SDK
     *@param   a_phQQClt[IN] 	      连接句柄
     *@param   uin[IN]			      QQ号
     *@param   a_pszSvrkey[IN]	      16字节签名密钥
     *@param   a_iAuthType[IN]        QQ签名类型,需要设为3,定义见tconnsecdef.h中tagTCONN_SEC_AUTH
     *@param   a_iTimeOut[IN]         收包阻塞超时时间
     *@param   a_iEncMethod[IN]       一般设为3,定义见tconnsecdef.h中tagTCONN_SEC_ENC
     *@param   a_iServiceID[IN]       游戏ID

     *@note        a_iAuthType=3      	0xde统一签名类型
     *@note	    a_iEncMethod=3		taes加解密算法


     *@retval             0:for success
     *@retval            <0:for fail
     *@retval             use tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_create_initial_connection(IN HTQQAPI a_phQQClt,IN unsigned int a_uin, IN const char *a_pszSvrkey,IN int a_iAuthType,IN int a_iTimeOut,IN int a_iEncMethod,IN int a_iServiceID);



    /**
     *@brief 重新连接tconnd,本接口为测试接口,正式发布环境客户端使用统一登录系统请联系基础开发组获取相关SDK
     *@param  	   a_phQQClt[IN] 		连接句柄
     *@param          uin[IN]				QQ号
     *@param          nPos[IN]			连接占位索引
     *@param          szConnectKey[IN]	16 字节 密钥
     *@param           szIdentity[IN]		16 字节验证码
     *@param          iRelayType[IN]           重连类型,目前一般为1
     *@param          a_iTimeOut[IN]           收包超时时间
     *@param          iEncMethod[IN]            加解密方法,一般填3,,定义见tconnsecdef.h中tagTCONN_SEC_ENC

     *@note            iRelayType =1             跨服跳转
     *@note            iRelayType =2             断线重连
     *@note            iEncMethod = 3           taes加解密算法


     *@retval             0:for success
     *@retval            <0:for fail
     *@retval             use tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_create_relay_connection(IN HTQQAPI a_phQQClt,IN unsigned int a_uin,IN int nPos,IN const char * szConnectKey,IN const char *szIdentity,IN int a_iTimeOut,IN int iRelayType,IN int iEncMethod);



    /**
     *@brief 发送消息
     *@param    a_hQQClt[IN] 		连接句柄
     *@param    a_pstHead[IN] 		PDU 包头, 对于通信消息,包头可以设为NULL
     *@param    a_pszMsg[IN]		应用消息发送缓冲区
     *@param    a_iMsg[IN]                消息大小
     *@param    a_iTimeout[IN]	       阻塞发送超时时间: 设为-1等待发送成功,零为不等待单位/ms

     *@retval		 =0:for success
     *@retval          <0:for fail
     *@retval          use tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_send_msg(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN const char* a_pszMsg,IN int a_iMsg,IN int a_iTimeout);



    /**
     *@brief 接收消息
     *@param      a_hQQClt[IN] 		 	连接句柄
     *@param      a_pstHead[IN]			接收消息的PDU包头,若不关心包头可以设为NULL
     *@param      a_pszMsg[IN,OUT]		应用消息体接收缓冲区
     *@param      a_iMsg[IN,OUT]		接收缓冲区大小(IN)
     *                                                      消息体大小(OUT)
     *@param      a_iTimeout[IN]			接收超时时间: 若阻塞设为-1,零为不等待单位/ms

     *@retval       =1:receive one package
     *@retval       =0:receive NULL
     *@retval       <0:for fail
     *@retval       use tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_recv_msg(IN HTQQAPI a_hQQClt,OUT TPDUHEAD* a_pstHead,INOUT char* a_pszMsg,INOUT int* a_piMsg,IN int a_iTimeout);



    /**
     *@brief 发送消息网络字节流
     *@param    a_hQQClt[IN] 		连接句柄
     *@param    a_pstHead[IN] 		PDU 包头, 对于通信消息,包头可以设为NULL
     *@param    a_pszBuffer[IN]		应用消息发送缓冲区
     *@param    iBufferLen[IN]           消息网络字节流大小
     *@param    a_iTimeout		      阻塞发送超时时间: 设为-1等待发送成功,零为不等待单位/ms

     *@retval		 =0   for success
     *@retval          <0   for fail
     *@retval          use tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_send_buffer(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN const char* a_pszBuffer,IN int iBufferLen,IN int a_iTimeout);



    /**
     *@brief 接收消息网络字节流
     *@param      a_hQQClt[IN] 		 	连接句柄
     *@param      a_pstHead[IN]			接收消息的PDU包头,若不关心包头可以设为NULL
     *@param      a_pszBuffer[IN,OUT]	应用消息体接收缓冲区
     *@param      a_iMsg[IN,OUT]		接收缓冲区大小(IN)
     *	                                                 消息网络字节流大小(OUT)
     *@param      a_iTimeout[IN]			接收超时时间: 若阻塞设为-1,零为不等待单位/ms

     *@retval       =1      receive one package
     *@retval       =0      receive NULL
     *@retval       <0       for fail
     *@retval       use     tqqapi_get_err/tqqapi_get_errorstring获得错误码或者提示信息
     *@see
     *     		  tqqapi_get_err
     *		  tqqapi_get_errorstring
     */
    TSF4G_API int tqqapi_recv_buffer(IN HTQQAPI a_hQQClt, IN TPDUHEAD* a_pstHead,IN  char* a_pszBuffer,INOUT int * piBufferLen,IN int a_iTimeout);


    /**
     *@brief   获得句柄错误码
     *@param  a_hQQClt[IN]  连接句柄
     *@retval  返回错误码
     *@see    TQQAPI_ERR
     */
    TSF4G_API int tqqapi_get_err(IN HTQQAPI a_hQQClt);



    /**
     *@brief   获得句柄错误信息
     *@param  a_hQQClt[IN]  连接句柄
     *@retval  错误字符串
     */
    TSF4G_API const char * tqqapi_get_errstring(IN HTQQAPI a_hQQClt);


    /**
     *@brief  释放句柄
     *@param  a_phQQClt[IN]  连接句柄指针
     *@retval  =0     success
     *@retval  <0     fail
     */
    TSF4G_API int tqqapi_free(IN HTQQAPI* a_phQQClt);



    /**
     *@brief 设置socket 发送缓冲区
     *@param    a_hQQClt[IN]    		句柄
     *@param    a_hQQClt[IN]    		缓冲区大小
     *@retval
     *@retval 		0        success
     *@retval         <0     fail
     */
    TSF4G_API int 	tqqapi_set_sendbuff(IN HTQQAPI a_phQQClt,IN  int a_iSize);


    /**
     *@brief 设置socket 接收缓冲区
     *@param    a_hQQClt[IN]    		句柄
     *@param    a_hQQClt[IN]    		缓冲区大小
     *@retval
     *@retva 		0       success
     *@retva         <0     fail
     */
    TSF4G_API int 	tqqapi_set_recvbuff(IN HTQQAPI a_phQQClt, IN int a_iSize);



    /**
     *@brief  获取接收的原始数据（解码之前的网络实际传输的数据）的总大小
     *@param    a_hQQClt   句柄
     *@retval  接收的原始数据（解码之前的网络实际传输的数据）的总大小
     */
    TSF4G_API uint64_t tqqapi_getTotalRecvLen(HTQQAPI a_hQQClt);

    /**
     *@brief  获取发送的网络数据（编码之后的网络实际传输的数据）的总大小
     *@param    a_hQQClt   句柄
     *@retval  发送的网络数据（编码之后的网络实际传输的数据）的总大小
     */
    TSF4G_API uint64_t tqqapi_getTotalSendLen(HTQQAPI a_hQQClt);




    /*
     * @name  ---------------内部接口-------------
     * @{
     */



    /**
     *@brief 初始化句柄
     *@param    a_hQQClt   		句柄

     *@retva 		   =0      success
     *@retva            <0      fail
     *@retva            =-1    参数非法
     *@retva            =-2    获取TPDUBase Meta出错
     *@retva            =-3    获取TPDUHead Meta出错
     * @deprecated
     */
    TSF4G_API int 	tqqapi_init(HTQQAPI a_hQQClt);


    /**
     *@brief 创建并 初始化句柄
     *@param    a_hQQClt   		句柄

     *@retval  	     =0   success
     *@retval             <0    fail
     *@retval             =-1  参数非法
     *@retval             =-2  初始化句柄出错
     * @deprecated
     */
    TSF4G_API int 	tqqapi_new(HTQQAPI* a_phQQClt);

    /**
     *@brief 初始化头部基本信息
     *@param    a_pstBase   		头部基本信息结构体
     *@retval
     */
    TSF4G_API void      tqqapi_init_base(TPDUBASE* a_pstBase);


    /**
     *@brief 设置句柄通信Socket
     *@param    a_hQQClt   		句柄
     *@param    a_s   		       通信Socket
     *@retval
     */
    TSF4G_API void 	tqqapi_attach(HTQQAPI a_hQQClt,int a_s);

    /**
     *@brief 返回句柄通信Socket
     *@param    a_hQQClt   		句柄
     *@retval     句柄通信Socket
     *@retval                >=0  正常socket
     *@retval                <0    异常socket
     */
    TSF4G_API int		tqqapi_get_sock(HTQQAPI a_phQQClt);

    /**
     *@brief 返回句柄通信凭据
     *@param    a_hQQClt   		句柄
     *@param    a_iPos   		位置索引指针
     *@param    a_pszIdentity   	位置验证字符串
     *@retval
     */
    TSF4G_API void 	tqqapi_get_identity(HTQQAPI a_hQQClt, int *a_iPos, char * a_pszIdentity);

    /**
     *@brief 设置句柄通信凭据
     *@param    a_hQQClt   		句柄
     *@param    a_iPos   		位置索引信息
     *@param    a_pszIdentity   	位置验证字符串
     *@retval
     */
    TSF4G_API void	tqqapi_set_identity(HTQQAPI a_hQQClt, int a_iPos,const char * a_pszIdentity);

    /**
     *@brief 设置通信双方版本,目前已不用
     *@param    a_hQQClt   		句柄
     *@param    a_iSelfVersion   	本地通信版本
     *@param    a_iPeerVersion   对端版本
     *@retval
     *@deprecated
     */
    TSF4G_API void 	tqqapi_set_version(HTQQAPI a_hQQClt,int a_iSelfVersion, int a_iPeerVersion);

    /**
     *@brief 设置通信双方通信密钥
     *@param    a_hQQClt   		句柄
     *@param    pszGameKey   	16字节通信密钥
     *@param    iEncMethod         通信加密方法
     *@retval     返回值
     *@retval      = 0      success
     *@retval      <0       fail
     *@retval       =-1     加密方法不对
     *@retval      =-2     设置密钥出错
     */
    TSF4G_API int 	tqqapi_set_gamekey(HTQQAPI a_hQQClt, const char* pszGameKey,int iEncMethod);


    /**
     *@brief 获取通信双方通信密钥
     *@param    a_hQQClt   		句柄
     *@param    pszGameKey   	16字节通信密钥
     *@retval     返回值
     *@retval     = 0  success
     *@retval     <0   fail
     *@retval     =-1  没有密钥
     */
    TSF4G_API int  	tqqapi_get_gamekey(HTQQAPI a_hQQClt, char* pszGameKey);

    /**
     *@brief 获取tqqapi MetaLib
     *@param    a_hQQClt   		句柄
     *@retval     返回MetaLib 地址
     */
    TSF4G_API const char*   tqqapi_get_meta_data(void);


    /**
     *@brief 卸载句柄通信Socket
     *@param    a_hQQClt   		句柄
     *@retval    当前的 通信Socket
     *@retval    >=0     正常socket
     *@retval    <0       异常socket
     */
    TSF4G_API int          tqqapi_detach(HTQQAPI a_hQQClt);


    /**
     *@brief获取句柄通信当前上行序列号
     *@param    a_hQQClt   		句柄
     *@retval   当前上行序列号
     */
    TSF4G_API unsigned int    tqqapi_get_sequence(HTQQAPI a_hQQClt);

    /**
     *@brief设置句柄通信上行序列号
     *@param    a_hQQClt   		句柄
     *@param    iSequence          上行序列号
     *@retval
     */
    TSF4G_API void        tqqapi_set_sequence(HTQQAPI a_hQQClt,unsigned int iSequence);

    /**
     *@brief  从TPDUHEAD 中提取通信凭据信息
     *@param    a_hQQClt   		句柄
     *@param    a_pstHead          TPDUHEAD
     *@param    pstIdentInfo         凭据信息

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD 协议类型不对
     *@retval     =-2    获取协议描述Meta失败
     *@retval     =-3    解密信息失败
     *@retval     =-4    解包凭据信息失败
     */
    TSF4G_API int          tqqapi_extract_identinfo(HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead, TPDUIDENTINFO* pstIdentInfo);

    /**
     *@brief  从TPDUHEAD 中提取通信密钥
     *@param    a_hQQClt   		句柄
     *@param    a_pstHead          TPDUHEAD
     *@param    pszSessionkey     16字节密钥,至少预留16 字节空间

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD 协议类型不对
     *@retval     =-2    解密密钥信息失败
     */
    TSF4G_API int           tqqapi_extract_Sessionkey(HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead,char *pszSessionkey);

    /**
     *@brief  从TPDUHEAD 中提取服务器验证信息
     *@param    a_hQQClt   		句柄
     *@param    a_pstHead          TPDUHEAD
     l
     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TPDUHEAD 协议类型不对
     *@retval     =-2    解密密钥信息失败
     *@retval     =-3    获取协议Meta失败
     *@retval     =-4    解包Syn 信息失败
     */
    TSF4G_API int           tqqapi_extract_syninfo(HTQQAPI a_hQQClt,TPDUHEAD* a_pstHead);

    /**
     *@brief  生成0x82 测试签名TQQAUTHINFO 信息
     *@param    pstAuthInfo   	 签名结构体
     *@param    pstGameSig        传入的0x82 签名第一段结构
     *@param    pstSigForS2        传入的0x82 签名第二段结构
     *@param    uin                     QQ号
     *@param    pszSvrSKey        16字节加密密钥信息

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    TDR 打包0x82 签名第一段结构失败
     *@retval     =-2    TDR 打包0x82 签名第二段结构失败
     */
    TSF4G_API int           tqqapi_make_authinfo(TQQAUTHINFO* pstAuthInfo, TQQGAMESIG* pstGameSig, TQQSIGFORS2* pstSigForS2, unsigned int uin, char* pszSvrSKey);

    /**
     *@brief  验证0x82 测试签名TQQAUTHINFO 信息是否正确
     *@param    pstGameSig        解包后0x82 签名第一段结构
     *@param    pstSigForS2        解包后0x82 签名第二段结构
     *@param    uin                     QQ号
     *@param    pstAuthInfo         签名结构体
     *@param    pszSvrSKey        16字节解密密钥信息

     *@retval     =0      success
     *@retval     <0      fail
     *@retval     =-1    解密0x82 签名第一段结构失败
     *@retval     =-2    TDR 打包0x82 签名第一段结构失败
     *@retval     =-3    解密0x82 签名第二段结构失败
     *@retval     =-4   TDR 打包0x82 签名第二段结构失败
     */
    TSF4G_API int          tqqapi_extract_authinfo(TQQGAMESIG* pstGameSig, TQQSIGFORS2* pstSigForS2, unsigned int uin, TQQAUTHINFO* pstAuthInfo,char* pszSvrSKey);


    /**
     *@brief  生成0xDE 测试签名TQQUNIFIEDAUTHINFO 信息
     *@param    pstUnifyAuthInfo   签名结构体
     *@param    uin                     QQ号
     *@param    pszSvrSKey        16字节加密密钥信息
     *@param    pszSessionkey    16字节通信初始密钥信息
     *@param    uCltIP                 客户端IP

     *@retval
     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   TDR 打包0xDE 签名结构失败
     *@retval     =-2   TDR 打包0xDE 签名协议失败
     */
    TSF4G_API int         tqqapi_make_QQUnify_Authinfo(TQQUNIFIEDAUTHINFO *pstUnifyAuthInfo,unsigned int uin ,char* pszSvrSKey,char *pszSessionkey,unsigned int uCltIP);


    /**
     *@brief  验证0xDE测试签名TQQUNIFIEDAUTHINFO 信息是否正确
     *@param    pstUnifyAuthInfo  签名信息
     *@param    pszSvrSKey        16字节解密密钥信息
     *@retval
     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   TDR 解包签名信息失败
     *@retval     =-2   解密0xDE 签名信息失败
     *@retval     =-3   TDR 解包签名结构失败
     */
    TSF4G_API int         tqqapi_extract_QQUnify_Authinfo(TQQUNIFIEDAUTHINFO *pstUnifyAuthInfo,char* pszSvrSKey);

    /**
     *@brief  生成测试验证包
     *@param    a_hQQClt         句柄
     *@param    a_pstHead        TPDUHEAD
     *@param    a_uin                QQ号
     *@param    pszSvrkey         16字节加密密钥
     *@param    iAuthType          签名类型
     *@param    uCltIP               客户端IP
     *@param    iEncMethod        加密方法
     *@param    iServiceID        业务APPID

     *@retval     =0  success
     *@retval     <0  fail
     *@retval     =-1    参数非法
     *@retval     =-2    设置通信密钥出错
     *@retval     =-3    生成测试签名出错
     *@retval     =-4    验证测试签名出错
     *@retval     =-5    签名类型非法
     */
    TSF4G_API int		tqqapi_build_auth_msg( HTQQAPI a_hQQClt, TPDUHEAD * a_pstHead, unsigned int a_uin, const char *pszSvrkey,int iAuthType,unsigned int uCltIP,int iEncMethod,int iServiceID);

    /**
     *@brief  生成三次握手请求包
     *@param  a_hQQClt    句柄
     *@param    a_pstHead        TPDUHEAD

     *@retval      =0      success
     *@retval      <0      fail
     *@retval      =-1    获取协议Meta错
     *@retval      =-2    打包握手请求包失败
     *@retval      =-3    加密出错

*/
    TSF4G_API int 	tqqapi_build_synack_msg(HTQQAPI a_hQQClt,TPDUHEAD* a_pstHead);


    /**
     *@brief  生成重连请求包
     *@param    a_hQQClt         句柄
     *@param    a_pstHead        TPDUHEAD
     *@param    a_uin                QQ号
     *@param    nPos                 位置索引信息
     *@param    szConnectKey    16字节通信密钥
     *@param    szIdentity          16字节连接验证信息
     *@param    iRelayType         重连类型
     *@param    iEncMethod        加密方法

     *@retval     =0     success
     *@retval     <0     fail
     *@retval     =-1   参数非法
     *@retval     =-2   获取协议Meta出错
     *@retval     =-3   设置通信密钥出错
     *@retval     =-4   TDR打包连接凭证信息出错
     *@retval     =-5   加密出错

*/
    TSF4G_API int       tqqapi_build_relay_msg( HTQQAPI a_hQQClt, TPDUHEAD* a_pstHead,unsigned int uin,int nPos,const char * szConnectKey,const char *szIdentity,int iRelayType,int iEncMethod);

    /**
     *@brief  解析服务器URL,若成功分配socket
     *@param a_pszUri[IN]:URL 地址eg:172.25.40.97:9088
     *@retval       >=0    success,返回socket
     *@retval       <0      解析url出错
     */
    TSF4G_API int 	tqqapi_open2(const char* a_pszUri);

    /**
     *@brief  返回当前句柄缓存应用数据大小
     *@param    a_hQQClt         句柄
     *@retval  缓存应用数据大小
     */
    TSF4G_API int		tqqapi_getBufferdLen(HTQQAPI a_hQQClt);

    /**
     *@brief  返回上一次句柄从socket 收取数据大小
     *@param    a_hQQClt         句柄
     *@retval    上一次收取数据大小
     */
    TSF4G_API int		tqqapi_getRecvdLen(HTQQAPI a_hQQClt);

    /**
     *@brief  返回上一次句柄发送到socket 数据大小
     *@param    a_hQQClt         句柄
     *@retval     发送到socket 数据大小
     */
    TSF4G_API int		tqqapi_sendLen(HTQQAPI a_hQQClt);

    /**加密接口,
     *@param[in] a_iNeedCopy 如果是明文消息，此字段用于控制 是否将消息拷贝到输出缓冲区，还是修改指针，其值为0，不拷贝，其值为非0，则需要拷贝
     */
    TSF4G_API  int       tqqapi_encrypt_i(HTQQAPI a_hQQClt, const char *a_pszInBuf, int a_iInLen, char **a_pszOutBuf,int *a_piOutLen, int a_iNeedCopy);

    /**解密接口,
     *@param[in] a_iIsPlain 如果消息头部命令字为TPDU_CMD_PLAIN则消息不加密
     *@param[in] a_iNeedCopy 如果是明文消息，此字段用于控制 是否将消息拷贝到输出缓冲区，还是修改指针，其值为0，不拷贝，其值为非0，则需要拷贝
     *@retval    成功返回零
     */
    TSF4G_API  int       tqqapi_decrypt_i(HTQQAPI a_hQQClt, const char *a_pszInBuf, int a_iInLen, char **a_pszOutBuf,int *a_piOutLen, int a_iIsPlain, int a_iNeedCopy);



    /** @}*/ // ---------------内部接口-------------





    /** @} */



#ifdef __cplusplus
}
#endif

#endif


