/*
 **  @file $RCSfile: tconnapi.h,v $
 **  general description of this module
 **  $Id: tconnapi.h,v 1.7 2009-09-23 07:41:26 hardway Exp $
 **  @author $Author: hardway $
 **  @date $Date: 2009-09-23 07:41:26 $
 **  @version $Revision: 1.7 $
 **  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
 **  @note Platform: Linux
 */

#ifndef TCONNAPI_H
#define TCONNAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "tdr/tdr_types.h"
#include "tframehead.h"

#ifndef IN
#define  IN
#endif

#ifndef OUT
#define  OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/** @defgroup tconnapi
 * @{
 */

enum tagTconnAPIOptionName
{
    TCONNAPI_OPT_NAME_TBUS_EXCLUSIVE_CHANNELS = 1,   /*只接收connect通道的包*/
    TCONNAPI_OPT_COUNT
};

typedef  intptr_t	TCONNDHANDLE;
typedef  intptr_t	*LPTCONNDHANDLE;

extern LPTDRMETA g_pstConnapiFrameHead;

#define TCONNAPI_FRAMEHEAD_HTON(net, host, ver)		tdr_hton(g_pstConnapiFrameHead, net,host, ver)
#define TCONNAPI_FRAMEHEAD_NTOH(host,net, ver)		tdr_ntoh(g_pstConnapiFrameHead, host, net, ver)
#define TCONNAPI_MAX_BUFF		0x10000

#define DEL_GUID_MASK        0x9fffffff  /* 多对一部署，消除idx中的GUID掩码 */


    /**  多对一部署中，消除ConnIdx中掩码的宏定义
     *
     *  @note 在前端接入存在多个tconnd的情况下，对于跨服跳转的预占连接的应答包，
     *        发给游戏客户端重连的ConnIdx需要用GET_UNMASK_IDX()来提取其消除掩码的后的值。
     *        例如： iIdx = GET_UNMASK_IDX(iConnIdx)
     */
#define GET_UNMASK_IDX(ConnIdx) ((ConnIdx) & DEL_GUID_MASK)




    /**
     *@brief 使用tconnapi之前先必须调用该函数，如果使用动态tbus模式，请调用tconnd_init_with_automatic_tbus
     *@brief 初始化API信息
     *@param pszBuff[IN]  tbus share memory key,if set to 0,use 1688 by default
     *@retval   =0:success
     *@retval   <0:fail
     *@retval  -1 初始化bus 失败,
     *@retval   -2 获取TFRAMEHEAD Meta失败
     */
    int tconnapi_init(IN int iKey);

    /**
     *@brief 使用tconnapi之前先必须调用该函数，动态tbus模式专用，如果是普通模式，请使用tconnapi_init
     *@brief 初始化tbus相关信息
     *@param a_pszTbusdUrl[IN]  tbusd的地址
     *只支持udp, 格式为"udp://127.0.0.1:xxxx"
     *-NULL或空串("") 使用缺省值 TBUS_DEFAULT_TBUSD_URL连接tbusd
     *@param[in]    a_iFlag 初始化操作标志位:
     *-TBUS_INIT_FLAG_USE_SAVED_TIME
     *@param[in] a_iTimeout 和tbusd进行通信时的超时时间,单位毫秒     
     *@retval   =0:success
     *@retval   <0:fail
     */
    int tconnapi_init_with_automatic_tbus(const char* a_pszTbusdUrl, int a_iFlag, int a_iTimeout);

    /**
     *@brief TFRAMEHEAD 解包
     *@param pszBuff[IN]  		解包目的缓冲区
     *@param iBuff[IN]  		目的缓冲区长度
     *@param pstHead[INOUT]	TFRAMEHEAD结构地址
     *@param piHeadLen[OUT]	解包后网络长度
     *@retval   =0:success
     *@retval   <0:解包失败
     *@retval   =-1 输入参数非法
     *@retval   =-2 tdr解包失败
     *@see
     */
    int tconnapi_decode(IN const char* pszBuff,IN  int iBuff,OUT TFRAMEHEAD* pstHead,OUT int* piHeadLen);
    int tconnapi_wap_decode(IN const char *pszBuff, IN int iBuff, OUT TWAPUPLOAD2 *pPkgBuff, INOUT int *piPkgLen);

    /**
     *@brief FrameHead 打包
     *@param pszBuff[IN]  		打包目的缓冲区
     *@param piBuff[INOUT]  	IN:目的缓冲区长度
     *                                         OUT:打包网络长度
     *@param pstHead[IN]		FrameHead地址
     *@retval   =0:success
     *@retval   <0:打包失败
     *@retval  =-1 输入参数非法
     *@retval   =-2 tdr 打包失败
     *@see
     */
    int tconnapi_encode(IN char* pszBuff,INOUT  int* piBuff, IN TFRAMEHEAD* pstHead);
    int tconnapi_wap_encode(OUT char *pszBuff, INOUT int *piBuff, IN TWAPDOWNLOAD2 *pPkgBuff, IN int iPkgLen);

    /**
     *@brief  释放API信息,对象生命周期结束必须调用该API释放
     */
    void tconnapi_fini(void);



    /**
     *@brief 创建API句柄收发消息
     *@param iProcID[IN]  bus ID
     *@param ppstHandle[OUT]  句柄指针
     *@retval     =0:success
     *@retval    <0:fail
     *@retval      = -1  分配句柄内存失败
     *@retval      = -2  创建BUS 句柄失败
     *@retval      = -3  绑定BUS 通道失败
     */
    int tconnapi_create(IN int iProcID, OUT TCONNDHANDLE *pstHandle);



    /**
     *@brief 根据对方BUS地址独占特定的通道(在调用tconnapi_connect之前，请先调用tconnapi_set_handle_opt)
     *@param pstHandle[IN]  句柄指针
     *@param iDst[IN]       对端地址
     *@retval    =0 success
     *@retval    <0 fail
     *@retval    =-1 底层tbus_connect调用失败
     *@retval    =-2 调用该接口的次数超过允许部署的tconnd的上限，目前只支持前端最多部署4个tconnd
     *@retval    =-3 输入参数非法

     *@note      对于需要前端接入多个tconnd的情况，必须多次调用本接口，并且分别传入tconnd的bus地址
     */
    int tconnapi_connect(IN TCONNDHANDLE iHandle,IN int iDst);

    /**
     *@brief  
     *@brief  从bus中收包
     *@param pstHandle[IN]  	句柄指针
     *@param piSrc[INOUT]  	bus源地址
     *@param pszBuff[OUT]  	应用消息体缓冲区起始地址
     *@param piBuff[INOUT]       IN  :缓冲区长度
     *						OUT:消息体长度
     *@param pstHead[OUT]  	解包的FrameHead
     *@retval     =0  收到包
     *@retval    <0 没有收到包,See errno
     *@retval    -1 Tbus通道没有包
     *@retval    -2 输入参数非法
     *@retval    -3 TFrameHead 解包错
     *@retval    -4 应用缓冲区不够
     
     *
     *@note：在前端接入存在多个tconnd的情况下，对于跨服跳转的预占连接的应答包，
     *       发给游戏客户端重连的ConnIdx需要用GET_UNMASK_IDX()来提取其消除掩码的后的值
     *       例如： iIdx = GET_UNMASK_IDX(iConnIdx)
     */
    int tconnapi_recv(IN TCONNDHANDLE iHandle, INOUT int *piSrc, OUT char* pszBuff,INOUT int* piBuff,OUT TFRAMEHEAD* pstHead);


    /**
     *@brief 往bus中发包
     *@brief 在使用多对一部署时，iDst参数会无效，tconnd会根据framehead中的iConnIdx算出对端地址
     *@brief 如果希望使用iDst参数，将链接无关的消息发给特定tconnd，请将iConnIdx置为-1
     *@param iHandle[IN]  		句柄指针
     *@param iDst[IN]  			bus目的地址
     *@param pszBuff[IN]  		应用消息体缓冲区起始地址
     *@param iBuff[IN  ]  	       缓冲区长度
     *@param pstHead[IN]  		打包的FrameHead,若为空则发送句柄上次收到的包
     *@retval    =0 send package success
     *@retval    <0 发送失败
     *@retval     = -1 TFRAMEHEAD打包出错
     *@retval     = -2 Tbus 发送失败
     *@retval     = -3 广播消息中有(小于0的)非法的 iConnIdx 值
     *@retval     = -4 输入参数非法
	 *@retval     = -10 TFRAMEHEAD结构体有非法数据
     */
    int tconnapi_send(IN TCONNDHANDLE iHandle, IN  int iDst, IN char* pszBuff,IN  int iBuff,IN TFRAMEHEAD* pstHead);



    /**
     *@brief 释放API句柄
     *@param ppstHandle[OUT]  句柄指针
     *@retval
     */
    void tconnapi_free(IN TCONNDHANDLE * piHandle);



    /**
     *@brief 设置bus句柄属性，需要注意的是该api应该在tconnapi_create之后、tconnapi_connect之前调用
     *@brief 示例  int flag = 1; tconnapi_set_handle_opt(iHandle, TCONNAPI_OPT_NAME_TBUS_EXCLUSIVE_CHANNELS ,&flag,sizeof(flag))       
     *@param iHandle  				句柄指针
     *@param iOptionName   		属性名
     *@param pvOptionValue               属性值,参考tagTconnAPIOptionName定义
     *@param dwOptionLen                  属性长度
     *@retval      =0  success
     *@retval      <0 fail
     *@retval     =-1 输入参数 iHandle 非法
     *@retval     =-2 输入参数 iOptionName 非法
     */
    int tconnapi_set_handle_opt(IN TCONNDHANDLE iHandle,IN int iOptionName,IN const void *pvOptionValue,IN unsigned int dwOptionLen);




    /**
     *@brief  获得tconnd 关闭连接提示信息
     *@param iReason  关闭原因
     *@retval   返回连接关闭提示信息
     */
    const char* tconnapi_get_closestring(IN int iReason);


    /**
     *@brief  获得tconnd 构建版本
     *@retval   返回版本构建信息
     */
    const char* tconnapi_get_version(void);


    /**
     *@brief  内部接口
     */
    int tconnapi_initialize(const char *a_pszGCIMKey, int a_iBusinessid);


    /**
     *@brief  获取tbus句柄
     *@retval >=0 tbus句柄
     *@retval =-1 输入参数非法
     */
    int tconnapi_get_tbusid(IN TCONNDHANDLE iHandle);


    /**
     *@brief 将tframehead结构体转换成可视化buffer
     *@param pszBuff[IN]  		目的缓冲区
     *@param piBuff[INOUT]  	IN:目的缓冲区长度
     *                                         OUT:打包网络长度
     *@param pstHead[IN]		FrameHead地址
     *@retval   =0:success
     *@retval  =-1 输入参数非法
     *@retval   =-2 buffer长度不够
     *@retval   =-3 其他错误
     *@see
     */
    int tconnapi_visualize_framehead(TFRAMEHEAD* pstHead, char* pszBuff, size_t* piBuff);
	
    /**
     *@brief 设置发送带时间戳选项
     *@param iHandle[IN]  句柄指针
     *@param iFlag[IN]  是否带时间戳的标志位, 0表示不带时间戳，非0表示带时间戳
     */
    void tconnapi_set_send_with_timestamp_flag(IN TCONNDHANDLE iHandle, IN int iFlag);	

    /**
     *@brief 获取上一个消息从进入 tbus 通道到当前时间的时间间隔(单位：ms)
     *@param iHandle[IN]  句柄指针
     *@param pllMsTime[OUT]     上一个消息从进入 tbus 通道到当前时间的时间间隔(单位：ms)
     *@param pstCurrentTime[IN]  	当前系统的时间，如果不为NULL，api 内部会使用此参数作为当前时间；
     *                              如果为NULL，api 内部会调用 gettimeofday 获取当前时间
     *
     *@retval   =0:success
     *@retval   =-1 输入参数非法
     *@retval   =-2 当前时间比上一条消息进入 tbus 通道的时间更早
     *@retval   =-3 收到的上一条消息并未开启时间戳
     *@retval   =-4 其他错误
     */
    int tconnapi_get_lastmsg_timestamp(IN TCONNDHANDLE iHandle, OUT long long* pllMsTime, IN struct timeval* pstCurrentTime);
    /** @} */

#ifdef __cplusplus
}
#endif


#endif /* TCONNAPI_H */
