/**
*
* @file     tdr_ctypes_info.h
* @brief    TDR֧�ֵ�һЩC��������Ļ�����������
*
* @author steve jackyai
* @version 1.0
* @date 2007-03-26
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/

#ifndef TDR_CTYPES_H
#define TDR_CTYPES_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "tdr/tdr_define.h"
#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"

/** @defgroup TDR_CTYPE_INFO TDR_�����������ʹ���
*  @{
*/

/** @name ������������ID
*  @{
*/
#define TDR_TYPE_UNKOWN         -1
#define TDR_TYPE_MIN		0x00
#define TDR_TYPE_UNION		TDR_TYPE_MIN
#define TDR_TYPE_STRUCT		0x01
#define	TDR_TYPE_COMPOSITE	TDR_TYPE_STRUCT
#define TDR_TYPE_CHAR		0x02
#define TDR_TYPE_UCHAR		0x03
#define TDR_TYPE_BYTE		0x04
#define TDR_TYPE_SMALLINT		0x05
#define TDR_TYPE_SHORT		0x05
#define TDR_TYPE_SMALLUINT		0x06
#define TDR_TYPE_USHORT		0x06
#define TDR_TYPE_INT		0x07
#define TDR_TYPE_UINT		0x08
#define TDR_TYPE_LONG		0x09
#define TDR_TYPE_ULONG		0x0a
#define TDR_TYPE_LONGLONG	0x0b
#define TDR_TYPE_ULONGLONG	0x0c
#define TDR_TYPE_DATE		0x0d
#define TDR_TYPE_TIME		0x0e
#define TDR_TYPE_DATETIME	0x0f
#define TDR_TYPE_MONEY		0x10
#define TDR_TYPE_FLOAT		0x11
#define TDR_TYPE_DOUBLE		0x12
#define TDR_TYPE_IP			0x13
#define TDR_TYPE_WCHAR		0x14
#define TDR_TYPE_STRING		0x15
#define TDR_TYPE_WSTRING	0x16
#define TDR_TYPE_VOID		0x17
#define TDR_TYPE_MAX		TDR_TYPE_VOID
/**   @}*/

/**
*��numbers-and-dots ��ʽ��IP��ַ��ת����tdr�Զ����ʽ��IP��ַ��������
*@param[out]	a_piTDRIp ����tdr�����IP��ַ���ݵ�ָ��
*@param[in]	a_pszIP �洢numbers-and-dots ��ʽ��IP��ַ����ָ��
*
*@retval 0   ����ɹ�
*@retval  non-zero  ����ʧ��
*
*@note ����255.255.255.255��ַ��Linux�ܹ��ɹ�ת����Windows�򷵻ش���һ������£� �˵�ַ��ʵ��ʹ�����塣
*
* @pre \e a_piTDRIp ����Ϊ NULL
* @pre \e a_pszIP ����Ϊ NULL
*
*/
TDR_API int tdr_ineta_to_tdrip(OUT tdr_ip_t *a_piTDRIp, IN const char *a_pszIP);

/**
*��tdr�Զ����ʽ��IP��ַ����ת����struct in_addr�ṹ����
*@param[out]	a_pInp ���struct in_addr�ṹ������
*@param[in]	a_iTDRIp tdr�Զ����ʽ��IP��ַ����
*
*@retval 0   ����ɹ�
*@retval  non-zero  ����ʧ��
*
* @pre \e a_pInp ����Ϊ NULL
*
*/
TDR_API int tdr_tdrip_to_inetaddr(OUT struct in_addr *a_pInp, IN tdr_ip_t a_iTDRIp);

/**
*��tdr�Զ����ʽ��IP��ַ����ת����numbers-and-dots ��ʽ��IP��ַ��
*@param[in]	a_iTDRIp tdr�Զ����ʽ��IP��ַ����
*
*@return �������ɹ�,����ip��ַ����ָ��,���򷵻�NULL
*@note IP��ַ��������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*/
TDR_API char *tdr_tdrip_to_ineta(IN tdr_ip_t a_iTDRIp);

/**
*��tdr�Զ����ʽ��IP��ַ����ת����numbers-and-dots ��ʽ��IP��ַ��
*@param[in]	a_iTDRIp tdr�Զ����ʽ��IP��ַ����
*@param[in] a_pszDstBuff	����IP��ַ��Ϣ���ַ���������
*@param[in] a_iBuffSize �������Ŀ��ô�С
*
*@return �������ɹ�,����a_pszDstBuff��ָ��,���򷵻�NULL

*
*/
TDR_API const char *tdr_tdrip_ntop(IN tdr_ip_t a_iTDRIp, IN char *a_pszDstBuff, IN int a_iBuffSize);
/**
*��'YYYY-MM-DD' ��ʽ��date�ַ���ת����tdr�Զ����ʽ����������
*@param[out]	a_piTdrDate ����tdr������������ݵ�ָ��
*@param[in]	a_pszDate �����ַ���
*
*@return  �ɹ�����0, ���򷵻ط�0ֵ
*
*@pre \e a_piTdrDate ����ΪNULL
*@pre \e a_pszDate ����ΪNULL
*/
TDR_API int tdr_str_to_tdrdate(OUT tdr_date_t *a_piTdrDate, IN const char *a_pszDate);

/**��tdr�Զ����ʽ����������ת����'YYYY-MM-DD' ��ʽ��date�ַ���
*@param[in]	a_piTdrDate ����tdr������������ݵ�ָ��
*
*@return �������ɹ�,���������ַ�����ָ��,���򷵻ؿմ�
*@note ���ص������ַ���������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*@pre \e a_piTdrDate ����ΪNULL
*
*/
TDR_API char *tdr_tdrdate_to_str(IN const tdr_date_t *a_piTdrDate);

/**��tdr�Զ����ʽ����������ת����'YYYY-MM-DD' ��ʽ��date�ַ��� ���ӿ����̰߳�ȫ�汾
*@param[in]	a_piTdrDate ����tdr������������ݵ�ָ��
*@param[in] a_pszDstBuf	����ʱ����Ϣ���ַ���������
*@param[in] a_iBuffSize �������Ŀ��ô�С
*
*@return �������ɹ�,���������ַ�����ָ��,���򷵻ؿմ�
*
*@pre \e a_piTdrDate ����ΪNULL
*
*/
TDR_API const char *tdr_tdrdate_to_str_r(IN const tdr_date_t *a_piTdrDate,
										 IN char *a_pszDstBuf, IN int a_iBuffSize);


/**
*��'HHH:MM:SS' ��ʽ��time�ַ���ת����tdr�Զ����ʽ��ʱ������
*@param[out]	a_piTdrTime ����tdr�����ʱ��time����
*@param[in]	a_pszTime ʱ���ַ���
*
*@note ֧��3���ַ�����ʾСʱֵ��Ŀ��: time�������Ͳ����������ڱ�ʾһ���ʱ��(��һ�����ᳬ�� 24 Сʱ)�����ҿ���������ʾ��������ʱ��������¼�֮���ʱ����(����ܱ� 24 Сʱ��������һ����ֵ)
*@return  �ɹ�����0, ���򷵻ط�0ֵ
*
*@pre \e a_piTdrTime ����ΪNULL
*@pre \e a_pszTime ����ΪNULL
*/
TDR_API int tdr_str_to_tdrtime(OUT tdr_time_t *a_piTdrTime, IN const char *a_pszTime);

/**
*��tdr�Զ����ʽ��ʱ������ת����'HHH:MM:SS' ��ʽ��time�ַ���
*@param[in]	a_piTdrTime ����tdr�����ʱ�����ݵ�ָ��
*
*@return �������ɹ�,����ʱ���ַ�����ָ��,���򷵻ؿմ�
*@note ���ص�ʱ���ַ���������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*@pre \e a_piTdrTime ����ΪNULL
*
*/
TDR_API char *tdr_tdrtime_to_str(IN tdr_time_t *a_piTdrTime);

/**
*��tdr�Զ����ʽ��ʱ������ת����'HHH:MM:SS' ��ʽ��time�ַ���.�̰߳�ȫ�汾
*@param[in]	a_piTdrTime ����tdr�����ʱ�����ݵ�ָ��
*@param[in] a_pszDstBuf	����ʱ����Ϣ���ַ���������
*@param[in] a_iBuffSize �������Ŀ��ô�С
*
*@return �������ɹ�,����ʱ���ַ�����ָ��,���򷵻ؿմ�
*@note ���ص�ʱ���ַ���������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*@pre \e a_piTdrTime ����ΪNULL
*
*/
TDR_API char *tdr_tdrtime_to_str_r(IN tdr_time_t *a_piTdrTime,
								   IN char *a_pszDstBuf, IN int a_iBuffSize);


/**
*��'YYYY-MM-DD HHH:MM:SS' ��ʽ������/ʱ���ַ���ת����tdr�Զ����ʽ������ʱ������
*@param[out]	a_piTdrDateTime ����tdr�����ʱ��time����
*@param[in]	a_pszDateTime ʱ���ַ���
*
*@return  �ɹ�����0, ���򷵻ط�0ֵ
*
*@pre \e a_piTdrDateTime ����ΪNULL
*@pre \e a_pszDateTime ����ΪNULL
*/
TDR_API int tdr_str_to_tdrdatetime(OUT tdr_datetime_t *a_piTdrDateTime, IN const char *a_pszDateTime);

/**
*convert the number of seconds elapsed since midnight (00:00:00), January 1, 1970, Coordinated Universal Time (UTC)  to tdr_datetime_t
*@param[out]	a_piTdrDateTime ����tdr�����ʱ��time����
*@param[in]	a_tTimer  ʱ��
*
*@return  �ɹ�����0, ���򷵻ط�0ֵ
*
*@pre \e a_piTdrDateTime ����ΪNULL
*/
TDR_API int tdr_utctime_to_tdrdatetime(OUT tdr_datetime_t *a_piTdrDateTime, IN time_t a_tTimer);

/**
*convert tdr_datetime_t to the number of seconds elapsed since midnight (00:00:00), January 1, 1970, Coordinated Universal Time (UTC)
*@param[out]	a_ptTimer  ����ʱ���ָ��
*@param[in]	a_iTdrDateTime ����tdr�����ʱ��time����
*
*@return  �ɹ�����0, ���򷵻ط�0ֵ
*
*@pre \e a_piTdrDateTime ����ΪNULL
*/
TDR_API int tdr_tdrdatetime_to_utctime(OUT  time_t *a_ptTimer, IN tdr_datetime_t a_iTdrDateTime);


/**
*��tdr�Զ����ʽ������/ʱ������ת����'YYYY-MM-DD HHH:MM:SS' ��ʽ������/ʱ���ַ���
*@param[in]	a_piTdrDateTime ����tdr���������/ʱ�����ݵ�ָ��
*
*@return �������ɹ�,��������/ʱ���ַ�����ָ��,���򷵻ؿմ�
*@note ���ص�����/ʱ���ַ���������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*@pre \e a_piTdrDateTime ����ΪNULL
*/
TDR_API char *tdr_tdrdatetime_to_str(IN const tdr_datetime_t *a_piTdrDateTime);

/**
*��tdr�Զ����ʽ������/ʱ������ת����'YYYY-MM-DD HHH:MM:SS' ��ʽ������/ʱ���ַ���.�̰߳�ȫ�汾
*@param[in]	a_piTdrDateTime ����tdr���������/ʱ�����ݵ�ָ��
*@param[in] a_pszDstBuf	����ʱ����Ϣ���ַ���������
*@param[in] a_iBuffSize �������Ŀ��ô�С
*
*@return �������ɹ�,��������/ʱ���ַ�����ָ��,���򷵻ؿմ�
*@note ���ص�����/ʱ���ַ���������tdr����ľ�̬������,���˺����´ε���ʱ,�����ݻᱻ����
*
*@pre \e a_piTdrDateTime ����ΪNULL
*/
TDR_API char *tdr_tdrdatetime_to_str_r(IN const tdr_datetime_t *a_piTdrDateTime,
									   IN char *a_pszDstBuf, IN int a_iBuffSize);


/**
*�Ƚ���������/ʱ��ֵ�Ĵ�С
*@param[in]	a_piTdrDateTime1 ����tdr���������/ʱ�����ݵ�ָ��
*@param[in]	a_piTdrDateTime2 ����tdr���������/ʱ�����ݵ�ָ��
*
*@retval >0 a_piTdrDateTime1��a_piTdrDateTime2��,��a_piTdrDateTime1ʱ������a_piTdrDateTime2
*@retval 0  ʱ����ͬ
*@retval <0 a_piTdrDateTime1��a_piTdrDateTime2С,��a_piTdrDateTime1ʱ������a_piTdrDateTime2
*
*@pre \e a_piTdrDateTime1 ����ΪNULL
*@pre \e a_piTdrDateTime2 ����ΪNULL
*/
TDR_API int tdr_compare_datetime(IN const tdr_datetime_t *a_piTdrDateTime1, IN const tdr_datetime_t *a_piTdrDateTime2);

/**�Ƚ���������ֵ�Ĵ�С
*@param[in]	a_piTdrDate1 ����tdr������������ݵ�ָ��
*@param[in]	a_piTdrDate2 ����tdr������������ݵ�ָ��
*
*@retval >0 a_piTdrDate1��a_piTdrDate2��,��a_piTdrDate1ʱ������a_piTdrDate2
*@retval 0  ʱ����ͬ
*@retval <0 a_piTdrDate1��a_piTdrDate2С,��a_piTdrDate1ʱ������a_piTdrDate2
*
*@pre \e a_piTdrDate1 ����ΪNULL
*@pre \e a_piTdrDate2 ����ΪNULL
*/
TDR_API int tdr_compare_date(IN const tdr_date_t *a_piTdrDate1, IN const tdr_date_t *a_piTdrDate2);

/**
*�Ƚ�����ʱ��ֵ�Ĵ�С
*@param[in]	a_piTdrTime1 ����tdr�����ʱ�����ݵ�ָ��
*@param[in]	a_piTdrTime2 ����tdr�����ʱ�����ݵ�ָ��
*
*@retval >0 a_piTdrTime1��a_piTdrTime2��,��a_piTdrTime1ʱ������a_piTdrTime2
*@retval 0  ʱ����ͬ
*@retval <0 a_piTdrTime1��a_piTdrTime2С,��a_piTdrTime1ʱ������a_piTdrTime2
*
*@pre \e a_piTdrTime1 ����ΪNULL
*@pre \e a_piTdrTime2 ����ΪNULL
*/
TDR_API int tdr_compare_time(IN const tdr_time_t *a_piTdrTime1, IN const tdr_time_t *a_piTdrTime2);

/**  @}*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*TDR_CTYPES_H*/
