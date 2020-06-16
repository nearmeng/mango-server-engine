/**
 *
 * @file    tlogload.h
 * @brief   ��־�����ļ�����ģ��
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

#ifndef TLOGLOAD_H
#define TLOGLOAD_H

#include "tlog/tlog.h"
#include "tmng/tmng.h"

#if defined(_WIN32) || defined(_WIN64)

#ifdef TSF4G_SMALL_LIB
#undef _TSF4G_LARGE_LIB_
#endif

#ifdef _TSF4G_LARGE_LIB_
#ifdef _DEBUG
#pragma comment(lib, "libtsf4g_d.lib")
#else
#pragma comment(lib, "libtsf4g.lib")
#endif
#else

#ifdef _DEBUG
#pragma comment(lib, "libtloghelp_d.lib")
#else
#pragma comment(lib, "libtloghelp.lib")
#endif
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define TLOG_DEF_LAYOUT_FORMAT "[%d.%u][%h][(%f:%l) (%F)][%M][%p] %m%n"
#define TLOG_DEF_BILL_LAYOUT_FORMAT "[%d.%u]%m%n"

#define TLOG_DEF_CATEGORY_TEXTROOT		"text"
#define TLOG_DEF_CATEGORY_DATAROOT      "data"
#define TLOG_DEF_CATEGORY_TEXTTRACE     "texttrace"
#define TLOG_DEF_CATEGORY_TEXTERR       "texterr"
#define TLOG_DEF_CATEGORY_BUS           "texttrace.bus"
#define TLOG_DEF_CATEGORY_BILL          "bill"
#define TLOG_DEF_CATEGORY_GDATA         "gdata"

/**�������ļ���ʼ����־���
 *
 * @param[in] a_pstzPath ��־�����ļ�·��
 * @return  NULL ʧ��
 *			��NULL ��ȡ����־������ָ��
 */
LPTLOGCTX tlog_init_from_file(const char *a_pstzPath);

/**
 * ָ�������ļ����ؾ��
 * @note �ļ��е�category��������, ���ܵ���λ��,��ǰ��Ҫ�ṩ�ı�ip/����
 * @param[in] a_pstzPath ��־�����ļ�·��(ԭ�ļ�)
 * @param[in] ���ļ����
 * @return 0 �ɹ�
 *         ��0 ʧ��, ʧ�ܺ�ԭ�ȵľ��a_pstCtx���ܴ��ڲ�����״̬, ��Ҫ�ر�(tlog_fini_ctx)������
 */
int tlog_reload_from_file(const char *a_pstzPath, LPTLOGCTX a_pstCtx);
/*
 * ��ʼ��Ĭ��ֻ��һ���ļ�category����־�ṹ
 * @param[in] a_pszCatName ��־�ռ���
 * @param[in] iPriorityLow ��־�ռ�ĵ����ȼ����ο���־���ȼ��ĵ�
 * @param[in] a_pszFilePattern ��־�ļ�pattern
 * @param[in] iMaxRotate ��־�ļ���������0 �����ƣ�1һ���ļ� >1 ����ļ�
 * @param[in] iSizeLimit ������־�ļ���С
 * @param[in] iRotateStick �Ƿ�д��ǰ�ļ�
 * @param[in] fmt�Զ�����־layout format��ʽ, �������ΪNULL, ��fmt��ʽΪTLOG_DEF_LAYOUT_FORMAT
 * @return NULL ʧ��
 *         ��NULL�ɹ���ȡ��־������ָ��
 * @note   �������tlog_fini_ctx�ͷŽӿ�
 * */
LPTLOGCTX tlog_init_file_ctx_ex(const char *a_pszCatName, int iPriorityLow, const char *a_pszFilePattern, int iMaxRotate, size_t iSizeLimit, int iRotateStick, const char *fmt);


/*
 * ��ʼ��Ĭ��ֻ��һ���ļ�category����־�ṹ
 * @param[in] a_pszCatName ��־�ռ���
 * @param[in] iPriorityLow ��־�ռ�ĵ����ȼ����ο���־���ȼ��ĵ�
 * @param[in] a_pszFilePattern ��־�ļ�pattern
 * @param[in] iMaxRotate ��־�ļ���������0 �����ƣ�1һ���ļ� >1 ����ļ�
 * @param[in] iSizeLimit ������־�ļ���С
 * @param[in] iRotateStick �Ƿ�д��ǰ�ļ�
 * @return NULL ʧ��
 *         ��NULL�ɹ���ȡ��־������ָ��
 * @note   �������tlog_fini_ctx�ͷŽӿ�
 * */
LPTLOGCTX tlog_init_file_ctx(const char *a_pszCatName , int iPriorityLow ,const char *a_pszFilePattern, int iMaxRotate, size_t iSizeLimit, int iRotateStick);

/*
 * ������־��ȫ�����ȼ�
 * @param[in] pstCtx ��־������
 * @param[in] iPriorityHigh ��־�����ȼ�,���Ϊ0��ʾ������
 * @param[in] iPriorityLow ��־�����ȼ������Ϊ0��ʾ������
 * @return 0 �ɹ�
 *          ��0 ʧ��
 * */
int tlog_set_ctx_priority(LPTLOGCTX pstCtx, int iPriorityHigh, int iPriorityLow);

/**�ͷ���־�����ļ����
 *
 * @param[in] a_pstzPath ��־�����ļ�·��
 * @return  NULL ʧ��
 *			��NULL ��ȡ����־������ָ��
 */
int tlog_fini_ctx(LPTLOGCTX *a_ppstCtx);

#ifdef __cplusplus
}
#endif

#endif /* TLOGLOAD_H */

