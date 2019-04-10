/**
*
* @file     tdr_metalib_init.h
* @brief    Ԫ���ݿ���Դ����
* : ��ʼ�����ͷ���Դ,��ȡ��Դ��С
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


#ifndef TDR_XML_METALIB_INIT_H
#define TDR_XML_METALIB_INIT_H


#include "tdr_types.h"
#include "tdr_define.h"
#include "tdr/tdr_external.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup TDR_METALIB_MNG TDR_Ԫ���ݿ�(metalib)����
 *  @{
 */


/**
 *���ݲ�����ʼ��Ԫ���ݿ�
  *�ڴ�������У���������﷨���������������⣬����fpError����ļ���������һЩ��ϸ����Ϣ
 *@param[out]	pstLib ���ݲ�����ʼ����Ԫ���ݿ�
 *@param[in]	pstParam ��ʼ��Ԫ���ݿ�Ĳ�����
 *
 *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
 *@retval 0   ����ɹ�
 *
 *@note Ԫ���ݿ�TDRMETALIB�Ŀռ��ں����ڷ��䣬�����tdr_free_lib�ͷſռ�
 *
 * @pre \e pstLib ����Ϊ NULL
 * @pre \e pstParam ����Ϊ NULL
 * @pre \e pstParam��ָ���ĺ�Ԫ�غ��Զ�������Ԫ�ص���Ŀ�������0��
 *pstParam->iMetaSize�������0�����Զ�������Ԫ�صĿռ�������0
 * @see	tdr_free_lib
 * @see	LPTDRLIBPARAM
 */
TDR_API int tdr_init_lib(OUT LPTDRMETALIB pstLib, IN const LPTDRLIBPARAM pstParam);

/**
 *�ͷ�Ԫ���ݿ����Դ������ָ��Ԫ�����ָ����ΪNULL��
 */
TDR_API void tdr_free_lib(INOUT LPTDRMETALIB* ppstLib);

/**
*��Ԫ���������⸴�Ƶ�ָ����������
*@param[out] a_pvDst ���渴��Ԫ����������Ļ�������ַ
*@param[in,out] a_piBuff ���뻺�����Ĵ�С�� ������ƺ�Ԫ���ݿ�Ĵ�С
*@param[in] a_pstSrcLib ������Ԫ���ݿ��ָ��
*
*@return ���Ƴɹ����ظ���Ԫ���ݿ��ָ�룬���򷵻�NULL
*@note ���ָ���������Ĵ�С��ʵ����Ҫ��ҪС,���ƻ�ʧ��
* @pre \e a_pvDst ����Ϊ NULL
* @pre \e a_piBuff ����ΪNULL��������ָ�������Ĵ�С�������0
* @pre \e a_pstSrcLib ����ΪNULL
*/
TDR_API LPTDRMETALIB tdr_clone_lib(OUT void* a_pvDst, INOUT int* a_piBuff, IN LPTDRMETALIB a_pstSrcLib);


/**
*��ȡԪ������ռ�ڴ��ֽ���
*@return success:Ԫ������ռ�ڴ��ֽ��� failed: 0
* @pre \e ppstLib ����Ϊ NULL
*/
TDR_API size_t tdr_size(IN LPTDRMETALIB pstLib);

/**��ȡԪ���������������
*@param[in] a_pstLib Ԫ�����������ָ��
*
*@return success:Ԫ����������������ַ�����ָ��  failed: ""
*
* @pre \e a_pstLib ����Ϊ NULL
*/
TDR_API const char *tdr_get_metalib_name(IN LPTDRMETALIB a_pstLib);

/**��ȡԪ����������İ汾
*@param[in] a_pstLib Ԫ�����������ָ��
*
*@return success: Ԫ����������İ汾 failed: -1
*
* @pre \e a_pstLib ����Ϊ NULL
*/
TDR_API int tdr_get_metalib_version(IN LPTDRMETALIB a_pstLib);

/**��ȡԪ����������Ĺ����汾
*@param[in] a_pstLib Ԫ�����������ָ��
*
*@return cuccess: Ԫ����������Ĺ����汾 failed : -1
*
* @pre \e a_pstLib ����Ϊ NULL
*/
TDR_API int tdr_get_metalib_build_version(IN LPTDRMETALIB a_pstLib);

/** @}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif  /* TDR_XML_METALIB_INIT_H */

