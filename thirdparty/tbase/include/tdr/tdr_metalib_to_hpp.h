/**
*
* @file     tdr_metalib_to_hpp.h 
* @brief    �ڲ�ʹ�õĺ궨��
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



#ifndef TDR_C_HEADER_H
#define TDR_C_HEADER_H

#include <stdio.h>
#include "tdr/tdr_types.h"
#include "tdr/tdr_define.h"
#include "tdr/tdr_external.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup TDR_WORK_FOR_C TDR_C���Խ����ش�����
*@{
*/



/**
 *�����ض�Ԫ��������ת����c����ͷ�ļ��ṹ
 *@param[in] a_fp: ��������������fpp�ļ����;
 *@param[in] a_pstMeta Ҫת����metaԪ��
 *@param[in] a_pstRule ���������Ĺ�����Ϣָ��
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@pre \e a_fp ����ΪNULL
 *@pre \e a_pstMeta ����Ϊ NULL
 *@pre \e a_pstRule ����Ϊ NULL
 */ 
TDR_API int tdr_meta_to_hpp(IN FILE* a_fp, IN LPTDRMETA a_pstMeta,  IN LPTDRHPPRULE a_pstRule);

/**
 *��Ԫ���ݿ��е���Ϣת����c����ͷ�ļ��ṹ
 *@param[in] a_pstLib: Ԫ���ݿ�ָ��
 *@param[in] a_pszHppFile ͷ�ļ����ļ���
 *@param[in] a_pstRule ���������Ĺ�����Ϣָ��
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@pre \e a_pszHppFile ����ΪNULL
 *@pre \e a_pstLib ����Ϊ NULL
 *@pre \e a_pstRule ����Ϊ NULL
 */
TDR_API int tdr_metalib_to_hpp(IN LPTDRMETALIB a_pstLib, IN const char* a_pszHppFile, IN LPTDRHPPRULE a_pstRule);


/**
 *���ض�XML�ļ�ת����c����ͷ�ļ�
 *��XML�ļ��������ĺ��union/struct������Ԫ���ݿ��еĲ��֣�����c���Խṹ����Ԫ���ݿ���û�ж�������ݽ�
 *����������
 *@param[in] a_pstLib: Ԫ���ݿ�ָ��
 *@param[in] a_pszXmlConf �����˺���Զ������͵�XML�ļ����ļ���
 *@param[in]	a_iTagSetVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in] a_pszHppFile ͷ�ļ����ļ���
 *@param[in] a_pstRule ���������Ĺ�����Ϣָ��
 *@param[in] a_fpError ���������Ϣ���ļ����
 * @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@pre \e a_pstLib ����Ϊ NULL
 *@pre \e a_pstRule ����Ϊ NULL
 *@pre \e a_pszXmlConf ����ΪNULL
 *@pre \e a_pszHppFile ����ΪNULL
 */
TDR_API int tdr_metalib_to_hpp_spec(IN LPTDRMETALIB a_pstLib, IN const char* a_pszXmlConf, IN int a_iTagSetVersion,
                        IN const char* a_pszHppFile, IN LPTDRHPPRULE a_pstRule, IN FILE* a_fpError);

/**
*��Ԫ���ݿ��е���Ϣת����.c�ļ�
*���ӿ�ʵ�ֵĻ���˼����:
*	- ��Ԫ���ݿ����Ϣд��.c�ļ���һ��ȫ�־�̬�޷����ַ������У�
*	- ����Ԫ���ݿ���ַ����ݵ����������ǣ�"g_szMetalib_" + Ԫ���ݿ�����֡�����ԭʼ���ݿ������Ϊ"net", 
*		Ԫ���ݿⱣ����"g_szMetalib_net"�����У�
*	- ��Ӧ�ó���Ҫʹ��Ԫ���ݿ�ʱ��ֻ�뽫��.c�ļ����ӵ������У�Ȼ�󽫴˾�̬����ǿ������ת����LPTDRMETALIB����ʹ��
*@param[in] a_pstLib: Ԫ���ݿ�ָ��
*@param[in] a_pszCFile .c�ļ����ļ���
*
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*
*@pre \e a_pstLib ����Ϊ NULL
*@pre \e a_pstRule ����Ϊ NULL
*/
TDR_API int tdr_metalib_to_cfile(IN LPTDRMETALIB a_pstLib, IN const char* a_pszCFile);


/** @}*/ /* defgroup TDR_WORK_FOR_C TDR_C���Խ����ش�����*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TDR_C_HEADER_H */
