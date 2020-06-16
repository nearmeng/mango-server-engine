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
/* above code should be Reserved because some function still use them. */

#define TDR_CAST_SWAP64(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[7]; \
        ((char *)dst)[1] = ((const char *)src)[6]; \
        ((char *)dst)[2] = ((const char *)src)[5]; \
        ((char *)dst)[3] = ((const char *)src)[4]; \
        ((char *)dst)[4] = ((const char *)src)[3]; \
        ((char *)dst)[5] = ((const char *)src)[2]; \
        ((char *)dst)[6] = ((const char *)src)[1]; \
        ((char *)dst)[7] = ((const char *)src)[0]; \
    } while (0)
		

#define TDR_SET_SWAP64(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & (uint64_t)0xff00000000000000LL) >> 56); \
        ((char *)dst)[1] = (((val) & (uint64_t)0x00ff000000000000LL) >> 48); \
        ((char *)dst)[2] = (((val) & (uint64_t)0x0000ff0000000000LL) >> 40); \
        ((char *)dst)[3] = (((val) & (uint64_t)0x000000ff00000000LL) >> 32); \
        ((char *)dst)[4] = (((val) & (uint64_t)0x00000000ff000000) >> 24); \
        ((char *)dst)[5] = (((val) & (uint64_t)0x0000000000ff0000) >> 16); \
        ((char *)dst)[6] = (((val) & (uint64_t)0x000000000000ff00) >> 8); \
        ((char *)dst)[7] = (((val) & (uint64_t)0x00000000000000ff)); \
    } while (0)


#define TDR_GET_SWAP64(dst) \
	((((unsigned char *)dst)[7])\
	 |((uint64_t)(((unsigned char *)dst)[6]) << 8)\
	 |((uint64_t)(((unsigned char *)dst)[5]) << 16)\
	 |((uint64_t)(((unsigned char *)dst)[4]) << 24)\
	 |((uint64_t)(((unsigned char *)dst)[3]) << 32)\
	 |((uint64_t)(((unsigned char *)dst)[2]) << 40)\
	 |((uint64_t)(((unsigned char *)dst)[1]) << 48)\
	 |((uint64_t)(((unsigned char *)dst)[0]) << 56));
		
#define TDR_CAST_SWAP32(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[3]; \
        ((char *)dst)[1] = ((const char *)src)[2]; \
        ((char *)dst)[2] = ((const char *)src)[1]; \
        ((char *)dst)[3] = ((const char *)src)[0]; \
    } while (0)

#define TDR_SET_SWAP32(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & (uint64_t)0x00000000ff000000) >> 24); \
        ((char *)dst)[1] = (((val) & (uint64_t)0x0000000000ff0000) >> 16); \
        ((char *)dst)[2] = (((val) & (uint64_t)0x000000000000ff00) >> 8); \
        ((char *)dst)[3] = (((val) & (uint64_t)0x00000000000000ff)); \
    } while (0)
		
		
#define TDR_GET_SWAP32(dst) \
	((((unsigned char *)dst)[3])\
	 |((uint32_t)(((unsigned char *)dst)[2]) << 8)\
	 |((uint32_t)(((unsigned char *)dst)[1]) << 16)\
	 |((uint32_t)(((unsigned char *)dst)[0]) << 24));

#define TDR_CAST_SWAP16(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[1]; \
        ((char *)dst)[1] = ((const char *)src)[0]; \
    } while (0)
		
#define TDR_SET_SWAP16(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & (uint64_t)0x000000000000ff00) >> 8); \
        ((char *)dst)[1] = (((val) & (uint64_t)0x00000000000000ff)); \
    } while (0)
		
#define TDR_GET_SWAP16(dst) \
	((((unsigned char *)dst)[1])\
	 |((uint16_t)(((unsigned char *)dst)[0]) << 8));

#define TDR_CAST64(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[0]; \
        ((char *)dst)[1] = ((const char *)src)[1]; \
        ((char *)dst)[2] = ((const char *)src)[2]; \
        ((char *)dst)[3] = ((const char *)src)[3]; \
        ((char *)dst)[4] = ((const char *)src)[4]; \
        ((char *)dst)[5] = ((const char *)src)[5]; \
        ((char *)dst)[6] = ((const char *)src)[6]; \
        ((char *)dst)[7] = ((const char *)src)[7]; \
    } while (0)

#define TDR_SET64(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & (uint64_t)0x00000000000000ff))); \
        ((char *)dst)[1] = (((val) & (uint64_t)0x000000000000ff00) >> 8); \
        ((char *)dst)[2] = (((val) & (uint64_t)0x0000000000ff0000) >> 16); \
        ((char *)dst)[3] = (((val) & (uint64_t)0x00000000ff000000) >> 24); \
        ((char *)dst)[4] = (((val) & (uint64_t)0x000000ff00000000LL) >> 32); \
        ((char *)dst)[5] = (((val) & (uint64_t)0x0000ff0000000000LL) >> 40); \
        ((char *)dst)[6] = (((val) & (uint64_t)0x00ff000000000000LL) >> 48); \
        ((char *)dst)[7] = (((val) & (uint64_t)0xff00000000000000LL) >> 56); \
    } while (0)

#define TDR_GET64(dst) \
	((((unsigned char *)dst)[0])\
	 |((uint64_t)(((unsigned char *)dst)[1]) << 8)\
	 |((uint64_t)(((unsigned char *)dst)[2]) << 16)\
	 |((uint64_t)(((unsigned char *)dst)[3]) << 24)\
	 |((uint64_t)(((unsigned char *)dst)[4]) << 32)\
	 |((uint64_t)(((unsigned char *)dst)[5]) << 40)\
	 |((uint64_t)(((unsigned char *)dst)[6]) << 48)\
	 |((uint64_t)(((unsigned char *)dst)[7]) << 56));
		
#define TDR_CAST32(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[0]; \
        ((char *)dst)[1] = ((const char *)src)[1]; \
        ((char *)dst)[2] = ((const char *)src)[2]; \
        ((char *)dst)[3] = ((const char *)src)[3]; \
    } while (0)

#define TDR_SET32(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & 0x000000ff))); \
        ((char *)dst)[1] = (((val) & 0x0000ff00) >> 8); \
        ((char *)dst)[2] = (((val) & 0x00ff0000) >> 16); \
        ((char *)dst)[3] = (((val) & 0xff000000) >> 24); \
    } while (0)
		
#define TDR_GET32(dst) \
	((((unsigned char *)dst)[0])\
	 |((uint32_t)(((unsigned char *)dst)[1]) << 8)\
	 |((uint32_t)(((unsigned char *)dst)[2]) << 16)\
	 |((uint32_t)(((unsigned char *)dst)[3]) << 24));

#define TDR_CAST16(dst, src) \
    do { \
        ((char *)dst)[0] = ((const char *)src)[0]; \
        ((char *)dst)[1] = ((const char *)src)[1]; \
    } while (0)

#define TDR_SET16(dst, val) \
    do { \
        ((char *)dst)[0] = (((val) & 0x00ff))); \
        ((char *)dst)[1] = (((val) & 0xff00) >> 8); \
    } while (0)

#define TDR_GET16(dst) \
	((((unsigned char *)dst)[0])\
	 |((uint16_t)(((unsigned char *)dst)[1]) << 8));
		
#ifdef TDR_OS_LITTLEENDIAN
    
    #define tdr_cast_ntoh64(dst, src)  TDR_CAST_SWAP64(dst, src)
    #define tdr_cast_hton64(dst, src)  TDR_CAST_SWAP64(dst, src)
    #define tdr_cast_ntoh32(dst, src)  TDR_CAST_SWAP32(dst, src)
    #define tdr_cast_hton32(dst, src)  TDR_CAST_SWAP32(dst, src)
    #define tdr_cast_ntoh16(dst, src)  TDR_CAST_SWAP16(dst, src)
    #define tdr_cast_hton16(dst, src)  TDR_CAST_SWAP16(dst, src)

	#define tdr_get_ntoh64(dst)  TDR_GET_SWAP64(dst)
    #define tdr_set_hton64(dst, src)  TDR_SET_SWAP64(dst, src)
    #define tdr_get_ntoh32(dst)  TDR_GET_SWAP32(dst)
    #define tdr_set_hton32(dst, src)  TDR_SET_SWAP32(dst, src)
    #define tdr_get_ntoh16(dst)  TDR_GET_SWAP16(dst)
    #define tdr_set_hton16(dst, src)  TDR_SET_SWAP16(dst, src)
#else
    
    #define tdr_cast_ntoh64(dst, src)  TDR_CAST64(dst, src)
    #define tdr_cast_hton64(dst, src)  TDR_CAST64(dst, src)
    #define tdr_cast_ntoh32(dst, src)  TDR_CAST32(dst, src)
    #define tdr_cast_hton32(dst, src)  TDR_CAST32(dst, src)
    #define tdr_cast_ntoh16(dst, src)  TDR_CAST16(dst, src)
    #define tdr_cast_hton16(dst, src)  TDR_CAST16(dst, src)

    #define tdr_get_ntoh64(dst)  TDR_GET64(dst)
    #define tdr_set_hton64(dst, src)  TDR_SET64(dst, src)
    #define tdr_get_ntoh32(ds)  TDR_GET32(ds)
    #define tdr_set_hton32(dst, src)  TDR_SET32(dst, src)
    #define tdr_get_ntoh16(dst)  TDR_GET16(dst)
    #define tdr_set_hton16(dst, src)  TDR_SET16(dst, src)

#endif



#define TDR_SET_INT_NET(p, iSize, i)		switch(iSize)		      \
{									      \
    case 2:								      \
    tdr_set_hton16(p, (unsigned short)(i));   \
    break;							      \
    case 4:								      \
    tdr_set_hton32(p, (uint32_t)(i));    \
    break;							      \
    case 8:									\
    tdr_set_hton64(p, (uint64_t)(i));    \
    break;								\
    default:							      \
    *(unsigned char*)(p)	=	(unsigned char)(i);	      \
}


#define TDR_GET_INT_NET(i, iSize, p)		switch(iSize)		      \
{									      \
    case 2:								      \
    i = (int)tdr_get_ntoh16(p);			      \
    break;							      \
    case 4:								      \
    i = (int)tdr_get_ntoh32(p);			      \
    break;							      \
    case 8:									\
    i = tdr_get_ntoh64(p);	\
    break;												\
    default:							      \
    i = (int)*(unsigned char*)(p);				      \
}



#define TDR_GET_UINT_NET(i, iSize, p)		switch(iSize)		      \
{									      \
    case 2:								      \
    i = (uint32_t)tdr_get_ntoh16(p);			      \
    break;							      \
    case 4:								      \
    i = (uint32_t)tdr_get_ntoh64(p);			      \
    break;							      \
    case 8:									\
    i = tdr_get_ntoh64(p);	\
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
*@note a_iVersion���벻С�ڱ��ؽṹ�������Ļ����汾(tdr_get_meta_based_version)��
*	���a_iVersionΪ0 ���� ���ڱ��ؽṹ����������߰汾(tdr_get_meta_current_version)����ǿ�ư��սṹ�嵱ǰ��߰汾����
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
