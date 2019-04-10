/**
*
* @file     tdr_XMLMetaLib.h
* @brief    Ԫ������������XML�����໥ת��
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


#ifndef TDR_XML_METALIB_H
#define TDR_XML_METALIB_H


#include <stdio.h>
#include "tdr_types.h"
#include "tdr_define.h"
#include "tdr/tdr_external.h"

/** @defgroup TDR_XMLMETALIB   TDR_Ԫ������������XML�����໥ת��
* @{
*/ 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */





/**
 *����XML��Ϣ�������е���Ϣ����Ԫ����������
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_ppstLib ����XML�ļ����ɵ�Ԫ���ݿ�
 *@param[in]	a_pszXml ����XML��Ϣ�Ļ�������ַ
 *@param[in]	a_iXml ����XML��Ϣ�Ļ�������С
 *@param[in]	a_iTagSetVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@note Ԫ����������Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
 * @note ��XML��Ϣ�а�����"tagsetversion"������������ֵ��ΪԪ����������ǩ���İ汾�������Բ���a_iXMLTagVersionΪ׼
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 *
 * @pre \e a_ppstLib ����Ϊ NULL
 * @pre \e a_pszXml ����Ϊ NULL
 * @pre \e a_iXml    �������0
 * @see	tdr_free_lib
 */
TDR_API int  tdr_create_lib(INOUT LPTDRMETALIB *a_ppstLib, IN const char* a_pszXml, IN int a_iXml, IN int a_iTagSetVersion, IN FILE* a_fpError);


/**
 *���õ���XML�ļ�����Ԫ���ݿ�
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_ppstLib ����XML�ļ����ɵ�Ԫ���ݿ�
 *@param[in]	a_fpXML ����XML��Ϣ���ļ�ָ��
 *@param[in]	a_iTagSetVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
 * @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 *
 *@pre \e a_ppstLib ����ΪNULL
 * @pre \e a_fpXML ����Ϊ NULL
 * @see	tdr_free_lib
 */
TDR_API int tdr_create_lib_fp(INOUT LPTDRMETALIB *a_ppstLib, IN FILE* a_fpXML, IN int a_iTagSetVersion, IN FILE* a_fpError);




/**
 *���õ���XML�ļ�����Ԫ���ݿ�
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_ppstLib ����XML�ļ����ɵ�Ԫ���ݿ�
 *@param[in]	a_pszFile ����XML��Ϣ���ļ���
 *@param[in]	a_iXMLTagVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
* @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 *
 * @pre \e a_ppstLib ����Ϊ NULL
 * @pre \e a_pszFile ����Ϊ NULL
 * @see	tdr_free_lib
 */
TDR_API int tdr_create_lib_file(INOUT LPTDRMETALIB *a_ppstLib, IN const char* a_pszFile, IN int a_iXMLTagVersion, IN FILE* a_fpError);


/**
 *���ö��XML�ļ�����Ԫ���ݿ�
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_ppstLib ����XML�ļ����ɵ�Ԫ���ݿ�
 *@param[in]	a_aszFile ����XML��Ϣ���ļ�������
 *@param[in] a_iFile xml�ļ�����
 *@param[in]	a_iXMLTagVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
* @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 * @note ���ܽ���ͬ���ֿռ��Ԫ���������ӵ�ͬһ��Ԫ���ݿ���, ����XML�ļ��и�Ԫ��
 * ��name����ֵ������ͬ
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 * @note �ӵ�ͬһ�����е�XMLԪ���������ļ���Ԫ�ص�ID����ֵ������ͬ
 *
 * @pre \e a_ppstLib ����Ϊ NULL
 * @pre \e a_aszFile ����Ϊ NULL
 * @pre \e a_iFile    �������0��������ָ��һ���ļ�
 * @see	tdr_free_lib
 */
TDR_API int tdr_create_lib_multifile(INOUT LPTDRMETALIB *a_ppstLib, IN const char* a_aszFile[], IN int a_iFile, IN int a_iXMLTagVersion, IN FILE* a_fpError);

/**
 *�����е�Ԫ����Lib������µ�Ԫ���ݶ�������
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_pstLib ����Ԫ���������Ŀ�
 *@param[in]	a_pszXml ����XML��Ϣ�Ļ�������ַ
 *@param[in]	a_iXml ����XML��Ϣ�Ļ�������С
 *@param[in]	a_iXMLTagVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
 * @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 * @note ���ܽ���ͬ���ֿռ��Ԫ���������ӵ�ͬһ��Ԫ���ݿ���, ����XML�ļ��и�Ԫ��
 * ��name����ֵ������ͬ
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 * @note �ӵ�ͬһ�����е�XMLԪ���������ļ���Ԫ�ص�ID����ֵ������ͬ
 *
 * @pre \e a_pstLib ����Ϊ NULL, �ұ����ǵ��� tdr_init_lib ���д����ģ��Ա�֤
 * pstLib�������㹻�Ŀռ�������µĶ���
 * @pre \e a_pszXml ����Ϊ NULL
 * @pre \e a_iXml    �������0��
 * @see	tdr_init_lib
 */
TDR_API int tdr_add_meta(INOUT LPTDRMETALIB a_pstLib, IN const char* a_pszXml, IN int a_iXml, IN int a_iXMLTagVersion, IN FILE* a_fpError);


/**
 *�����е�Ԫ����Lib������µ�Ԫ���ݶ�������
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[in,out]	a_pstLib ����Ԫ���������Ŀ�
 *@param[in]	a_pszFileName ����XML��Ϣ���ļ�
  *@param[in]	a_iXMLTagVersion ָ��Ԫ��������XML��ǩ���弯�İ汾
 *@param[in]	a_fpError	�������������Ϣ���ļ����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
 * @note XML��Ϣ�а�����"tagsetversion"��������ֵ��Ϊ��ǩ���İ汾�����û��
 * ����"tagsetversion"��Ϣ�����Բ���a_iXMLTagVersionΪ׼
 * @note ���ܽ���ͬ���ֿռ��Ԫ���������ӵ�ͬһ��Ԫ���ݿ���, ����XML�ļ��и�Ԫ��
 * ��name����ֵ������ͬ
 * @note Ԫ����������XMLԪ�����ĸ�Ԫ�ر������ name���ԣ���ȷ��Ԫ�������������ֿռ�
 * @note �ӵ�ͬһ�����е�XMLԪ���������ļ���Ԫ�ص�ID����ֵ������ͬ
 *
 * @pre \e a_pstLib ����Ϊ NULL, �ұ����ǵ��� tdr_init_lib ���д����ģ��Ա�֤
 * pstLib�������㹻�Ŀռ�������µĶ���
 * @pre \e a_pszFileName ����Ϊ NULL
 * @see	tdr_init_lib
 */
TDR_API int tdr_add_meta_file(INOUT LPTDRMETALIB a_pstLib, IN const char* a_pszFileName, IN int a_iXMLTagVersion, IN FILE* a_fpError);


/**
 *��Ԫ���ݿ�ת����XML������Ϣ�������浽�ڴ滺�����С�
 *@param[in] a_pstLib ����XML��Ϣ���ɵ�Ԫ���ݿ�
 *@param[in]	a_pszXML ����XML��Ϣ�Ļ�������ַ; 
 *@param[in,out]	a_iXML XML��Ϣ�Ĵ�С
 *  -���룺 pszXml��ָ�������Ĵ�С��
 *  -���:  ���ɵĵ�XML��Ϣ�Ĵ�С
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 *@pre \e a_pstLib ����ΪNULL
 * @pre \e a_pszXML ����Ϊ NULL
 * @pre \e a_iXML ����Ϊ NULL,������ָ����ֵ���������
 */
TDR_API int tdr_save_xml(IN LPTDRMETALIB a_pstLib, OUT char *a_pszXML, INOUT int *a_iXML);


/**
 *��Ԫ���ݿ�ת����XML������Ϣ�������浽�ļ��С�
 *@param[in] a_pstLib ����XML��Ϣ���ɵ�Ԫ���ݿ�
 *@param[out]	a_fp ����XML��Ϣ���ļ�ָ��; 
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @pre \e a_pstLib ����Ϊ NULL
 * @pre \e a_fp ����Ϊ NULL
 */
TDR_API int tdr_save_xml_fp(IN LPTDRMETALIB a_pstLib, OUT FILE* a_fp);


/**
 *��Ԫ���ݿ�ת����XML������Ϣ�������浽�ļ��С�
 *@param[in] a_pstLib ����XML��Ϣ���ɵ�Ԫ���ݿ�
 *@param[in]	a_pszXmlFile ����XML��Ϣ���ļ����ļ���; 
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *@retval >0  ����ɹ���������ĳЩ�������
 *
 * @pre \e a_pstLib ����Ϊ NULL
 * @pre \e a_pszXmlFile ����Ϊ NULL
 *	
 */
TDR_API int tdr_save_xml_file(IN LPTDRMETALIB a_pstLib, IN const char* a_pszXmlFile);


#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */ //   Ԫ������������XML�����໥ת��


#endif  /* TDR_XML_METALIB_H */
