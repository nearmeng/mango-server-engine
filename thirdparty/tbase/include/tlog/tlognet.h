/**
*
* @file    tlognet.h  
* @brief   ��־����ģ��
* 
* @author steve
* @version 1.0
* @date 2007-04-05 
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
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

#define MAX_TLOG_NET_NUM 256 // ��־������NET_DEV��������

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
	int iIsSendNetSucc; /*�����ж���־�Ƿ��͵�SOCKET�������еı�־λ������:���ͳɹ������ʾʧ��*/
};

typedef struct tagTLogDevNetInst	TLOGDEVNETINST;
typedef struct tagTLogDevNetInst	*LPTLOGDEVNETINST;

struct tagTLogNet
{
    TLOGDEVNET* pstDev;
    TLOGDEVNETINST stInst; // category

    int is_udp_domain;  // ʹ��udpЭ����������ʱ���Ŷ�ʱ����
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
    char domain[TLOGNET_URL_LEN];   // TLOGNET��Ӧ������
    struct sockaddr_in ipv4;    // �洢�������ip��ַ
    struct sockaddr_in6 ipv6;   // �洢�������ip��ַ
    struct timeval last_result_time;    // ��ȡ����������ʱ��� 
    LPTLOGNET a_pstLogNet;  // ��dns_ctx��Ӧ��TLOGNET��һ��һ 
    int addrtype;   // �������ip����, ipv4 or ipv6
    int is_deleted; // �Ƿ��Ѿ�ͣ��
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
    int is_inited; // �Ƿ��ʼ����
    DNSCONTEXTLIST dns_ctx_list;
    struct evutil_addrinfo hints;   // ��ȡ��������������
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
    LPTLOGNET a_pstLogNet;  // һ��TLOGNET��Ӧһ������
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
    int is_inited; // �Ƿ��ʼ����
    DNSCONTEXTLIST dns_ctx_list;
    ASYNCDNSREQUESTLIST req_list;
    struct addrinfo hints;  // ��ȡ��������������
};
#endif

typedef struct tagASyncDNSHelper ASYNCDNSHELPER;
typedef struct tagASyncDNSHelper *LPASYNCDNSHELPER;

/**
* ��ʼ��������
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pstDev ��־����ģ������
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_init(TLOGNET* a_pstLogNet, TLOGDEVNET* a_pstDev);


int tlognet_reload_dns_ctx(TLOGNET* a_pstLogNet);

/**
* ��������д����־����
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pszBuff ��Ҫд����ַ�������
* @param[in] a_iBuff ��Ҫд����ַ�������
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_write(TLOGNET* a_pstLogNet, const char* a_pszBuff, int a_iBuff);

/**
* �ͷ�������
* @param[in,out] a_pstLogNet ��־����ģ����
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_fini(TLOGNET* a_pstLogNet);

/**
* ��������д������
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pstIOVec д����ַ�����������
* @param[in] a_iCount д����ַ������������С
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_writev(TLOGNET* a_pstLogNet, const TLOGIOVEC* a_pstIOVec, int a_iCount);

int tlognet_parse_url(const char* url, URLPARTS* url_parts);

int tlognet_get_ip_port(const char* a_pszAddr, char* host, int host_size, uint32_t *port);

#ifdef __cplusplus
}
#endif

#endif /* TLOGNET_H */


