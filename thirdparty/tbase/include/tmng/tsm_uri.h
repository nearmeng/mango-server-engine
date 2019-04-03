#ifndef _TSM_URI_H_
#define _TSM_URI_H_
//============================================================================
// @Id:       $Id: tsm_uri.h 45743 2013-03-07 07:27:25Z laynehu $
// @Author:   $Author: laynehu $
// @Date:     $Date:: 2013-03-07 15:27:25 +0800 #$
// @Revision: $Revision: 45743 $
// @HeadURL:  $HeadURL:: http://tc-svn.tencent.com/ied/ied_tsf4g_rep/tsf4g_p#$
//----------------------------------------------------------------------------

/*!
 * \file tsm_uri.h
 * \brief tsm �ڲ� URI���� API
 * \version 1.0
 * \date 2010-10-11
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>

/*!
 * \fn                  void *tsm_uri_alloc(const char *a_pszUri);
 * \brief               ���ַ����� uri������һ�� uri�ṹ
 * \param   a_pszUri    �������� URI�ַ�����ʾ
 * \return              ����һ�� URI�ṹ�����Ƕ��û�͸���������� void *
 * \n
 *          ʧ��ʱ���� NULL ���������ô����ջ
 */
void *tsm_uri_alloc(const char *a_pszUri);

/*!
 * \fn                  const char *tsm_uri_str(void *a_pstUri);
 * \brief               �� uri�ṹ��ת�����ַ����� uri
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              ���� URI���ַ�����ʾ
 * \n
 *          a_pstUriΪ NULL ʱ������ NULL
 *          ����������������
 */
const char *tsm_uri_str(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_set_bid(void *a_pstUri,int a_iBid);
 * \brief               ���� uri�ṹ�е� bissiness id
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 *          a_iBid      ϣ�����õ��ṹ�е� bissiness id
 * \return              �ɹ����� 0
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_set_bid(void *a_pstUri,int a_iBid);

/*!
 * \fn                  int tsm_uri_std(void *a_pstUri);
 * \brief               ��� uri�ṹ���� meta ��ص����ԣ����ɱ�׼���� URI
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ����� 0
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -3      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_std(void *a_pstUri);

/*!
 * \fn                  void tsm_uri_free(void *a_pstUri);
 * \brief               �ͷ� uri�ṹ
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              ��
 */
void tsm_uri_free(void *a_pstUri);


//----------------------------------------------------------------------------
//  GET functions
//----------------------------------------------------------------------------

/*!
 * \fn                  int tsm_uri_get_type(void *a_pstUri);
 * \brief               ��� URI������
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ���������ֵ
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_type(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_get_bid(void *a_pstUri);
 * \brief               ��� URI�� bissiness id
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� bissiness id
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_bid(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_get_size(void *a_pstUri);
 * \brief               ��� URI�ĳߴ�
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� URI�ĳߴ�
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_size(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_libname(void *a_pstUri);
 * \brief               ��� META URI �Ŀ���
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ؿ���
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_libname(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_get_lib_ver(void *a_pstUri);
 * \brief               ��� META URI �Ŀ�汾��
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ؿ�汾��
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_lib_ver(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_get_lib_build_ver(void *a_pstUri);
 * \brief               ��� META URI �Ŀ⹹���汾��
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ؿ⹹���汾��
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_lib_build_ver(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_metaname(void *a_pstUri);
 * \brief               ��� META URI �� META ��
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� META ��
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_metaname(void *a_pstUri);

/*!
 * \fn                  int tsm_uri_get_meta_ver(void *a_pstUri);
 * \brief               ��� META URI �� META �汾��
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� META �汾��
 * \n
 *          ʧ��ʱ����
 *          \li -1      a_pstUriΪ��
 *          \li -2      a_pstUri�б�ʾ���͵��ֶηǷ�
 */
int tsm_uri_get_meta_ver(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_other(void *a_pstUri);
 * \brief               ��� META URI �ĸ����ֶ�
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ظ����ֶ�
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_other(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_prog(void *a_pstUri);
 * \brief               ��� DATA URI �Ľ�����
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ؽ�����
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_prog(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_id(void *a_pstUri);
 * \brief               ��� DATA URI �Ľ��� ID
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ؽ��� ID
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_id(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_func(void *a_pstUri);
 * \brief               ��� DATA URI �Ĺ�����
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���ع�����
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_func(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_value(void *a_pstUri);
 * \brief               ��� DATA URI �� value�ֶ�
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� value�ֶ�
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_value(void *a_pstUri);

/*!
 * \fn                  const char *tsm_uri_get_path(void *a_pstUri);
 * \brief               ��� DATA URI �� path �ֶ�
 * \param   a_pstUri    �� tsm_uri_alloc ���صģ���ʾ URI�ṹ��ָ��
 * \return              �ɹ�ʱ���� path �ֶ�
 * \n
 *          ʧ��ʱ����
 *          \li NULL    a_pstUriΪ�� �� a_pstUri�б�ʾ���͵��ֶηǷ�
 */
const char *tsm_uri_get_path(void *a_pstUri);


#ifdef __cplusplus
}
#endif


//----------------------------------------------------------------------------
// THE END
//============================================================================
#endif
