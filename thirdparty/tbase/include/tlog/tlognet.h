/**
*
* @file    tlognet.h  
* @brief   日志网络模块
* 
* @author steve
* @version 1.0
* @date 2007-04-05 
*
*
* Copyright (c)  2007, 腾讯科技有限公司互动娱乐研发部
* All rights reserved.
*
*/

#ifndef TLOGNET_H
#define TLOGNET_H

#include "tlogio.h"
#include "tlognetdef.h"

#if defined(_WIN32) || defined(_WIN64)
// win use libevent
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#else
// linux use getaddrinfo_a
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <netdb.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_TLOG_NET_NUM 256 // 日志中配置NET_DEV的最大个数

#if !defined(_WIN32) && !defined(_WIN64)
#define IF_TRUE_THEN_STDERR_EXIT(condition, format, args...)  \
{                                                                   \
    if ((condition))                                           \
    {                                                                \
        printf((format), ##args);                                    \
        return -1;                                                   \
    }                                                                \
}
#else
#define IF_TRUE_THEN_STDERR_EXIT(condition, format, ...)              \
{                                                                     \
    if ((condition))                                                  \
    {                                                                 \
        printf((format), __VA_ARGS__);                                \
        return -1;                                                    \
    }                                                                 \
}
#endif

struct tagTLogDevNetInst
{
	int s;
	int iType; 
	int iCurRetry;
	char *pszBuff;
	int iIsSendNetSucc; /*用于判断日志是否发送到SOCKET缓冲区中的标志位，非零:发送成功，零表示失败*/
};

typedef struct tagTLogDevNetInst	TLOGDEVNETINST;
typedef struct tagTLogDevNetInst	*LPTLOGDEVNETINST;

struct tagTLogNet
{
    TLOGDEVNET* pstDev;
    TLOGDEVNETINST stInst; // category

    int is_udp_domain;  // 使用udp协议且是域名时，才定时更新
};

typedef struct tagTLogNet		TLOGNET;
typedef struct tagTLogNet		*LPTLOGNET;

struct UserData
{
    LPTLOGNET a_pstLogNet;
};

typedef struct UserData   USERDATA;
typedef struct UserData   *LPUSERDATA;

struct tagUrlParts
{
    int proto_id;
    char addr[TLOGNET_URL_LEN];
    unsigned int port;
    int iArgs;
    TNETARG	args[TNET_MAX_ARGS];
};
typedef struct tagUrlParts URLPARTS;
typedef struct tagUrlParts *LPURLPARTS;

struct tagDNSContext
{
    char domain[TLOGNET_URL_LEN];   // TLOGNET对应的域名
    struct sockaddr_in ipv4;    // 存储解析后的ip地址
    struct sockaddr_in6 ipv6;   // 存储解析后的ip地址
    struct timeval last_result_time;    // 获取域名解析的时间点 
    LPTLOGNET a_pstLogNet;  // 该dns_ctx对应的TLOGNET，一对一 
    int addrtype;   // 解析后的ip类型, ipv4 or ipv6
    int is_deleted; // 是否已经停用
};
typedef struct tagDNSContext DNSCONTEXT;
typedef struct tagDNSContext *LPDNSCONTEXT;

struct tagDNSContextList
{
    DNSCONTEXT dns_ctxs[MAX_TLOG_NET_NUM];
    int size;
};
typedef struct tagDNSContextList DNSCONTEXTLIST;
typedef struct tagDNSContextList *LPDNSCONTEXTLIST;

// win
#if defined(_WIN32) || defined(_WIN64)
struct tagASyncDNSHelper
{
    int is_inited; // 是否初始化过
    DNSCONTEXTLIST dns_ctx_list;
    struct evutil_addrinfo hints;   // 获取域名的期望配置
    struct event_base *base;
    struct evdns_base *dnsbase;
#ifdef TSF4G_TLOG_THREAD_SAFE
    pthread_mutex_t callback_mutex;      // this mutex is used for event_loop callback function
#endif
};
#else
// for linux getaddrinfo_a
struct tagASyncDNSRequest
{
    struct gaicb gaicb_struct;
    struct gaicb *gaicb_ptrs[1];
    LPTLOGNET a_pstLogNet;  // 一个TLOGNET对应一个请求
};
typedef struct tagASyncDNSRequest ASYNCDNSREQUEST;
typedef struct tagASyncDNSRequest *LPASYNCDNSREQUEST;

struct tagASyncDNSRequestList
{
    ASYNCDNSREQUEST reqs[MAX_TLOG_NET_NUM];
    int size;
};
typedef struct tagASyncDNSRequestList ASYNCDNSREQUESTLIST;
typedef struct tagASyncDNSRequestList *LPASYNCDNSREQUESTLIST;

struct tagASyncDNSHelper
{
    int is_inited; // 是否初始化过
    DNSCONTEXTLIST dns_ctx_list;
    ASYNCDNSREQUESTLIST req_list;
    struct addrinfo hints;  // 获取域名的期望配置
};
#endif

typedef struct tagASyncDNSHelper ASYNCDNSHELPER;
typedef struct tagASyncDNSHelper *LPASYNCDNSHELPER;

/**
* 初始化网络句柄
* @param[in,out] a_pstLogNet 日志网络模块句柄
* @param[in] a_pstDev 日志网络模块配置
*
* @return  0 成功
*			非0 失败
*/
int tlognet_init(TLOGNET* a_pstLogNet, TLOGDEVNET* a_pstDev);


int tlognet_reload_dns_ctx(TLOGNET* a_pstLogNet);

/**
* 向网络句柄写入日志内容
* @param[in,out] a_pstLogNet 日志网络模块句柄
* @param[in] a_pszBuff 需要写入的字符串内容
* @param[in] a_iBuff 需要写入的字符串长度
*
* @return  0 成功
*			非0 失败
*/
int tlognet_write(TLOGNET* a_pstLogNet, const char* a_pszBuff, int a_iBuff);

/**
* 释放网络句柄
* @param[in,out] a_pstLogNet 日志网络模块句柄
*
* @return  0 成功
*			非0 失败
*/
int tlognet_fini(TLOGNET* a_pstLogNet);

/**
* 向网络句柄写入向量
* @param[in,out] a_pstLogNet 日志网络模块句柄
* @param[in] a_pstIOVec 写入的字符串向量数组
* @param[in] a_iCount 写入的字符串向量数组大小
*
* @return  0 成功
*			非0 失败
*/
int tlognet_writev(TLOGNET* a_pstLogNet, const TLOGIOVEC* a_pstIOVec, int a_iCount);

int tlognet_parse_url(const char* url, URLPARTS* url_parts);

int tlognet_get_ip_port(const char* a_pszAddr, char* host, int host_size, uint32_t *port);

#ifdef __cplusplus
}
#endif

#endif /* TLOGNET_H */


