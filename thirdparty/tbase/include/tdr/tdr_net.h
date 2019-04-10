/**
*
* @file     tdr_net.h
* @brief    TDRԪ�������罻����Ϣ�����ģ��
*
* @author steve jackyai
* @version 1.0
* @date 2007-04-28
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/



#ifndef TSF_G_TDR_NET_H
#define TSF_G_TDR_NET_H


#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"


#ifdef TDR_OS_LITTLEENDIAN
#define tdr_ntoh64(x)                    TDR_OS_SWAP64(x)
#define tdr_hton64(x)                    TDR_OS_SWAP64(x)
#define tdr_ntoh32(x)                    TDR_OS_SWAP32(x)
#define tdr_hton32(x)                    TDR_OS_SWAP32(x)
#define tdr_ntoh16(x)                    TDR_OS_SWAP16(x)
#define tdr_hton16(x)                    TDR_OS_SWAP16(x)
#else
#define tdr_ntoh64(x)                    (x)
#define tdr_hton64(x)                    (x)
#define tdr_ntoh32(x)                   (x)
#define tdr_hton32(x)                   (x)
#define tdr_ntoh16(x)                   (x)
#define tdr_hton16(x)                   (x)
#endif



#define TDR_SET_INT_NET(p, iSize, i)		switch(iSize)		      \
{									      \
    case 2:								      \
    *(unsigned short*)(p)	=	tdr_hton16((unsigned short)(i));   \
    break;							      \
    case 4:								      \
    *(uint32_t*)(p)	=	tdr_hton32((uint32_t)(i));    \
    break;							      \
    case 8:									\
    *(uint64_t*)(p)	=	tdr_ntoh64((uint64_t)(i));    \
    break;								\
    default:							      \
    *(unsigned char*)(p)	=	(unsigned char)(i);	      \
}


#define TDR_GET_INT_NET(i, iSize, p)		switch(iSize)		      \
{									      \
    case 2:								      \
    i = (int)tdr_ntoh16(*(unsigned short*)(p));			      \
    break;							      \
    case 4:								      \
    i = (int)tdr_ntoh32(*(uint32_t*)(p));			      \
    break;							      \
    case 8:									\
    i = tdr_ntoh64(*(uint64_t*)p);	\
    break;												\
    default:							      \
    i = (int)*(unsigned char*)(p);				      \
}



#define TDR_GET_UINT_NET(i, iSize, p)		switch(iSize)		      \
{									      \
    case 2:								      \
    i = (uint32_t)tdr_ntoh16(*(unsigned short*)(p));			      \
    break;							      \
    case 4:								      \
    i = (uint32_t)tdr_ntoh32(*(uint32_t*)(p));			      \
    break;							      \
    case 8:									\
    i = tdr_ntoh64(*(uint64_t*)p);	\
    break;												\
    default:							      \
    i = (uint32_t)*(unsigned char*)(p);				      \
}




#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup TDR_NETMSG_PACKUNPACK TDR_���罻����Ϣ��/����
* @{
*/

/**
	*����ָ���汾����Ԫ�������������ش洢����ת�������罻����Ϣ
	*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
	*@param[in,out]	a_pstNet �洢������Ϣ�ṹ��ָ��
	*	- ����	a_pstNet.pszBuff ָ������������Ϣ�Ļ�������ʼ��ַ
	*	- ����  a_pstNet.iBuff	ָ���������Ĵ�С
	*	- ���  a_pstNet.iBuff  ʵ�ʱ���������Ϣ���ܳ���
	*@param[in,out]	a_pstHost �洢���ؽṹ��Ϣ�Ľṹ��ָ��
	*	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
	*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
	*	- ���  a_pstHost.iBuff  �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                 ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
	*@param[in] a_iVersion	���õİ汾�ţ���Ҫ���뵽������Ϣ���еĽṹ��汾��
	*
	*@note a_iVersion���벻С�ڱ��ؽṹ�������Ļ����汾(tdr_get_meta_based_version)�����a_iVersionΪ0 ����װ���õ��ṹ��ǰ��߰汾��
	*@note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
	*	- 1.���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
	*	- 2.����a_pstHost.iBuff�õ���ʵ�ʱ������Ϣ���ܳ��ȣ�ͨ��a_pstHost�м�¼����Ϣ�����Ե���
	*		tdr_fprintf/tdr_sprintf ��ӡ���Ѿ��ɹ�����Ľṹ��Ϣ��ͨ�����tdr_hton����ʧ�ܣ�ͨ��
	*		���ֶο��Զ�λ�������ľ���λ�á�
	*
	* @pre \e a_pstMeta ����Ϊ NULL�����ұ����Ӧ xml �����ļ��� struct Ԫ�ء�
	* @pre \e a_pstNet ����Ϊ NULL
	* @pre \e a_pstNet.pszBuff ����Ϊ NULL
	* @pre \e a_pstNet.iBuff �������0
	* @pre \e a_pstHost ����Ϊ NULL
	* @pre \e a_pstHost.pszBuff ����Ϊ NULL
	* @pre \e a_pstHost.iBuff �������0
    * @pre \e
	*
	*@retval 0   ����ɹ�
	*@retval >0  ����ɹ���������ĳЩ�������
	*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
	*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
	*		- TDR_ERR_NET_NO_NETBUFF_SPACE ����������Ϣ�Ļ�����ʣ��ռ䲻��
	*		- TDR_ERROR_NO_HOSTBUFF_SPACE ���ش洢������ʣ��ռ䲻��
	*		- TDR_ERROR_INVALID_REFER_VALUE		��Ԫ�ص�refer����ֵ����ȷ����ֵ����Ϊ�����ұ����count����ֵС
	*		- TDR_ERROR_TOO_COMPLIEX_META Ԫ��������������������Ƕ�ײ�γ���32��
	*		- TDR_ERROR_NET_UNSUPPORTED_TYPE    ��֧�ֵ�Ԫ��������
	*		- TDR_ERROR_NET_INVALID_STRING_LEN string���͵�Ԫ�����ַ������ȳ�����Ԥ����󳤶�
	*		- TDR_ERROR_NET_INVALID_VERSIONINDICATOR Ԫ���ݰ汾ָʾ����ֵ�ȱ�����ֵ��Ԫ���ݳ�Ա�İ汾ҪС
	*
    * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
	*
	* @see	tdr_get_meta_by_name
	* @see	tdr_get_meta_by_id
	* @see	tdr_error_string
	* @see	tdr_entry_off_to_path
	* @see  tdr_get_meta_based_version
    * @see  tdr_get_error_detail
	*/
TDR_API int tdr_hton(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstNet, INOUT LPTDRDATA a_pstHost, IN int a_iVersion);

/**
*����ָ���汾����Ԫ�������������罻����Ϣת���ɱ��ش洢����
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in,out]	a_pstHost �洢���ؽṹ��Ϣ�Ľṹ��ָ��
*	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
*	- ���  a_pstHost.iBuff   �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                              ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
*@param[in,out]	a_pstNet �洢������Ϣ�ṹ��ָ��
*	- ����	a_pstNet.pszBuff ָ������������Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstNet.iBuff	ָ���������Ĵ�С
*	- ���  a_pstNet.iBuff  ʵ�ʽ���������Ϣ���ܳ���
*@param[in] a_iVersion	ָ��������������Ϣ���нṹ��İ汾�ţ�
*
*@note a_iVersion���벻С�ڱ��ؽṹ�������Ļ����汾(tdr_get_meta_based_version)���Ҳ����ڱ��ؽṹ����������߰汾(tdr_get_meta_current_version)��
*	���a_iVersionΪ0 �����սṹ�嵱ǰ��߰汾����
*@note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
	*	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
	*	- ����a_pstHost.iBuff�õ���ʵ�ʽ������Ϣ���ܳ��ȣ����������Ϊƫ��ֵ������tdr_entry_off_to_path���Ի�ȡ
	*		����ʱ��Ա��·����Ϣ���Ӷ���λ����������ĸ�λ�ó���
	*	- ����tdr_fprintf/tdr_sprintf���Դ�ӡ���Ѿ����������
*
* @pre \e a_pstMeta ����Ϊ NULL�����ұ����Ӧ xml �����ļ��� struct Ԫ�ء�
* @pre \e a_pstNet ����Ϊ NULL
* @pre \e a_pstNet.pszBuff ����Ϊ NULL
* @pre \e a_pstNet.iBuff �������0
* @pre \e a_pstHost ����ΪNULL
* @pre \e a_pstHost.pszBuff ����Ϊ NULL
* @pre \e a_pstHost.iBuff �������0
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС,�����Ԫ���ݵ�ǰ��߰汾
*		- TDR_ERR_NET_NO_NETBUFF_SPACE ����������Ϣ�Ļ�����ʣ��ռ䲻��
*		- TDR_ERROR_NO_HOSTBUFF_SPACE ���ش洢������ʣ��ռ䲻��
*		- TDR_ERROR_INVALID_REFER_VALUE		��Ԫ�ص�refer����ֵ����ȷ����ֵ����Ϊ�����ұ����count����ֵС
*		- TDR_ERROR_TOO_COMPLIEX_META Ԫ��������������������Ƕ�ײ�γ���32��
*		- TDR_ERROR_NET_UNSUPPORTED_TYPE    ��֧�ֵ�Ԫ��������
*		- TDR_ERROR_NET_INVALID_STRING_LEN string���͵�Ԫ�����ַ������ȳ�����Ԥ����󳤶�
*
* @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
* @see	tdr_error_string
* @see	tdr_entry_off_to_path
* @see  tdr_fprintf
* @see  tdr_get_meta_current_version
* @see  tdr_get_meta_based_version
* @see  tdr_get_error_detail
*/
TDR_API int tdr_ntoh(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, INOUT LPTDRDATA a_pstNet, IN int a_iVersion);

/** @} */ /*@defgroup TDR_���罻����Ϣ��/����*/



/**
 *��ȡԪ���ݶ�Ӧ�ṹ���ھ���tdr_hton����������������ռ��С
 *@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
 *@param[in]    a_iVersion	ָ��Ԫ���ݵļ��ð汾
 *@param[out]   a_ptMaxNetSize ���������ܵ�����ռ��С
 *
 *@note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
 *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
 *
 *@pre \e a_pstMeta ����Ϊ NULL�����ұ����Ӧ xml �����ļ��� struct Ԫ�ء�
 *@pre \e a_piMaxNetSize ����Ϊ NULL
 *
 *@retval >0  ����ɹ���������ĳЩ�������
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
 *		- TDR_ERROR_TOO_COMPLIEX_META Ԫ��������������������Ƕ�ײ�γ���32��
 *
 *
 * @see	tdr_get_meta_by_name
 * @see	tdr_get_meta_by_id
 * @see	tdr_error_string
 * @see  tdr_fprintf
 * @see  tdr_get_meta_current_version
 * @see  tdr_get_meta_based_version
 */
TDR_API int tdr_get_meta_max_net_size(IN LPTDRMETA a_pstMeta, IN int a_iVersion, OUT size_t* a_ptMaxNetSize);



#ifdef __cplusplus
}
#endif

#endif /* TSF_G_TDR_NET_H */
