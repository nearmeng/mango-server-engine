/**
 *
 * @file    tlog_category.h
 * @brief   ��־���¼��־ģ��
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

#ifndef TLOG_CATEGORY_H
#define TLOG_CATEGORY_H

#include <stdarg.h>
#include <tlog/tlog_event.h>
#include "pal/ttypes.h"

#define TLOG_NAME_LEN 64
#ifdef __cplusplus
extern "C"
{
#endif

struct tagTLogCategoryInst
{
    // ��a��ͷ�ı����Ѿ�����ʹ�ã����ƶ����ڲ��ˣ�����Ϊ�˱��ּ��ݣ���Ҫɾ����Щ����������
    // Ҳ��Ҫ�ڴ�����ʹ����Щ����
    char a_szName[TLOG_NAME_LEN];
    struct tagTLogCategoryInst* a_pstParent;
    struct tagTLogCategoryInst* a_pstForward;
    struct tagTLogCategoryInst* a_pstCloneParent;
    struct tagTLOGCategory * a_pstCat;
    int a_iInited;
    int a_iSeq;
    // ������Ϊֹ��Ϊ�˺�1.11���ݣ�ǰ��Ķ����������޸ģ�����Ķ�������ȥ��
    int* piPriorityHigh;
    int* piPriorityLow;
};

typedef struct tagTLogCategoryInst TLOGCATEGORYINST;
typedef struct tagTLogCategoryInst *LPTLOGCATEGORYINST;

/**
 * @bref ����־��д��־���д��ݹ�ϵ
 * @param[in] a_pstCatInst ��־�ļ����
 * @param[in] a_pstEvt     ��־�¼�����
 *
 * @return 0 �ɹ�
 *		��0 ʧ��
 */
int tlog_category_log(LPTLOGCATEGORYINST a_pstCatInst, TLOGEVENT* a_pstEvt);

/**
 * ʹ��valist��ʽд����־���д��ݹ�ϵ
 * @param[in] a_pstCatInst ��־�ļ����
 * @param[in] a_pstEvt     ��־�¼�����
 * @param[in] a_pszFmt     fmt�ַ���
 * @param[in] ap     	  valist
 *
 * @return 0 �ɹ�
 *		��0 ʧ��
 */
int tlog_category_logv_va_list(TLOGCATEGORYINST *a_pstCatInst, TLOGEVENT *a_pstEvt, const char* a_pszFmt, va_list a_ap);

#if defined(_WIN32) || defined(_WIN64)
int tlog_category_logv(TLOGCATEGORYINST *a_pstCatInst, TLOGEVENT* a_pstEvt, const char* a_pszFmt, ...);
#else
int tlog_category_logv(TLOGCATEGORYINST *a_pstCatInst, TLOGEVENT* a_pstEvt, const char* a_pszFmt, ...) __attribute__((format(printf,3,4)));
#endif

/*
 * ������־category�����ȼ�
 * @param [in] a_pstCatInst ��־��
 * @param [in] iPriorityHigh �����ȼ�
 * @param [in] iPriorityLow �����ȼ�
 * @return 0 �ɹ�
 *      ��0 ʧ��
 * */
int tlog_category_set_priority(TLOGCATEGORYINST *a_pstCatInst, int iPriorityHigh, int iPriorityLow);

/*
 * ��tdr�ķ�ʽ���ڴ����ݽṹд����־
 * @param [in] a_pstCatInst ��־��
 * @param [in] a_pstEvt ��־�¼�
 * @param [in] a_pmeta ��Ӧ��������tdr
 * @param [in] a_buff ������
 * @param [in] a_bufflen ��������С
 * @param [in] a_version ���ݲü��汾
 * @return 0 �ɹ�
 *         ��0 ʧ��
 * */
int tlog_category_log_tdr(TLOGCATEGORYINST *a_pstCatInst, TLOGEVENT *a_pstEvt, void * a_pmeta, void *a_buff, intptr_t a_bufflen,
        int a_version);
/*
 * ������־���format
 * @param [in] a_pstCatInst ��־��
 * @param [in] pszPattern ��־format
 * @return 0 �ɹ�
 *         ��0 ʧ��
 * */
int tlog_category_set_format(TLOGCATEGORYINST *a_pstCatInst, const char *pszPattern);

/**
 * ������־���ʱ��λ�ã���������ˣ�ÿ�μ�¼��־�������ʱ�䡣
 * @param[in] a_pstCatInst ��־����
 * @param[in] pstCurr     ָ��ǰʱ���ָ��
 *
 * @return 0 ���óɹ�
 *       ��0 ����
 */
int tlog_category_set_time_ptr(TLOGCATEGORYINST *a_pstCatInst, struct timeval *pstCurr);

/**
 * д��ʮ��������־
 * @param[in] a_pstCatInst ��־����
 * @return 0 �ɹ�
 *       ��0 ʧ��
 */
int tlog_category_log_hex(TLOGCATEGORYINST *a_pstCatInst, TLOGEVENT *a_pstEvt);

/**
 * �鿴category�����ȼ��Ƿ���ϣ���������־��������Ĵ�����־�����д����־������1�����򷵻�0
 * @param[in] a_pstCatInst ��־����
 * @param[in] priority     ָ�����ȼ�
 *
 * @return 0 ����Ҫд����־
 *      ��0 ��Ҫд����־
 */
int tlog_category_can_write(TLOGCATEGORYINST *a_pstCatInst, int a_priority, unsigned int a_uId, unsigned int a_uCls);


/**
* ��ѯtlog�Ƿ���־���͵�SOCKET��������
* @param[in] a_pstCatInst ��־����
*
* @return 0 : ����־���͵�SOCKET�������ɹ�
*    ��0    : ʧ��(������������ͷ��͵����绺����ʧ��;
                   �����vec���ͣ�ֻҪ��һ���������ͷ���ʧ�ܾ���Ϊ��ʧ��)
*/
int tlog_query_net_category_status(TLOGCATEGORYINST *a_pstCatInst);


#ifdef __cplusplus
}
#endif

#endif /* TLOG_CATEGORY_H */

