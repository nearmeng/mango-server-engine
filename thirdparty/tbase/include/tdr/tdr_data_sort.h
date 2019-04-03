/**
*
* @file     tdr_data_sort.h  
* @brief    TDR��������
* 
* @author steve jackyai  
* @version 1.0
* @date 2007-05-18 
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/

#ifndef TDR_DATA_SROT_H
#define TDR_DATA_SROT_H


#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/** @defgroup TDR_DATA_SORT TDR_��������
	*  @{
	*/

	/**
	*����ָ���汾����Ԫ����������������������Ҫ����������������
	*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
	*@param[in,out]	a_pstHost �洢���ؽṹ���ݵĽṹ��ָ��
	*	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ���ݵĻ�������ʼ��ַ
	*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
	*	- ���  a_pstHost.iBuff  ʵ�ʽ��뱾�ؽṹ��Ϣ���ܳ���
	*@param[in] a_iVersion	Ҫ���õİ汾��
	*
	*@note ���a_pstMeta������union���͵Ľṹ,�򲻽�������ֱ�ӷ���
	*@note ���a_iVersionΪ0 ����װ���õ���߰汾
	*@note �����Ԫ����ָ��������ؼ���sortkey���ԣ�������ؼ��ֽ������򣻷������Ԫ���ݸ���Ա����˳��
	*���αȽϸ���Ա�Ĵ�С��������:
	*	- ���ĳ��ԱΪ���飬�Ҵ˳�Աû��ָ�����򷽷�������Ϊ�˳�Ա��ȣ��������αȽϸ������Ա�������Ա���鳤�Ȳ��ȣ�
	*     ��������ǰ��Ԫ�ض��ȵ�����£����鳤�ȴ�ĳ�Ա��Ϊ��
	*
	* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
	* @pre \e a_pstMeta ����Ϊ NULL
	* @pre \e a_pstHost ����Ϊ NULL
	* @pre \e a_pstHost.pszBuff ����Ϊ NULL
	* @pre \e a_pstHost.iBuff �������0	
	*
	*@retval 0   ����ɹ�
	*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������	
	*
	*
	* @see	tdr_get_meta_by_name
	* @see	tdr_get_meta_by_id
	*/
TDR_API int tdr_sort(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN int a_iVersion);

/**
*����ָ���汾����Ԫ���������Ը������ݽṹ�����������
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in,out]	a_pstHost �洢���ؽṹ���ݵĽṹ��ָ��
*	- ����	a_pstHost.pszBuff ָ������ṹ�������ݵĻ�������ʼ��ַ
*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
*	- ���  a_pstHost.iBuff  ʵ�ʽ��뱾�ؽṹ��Ϣ���ܳ���
*@param[in] a_iCount	ָ��������Ľṹ���ݵĸ���
*@param[in] a_enSortMethod	���򷽷�
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note ���a_pstMeta������union���͵Ľṹ,�򲻽�������ֱ�ӷ���
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note �����Ԫ����ָ��������ؼ���sortkey���ԣ�������ؼ��ֽ������򣻷������Ԫ���ݸ���Ա����˳��
*���αȽϸ���Ա�Ĵ�С��������:
*	- ���ĳ��ԱΪ���飬�Ҵ˳�Աû��ָ�����򷽷�������Ϊ�˳�Ա��ȣ��������αȽϸ������Ա�������Ա���鳤�Ȳ��ȣ�
*     ��������ǰ��Ԫ�ض��ȵ�����£����鳤�ȴ�ĳ�Ա��Ϊ��
*
* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstHost ����Ϊ NULL
* @pre \e a_pstHost.pszBuff ����Ϊ NULL
* @pre \e a_pstHost.iBuff �������0	
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������	
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_sort_metas(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN int a_iCount, IN TDRSORTMETHOD a_enSortMethod, IN int a_iVersion);


/**
*�����ݽṹ�е�ָ�������Ա����ָ�����͵�����
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_pstHost �洢���ؽṹ���ݵĽṹ��ָ��
*	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ���ݵĻ�������ʼ��ַ
*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
*@param[in] a_iSortType ָ�����򷽷�,��ЧȡֵΪ0,1,2,�ֱ��ʾ"������", "����������","����������"�������򷽷�
*@param[in] a_pszSortEntry ָ��Ҫ��������ݳ�Ա,��ȡֵΪ�ó�Ա��Ԫ���������е�·��Path
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note �����Ԫ����ָ��������ؼ���sortkey���ԣ�������ؼ��ֽ������򣻷������Ԫ���ݸ���Ա����˳��
*���αȽϸ���Ա�Ĵ�С��������:
*	- ���ĳ��ԱΪ���飬�Ҵ˳�Աû��ָ�����򷽷�������Ϊ�˳�Ա��ȣ��������αȽϸ������Ա�������Ա���鳤�Ȳ��ȣ�
*     ��������ǰ��Ԫ�ض��ȵ�����£����鳤�ȴ�ĳ�Ա��Ϊ��
*
* @pre \e a_pstHost.pszBuff�������б����ǰ���a_pstMeta����������
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstHost ����Ϊ NULL
* @pre \e a_pstHost.pszBuff ����Ϊ NULL
* @pre \e a_pstHost.iBuff �������0	
* @pre \e a_pszSortEntry ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
*	-	TDR_ERROR_INVALID_CUTOFF_VERSION ָ���汾��Ԫ���ݻ�׼�汾С
*	-	TDR_ERROR_ENTRY_INVALID_SORTMETHOD_VALUE	���򷽷�����ȷ
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_sort_spec(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN int a_iSortType, IN const char* a_pszSortEntry, IN int a_iVersion);

/** @}*/ /* TDR_DATA_SORT TDR_��������*/

/** @defgroup TDR_DATA_NORMALIZE TDR_��������
*  @{
*/

/** 
*@brief ����Ԫ���ݶ��壬������ݵ������Ƿ����Ԫ�����ж����Լ����
*��ЩԼ����Ҫ�����������С��ֵ��Լ������Լ��������ȡ��������û�����򣬼�����������������������С��ֵ��Լ������Լ��
*��ֱ�ӱ���
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
TDR_API int tdr_normalize(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN int a_iVersion);

/** @}*/  /* TDR_DATA_NORMALIZE TDR_��������*/

#ifdef __cplusplus
}
#endif

#endif /* TDR_DATA_SROT_H */
