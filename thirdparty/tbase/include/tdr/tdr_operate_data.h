/**
*
* @file     tdr_operate_data.h
* @brief    TDR���ݳ�ʼ��/����
*
* @author steve jackyai
* @version 1.0
* @date 2007-06-05
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/
#ifndef TDR_OPERATE_DATA
#define TDR_OPERATE_DATA


#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/** @defgroup TDR_OPERATE_DATA TDR_���ݲ���
*  @{
*/

/**
*@brief ���ݳ�ʼ��
*����Ԫ���ݵ�ȱʡֵԼ�����ڴ�ṹ����Ӧ��Ա����ʼֵ
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_pstHost �洢���ؽṹ���ݵĽṹ��ָ��
*	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ���ݵĻ�������ʼ��ַ
*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note ���a_pstMeta������union���͵Ľṹ,�򲻽��д���ֱ�ӷ���
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*
* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstHost ����Ϊ NULL
* @pre \e a_pstHost.pszBuff ����Ϊ NULL
* @pre \e a_pstHost.iBuff �������0
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_init(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN int a_iVersion);


/**
*@brief ���ݿ���
*����Ԫ�����������ڴ����ݿ�����ָ���ڴ���
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_pstHostDst ������ÿ������ݵĻ������ṹ��ָ��
*	- ����	a_pstHostDst.pszBuff ָ�����濽�����ݵĻ�������ʼ��ַ
*	- ����  a_pstHostDst.iBuff	ָ���������Ĵ�С
*	- ���	a_pstHostDst.iBuff  �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
*@param[in]	a_pstHostSrc �洢Դ���ݵ��ڴ滺�����ṹ��ָ��
*	- ����	a_pstHostSrc.pszBuff ָ������Դ���ݵĻ�������ʼ��ַ
*	- ����  a_pstHostSrc.iBuff	ָ���������Ĵ�С
*
*@note ���ݿ�����Դ�ڴ�����Ŀ���ڴ��������ص�
*@note ���a_pstMeta������union���͵Ľṹ,�򲻽��д���ֱ�ӷ���
*
* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstHostDst ����Ϊ NULL
* @pre \e a_pstHostDst.pszBuff ����Ϊ NULL
* @pre \e a_pstHostDst.iBuff �������0
* @pre \e a_pstHostSrc ����Ϊ NULL
* @pre \e a_pstHostSrc.pszBuff ����Ϊ NULL
* @pre \e a_pstHostSrc.iBuff �������0
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_copy(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHostDst, IN LPTDRDATA a_pstHostSrc);

/**
*@brief ���ݱ���
*����Ԫ��������������������
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in] a_pHandle ���ݴ�����
*@param[in]	a_pstHost �������ݵĻ������ṹ��ָ��
*	- ����	a_pstHost.pszBuff ָ�����濽�����ݵĻ�������ʼ��ַ
*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
*@param[in]	a_iVersion ���ݵİ汾,�����ֵΪ0,���յ�ǰ��߰汾����
*@param[in] pfnEnter ����������������ʱ�Ļص�����
*@param[in] pfnVistEntry ���ʼ��������͵Ļص�����
*
*@note ���a_pstMeta������union���͵Ľṹ,�򲻽��д���ֱ�ӷ���
*
* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstHostDst ����Ϊ NULL
* @pre \e a_pstHostDst.pszBuff ����Ϊ NULL
* @pre \e a_pstHostDst.iBuff �������0
* @pre \e pfnEnter ����Ϊ NULL
* @pre \e pfnEnter  ����Ϊ NULL
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
* @see  PFN_ENTER_META
* @see  PFN_VISIT_SIMPLE_ENTRY
*/
TDR_API int tdr_browse(IN LPTDRMETA a_pstMeta, IN void *a_pHandle, IN LPTDRDATA a_pstHost, IN int a_iVersion,
					   IN PFN_ENTER_META pfnEnter, IN PFN_VISIT_SIMPLE_ENTRY pfnVistEntry);

/** @}*/ /* TDR_OPERATE_DATA TDR_���ݲ���*/


#ifdef __cplusplus
}
#endif

#endif /* TDR_OPERATE_DATA */
