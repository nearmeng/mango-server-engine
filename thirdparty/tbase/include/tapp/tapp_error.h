/**
 *
 * @file        tapp_error.h
 * @brief       TAPP����ֵ����ͷ�ļ�
 *
 * @author      flyma
 * @version     1.0
 * @date        2011-12-5
 *
 * Copyright (c)  2010-2020, ��Ѷ�Ƽ����޹�˾���������з���
 * All rights reserved.
 *
 */

#ifndef TAPP_ERROR_H
#define TAPP_ERROR_H


#include "comm/tmodid.h"
#include "pal/pal.h"


/** @defgroup TAPP_ERR TAPP_������
 * @{
 *
 *@note TAPP-RETURN-VALUE:
 *  - ʹ��һ���������洢����ֵ��������ֵ�ĺ�������:
 *      -#  0 ��ʾ�ɹ�, ��ʱ TAPP_RET_IS_ERR(ret) == 0
 *      -# <0 ��ʾ����, ��ʱ TAPP_RET_IS_ERR(ret) == 1
 *  - ����ֵ��ʾ����ʱ���������½ṹ���ͷ���ֵ��
 *      -# ���󼶱�: 1�ֽڣ���Ӧ����ֵ�ĵ�1���ֽ�
 *      -# ģ��ID:   1�ֽڣ���Ӧ����ֵ�ĵ�2���ֽ�, �ο�����"ģ��ID"
 *      -# �����:   2�ֽڣ���ʼֵΪ 1024 ([0,1023] Ϊͨ�ô���Ԥ��)
 *  - ģ��ID (MOD-ID):
 *      һ��MOD-IDռ��һ���ֽ�(8bit)����Ӧ����ֵ�ĵ�2���ֽڣ�
 *      Ϊ�˺��ڿ���ȡ����ģ��� MOD-ID���Ӷ�ȡ����ģ��Ĵ�����Ϣ��
 *      TAPP�� MOD-ID ʹ�� MOD-ID ��Ӧ���ֽڵĵ�һbit���������ģ��ʹ�ã�
 *      ���磺TMODID_TAPP | TAPP_SUB_MOD_DR �м���TAPPģ��IDҲ����ģ��(TDR)��ID��
 *  - ���ر�ʾ����ʱ�����践��ֵ�����ڱ��� ret ��, ����ͨ�����·�ʽȡ�ô�����Ϣ��
 *      - ���ú��� tapp_error_string ȡ�ô���������Ϣ
 *      - ʹ�ú� TAPP_ERR_GET_ERRNO(ret) ȡ�� TAPP_ERRNO_* ��ʽ�Ĵ�����
 *      - ʹ�ú� TAPP_RET_MATCH(ret, TAPP_ERROR_*) �ж��Ƿ������ض��Ĵ���
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


#define TAPP_ERROR         -1
#define TAPP_OK             0
#define TAPP_SUCCESS        0

    /**
     * ���ﶨ�����ģ��IDֵ��ȫ��Ӧ���ļ�comm/tmodid.h�ж����ֵ��
     * ֻ����Ϊ�ļ�tmodid.h��û��(ʵ����Ҳ���޷�)����BUS��ģ��ID��
     * ����������������ȫ����һ����ģ��ID�������ָ���ģ��Ĵ����룬
     */
#define TAPP_SUB_MOD_NON            0x00
#define TAPP_SUB_MOD_DR			    0x01
#define TAPP_SUB_MOD_LOG            0x02
#define TAPP_SUB_MOD_BUS            0x03
#define TAPP_SUB_MOD_MNG			0x04
#define TAPP_SUB_MOD_SEC			0x05


    /**
     * ���� TAPP-API �ķ���ֵ�ж��Ƿ��д�����
     */
#define TAPP_RET_IS_ERR(ret)                ((ret) & 0x80000000 )


    /**
     * �ж� TAPP-API �ķ���ֵ���Ƿ�Я����Ӧ�Ĵ�����Ϣ
     */
#define TAPP_RET_MATCH(ret, error)          (((ret) & 0xFF80FFFF) == ((error) & 0xFF80FFFF))


    /*
     * ���������е� MOD_ID �� TAPP_SUB_MOD_*
     */
#define TAPP_RET_SET_MOD_ID(ret, modid)     ((ret) | (((modid) << 16) & 0x007F0000))
#define TAPP_RET_GET_MOD_ID(ret)            (((ret) & 0x007F0000) >> 16)
#define TAPP_RET_CLR_MOD_ID(ret)            ((ret) & 0xFF80FFFF)


    /*
     * ���º���������Я��������Ϣ�ķ���ֵ
     */
#define TAPP_ERR_LEVEL_WARN                 0x01
#define TAPP_ERR_LEVEL_ERROR                0x02
#define TAPP_ERR_LEVEL_FATAL                0x03

#define TAPP_ERR_START_ERRNO                1024
    /**
     * �ӷ���ֵȡ�� TAPP_ERRNO_* ��ʽ�Ĵ�����
     */
#define TAPP_ERR_GET_ERRNO(error)           (((error) & 0xFFFF) - TAPP_ERR_START_ERRNO)

#define TAPP_ERR_MAKE(level, mod, errno)    (0x80000000 | ((level)<<24) | ((mod)<<16) | (errno))

#define TAPP_ERR_MAKE_WARN(errno)           TAPP_ERR_MAKE(TAPP_ERR_LEVEL_WARN,   TMODID_TAPP, errno)
#define TAPP_ERR_MAKE_ERROR(errno)          TAPP_ERR_MAKE(TAPP_ERR_LEVEL_ERROR,  TMODID_TAPP, errno + TAPP_ERR_START_ERRNO)
#define TAPP_ERR_MAKE_FATAL(errno)          TAPP_ERR_MAKE(TAPP_ERR_LEVEL_FATAL,  TMODID_TAPP, errno)


    /**
     * TAPP ������붨��
     * ����ͨ�� TAPP_ERR_GET_ERRNO �� TAPP-API ����ֵ����ȡ
     *
     * ʹ��ʾ������:
     *
     *
     * if (TAPP_RET_IS_ERR(ret) && (TAPP_ERRNO_ARG == TAPP_ERR_GET_ERRNO(ret)))
     * {
     *      *** ***
     * }
     */
    typedef enum
    {
        TAPP_ERRNO_NONE = 0,                /**<no error*/
        TAPP_ERRNO_ARG_INVALID,             /**<invalid argument*/
        TAPP_ERRNO_BUF_FULL,                /**<send-buffer is full*/
        TAPP_ERRNO_BUF_EMPTY,               /**<no data in recv-buffer*/
        TAPP_ERRNO_COUNT                    /**<end of error number*/
    } TAPPERRNO;


    /**
     * TAPP-API ����ֵЯ���Ĵ�����Ϣ����
     * ע��: ������Ϣֻ��ͨ�� TAPP_RET_MATCH �뷵��ֵƥ��,
     *
     * ʹ��ʾ������:
     *
     * if (TAPP_RET_IS_ERR(ret) && TAPP_RET_MATCH(ret, TAPP_ERROR_INVALID_ARG))
     * {
     *      *** ***
     * }
     */
    typedef enum
    {
        TAPP_ERROR_INVALID_ARG  = TAPP_ERR_MAKE_ERROR(TAPP_ERRNO_ARG_INVALID),
        TAPP_ERROR_BUF_FULL     = TAPP_ERR_MAKE_ERROR(TAPP_ERRNO_BUF_FULL),
        TAPP_ERROR_BUF_EMPTY    = TAPP_ERR_MAKE_ERROR(TAPP_ERRNO_BUF_EMPTY),
    } TAPPERROR;


    /**
     * ���ݴ�������ȡ������Ϣ
     * @param[in] iErrorCode �������
     *
     * @return  ������Ϣ����ָ��
     */
    TSF4G_API const char *tapp_error_string(IN int iRetCode);


#ifdef __cplusplus
}
#endif


/** @} */


#endif /* TAPP_ERROR_H */
