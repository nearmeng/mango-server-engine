/**
*
* @file     tdr_meta_entries_index.h
* @brief    TDR�ṹ���ڼӿ��Ա�����ٶȵ�����ģ��ͷ�ļ�
*
* @author jackyai
* @version 1.0
* @date 2007-12-04
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/
#ifndef TORM_META_ENTRIES_INDEX_H
#define TORM_META_ENTRIES_INDEX_H

#include <stdio.h>
#include "tdr/tdr_types.h"
#include "tdr/tdr_external.h"

typedef struct  tagTDRNameEntryIndexInfo TDRNAMEENTRYINDEXINFO;
typedef struct  tagTDRNameEntryIndexInfo	*LPTDRNAMEENTRYINDEXINFO;
typedef struct tagTDRMetaEntryNameIndex TDRMETAENTRYNAMEINDEX;
typedef struct tagTDRMetaEntryNameIndex *LPTDRMETAENTRYNAMEINDEX;
typedef struct  tagTDREntryOffIndexInfo TDRENTRYOFFINDEXINFO;
typedef struct  tagTDREntryOffIndexInfo	*LPTDRENTRYOFFINDEXINFO;
typedef struct tagTDRMetaEntryOffIndex TDRMETAENTRYOFFINDEX;
typedef struct tagTDRMetaEntryOffIndex *LPTDRMETAENTRYOFFINDEX;

/**�ṹ���ڳ�Ա����ָ�� ������Ϣ
*/
struct  tagTDRNameEntryIndexInfo
{
	TDRBOOLEAN bIsInExtendArr;	/**<�˳�Ա�Ƿ��ǿ���չ�Ľṹ���ݳ�Ա�������Ա��Ŀǰ�˳�Ա�Ѿ�����date:2009-3-20*/
	LPTDRMETAENTRY	pstEntry;	/**<�˳�Ա����Ԫ�����е�ָ��*/
};


/*�ṹ�� ��Ա����ָ��������*/
struct tagTDRMetaEntryNameIndex
{
	int iFlag;	/**<�������־��Ϣ���ϲ�Ӧ�ò����޸�*/
	int iEntryNum;	/**<Meta�н�����Ա��������Ŀ*/
	TDRNAMEENTRYINDEXINFO astIndexs[1];		/**<������Ϣ��*/
};


/**�ṹ���ڳ�Աָ�룭�洢ƫ�� ������Ϣ
*/
struct  tagTDREntryOffIndexInfo
{
	LPTDRMETAENTRY	pstEntry;	/**<�˳�Ա����Ԫ�����е�ָ��*/
	TDROFF iHOff;			/**<�˳�Ա�ڽṹ�е�ƫ��*/
};



/*�ṹ�� ��Աָ�룭�洢ƫ��������*/
struct tagTDRMetaEntryOffIndex
{
	int iFlag;	/**<�������־��Ϣ���ϲ�Ӧ�ò����޸�*/
	int iEntryNum;	/*Meta�н��������ĳ�Ա��Ŀ*/
	TDRENTRYOFFINDEXINFO astIndexs[1];		/*������Ϣ��*/
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup TDR_META_ENTRY_INDEX TDR_�ṹ���Ա���������
* @{
*/

/**Ϊ�ṹ���ڵĳ�Ա�������֣�ָ��������
*@param[in,out]	a_pstIndex ��ȡ��Ա���֣�ָ�����������ݽṹָ���ָ��
*@param[in] a_pstMeta	�����ṹ���Ԫ���ݾ��
*@param[in,out] a_pstBuff ������洢����Ϣ������˲���ΪNULL������������ڴ�洢���ں����ڶ�̬���䣻�����������Դ˲�����ָ����������Ϊ�洢��
	- ����	a_pstBuff.pszbuff ָ�����������׵�ַ
	- ����	a_pstBuff.iBuff ָ���������Ŀ��ÿռ��С���ֽ�����
	- ���  a_pstBuff.iBuff �������������ʹ�õĻ������ռ��С
*@note ��Ա���֣�ָ������������ǰ���ǽṹ���г�Ա�����ֲ��ظ���Ŀǰ��TDR֧�ֵ�ORMϵͳ�еĽṹ���ϴ�Լ��
*@retval 0   ����ɹ�
*@retval <0  ����ʧ��,�������
*	-	TDR_ERROR_NO_MEMORY �����ڴ�ʧ�ܣ���ָ���Ļ�����ָ��Ϊ�գ���ָ���������ռ䲻��
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e a_pstMeta ����ΪNULL
*/
TDR_API int tdr_create_entry_name_index(INOUT LPTDRMETAENTRYNAMEINDEX *a_pstIndex, IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstBuff);

/**���ٳ�Ա���֣�ָ��������
*@param[in,out] a_pstIndex ָ���Ա���֣�ָ�����������ݽṹָ���ָ��
*@note �������������ʱû��ָ���洢�������ģ���������Ĵ洢�ռ����ڽӿ��ڷ���ģ��������ô˽ӿ����ͷſռ�
*/
TDR_API void tdr_destroy_entry_name_index(INOUT LPTDRMETAENTRYNAMEINDEX *a_pstIndex);

/**�Կ��ӻ��ķ�ʽ����������
*@param[in] a_pstIndex ָ���Ա���֣�ָ�����������ݽṹ��ָ��
*@param[in] a_fpFile ���浼����Ϣ���ļ����
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e a_fpFile ����ΪNULL
*/
TDR_API void tdr_dump_entry_name_index(IN FILE *a_fpFile, IN LPTDRMETAENTRYNAMEINDEX a_pstIndex);

/**���ݳ�Ա�����Ҵ˳�Ա������_ָ��������Ϣ
*@param[in] a_pstIndex ָ���Ա���֣�ָ�����������ݽṹ��ָ��
*@param[in] a_pszName ��Ա�����ַ���ָ��
*@return ����ҵ�����ƥ���������򷵻ش��������ָ�룻���򷵻�NULL
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e  a_pszName ����ΪNULL
*/
TDR_API LPTDRNAMEENTRYINDEXINFO tdr_find_entry_name_index(IN LPTDRMETAENTRYNAMEINDEX a_pstIndex, IN const char *a_pszName);



/**Ϊ�ṹ���ڵĳ�Ա����ָ�룭ƫ��������
*@param[in,out]	a_pstIndex ��ȡ��Աָ�룭ƫ�����������ݽṹָ���ָ��
*@param[in] a_pstMeta	�����ṹ���Ԫ���ݾ��
*@param[in,out] a_pstBuff ������洢����Ϣ������˲���ΪNULL������������ڴ�洢���ں����ڶ�̬���䣻�����������Դ˲�����ָ����������Ϊ�洢��
- ����	a_pstBuff.pszbuff ָ�����������׵�ַ
- ����	a_pstBuff.iBuff ָ���������Ŀ��ÿռ��С���ֽ�����
- ���  a_pstBuff.iBuff �������������ʹ�õĻ������ռ��С
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e a_pstMeta ����ΪNULL
s
*@retval 0   ����ɹ�
*@retval <0  ����ʧ��
*/
TDR_API int tdr_create_entry_off_index(INOUT LPTDRMETAENTRYOFFINDEX *a_pstIndex, IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstBuff);

/**���ٳ�Աָ��_ƫ��������
*@param[in,out]	a_pstIndex ָ���Աָ�룭ƫ�����������ݽṹָ���ָ��
*@note �������������ʱû��ָ���洢�������ģ���������Ĵ洢�ռ����ڽӿ��ڷ���ģ��������ô˽ӿ����ͷſռ�
*/
TDR_API void tdr_destroy_entry_off_index(INOUT LPTDRMETAENTRYOFFINDEX *a_pstIndex);

/**�Կ��ӻ��ķ�ʽ����������
*@param[in]	a_pstIndex ָ���Աָ�룭ƫ�����������ݽṹ��ָ��
*@param[in]	a_fpFile ���浼����Ϣ���ļ����
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e a_fpFile ����ΪNULL
*/
TDR_API void tdr_dump_entry_off_index(IN FILE *a_fpFile, IN LPTDRMETAENTRYOFFINDEX a_pstIndex);

/**���ݳ�ԱԪ����ָ����Ҵ˳�Ա��ָ�룭ƫ��������Ϣ
*@param[in] a_pstIndex ָ���Աָ�룭ƫ�����������ݽṹ��ָ��
*@param[in] a_pstEntry ��ԱԪ����������ָ��
*@return ����ҵ�����ƥ���������򷵻ش��������ָ�룻���򷵻�NULL
*@pre \e  a_pstIndex ����ΪNULL
*@pre \e  a_pszName ����ΪNULL
*/
TDR_API LPTDRENTRYOFFINDEXINFO tdr_find_entry_off_index(IN LPTDRMETAENTRYOFFINDEX a_pstIndex, IN LPTDRMETAENTRY a_pstEntry);


/**����Ԫ���ݳ�Ա�������Ԫ���ݵĻ�ַ����ĳ����Ա�Ĵ洢��ַ
*@param[in]	a_pMetaBase ��Ԫ���ݴ洢�����׵�ַ
*@param[in]	a_pstNameIndex ָ���Ա���֣�ָ�����������ݽṹ��ָ��
*@param[in]	a_pstOffIndex ָ���Աָ�룭ƫ�����������ݽṹ��ָ��
*@param[in] a_pszName ��Ա�����ַ���ָ��
*@return ����ɹ��ҵ�����ƥ��ĳ�Ա,�򷵻ش˳�Ա�Ĵ洢��ַ,���򷵻�NULL
*@pre \e  a_pMetaBase ����ΪNULL
*@pre \e  a_pstNameIndex ����ΪNULL
*@pre \e  a_pstOffIndex ����ΪNULL
*@pre \e  a_pszName ����ΪNULL
*/
TDR_API void *tdr_calc_entry_address(IN void *a_pMetaBase, IN LPTDRMETAENTRYNAMEINDEX a_pstNameIndex,
									 IN LPTDRMETAENTRYOFFINDEX a_pstOffIndex, IN const char *a_pszName);

/** @}*/ /*defgroup TDR_META_ENTRY_INDEX TDR_�ṹ���Ա���������*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

