/**
 *
 * @file     tdr_data_io.h
 * @brief    TDRԪ���ݿ�IO����ģ��
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


#ifndef TDR_DATA_IO_H
#define TDR_DATA_IO_H

#include <stdio.h>
#include "tdr/tdr_external.h"
#include "tdr/tdr_types.h"
#include "tdr/tdr_define.h"
#include "tdr/tdr_operate_data.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /** XML�ļ���������ṹ�����Ķ���
    */

    struct tagTDRInOutputSrc
    {
        const char* szFileName;
        FILE* fp;
        TDRDATA stXmlBuf;
    };
    typedef struct tagTDRInOutputSrc TDRINOUTPUTSRC;

    struct tagTDRIOWrap
    {
        LPTDRMETA pstMeta;
        TDRINOUTPUTSRC stInOutputSrc;
        TDRDATA stHost;
        int iCutOffVer;
        int iXmlDataFormat;
        TDRBOOLEAN bEsc;    /** �Ƿ�ת�崦�������ַ�(ע�⣺����/���֧�ֵ�ת���ַ�����ͬ) */
    };

    typedef struct tagTDRIOWrap TDRIOWRAP, *LPTDRIOWRAP;


    /** @defgroup  TDR_METALIB_IO TDR_���ļ�/�ļ����б���/�ָ�Ԫ���ݿ�
     * @{
     */

    /**��Ԫ�������������Ƹ�ʽ�ļ��м���Ԫ���ݿ�,��У����ɢ��ֵ�Ƿ��������ɢ��ֵ(NULL)һ��
     * @param[in,out] a_ppstLib ����Ԫ���ݿ��ָ���ָ��
     * @param[in] a_pszBinFile ����Ԫ���ݿ�����Ƹ�ʽ���ļ����ļ���
     * @param[in] a_pszHash ���ӻ���ҪУ���Ԫ����������ɢ��ֵ�ַ���ָ��
     *
     * @note Ԫ���ݿ�Ŀռ��ں����ڶ�̬���䣬�����tdr_free_lib�ͷ�Ԫ���ݿ�Ŀռ�
     * @note ����TDR�Ĺ��߽�Ԫ����������ת����.h�ļ�ʱ���ὫԪ���ݿ��ɢ��ֵ�ַ�����Ϊ�궨��д��.h�ļ��У��˺�
     * ��������ֵ���������Ϊ: "TDR_" + "Ԫ������������_" + "HASH"
     * @note ���a_pszHash����ΪNULL���򲻽���ɢ��ֵУ�飬��ʱ�˽ӿں�tdr_load_metalib�ӿڵĹ�����ͬ
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
     *	- TDR_ERROR_FAILED_OPEN_FILE_TO_READ	���ļ���ʧ��
     *	- TDR_ERROR_INVALID_METALIB_FILE	Ԫ���ݿ��ļ����ֽ���Ϊ0�����ȡ���ֽ�����Ԫ����������ṹ�м�¼���ֽ�����ͬ
     *	- TDR_ERROR_BUILD_VERSION_CONFLICT	TDR����Ԫ���ݿ��ļ��Ĺ��ߵĹ����汾�ͼ��ع��߲�һ��
     *	- TDR_ERROR_DIFF_METALIB_HASH	Ԫ���ݿ��ɢ��ֵ��������ɢ��ֵ��һ��
     *
     * @pre \e a_ppstLib ����ΪNULL
     * @pre \e a_pszBinFile ����Ϊ NULL
     * @see tdr_free_lib
     */
    TDR_API int tdr_load_verify_metalib(INOUT LPTDRMETALIB* a_ppstLib, IN const char* a_pszBinFile, IN const char *a_pszHash);

    /**У��metalib��ɢ��ֵ�Ƿ��������ɢ��ֵһ��
     * @param[in,out] a_pstLib ����Ԫ���ݿ��ָ��
     * @param[in] a_pszHash ���ӻ���ҪУ���Ԫ����������ɢ��ֵ�ַ���ָ��
     *
     * @note ����TDR�Ĺ��߽�Ԫ����������ת����.h�ļ�ʱ���ὫԪ���ݿ��ɢ��ֵ�ַ�����Ϊ�궨��д��.h�ļ��У��˺�
     * ��������ֵ���������Ϊ: "TDR_" + "Ԫ������������_" + "HASH"
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
     *	- TDR_ERROR_DIFF_METALIB_HASH	Ԫ���ݿ��ɢ��ֵ��������ɢ��ֵ��һ��
     *
     * @pre \e a_ppstLib ����ΪNULL
     * @pre \e a_pszHash ����ΪNULL
     */
    TDR_API int tdr_verify_metalib(IN LPTDRMETALIB a_pstLib, IN const char *a_pszHash);


    /**��Ԫ�������������Ƹ�ʽ�ļ��м���Ԫ���ݿ�
     * @param[in,out] a_ppstLib ����Ԫ���ݿ��ָ���ָ��
     * @param[in] a_pszBinFile ����Ԫ���ݿ�����Ƹ�ʽ���ļ����ļ���
     * @note Ԫ���ݿ�Ŀռ��ں����ڶ�̬���䣬�����tdr_free_lib�ͷ�Ԫ���ݿ�Ŀռ�
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
     *	- TDR_ERROR_FAILED_OPEN_FILE_TO_READ	���ļ���ʧ��
     *	- TDR_ERROR_INVALID_METALIB_FILE	Ԫ���ݿ��ļ����ֽ���Ϊ0�����ȡ���ֽ�����Ԫ����������ṹ�м�¼���ֽ�����ͬ
     *	- TDR_ERROR_BUILD_VERSION_CONFLICT	TDR����Ԫ���ݿ��ļ��Ĺ��ߵĹ����汾�ͼ��ع��߲�һ��
     *
     * @pre \e a_ppstLib ����ΪNULL
     * @pre \e a_pszBinFile ����Ϊ NULL
     * @see tdr_free_lib
     */
    TDR_API int tdr_load_metalib(INOUT LPTDRMETALIB* a_ppstLib, IN const char* a_pszBinFile);

    /**��Ԫ�������������Ƹ�ʽ�м���Ԫ���ݿ�
     * @param[in,out] a_ppstLib ����Ԫ���ݿ��ָ���ָ��
     * @param[in] a_fpBin ����Ԫ���ݿ��Ԫ�������������Ƹ�ʽ���ļ����
     * @note Ԫ���ݿ�Ŀռ��ں����ڶ�̬���䣬�����tdr_free_lib�ͷ�Ԫ���ݿ�Ŀռ�
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
     *	- TDR_ERROR_FAILED_OPEN_FILE_TO_READ	���ļ���ʧ��
     *	- TDR_ERROR_INVALID_METALIB_FILE	Ԫ���ݿ��ļ����ֽ���Ϊ0�����ȡ���ֽ�����Ԫ����������ṹ�м�¼���ֽ�����ͬ
     *	- TDR_ERROR_BUILD_VERSION_CONFLICT	TDR����Ԫ���ݿ��ļ��Ĺ��ߵĹ����汾�ͼ��ع��߲�һ��
     *
     * @pre \e a_ppstLib ����ΪNULL
     * @pre \e a_fpBin ����Ϊ NULL
     * @see tdr_free_lib
     */
    TDR_API int tdr_load_metalib_fp(INOUT LPTDRMETALIB* a_ppstLib, IN FILE* a_fpBin);

    /**��Ԫ���������������м���Ԫ���ݿ�
     * @param[in,out] a_ppstLib ����Ԫ���ݿ��ָ���ָ��
     * @param[in] a_pBuf ����Ԫ���ݿ�Ļ�����
     * @param[in,out] a_ptLen ���뱣��Ԫ���ݿ�Ļ���ĳ��ȣ����ʵ�ʶ�ȡ�ĳ���
     * @note Ԫ���ݿ�Ŀռ��ں����ڶ�̬���䣬�����tdr_free_lib�ͷ�Ԫ���ݿ�Ŀռ�
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������:
     *	- TDR_ERROR_FAILED_OPEN_FILE_TO_READ	���ļ���ʧ��
     *	- TDR_ERROR_INVALID_METALIB_FILE	Ԫ���ݿ��ļ����ֽ���Ϊ0�����ȡ���ֽ�����Ԫ����������ṹ�м�¼���ֽ�����ͬ
     *	- TDR_ERROR_BUILD_VERSION_CONFLICT	TDR����Ԫ���ݿ��ļ��Ĺ��ߵĹ����汾�ͼ��ع��߲�һ��
     *
     * @pre \e a_ppstLib ����ΪNULL
     * @pre \e a_pBuf ����Ϊ NULL
     * @pre \e a_ptLen ����Ϊ NULL
     * @see tdr_free_lib
     */
    TDR_API int tdr_load_metalib_buf(INOUT LPTDRMETALIB* a_ppstLib, IN const char* a_pBuf, IN size_t* a_ptLen);

    /**��Ԫ���ݿⱣ�浽�����Ƹ�ʽ���ļ���
     * @param[in,out] a_pstLib ����Ԫ���ݿ��ָ��
     * @param[in] a_pszBinFile ����Ԫ���ݿ�Ķ����Ƹ�ʽ�ļ����ļ���
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ܵĴ����������:
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     - TDR_ERRIMPLE_FAILED_OPEN_FILE_TO_WRITE failed to open file to write
     - TDR_ERROR_FAILED_TO_WRITE_FILE	failed to write data to file
     *
     * @pre \e a_pstLib ����ΪNULL
     * @pre \e a_pszBinFile ����Ϊ NULL
     */
    TDR_API int tdr_save_metalib(IN LPTDRMETALIB a_pstLib, IN const char* a_pszBinFile);

    /**��Ԫ���ݿⱣ�浽�����Ƹ�ʽ���ļ���
     * @param[in,out] a_pstLib ����Ԫ���ݿ��ָ��
     * @param[in] a_fpBin ����Ԫ���ݿ�Ķ����Ƹ�ʽ���ļ����
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ܵĴ����������:
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     - TDR_ERROR_FAILED_TO_WRITE_FILE	failed to write data to file
     *
     * @pre \e a_pstLib ����ΪNULL
     * @pre \e a_fpBin ����Ϊ NULL
     */
    TDR_API int tdr_save_metalib_fp(IN LPTDRMETALIB a_pstLib, IN FILE* a_fpBin);

    /** @} */ /*  �ļ�/�ļ����б���/�ָ�Ԫ���ݿ�*/

    /** @defgroup  TDR_METALIB_DUMP TDR_Ԫ���ݿ⵼��(����ʱ��)
     * @{
     */

    /**��ӡԪ���ݿ������
     * @param[in] a_pstLib Ҫ�����Ԫ���ݿ�
     * @param[in] a_fp ����ļ����
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ܵĴ����������:
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     *
     * @pre \e a_pstLib ����ΪNULL
     * @pre \e a_fp ����Ϊ NULL
     */
    TDR_API void tdr_dump_metalib(IN LPTDRMETALIB a_pstLib, IN FILE* a_fp);

    /**��ӡԪ���ݿ������
     *�����ݱ�����a_pszFileָ�����ļ���
     * @param[in] a_pstLib Ҫ�����Ԫ���ݿ�
     * @param[in] a_pszFile ����ļ����ִ���ָ��
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     *
     * @pre \e a_pstLib ����ΪNULL
     * @pre \e a_pszFile ����Ϊ NULL
     */
    TDR_API void tdr_dump_metalib_file(IN LPTDRMETALIB a_pstLib, IN const char* a_pszFile);

    /** @} */ /*  TDR_METALIB_DUMP TDR_Ԫ���ݿ⵼��(����ʱ��)*/


    /** @defgroup  TDR_DATA_IO TDR_������XML��ʽ����/���
     * @{
     */

    /**���ñ�tdr�ӿڴ�����ڴ����ݵ��ַ�������"GBK","UTF8"��
     * @param[in] a_pszEncoding ָ���ַ�����Ϣ���ַ����������׵�ַ
     * @note ���ӿ��Ƿ��̰߳�ȫ�ģ�����ڶ��̻߳������ô˽ӿڣ������߱��뱣֤������ñ��ӿ�
     * @note tdrȱʡ��Ϊ���ݵ��ַ���Ϊ"GBK"
     */
    void tdr_set_encoding(IN const char *a_pszEncoding);

    /**��ʼ��tdr_input_ex��tdr_output_ex�ӿ��нṹ��TDRIOWRAP���͵��������
     *
     * @param[in] a_pstIOWrap
     * @see TDRIOWRAP
     * */
    TDR_API void tdr_data_inoutput_init(LPTDRIOWRAP a_pstIOWrap);

    /**
     *����Ԫ������������һ���ڴ����ݽṹ��ʽת����һ�������ض������xml��Ϣ����������ָ����������
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstXml �洢XML��ʽ��Ϣ�Ľṹ
     *	- ����	a_pstXml.pszBuff ָ������XML��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstXml.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstXml.iBuff  ʵ��ת�������Ϣ���ܳ���
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ����ӡ��a_pstXml���ݣ����Ը�����λ�����λ��
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     *		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
     *		- TDR_ERROR_NO_HOSTBUFF_SPACE ���ش洢������ʣ��ռ䲻��
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
     * @see tdr_get_error_detail
     *
     * @pre \e a_pstMeta ����ΪNULL
     * @pre \e a_pstXml ����ΪNULL
     * @pre \e a_pstXml->pszBuff����ΪNULL
     * @pre \e a_pstXml->iBuff �������0
     * @pre \e a_pstHost ����ΪNULL
     * @pre \e a_pstHost->pszBuff����ΪNULL
     * @pre \e a_pstHost->iBuff �������0
     */
    TDR_API int tdr_output(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstXml, IN LPTDRDATA a_pstHost,
                           IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /**
     *����Ԫ������������һ���ڴ����ݽṹ��ʽת����һ�������ض������xml��Ϣ����������ָ���ļ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in]	a_fpXml �洢XML��ʽ��Ϣ���ļ����
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ��������߰汾����
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����

     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴����ļ����ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_fpXml ����Ϊ NULL
     * @pre \e a_pstHost ����ΪNULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     *		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
     *		- TDR_ERROR_NO_HOSTBUFF_SPACE ���ش洢������ʣ��ռ䲻��
     *		- TDR_ERROR_TOO_COMPLIEX_META Ԫ��������������������Ƕ�ײ�γ���32��
     *		- TDR_ERROR_NET_UNSUPPORTED_TYPE    ��֧�ֵ�Ԫ��������
     *		- TDR_ERROR_NET_INVALID_STRING_LEN string���͵�Ԫ�����ַ������ȳ�����Ԥ����󳤶�
     *		- TDR_ERROR_NET_INVALID_VERSIONINDICATOR Ԫ���ݰ汾ָʾ����ֵ�ȱ�����ֵ��Ԫ���ݳ�Ա�İ汾ҪС
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see tdr_output
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_output_fp(IN LPTDRMETA a_pstMeta, IN FILE *a_fpXml, IN LPTDRDATA a_pstHost,
                              IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /**
     *����Ԫ������������һ���ڴ����ݽṹ��ʽת����һ�������ض������xml��Ϣ����������ָ���ļ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in]	a_szFile �洢XML��ʽ��Ϣ���ļ����ļ����ַ���ָ��
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴����ļ����ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����ΪNULL
     * @pre \e a_szFile ����ΪNULL
     * @pre \e a_pstHost ����ΪNULL
     * @pre \e a_pstHost->pszBuff����ΪNULL
     * @pre \e a_pstHost->iBuff ����ΪNULL
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see tdr_output
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_output_file(IN LPTDRMETA a_pstMeta, IN const char *a_szFile, IN LPTDRDATA a_pstHost,
                                IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /**����Ԫ����������"��һ���ڴ����ݽṹ��ʽת����һ�������ض������xml��Ϣ" ��ͳһ�ӿ�
     * ������ tdr_output/tdr_output_fp/tdr_output_file �ӿڵĹ��ܣ�����֧��ת�����ĳЩ�ַ�
     *
     * @param[in] a_pstIOWrap ���ܿ��Ʋ���
     *
     * @note ���øýӿ�֮ǰ�����ȵ��ýӿ�tdr_data_inoutput_init��ʼ��������������߰��������ָ��Ķ��� memset Ϊ 0
     *
     * @note ת�崦�����֧���԰˽��Ƹ�ʽ(\0??)��� �س���('\\r')�����з�('\\n')���Ʊ��('\\t')���ո�(' '),  �� \\\\ ��ʾ ��б��('\\')
     *
     * @note a_pstIOWrap->stInOutputSrc�ĳ�Ա�����ȼ���szFileName > fp > stXmlBuf������ szFileName!=NULL ʱ����szFileNameΪ׼������ fp �� stXmlBuf ��ֵ
     *
     * @note ����ýӿڷ���ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴����ļ�/���������ݣ����Ը�����λ����λ��
     *	- ���� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see tdr_data_inoutput_init
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     * @see TDRIOWRAP
     */
    TDR_API int tdr_output_ex(IN LPTDRIOWRAP a_pstIOWrap);

    /**����Ԫ���������������ض������xml��Ϣת���ɱ����ڴ����ݽṹ��ʽ
     *xml��Ϣ������ָ����������
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstHost.iBuff  �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                 ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
     * @param[in]	a_pstXml �洢XML��ʽ��Ϣ�Ľṹ
     *	- ����	a_pstXml.pszBuff ָ������XML��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstXml.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     * @note ���ӿڲ�֧��versionindicator����
     * @note ���ӿڲ�֧�� ָ������� ����
     *
     * @note ���������ж�������飬������õ�Ԫ�ظ�����������Ĵ�С���ýӿ�ĬĬ���������Ԫ��
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pstXml ����Ϊ NULL
     * @pre \e a_pstXml.pszBuff ����Ϊ NULL
     * @pre \e a_pstXml.iBuff �������0
     * @pre \e a_pstXml ����Ϊ NULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     *	- TDR_ERROR_NO_XML_ROOT	û�к��ʵĸ��ڵ㣬��ȷ��XMLԪ�����а�����Ԫ������Ϊ���Ľڵ�
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see  tdr_output
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_input(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN LPTDRDATA a_pstXml,
                          IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /**����Ԫ���������������ض������xml��Ϣת���ɱ����ڴ����ݽṹ��ʽ
     *xml��Ϣ������ָ���ļ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstHost.iBuff   �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                  ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
     * @param[in]	a_fp �洢XML��ʽ��Ϣ���ļ����
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     * @note ���ӿڲ�֧��versionindicator���ԣ�
     * @note ���ӿڲ�֧�� ָ������� ����
     *
     * @note ���������ж�������飬������õ�Ԫ�ظ�����������Ĵ�С���ýӿ�ĬĬ���������Ԫ��

     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_fp ����Ϊ NULL
     * @pre \e a_psHost ����ΪNULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
     *	- TDR_ERROR_NO_XML_ROOT	û�к��ʵĸ��ڵ㣬��ȷ��XMLԪ�����а�����Ԫ������Ϊ���Ľڵ�
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see  tdr_output
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_input_fp(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN FILE *a_fp,
                             IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /**����Ԫ���������������ض������xml��Ϣת���ɱ����ڴ����ݽṹ��ʽ
     *xml��Ϣ������ָ���ļ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstHost.iBuff   �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                  ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
     * @param[in]	a_pszFile �洢XML��ʽ��Ϣ���ļ����ļ����ַ���ָ��
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_MATA_NAME
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �⼸�ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_MATA_NAME��TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��
             TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ����ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note ����xml�����ļ��ľ����ʽ˵������ο��ĵ�<TSF-G-DR-FAQ.doc>,
             ��Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     * @note ���ӿڲ�֧��versionindicator����
     * @note ���ӿڲ�֧�� ָ������� ����
     *
     * @note ���������ж�������飬������õ�Ԫ�ظ�����������Ĵ�С���ýӿ�ĬĬ���������Ԫ��
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pszFile ����Ϊ NULL
     * @pre \e a_psHost ����ΪNULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
     *	- TDR_ERROR_INVALID_PARAM	������Ч
     *	- TDR_ERROR_NO_XML_ROOT	û�к��ʵĸ��ڵ㣬��ȷ��XMLԪ�����а�����Ԫ������Ϊ���Ľڵ�
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see  tdr_output
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_input_file(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN const char *a_pszFile,
                               IN int a_iCutOffVersion, IN int a_iXmlDataFormat);


    /**����Ԫ��������, "�������ض������xml��Ϣת���ɱ����ڴ����ݽṹ��ʽ" ��ͳһ�ӿڣ�
     * ������ tdr_input/tdr_input_fp/tdr_input_file �ӿڵĹ��ܣ�����֧������ת���ַ�����
     *
     * @param[in] a_pstIOWrap ���ܿ��Ʋ���
     *
     * @note ���øýӿ�֮ǰ�����ȵ��ýӿ�tdr_data_inoutput_init��ʼ��������������߰��������ָ��Ķ��� memset Ϊ 0
     *
     * @note ת�崦�����֧������˽��Ƹ�ʽ��ת���ַ����С�����, \\012 ��ʾ �س���('\\n'), \\\\ ��ʾ ��б��('\\')
     *
     * @note a_pstIOWrap->stInOutputSrc�ĳ�Ա�����ȼ���szFileName > fp > stXmlBuf������ szFileName!=NULL ʱ����szFileNameΪ׼������ fp �� stXmlBuf ��ֵ
     *
     * @note ���������ж�������飬������õ�Ԫ�ظ�����������Ĵ�С���ýӿ�ĬĬ���������Ԫ��
     *
     * @note ����ýӿڷ���ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *  - ���� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see tdr_data_inoutput_init
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     * @see TDRIOWRAP
     *
     */
    TDR_API int tdr_input_ex(IN LPTDRIOWRAP a_pstIOWrap);


    /** @}*/ /*defgroup  TDR_DATA_IO TDR_������XML��ʽ����/��� */


    /** @defgroup  TDR_DATA_IO_JSON TDR_������JSON��ʽ����/���
     * @{
     */

    /**��JSON��ʽ����ת����C���Զ������ڴ�����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_pstHost �ڴ����ݻ�����
     * @param[in]       a_pstJson ���뻺����, ָ��JSON����
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     *
     * @note ���������JSON��ʽ���ݣ����ӿ�������Ҫ��
     * - ����keyʹ��""����, ���������δ��""���õ�key
     * - ��������ֿ�����[]
     * - ��������� null  ֵ
     * - ��������� true  ֵ
     * - ��������� false ֵ
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note ָ��/�������͵�entry������
     * @note ����ָ����refer���Ե�entry, ���entry����Ӧ��JSON���������refer��ֵ�����������Ƿ���ȷ
     * @note ����ָ����refer���Ե�entry, ���entryû����Ӧ��JSON������򱾽ӿڲ�������ݵ���Ч��
     */
    TDR_API int tdr_input_json(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost,
                               IN LPTDRDATA a_pstJson, IN int a_iCutOffVersion);


   /**ʹ��rapidjson���JSON��ʽ����ת����C���Զ������ڴ�����
    * ����rapidjson��������ԭ�򣬱��ӿڲ�֧��windows VC6�汾
	* @param[in]	   a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
	* @param[in,out]   a_pstHost �ڴ����ݻ�����,�����a_pstHost->iBuff������ڴ����ַ���ʵ��ռ�õ��ֽ����������ַ�������(strlen���)��1(�ַ���������־)
	* @param[in]	   a_pstJson ���뻺����, ָ��JSON����
	* @param[in]	   a_iCutOffVersion ���ݳ�Ա���ð汾
	*
	* @pre a_pstMeta ����ΪNULL
	* @pre a_pstHost ����ΪNULL
	* @pre a_pstJson ����ΪNULL
	*
	* @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
	* - TDR float/double						  ��Ӧ JSON float
	* - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
	* - TDR wchar �������������͡�				  ��Ӧ JSON integer
	*
	* @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
	*
	* @note ���������JSON��ʽ���ݣ����ӿ�������Ҫ��
	* - ����keyʹ��""����, ���������δ��""���õ�key
	* - ��������ֿ�����[]
	* - ��������� null  ֵ
	* - ��������� true  ֵ
	* - ��������� false ֵ
	*
	* @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
	* @note ָ��/�������͵�entry������
	* @note ����ָ����refer���Ե�entry, ���entry����Ӧ��JSON���������refer��ֵ�����������Ƿ���ȷ
	* @note ����ָ����refer���Ե�entry, ���entryû����Ӧ��JSON������򱾽ӿڲ�������ݵ���Ч��
     */
	TDR_API int tdr_input_json_ex(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost,
                   IN LPTDRDATA a_pstJson, IN int a_iCutOffVersion);



    /**��JSON��ʽ����ת����C���Զ������ڴ�����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_pstHost �ڴ����ݻ�����
     * @param[in]       a_fpJson  JSON�����ļ�ָ��
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     *
     * @note ���������JSON��ʽ���ݣ����ӿ�������Ҫ��
     * - ����keyʹ��""����, ���������δ��""���õ�key
     * - ��������ֿ�����[]
     * - ��������� null  ֵ
     * - ��������� true  ֵ
     * - ��������� false ֵ
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note ָ��/�������͵�entry������
     * @note ����ָ����refer���Ե�entry, ���entry����Ӧ��JSON���������refer��ֵ�����������Ƿ���ȷ
     * @note ����ָ����refer���Ե�entry, ���entryû����Ӧ��JSON������򱾽ӿڲ�������ݵ���Ч��
     */
    TDR_API int tdr_input_json_fp(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost,
                                  IN FILE *a_fpJson, IN int a_iCutOffVersion);

    /**��JSON��ʽ����ת����C���Զ������ڴ�����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_pstHost �ڴ����ݻ�����
     * @param[in]       a_pszJsonFile JSON�����ļ�
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     *
     * @note ���������JSON��ʽ���ݣ����ӿ�������Ҫ��
     * - ����keyʹ��""����, ���������δ��""���õ�key
     * - ��������ֿ�����[]
     * - ��������� null  ֵ
     * - ��������� true  ֵ
     * - ��������� false ֵ
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note ָ��/�������͵�entry������
     * @note ����ָ����refer���Ե�entry, ���entry����Ӧ��JSON���������refer��ֵ�����������Ƿ���ȷ
     * @note ����ָ����refer���Ե�entry, ���entryû����Ӧ��JSON������򱾽ӿڲ�������ݵ���Ч��
     */
    TDR_API int tdr_input_json_file(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost,
                                    IN const char *a_pszJsonFile, IN int a_iCutOffVersion);

    /**��C���Զ������ڴ�����ת����JSON��ʽ����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_pstJson ��������������ڱ������ɵ�JSON����
     * @param[in]       a_pstHost �ڴ����ݻ�����
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note �����ָ��/�������͵����ݣ������Ԫ����������ָ���� io="nooutput" ���Ե�����
     *
     * @note �����JSON�������ʹ��{}��װ
     * @note �����JSON�����У��ؼ���ȫ��ʹ��""����
     * @note �����JSON�����У��ؼ���˳���� a_pstMeta �����˳����ͬ
     */
    TDR_API int tdr_output_json(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstJson,
                                IN LPTDRDATA a_pstHost, IN int a_iCutOffVersion);

    /**��C���Զ������ڴ�����ת����JSON��ʽ����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_fpJson  ���ڱ������ɵ�JSON���ݵ��ļ�ָ��
     * @param[in]       a_pstHost �ڴ����ݻ�����
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note �����ָ��/�������͵����ݣ������Ԫ����������ָ���� io="nooutput" ���Ե�����
     *
     * @note �����JSON�������ʹ��{}��װ
     * @note �����JSON�����У��ؼ���ȫ��ʹ��""����
     * @note �����JSON�����У��ؼ���˳���� a_pstMeta �����˳����ͬ
     */
    TDR_API int tdr_output_json_fp(IN LPTDRMETA a_pstMeta, IN FILE *a_fpJson,
                                   IN LPTDRDATA a_pstHost, IN int a_iCutOffVersion);

    /**��C���Զ������ڴ�����ת����JSON��ʽ����
     * @deprecated Ŀǰ�Ѳ��Ƽ�ʹ��
     * @param[in]       a_pstMeta Ԫ����������ָ��, ֻ��Ϊ<struct>������
     * @param[in,out]   a_pszJsonFile  ���ڱ������ɵ�JSON���ݵ��ļ�
     * @param[in]       a_pstHost �ڴ����ݻ�����
     * @param[in]       a_iCutOffVersion ���ݳ�Ա���ð汾
     *
     * @pre a_pstMeta ����ΪNULL
     * @pre a_pstJson ����ΪNULL
     * @pre a_pstHost ����ΪNULL
     *
     * @note TDR֧�ֵ�C�Ļ���������JSON�������͵Ķ�Ӧ��ϵ����:
     * - TDR float/double                          ��Ӧ JSON float
     * - TDR string/wstring/date/time/datetime/ip  ��Ӧ JSON string
     * - TDR wchar �������������͡�                ��Ӧ JSON integer
     *
     * @note �����ַ���������⣬���ڱ��ӿڣ�����Ԫ���������в�Ҫʹ�� wchar/wstring
     * @note �����ָ��/�������͵����ݣ������Ԫ����������ָ���� io="nooutput" ���Ե�����
     *
     * @note �����JSON�������ʹ��{}��װ
     * @note �����JSON�����У��ؼ���ȫ��ʹ��""����
     * @note �����JSON�����У��ؼ���˳���� a_pstMeta �����˳����ͬ
     */
    TDR_API int tdr_output_json_file(IN LPTDRMETA a_pstMeta, IN const char *a_pszJsonFile,
                                     IN LPTDRDATA a_pstHost, IN int a_iCutOffVersion);

    /** @}*/ /*defgroup  TDR_DATA_IO_JSON TDR_������JSON��ʽ����/��� */



    /** @defgroup  TDR_PRINT_DATA TDR_���ݴ�ӡ
     * @{
     */

    struct tagTdrPrintFormat
    {
        const char* pszSepStr; /* �ֶηָ��ַ��� */
        int  iNoVarName;       /* ����������� */
        char chIndentChar;     /* �����ַ�, ÿ������ 4 �������ַ� */
        int iWithVersion;  /* ���ð汾 */
    };

    typedef struct tagTdrPrintFormat      TDRPRINTFORMAT;
    typedef struct tagTdrPrintFormat*   LPTDRPRINTFORMAT;

    /**ȡ��tdr_sprintf/tdr_fprintfʹ�õ�Ĭ�ϴ�ӡ��ʽ
     * @param[in, out] ���뱣���ӡ��ʽ�Ļ����������Ĭ�ϵĴ�ӡ��ʽ
     *
     * @note a_pstFormat ����Ϊ NULL
     */
    TDR_API int tdr_get_default_format(INOUT LPTDRPRINTFORMAT a_pstFormat);

    /**����Ԫ���������������ڴ����ݽṹ�Կ��ӻ��ķ�ʽ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in]	a_fp �洢���ӻ�������Ϣ���ļ����
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴����ļ����ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_fp ����Ϊ NULL
     * @pre \e a_psHost ����ΪNULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_fprintf(IN LPTDRMETA a_pstMeta, IN FILE  *a_fp, IN LPTDRDATA a_pstHost, IN int a_iCutOffVersion);

    /**����Ԫ���������������ڴ����ݽṹ�Կ��ӻ��ķ�ʽ���
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in]	a_fp �洢���ӻ�������Ϣ���ļ����
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_pstFormat �����ʽ���ƽṹ
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ����a_pstFormat�ĳ�Ա֮ǰ�������ȵ��� tdr_get_default_format ȡ��Ĭ�ϵĸ�ʽ����
     * @note ���a_pstFormat->iNoVarName!=0, �����������
     * @note ���a_pstFormat->pszSepStr==NULL����ʹ��"\\n"��Ϊ�ָ��ַ���
     * @note ����a_pstFormat->chIndentChar=0, �Ϳ���������յĿ��ӻ�����
     * @note ���a_pstFormat->iWithVersion=0�������'V+���ð汾'��Ϣ���������ַ���ͷ����Ϣ�����'V+���ð汾��'
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴����ļ����ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_fp ����Ϊ NULL
     * @pre \e a_psHost ����ΪNULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     * @pre \e a_pstFormat ����Ϊ NULL
     *
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_fprintf_ex(IN LPTDRMETA a_pstMeta, IN FILE  *a_fp, IN LPTDRDATA a_pstHost,
                               IN int a_iCutOffVersion, IN LPTDRPRINTFORMAT a_pstFormat);

    /**����Ԫ���������������ڴ����ݽṹ�Կ��ӻ��ķ�ʽ������ַ�����������
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstOut �洢���ӻ����ݵĽṹ��ָ��
     *	- ����	a_pstOut.pszBuff ָ������洢���ӻ����ݵ���Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstOut.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstOut.iBuff  ʵ�����ɵĴ洢���ӻ����ݵ���Ϣ���ܳ���
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴������������ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pstHost ����Ϊ NULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     * @pre \e a_pstOut ����Ϊ NULL
     * @pre \e a_pstOut.pszBuff ����Ϊ NULL
     * @pre \e a_pstOut.iBuff �������0
     *
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     */
    TDR_API int tdr_sprintf(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstOut, IN LPTDRDATA a_pstHost, IN int a_iCutOffVersion);

    /**����Ԫ���������������ڴ����ݽṹ�Կ��ӻ��ķ�ʽ������ַ�����������
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstOut �洢���ӻ����ݵĽṹ��ָ��
     *	- ����	a_pstOut.pszBuff ָ������洢���ӻ����ݵ���Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstOut.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstOut.iBuff  ʵ�����ɵĴ洢���ӻ����ݵ���Ϣ���ܳ���
     * @param[in]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_pstFormat �����ʽ���ƽṹ
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾
     * @note ����a_pstFormat�ĳ�Ա֮ǰ�������ȵ��� tdr_get_default_format ȡ��Ĭ�ϵĸ�ʽ����
     * @note ���a_pstFormat->iNoVarName!=0, �����������
     * @note ���a_pstFormat->pszSepStr==NULL����ʹ��"\\n"��Ϊ�ָ��ַ���
     * @note ����a_pstFormat->chIndentChar=0, �Ϳ���������յĿ��ӻ�����
     * @note ���a_pstFormat->iWithVersion=0�������'V+���ð汾'��Ϣ���������ַ���ͷ����Ϣ�����'V+���ð汾��'
     *
     * @note ������ɵ�������Ҫʹ�ýӿ�tdr_sscanf_ex����������tdr_sscanf_ex�����ƣ��ָ��ַ���ֻ�ܰ���һ���ַ�
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *	- ͨ���鿴������������ݣ����Ը�����λ����λ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pstHost ����Ϊ NULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     * @pre \e a_pstOut ����Ϊ NULL
     * @pre \e a_pstOut.pszBuff ����Ϊ NULL
     * @pre \e a_pstOut.iBuff �������0
     * @pre \e a_pstFormat ����Ϊ NULL
     *
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     * @see tdr_sscanf_ex
     */
    TDR_API int tdr_sprintf_ex(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstOut, IN LPTDRDATA a_pstHost,
                               IN int a_iCutOffVersion, IN LPTDRPRINTFORMAT a_pstFormat);

    /**����Ԫ���������ѿ��ӻ�����ת���ɱ����ڴ����ݽṹ, �ǽӿ�tdr_sprintf_ex�ķ�����
     * @param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in,out]	a_pstHost �洢�����ڴ����ݽṹ��Ϣ�Ľṹ��ָ��
     *	- ����	a_pstHost.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstHost.iBuff	ָ���������Ĵ�С
     *	- ���  a_pstHost.iBuff   �������ĳ��ȴ��ڽṹ�峤�ȣ��޸�Ϊ�ṹ�峤�ȣ�
                                  ���С�ڽṹ��ĳ��ȣ��򲻻��޸ĸ�ֵ
     * @param[in]	a_pstVisual �洢���ӻ����ݵĽṹ��ָ��
     *	- ����	a_pstVisual.pszBuff ָ������洢���ӻ����ݵ���Ϣ�Ļ�������ʼ��ַ
     *	- ����  a_pstVisual.iBuff	ָ���������Ĵ�С
     * @param[in] a_iCutOffVersion	Ҫ���õİ汾��
     * @param[in] a_pstFormat �����ʽ���ƽṹ
     *
     * @note ���a_iCutOffVersionΪ0 ����װ���õ���߰汾,���a_pstFormat->iWithVersionΪ0(��ʱ��Ϊ����ı��в���V+�汾��)����a_iCutOffVersionֵ���������ʱa_iCutOffVersion�汾ֵ��ͬ
     * @note ����a_pstFormat�ĳ�Ա֮ǰ�������ȵ��� tdr_get_default_format ȡ��Ĭ�ϵĸ�ʽ���ó�ʼ��
     * @note ���a_pstFormat->iNoVarName!=0, ���ȡʱ���Ա�����������Ϳ��ӻ��ı���һ�£����ȡʧ��
     * @note ���a_pstFormat->pszSepStr==NULL����ʹ��"\\n"��Ϊ�ָ��ַ���������Ϳ��ӻ��ı��зָ�����ͬ
     * @note ����������ʵ�ֵĿ��ǣ���ʱ����֧��һ���ָ���(�ַ���a_pstFormat->pszSepStr�ĳ���Ϊ1)
     * @note ����a_pstFormat->chIndentCharΪҪ��ȡ�Ŀ��ӻ��ı���������������Ϳ��ӻ���������������ͬ
     * @note ���a_pstFormat->iWithVersion=0����ٶ����ӻ��ı���û�а汾��Ϣ��������Ϊ���ӻ��ı��о��а汾��Ϣ��
     * ���a_pstFormat->iWithVersionΪ0������a_iCutOffVersion�ֶ�Ϊ���ð汾��׼ֵ�������Խ���������'V+'�汾ֵΪ���ð汾��׼ֵ��
     *
     * @note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
     *	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pstHost ����Ϊ NULL
     * @pre \e a_pstHost.pszBuff ����Ϊ NULL
     * @pre \e a_pstHost.iBuff �������0
     * @pre \e a_pstVisual ����Ϊ NULL
     * @pre \e a_pstVisual.pszBuff ����Ϊ NULL
     * @pre \e a_pstVisual.iBuff �������0
     * @pre \e a_pstFormat ����Ϊ NULL
     *
     * @retval 0   ����ɹ�
     * @retval >0  ����ɹ���������ĳЩ�������
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     * @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
     *
     * @see	tdr_get_meta_by_name
     * @see	tdr_get_meta_by_id
     * @see	tdr_error_string
     * @see tdr_get_error_detail
     * @see tdr_sprintf_ex
     */
    TDR_API int tdr_sscanf_ex(IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstHost, IN LPTDRDATA a_pstVisual,
                               IN int a_iCutOffVersion, IN LPTDRPRINTFORMAT a_pstFormat);
    /** @}*/ /* @defgroup  TDR_PRINT_DATA TDR_���ݴ�ӡ*/


    /** @defgroup TDR_GEN_XMLTMPLT TDR_ΪԪ������������ģ��
     * @{
     */

    /**
     * Ϊָ��Ԫ��������XML��ʽ������ģ�壬�����浽ָ���ļ���
     * @param[in] a_pstMeta     Ԫ��������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
     * @param[in] a_fpFile      �洢����ģ����ļ����
     * @param[in] a_iCutOffVersion Ҫ���õİ汾��
     * @param[in] a_iXmlDataFormat XML�����ļ��ĸ�ʽ,Ŀǰ֧�ֵĸ�ʽ�У�
     *	- TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME
     *	- TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME
     *	- �����ָ�ʽ��������μ��ļ� TSF4G-DR-FAQ.doc ����xml�����ļ�����/�������
     *
     * @note    ����xml�����ļ��ľ����ʽ˵������ο�
     *          �ĵ�<TSF-G-DR-FAQ.doc>�С�Xml�����ļ�����/���ƪ�� -> ��Tdr֧��ʲô��ʽ��xml�����ļ�����
     * @note    ���a_iCutOffVersionΪ0 ��������߰汾����
     * @note    ���a_iXmlDataFormatָ����TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME��TDR_XML_DATA_FORMAT_ATTR_ENTRY_NAME�����ֵ��
     *          ��ǿ�ư��ո�ʽTDR_XML_DATA_FORMAT_LIST_ENTRY_NAME���д���
     * @note    �����Ա��desc������Ϣ������ע�ͷ�ʽ���desc����
     * @note    ����union�ṹ�����������union��select��ǰֵ��Ӧ���ӳ�Ա��Ȼ����"ע��"����ʽ��������ӳ�Ա
     *          ����XML��֧��ע��Ƕ�ף�Ŀǰʹ�á�commented_union_item����װ��ע�͵��ӳ�Ա��
     *          ʾ���ӳ�Ա�Ѿ���ע��
     * @note    ���������Ա����ע�͵���ʽָ������Ĵ�С��
     *          �����ص�refer����ֵ��Ϊ0, �������Ԫ�ظ�����refer����ֵָ����
     *          ����������2��Ԫ�ء�
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_fpFile ����Ϊ NULL
     *
     * @retval 0   ����ɹ�
     * @retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *
     */
    TDR_API int tdr_gen_xmltmplt(IN LPTDRMETA a_pstMeta, OUT FILE *a_fpFile, IN int a_iCutOffVersion, IN int a_iXmlDataFormat);

    /** @}*/ /* @defgroup TDR_GEN_XMLTMPLT TDR_ΪԪ������������ģ��*/

#ifdef __cplusplus
}
#endif

#endif /* TDR_DATA_IO_H */
