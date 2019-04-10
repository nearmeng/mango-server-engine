/**
 *
 * @file     tdr_metalib_manage.h
 * @brief    Ԫ���ݿ�������ģ��
 *
 * @author steve jackyai
 * @version 1.0
 * @date 2007-04-02
 *
 *
 * Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
 * All rights reserved.
 *
 */

#ifndef TDR_METALIB_MANAGE_H
#define TDR_METALIB_MANAGE_H


#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


    /** @defgroup TDR_META_MANAGE TDR_Ԫ����(meta)����
     *  @{
     */


    /** ����Ԫ��������Ԫ���ݿ��в��Ҵ�Ԫ����
     *@param[in] a_pstLib Ԫ���ݿ�ָ��
     *@param[in] a_pszName Ԫ�������ִ�ָ��
     *
     *@return ����ҵ��򷵻�Ԫ���ݵĳ�Ա�����򷵻�NULL
     *
     * @pre \e a_pstLib ����Ϊ NULL
     * @pre \e a_pszName ����Ϊ NULL
     */
    TDR_API LPTDRMETA tdr_get_meta_by_name(IN LPTDRMETALIB a_pstLib, IN const char* a_pszName);

    /**����Ԫ���ݵ�ID��Ԫ���ݿ��в��Ҵ�Ԫ����
     *@param[in] a_pstLib Ԫ���ݿ�ָ��
     *@param[in] a_iID Ԫ���ݵ�ID
     *
     *@return ����ҵ��򷵻�Ԫ���ݵĳ�Ա�����򷵻�NULL
     *
     * @pre \e a_pstLib ����Ϊ NULL
     */
    TDR_API LPTDRMETA tdr_get_meta_by_id(IN LPTDRMETALIB a_pstLib, IN int a_iID);


    /** ��ȡԪ���������Ļ�׼�汾��
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return success: Ԫ���������Ļ�׼�汾�� failed: -1
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API int tdr_get_meta_based_version(IN LPTDRMETA a_pstMeta);



    /**
     *��ȡԪ���������ĵ�ǰ�汾��
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return success:Ԫ���������ĵ�ǰ�汾�� failed: -1
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API int tdr_get_meta_current_version(IN LPTDRMETA a_pstMeta);

    /**
     *��ȡԪ���������Ļ�����������ID
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return ��������ID: TDR_TYPE_UNION �� TDR_TYPE_STRUCT
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API int tdr_get_meta_type(IN LPTDRMETA a_pstMeta);

    /**��ȡԪ��������������
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return Ԫ���������ַ�����ָ��
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API const char *tdr_get_meta_name(IN LPTDRMETA a_pstMeta);



    /**
     *��ȡ����Ԫ��������(meta)�Ĵ洢�ռ�
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return success: ����Ԫ��������(meta)�Ĵ洢���ֽ��� failed : -1
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API size_t tdr_get_meta_size(IN LPTDRMETA a_pstMeta);

	 /**
     *��ȡԪ���ݵ�ID
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return success : Ԫ���ݶ�Ӧ��ID
     *        ����-1 : ��ʾ��ȡʧ��(a_pstMetaΪ�ջ��߸�Ԫ���ݵ�ID������Ч)
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
	TDR_API int tdr_get_meta_id(IN LPTDRMETA a_pstMeta);

    /**��ȡԪ����id����ֵ��Ӧ�ĺ궨�������
     *@param[in] a_pstMeta Ԫ����������ָ��
     *@param[in] a_pstLib Ԫ�����������ָ��
     *
     *@note ͨ������tdr_get_meta_by_name���Ի�ȡԪ���������ṹ��ָ��
     *
     *@return �����Ԫ���ݵ�id����ֵΪ�궨��,�򷵻ش˺궨�������;���򷵻ؿմ�""
     *
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     *@see tdr_get_meta_by_name
     */
    TDR_API const char *tdr_get_meta_id_name(IN LPTDRMETALIB a_pstLib, IN LPTDRMETA a_pstMeta);

	 /**
     *��ȡԪ���ݵ�primarykey
     *@param[in]      a_pstMeta  Ԫ����������ָ��
     *@param[in, out] a_pszBuf   ����primarykey��Ϣ���ַ�������
     *@param[in]      a_iBufSize ����primarykey��Ϣ���ַ���������С
     *
     *@return success : 0
     *        failed :  !0
     *
     *@note ���primarykey�Ǹ�������������֮���Զ��ŷָ�
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     * @pre \e a_pszBuf  ����Ϊ NULL
     * @pre \e a_iBufSize�������0
     */
    TDR_API int tdr_get_meta_primarykey(IN const LPTDRMETA a_pstMeta,
                                        INOUT char* a_pszBuf, IN size_t a_iBufSize);

    /**
     *��ȡ����Ԫ�������ͳ�ԱԪ�صĸ���
     *@param[in] a_pstMeta Ԫ����������ָ��
     *
     *@return ���Idֵ
     *
     * @pre \e a_pstMeta ����Ϊ NULL
     */
    TDR_API int tdr_get_entry_num(IN LPTDRMETA a_pstMeta);


    /** ȡ��target��sizeinfo����ֵ(a_pszName)��meta�е�ƫ��
     *@param[out] a_pstRedirector ��Ԫ�ص�ƫ��
     *@param[in] a_pstMeta ��Ҫ��λ�Ľṹ��Ԫ�����������
     *@param[in] a_iEntry ��ǰa_iEntry+1��Ԫ���в���, ��������Ԫ���в���(-1)
     *@param[in] a_pszName sizeinfo����ֵ(�����ǻ�������������)
     *
     *@return �ɹ�����TDR_SUCCESS�����򷵻ش������
     *
     *@pre \e a_pstRedirector ����ΪNULL
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_pszName ����ΪNULL
     *@pre \e �����Ȼ�ȡԪ�ص�type,count ,refer����
     */
    int tdr_sizeinfo_to_off(LPTDRSIZEINFO a_pstRedirector, LPTDRMETA a_pstMeta, int a_iEntry, const char* a_pszName);

#define tdr_sizeinfo_to_off_i   tdr_sizeinfo_to_off

    /**���ݳ�Ա�ڽṹ���е�·��(Path)��Ϣ��ȡ�˳�Ա���ڽṹ���׵�ַ��ƫ��ֵ
     *@param[in] a_pstMeta ��Ҫ��λ�Ľṹ��Ԫ�����������
     *@param[in,out] a_ppstEntry �����Ӧ��Ա�����������ָ��
     *@param[out] a_piHOff �����Ӧ��Ա��ƫ��ֵ
     *@param[in] a_pszPath  ����˳�Ա·����Ϣ�Ļ�����
     *
     *@return  �ɹ�����0�����򷵻ظ���
     *
     *@pre \e a_pszBuff ����ΪNULL
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_iBuff �������0
     *@pre \e a_iOff ������ڵ���0
     *@pre \e a_pszPath ����ΪNULL
     */
    TDR_API int tdr_entry_path_to_off(IN LPTDRMETA a_pstMeta, INOUT LPTDRMETAENTRY *a_ppstEntry, OUT TDROFF *a_piHOff, IN const char *a_pszPath);


    /**���ݳ�Ա�ڽṹ���е�ƫ�ƶ�λ�˳�Ա�ڽṹ�е�·����Ϣ
     *@param[in] a_pstMeta ��Ҫ��λ�Ľṹ��Ԫ�����������
     *@param[in] a_iOff  ��Ա��ƫ��
     *
     *@note ·����Ϣ�������ھ�̬�������У��ٴε��ñ��ӿ�ʱ���ϴε��û�ȡ��·����Ϣ������д
     *
     *@return  �ɹ�����·���ַ��������򷵻ؿմ�("")
     *
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_iOff ������ڵ���0
     */
    TDR_API char *tdr_entry_off_to_path(IN LPTDRMETA a_pstMeta, IN int a_iOff);


    /**��ѯ�˽ṹ���Ƿ�����������������Եĳ�Ա
     *@param[in] a_pstMeta �ṹ���Ա��Ԫ�����������
     *
     *@pre \e a_pstMeta ����ΪNULL
     *
     *@return ������������Գ�Ա���򷵻ط���ֵ�����򷵻�0
     */
    TDR_API int tdr_do_have_autoincrement_entry(IN LPTDRMETA a_pstMeta);

    /**ȡ�ô˽ṹ���desc����
     *@param[in] a_pstMeta �ṹ���Ա��Ԫ�����������
     *
     *@pre \e a_pstMeta ����ΪNULL
     *
     *@return �����desc���ԣ��򷵻�ָ��desc�����ַ�����ָ�룬���򷵻ؿմ�("")
     */
    TDR_API const char *tdr_get_meta_desc(IN LPTDRMETA a_pstMeta);

    /** @}*/ /* TDR_META_MANAGE TDR_Ԫ����(meta)����*/


    /** @defgroup TDR_ENTRY_MANAGE TDR_���ݳ�Ա(entry)����
     *  @{
     */

    /**��ȡԪ��������������
     *@param[in] a_pstEntry Ԫ����������ָ��
     *
     *@return Ԫ���������ַ�����ָ��
     *
     * @pre \e a_pstEntry ����Ϊ NULL
     */
    TDR_API const char *tdr_get_entry_name(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡԪ����������cname
     *@param[in] a_pstEntry Ԫ����������ָ��
     *
     *@return Ԫ���������ַ�����ָ��
     *
     * @pre \e a_pstEntry ����Ϊ NULL
     */
    TDR_API const char *tdr_get_entry_cname(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡԪ����������desc�����ַ���
     *@param[in] a_pstEntry Ԫ����������ָ��
     *
     *@return Ԫ����desc�����ַ�����ָ��
     *
     * @pre \e a_pstEntry ����Ϊ NULL
     */
    TDR_API const char *tdr_get_entry_desc(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡԪ���������󶨵�macrosgroup��ָ��
     *@param[in] a_pstEntry Ԫ����������ָ��
     *
     *@return ���Ԫ���ݰ���macrosgroup, ����Ԫ���ݰ󶨵�macrosgroup��ָ��; ���򷵻�NULL
     *
     * @pre \e a_pstEntry ����Ϊ NULL
     */
    TDR_API LPTDRMACROSGROUP tdr_get_entry_macrosgroup(IN LPTDRMETAENTRY a_pstEntry);

    /**����Ԫ���ݳ�Ա����Ԫ���ݿ��в��Ҵ˳�Ա�ľ��
     *@param[in] a_pstMeta ������Ԫ���ݵ�ָ��
     *@param[in] a_pszName entry������
     *
     *@return �������ָ�����ֵĳ�Ա���򷵻ش˳�Ա�ľ�������򷵻�NULL
     *
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_pszName ����Ϊ NULL
     */
    TDR_API LPTDRMETAENTRY tdr_get_entryptr_by_name(IN LPTDRMETA a_pstMeta, IN const char* a_pszName);

    /**����Ԫ���ݳ�Ա����Ԫ���ݿ��в��Ҵ˳�Ա�����ڸ�Ԫ���ݳ�Ա�����е��±���ʽ���أ�
     *@param[out] a_piIdx ����˳�Ա������
     *@param[in] a_pstMeta ������Ԫ���ݵ�ָ��
     *@param[in] a_pszName Ԫ�������ִ�ָ��
     *
     *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *@retval 0   ����ɹ�
     *@retval >0  ����ɹ���������ĳЩ�������
     *
     * @pre \e a_piIdx ����Ϊ NULL
     *@pre \e a_pstMeta ����ΪNULL
     * @pre \e a_pszName ����Ϊ NULL
     */
    TDR_API int tdr_get_entry_by_name(OUT int* a_piIdx, IN LPTDRMETA a_pstMeta, IN const char* a_pszName);

    /**����Ԫ���ݳ�Ա��ID��Ԫ���ݿ��в��Ҵ˳�Ա�����ڸ�Ԫ���ݳ�Ա�����е��±���ʽ���أ�
     *���Ԫ������union�ṹ�������a_iId��ĳ��Ա[minid, maxid]����֮�䣬��ѡ��˳�Ա������
     *ѡ��ȱʡ��Ա��������ڵĻ���
     *@param[out] a_piIdx ����˳�Ա������
     *@param[in] a_pstMeta ������Ԫ���ݵ�ָ��
     *@param[in] a_iId Ԫ���ݵ�ID
     *
     *@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ������
     *@retval 0   ����ɹ�
     *
     * @pre \e a_piIdx ����Ϊ NULL
     *@pre \e a_pstMeta ����ΪNULL
     */
    TDR_API int tdr_get_entry_by_id(OUT int* a_piIdx, IN LPTDRMETA a_pstMeta, IN int a_iId);

    /**����Ԫ���ݳ�Ա��Ԫ���ݽṹ�е�������ȡ�˳�Ա�����ľ��
     *@param[in] a_pstMeta ������Ԫ���ݵ�ָ��
     *@param[in] a_idxEntry entry������
     *
     *@return ���������ȷ,�򷵻ش˳�Ա�����ľ��,���򷵻�NULL
     *
     *@pre \e a_pstMeta ����ΪNULL
     *@see tdr_get_meta_by_id
     *@see tdr_get_entry_by_id
     */
    TDR_API LPTDRMETAENTRY tdr_get_entry_by_index(IN LPTDRMETA a_pstMeta, IN int a_idxEntry);

    /**��ȡentryԪ��id����ֵ
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *@note ͨ������tdr_get_entry_by_index���Ի�ȡentryԪ�������ṹ��ָ��
     *
     *@return entryԪ��id����ֵ
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@see tdr_get_entry_by_index
     */
    TDR_API int tdr_get_entry_id(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentryԪ��id����ֵ��Ӧ�ĺ궨�������
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *@param[in] a_pstLib Ԫ�����������ָ��
     *
     *@note ͨ������tdr_get_entry_by_index���Ի�ȡentryԪ�������ṹ��ָ��
     *
     *@return ����˳�Ա��id����ֵΪ�궨��,�򷵻ش˺궨�������;���򷵻ؿմ�""
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     *@see tdr_get_entry_by_index
     */
    TDR_API const char *tdr_get_entry_id_name(IN LPTDRMETALIB a_pstLib, IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentryԪ��maxid����ֵ��Ӧ�ĺ궨�������
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *@param[in] a_pstLib Ԫ�����������ָ��
     *
     *@note ͨ������tdr_get_entry_by_index���Ի�ȡentryԪ�������ṹ��ָ��
     *
     *@return ����˳�Ա��id����ֵΪ�궨��,�򷵻ش˺궨�������;���򷵻ؿմ�""
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     *@see tdr_get_entry_by_index
     */
    TDR_API const char *tdr_get_entry_maxid_name(IN LPTDRMETALIB a_pstLib, IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentryԪ��minid����ֵ��Ӧ�ĺ궨�������
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *@param[in] a_pstLib Ԫ�����������ָ��
     *
     *@note ͨ������tdr_get_entry_by_index���Ի�ȡentryԪ�������ṹ��ָ��
     *
     *@return ����˳�Ա��id����ֵΪ�궨��,�򷵻ش˺궨�������;���򷵻ؿմ�""
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     *@see tdr_get_entry_by_index
     */
    TDR_API const char *tdr_get_entry_minid_name(IN LPTDRMETALIB a_pstLib, IN LPTDRMETAENTRY a_pstEntry);


    /**��ȡ����entry��Ա�ı��ش洢�ռ��С
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *@note ͨ������tdr_get_entry_by_index���Ի�ȡentryԪ�������ṹ��ָ��
     *
     *@return entryԪ�ص��������ı��ش洢�ռ��С
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@see tdr_get_entry_by_index
     */
    TDR_API size_t tdr_get_entry_unitsize(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry����������ID
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return ���س�Ա����������ID������TDR֧�ֵ�����ID������μ�tdr_ctypes_info.h��"������������"
     *
     */
    TDR_API int tdr_get_entry_type(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry�����������Ϣ
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@retval 0	��ʾ�˳�ԱΪ�ɱ䳤������
     *@retval 1	��ʾ�˳�Ա��������
     *@retval >1 ��ʾ�˳�ԱΪ�̶����ȵ�����
     *
     */
    TDR_API int tdr_get_entry_count(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentryԪ����ָmeta�ṹ������ָ��
     * ���entryԪ��Ϊ������������ʱ,ͨ���˽ӿڿ��Ի�ȡ��entry�����������͵������ṹ��ָ��
     *@param[in] a_pstLib	Ԫ�����������ָ��
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return ���entryԪ��Ϊ������������ʱ,������ָ������������������ָ��;���򷵻�NULL
     *
     */
    TDR_API LPTDRMETA tdr_get_entry_type_meta(IN LPTDRMETALIB a_pstLib, IN LPTDRMETAENTRY a_pstEntry);

    /**����Ԫ���ݳ�Ա����·����Ԫ���ݿ��в��Ҵ˳�Ա
     *@param[in] a_pstMeta ������Ԫ���ݵ�ָ��
     *@param[in] a_pszEntryPath entry�ڽṹ�е�����·����
     *
     *@return �������ָ�����ֵĳ�Ա���򷵻ش˳�Ա�ľ�������򷵻�NULL
     *
     *@pre \e a_pstMeta ����ΪNULL
     *@pre \e a_pszEntryPath ����Ϊ NULL*/
    TDR_API LPTDRMETAENTRY tdr_get_entry_by_path(IN LPTDRMETA a_pstMeta, IN const char* a_pszEntryPath);

    /**��ȡ��Ա���Զ�������ֵ
     *@param[in] a_pstEntry ��Ա��Ԫ�����������
     *@param[in] a_pstLib	Ԫ�����������ָ��
     *
     *@return ����˳�Ա������customattr����,�򷵻ش����Ե��׵�ַ;���򷵻�""
     *
     *@pre \e a_pstEntry ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     */
    TDR_API const char *tdr_get_entry_customattr(IN LPTDRMETALIB a_pstLib,
                                                 IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry�������ṹ�е�ƫ����
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return >=0, ��Ա�������ṹ�е�ƫ������
     *@return <0,  a_pstEntry == NULL
     *
     */
    TDR_API int tdr_get_entry_offset(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry��selectԪ�ص������ṹ��ָ��
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return !NULL, ��Ա��selectԪ�ص������ṹ��ָ��
     *@return NULL,  ��Աû��selectԪ��, ��a_pstEntry==NULL
     *
     */
    TDR_API LPTDRMETAENTRY tdr_get_entry_select(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry��referԪ�ص������ṹ��ָ��
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return !NULL, ��Ա��referԪ�ص������ṹ��ָ��
     *@return NULL,  ��Աû��referԪ��, ��a_pstEntry==NULL
     *
     */
    TDR_API LPTDRMETAENTRY tdr_get_entry_refer(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry��version����ֵ
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return >=0, version���Ե�ֵ
     *@return <0,  a_pstEntry == NULL
     *
     */
    TDR_API int tdr_get_entry_version(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry��usesubtable����ֵ
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ��
     *
     *
     *@return  0, usesubtable���Ե�ֵΪfalse (Ĭ�ϵ�usesubtable����ֵ)
     *@return !0, usesubtable���Ե�ֵΪtrue
     *
     */
    TDR_API int tdr_get_entry_subtable_attr(IN LPTDRMETAENTRY a_pstEntry);

    /**��ȡentry��defaultvalue����ֵ�����ַ�����ʽ���
     *@param[in] a_pstEntry entryԪ�������ṹ��ָ�룬����ΪNULL
     *@param[in] a_pszDefault ����entryԪ��Ĭ��ֵ�Ļ�����ָ�룬����ΪNULL
     *@param[in] a_iSize ����entryԪ��Ĭ��ֵ�Ļ���ռ��С���������0
     *
     *@note wstring���ַ���֧�ֻ�ȡdefaultvalue
     *
     *@return  NULL, defaultvalue���Ե�ֵΪ�ջ�������������Ϸ�
     *@return  !NULL, ����ָ��ָ�򱣴�defaultvalue����ֵ������
     *
     */
    TDR_API const char* tdr_get_entry_defaultvalue(IN LPTDRMETAENTRY a_pstEntry, OUT char* a_pszDefault, IN size_t a_iSize);

    /** @}*/ /* TDR_ENTRY_MANAGE TDR_���ݳ�Ա(entry)����*/


    /** @defgroup TDR_INDEX_MANAGE TDR_INDEX(index)����
     *  @{
     */

    /**���� index id ��ȡ index
     *@param[in] a_pstMeta  �����ҵ�Ԫ���ݵ�ָ��
     *@param[in] a_iID      index id
     *
     *@return NULL  ���Ҳ�����������
     *@return !NULL ����ɹ������� index �ṹ��ָ��
     *
     *@pre \e a_pstMeta ����ΪNULL
     */
    TDR_API LPTDRMETAINDEX tdr_get_index_by_id(IN LPTDRMETA a_pstMeta, IN int a_iID);

    /**���� index ���ƻ�ȡ index
     *@param[in] a_pstMeta  �����ҵ�Ԫ���ݵ�ָ��
     *@param[in] a_pszName  index ����
     *
     *@return NULL  ���Ҳ�����������
     *@return !NULL ����ɹ������� index �ṹ��ָ��
     *
     *@pre \e a_pstMeta ����ΪNULL
     */
    TDR_API LPTDRMETAINDEX tdr_get_index_by_name(IN LPTDRMETA a_pstMeta, IN const char* a_pszName);

    /**��ȡ index Ԫ�ص� id ����ֵ
     *@param[in] a_pstIndex index �ṹ��ָ��
     *
     *@return >=0   index Ԫ�ص� id ����ֵ
     *@return <0    �� id ���Ի�������
     *
     *@pre \e a_pstIndex ����ΪNULL
     */
    TDR_API int tdr_get_index_id(IN LPTDRMETAINDEX a_pstIndex);

    /**��ȡ index ������
     *@param[in] a_pstIndex index �ṹ��ָ��
     *
     *@return index ������
     *
     *@pre \e a_pstIndex ����Ϊ NULL
     */
    TDR_API const char *tdr_get_index_name(IN LPTDRMETAINDEX a_pstIndex);

    /** @}*/ /* TDR_INDEX_MANAGE TDR_INDEX(index)����*/


    /** @defgroup TDR_MACRO_MANAGE TDR_�궨��(macro)����
     *  @{
     */

    /** ���ݺ�����ú궨��
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pszName ����Һ������
     *
     *@return �ɹ�����ָ��궨���ָ�룬���򷵻� NULL
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pszName ����ΪNULL
     */
    TDR_API LPTDRMACRO tdr_get_macro_by_name(IN LPTDRMETALIB a_pstLib, IN const  char *a_pszName);

    /** ���ݺ����������ֵ
     *@param[out] a_piID ��ú궨�����ֵ
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pszName ����Һ������
     *
     *@return �ɹ�����0�����򷵻ط�0
     *
     *@pre \e a_piID ����ΪNULL
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pszName ����ΪNULL
     */
    TDR_API int tdr_get_macro_value(OUT int *a_piID, IN LPTDRMETALIB a_pstLib, IN const  char *a_pszName);

    /** ��ȡa_pstLib�к궨�������������macrosgroup�ж���ĺ꣩
     *@param[in] a_pstLib Ԫ���ݿ�
     *
     *@return ���ж���ĺ������
     *
     *@pre \e a_pstLib ����ΪNULL
     */
    TDR_API int tdr_get_metalib_macro_num(IN LPTDRMETALIB a_pstLib);

    /** ���ݺ��index�����ָ��
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_iIdx ����Һ��index
     *
     *@return �ɹ����غ�ָ�룬���򷵻�NULL
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_iIdx ����>=0����С��a_pstLib�еĺ������
     */
    TDR_API LPTDRMACRO tdr_get_metalib_macro_by_index(IN LPTDRMETALIB a_pstLib, IN int a_iIdx);

    /** ���ݺ�ָ����������
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pstMacro ��ָ��
     *
     *@return ָ������ֵ�ָ��
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pstMacro ����ΪNULL
     */
    TDR_API const char* tdr_get_macro_name_by_ptr(IN LPTDRMETALIB a_pstLib, IN LPTDRMACRO a_pstMacro);

    /** ���ݺ�ָ������ֵ
     *@param[out] a_piID ��ú궨�����ֵ
     *@param[in] a_pstMacro ��ָ��
     *
     *@return �ɹ����� 0�����򷵻� !0
     *
     *@pre \e a_piID ����ΪNULL
     *@pre \e a_pstMacro ����ΪNULL
     */
    TDR_API int tdr_get_macro_value_by_ptr(OUT int *a_piID, IN LPTDRMACRO a_pstMacro);

    /** ���ݺ�ָ������desc�ַ���
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pstMacro ��ָ��
     *
     *@return ָ����desc�����ַ�����ָ��
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pstMacro ����ΪNULL
     */
    TDR_API const char* tdr_get_macro_desc_by_ptr(IN LPTDRMETALIB a_pstLib, IN LPTDRMACRO a_pstMacro);

    /**��ȡ�����ڵ�macrosgroup��ָ��
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pstMacro ��ָ��
     *
     *@return ���������ĳ��macrosgroup, ����ָ���macrosgroup��ָ��; ���򷵻�NULL
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pstMacro ����Ϊ NULL
     *
     *@note �����㷨�Ǽ򵥵ı������������O(n*n)
     */
    TDR_API LPTDRMACROSGROUP tdr_get_macro_macrosgroup(IN LPTDRMETALIB a_pstLib,
                                                       IN LPTDRMACRO a_pstMacro);


    /** @}*/ /* TDR_MACRO_MANAGE TDR_�궨��(macro)����*/

    /** @defgroup TDR_MACROSGROUP_MANAGE TDR_�궨����(macrosgroup)����
     *  @{
     */

    /** ��ȡԪ���ݿ��ж����macrosgroup������
     *@param[in] a_pstLib Ԫ���ݿ�
     *
     *@return Ԫ���ݿ��ж����macrosgroup������
     *
     *@pre \e a_pstLib ����ΪNULL
     */
    TDR_API int tdr_get_macrosgroup_num(IN LPTDRMETALIB a_pstLib);

    /** ���ݺ궨�����index��ȡ��ָ��
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_iIdx �궨�����index
     *
     *@return ָ��궨�����ָ��
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_iIdx ����>=0, ��С��a_pstLib�еĺ궨���������
     */
    TDR_API LPTDRMACROSGROUP tdr_get_macrosgroup_by_index(IN LPTDRMETALIB a_pstLib, IN int a_iIdx);

    /** ���ݺ�������ú궨����
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pszName ����Һ��������
     *
     *@return �ɹ�����ָ��궨�����ָ�룬���򷵻� NULL
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pszName ����ΪNULL
     */
    TDR_API LPTDRMACROSGROUP tdr_get_macrosgroup_by_name(IN LPTDRMETALIB a_pstLib, const char* a_pszName);

    /** ��ȡ�궨����a_pstGroup������
     *@param[in] a_pstGroup �궨����ָ��
     *
     *@return ָ��궨����������ַ�����ָ��
     *
     *@pre \e a_pstGroup ����ΪNULL
     */
    TDR_API const char* tdr_get_macrosgroup_name(IN LPTDRMACROSGROUP a_pstGroup);

    /** ��ȡ�궨����a_pstGroup�а����ĺ궨�������
     *@param[in] a_pstGroup �궨����ָ��
     *
     *@return �궨�������
     *
     *@pre \e a_pstGroup ����ΪNULL
     */
    TDR_API int tdr_get_macrosgroup_macro_num(IN LPTDRMACROSGROUP a_pstGroup);

    /** ��ȡ�궨����a_pstGroup��index��Ӧ�ĺ궨��
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pstGroup �궨����ָ��
     *@param[in] a_iIdx �궨�����index
     *
     *@return ָ��궨���ָ��
     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pstGroup ����ΪNULL
     *@pre \e a_iIdx ����>=0, ��С��a_pstGroup�еĺ궨�������
     */
    TDR_API LPTDRMACRO tdr_get_macrosgroup_macro_by_index(IN LPTDRMETALIB a_pstLib,
                                                          IN LPTDRMACROSGROUP a_pstGroup,
                                                          IN int a_iIdx);

    /** ������ֵ�����ڵĺ�������ƻ�ȡ��Ӧ�ĺ궨������
     *@param[in] a_pstLib Ԫ���ݿ�
     *@param[in] a_pszMacrosGroupName ��ֵ���ڵĺ�������
     *@param[in] a_iValue �궨�����ֵ
     *
     *@return ��ֵΪa_iValue�ĺ궨�����ƣ�����""��ʾû���ҵ����߳���

     *
     *@pre \e a_pstLib ����ΪNULL
     *@pre \e a_pszMacrosGroupName ����ΪNULL
     */
    TDR_API const char *tdr_get_macro_name_by_value(IN LPTDRMETALIB a_pstLib,
                                               IN const char *a_pszMacrosGroupName,
                                               IN int a_iValue);
    /** @}*/ /* TDR_MACROSGROUP_MANAGE TDR_�궨����(macro)����*/

#ifdef __cplusplus
}
#endif


#endif /* TDR_METALIB_MANAGE_H */
