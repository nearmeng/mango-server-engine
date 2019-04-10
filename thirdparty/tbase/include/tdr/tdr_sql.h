/**
*
* @file     tdr_sql.h
* @brief    TSF-G-DR DB��ϴ�����
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


#ifndef TSF_G_TDR_SQL_H
#define TSF_G_TDR_SQL_H

#include <stdio.h>
#include "tdr/tdr_external.h"
#include "tdr/tdr_types.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup TDR_CREATE_TABLE TDR_DB_����Ԫ���������������ݶ�������DDL
* @{
*/

/**����Ϊ�ض�DBMS���õġ�����ض�Ԫ���ݰ汾�Ľ������
*���ɵĽ�����䱣�����ַ�����������
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in,out]	a_pstSql �洢�������Ľṹ��ָ��
*	- ����	a_pstSql.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstSql.iBuff	ָ���������Ĵ�С
*	- ���  a_pstSql.iBuff  ʵ��������Ϣ���ܳ���
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e a_pstDBMS ����ΪNULL
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstSql ����Ϊ NULL
* @pre \e a_pstSql.pszBuff ����Ϊ NULL
* @pre \e a_pstSql.iBuff �������0

*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_create_table(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstSql, IN int a_iVersion);

/**����Ϊ�ض�DBMS���õġ�����ض�Ԫ���ݰ汾�Ľ������
*���ɵĽ�����䱣����ָ���ļ���
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_fpSql �洢���������ļ����
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e a_pstDBMS ����ΪNULL
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_fpSql ����Ϊ NULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_create_table_fp(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, IN FILE *a_fpSql, IN int a_iVersion);

/**����Ϊ�ض�DBMS���õġ�����ض�Ԫ���ݰ汾�Ľ������
*���ɵĽ�����䱣����ָ���ļ���
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_pszSqlFile �洢���������ļ���ָ��
*@param[in] a_iVersion	Ҫ���õİ汾��
*
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pszSqlFile ����Ϊ NULL
* @pre \e a_pstDBMS ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_create_table_file(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, IN const char *a_pszSqlFile, IN int a_iVersion);

/**����Ϊ�ض�DBMS���õġ���Դ��ض�Ԫ���ݰ汾����ǰ���°汾�ı��޸����
*���ɵĽ�����䱣�����ַ�����������
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in,out]	a_pstSql �洢�������Ľṹ��ָ��
*	- ����	a_pstSql.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstSql.iBuff	ָ���������Ĵ�С
*	- ���  a_pstSql.iBuff  ʵ��������Ϣ���ܳ���
*@param[in] a_iVersion	�ض�Ԫ���ݰ汾
*
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersion���ڵ���Ԫ���ݵ���߰汾����ֱ�ӷ���
*@note Ŀǰ���ӿ�ֻ֧��:��ԭ�����ݱ������������,�޸�������unique��
*@note ���a_iVersionΪ0 ����װ���õ���߰汾
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e ����a_iVersion�汾��Ԫ����������Ӧ�����ݿ���Ѿ�����
* @pre \e a_pstDBMS ����ΪNULL
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pstSql ����Ϊ NULL
* @pre \e a_pstSql.pszBuff ����Ϊ NULL
* @pre \e a_pstSql.iBuff �������0

*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_alter_table(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, INOUT LPTDRDATA a_pstSql, IN int a_iVersion);

/**����Ϊ�ض�DBMS���õġ���Դ��ض�Ԫ���ݰ汾����ǰ���°汾�ı��޸����
*���ɵĽ�����䱣����ָ���ļ���
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_fpSql �洢���������ļ����
*@param[in] a_iVersion	�ض�Ԫ���ݰ汾
*
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersion���ڵ���Ԫ���ݵ���߰汾����ֱ�ӷ���
*@note Ŀǰ���ӿ�ֻ֧��:��ԭ�����ݱ������������,�޸�������unique��
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e ����a_iVersion�汾��Ԫ����������Ӧ�����ݿ���Ѿ�����
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_fpSql ����Ϊ NULL
* @pre \e a_pstDBMS ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_alter_table_fp(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, IN FILE *a_fpSql, IN int a_iVersion);

/**����Ϊ�ض�DBMS���õġ���Դ��ض�Ԫ���ݰ汾����ǰ���°汾�ı��޸����
*���ɵĽ�����䱣����ָ���ļ���
*@param[in] a_pstDBMS ����DBMS��Ϣ��ָ��
*@param[in]	a_pstMeta Ԫ����������ָ�룬ͨ��tdr_get_meta_by_name��tdr_get_meta_by_id���Դ�Ԫ���ݿ��л�ȡ����ָ��
*@param[in]	a_pszSqlFile �洢���������ļ���ָ��
*@param[in] a_iVersion	�ض�Ԫ���ݰ汾
*
*@note ����a_pstDBMS������ֻҪ��szDBMSName�����������˺����ֵ���ɣ��������ݿ��Բ���ֵ
*@note Ŀǰ֧�ֵ����ݿ����ϵͳDBMS��mysql
*@note ���a_iVersion���ڵ���Ԫ���ݵ���߰汾����ֱ�ӷ���
*@note Ŀǰ���ӿ�ֻ֧��:��ԭ�����ݱ������������,�޸�������unique��
*@note ��֧��ָ����������ݳ�Ա�������ɽ������ʱָ����������ݳ�Ա��������
*@note ��֧��union/struct�����Ա����������ת�������Ҫ�洢union/struct���飬��Ҫ��һ���ṹ���װ�ɵ�һ�ķ������Ա
*@note ��֧��wstring����ָ��ȱʡֵ
*
* @pre \e ����a_iVersion�汾��Ԫ����������Ӧ�����ݿ���Ѿ�����
* @pre \e a_pstMeta ����Ϊ NULL
* @pre \e a_pszSqlFile ����Ϊ NULL
* @pre \e a_pstDBMS ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*		- TDR_ERROR_INVALID_CUTOFF_VERSION ָ�����ð汾��Ԫ���ݻ�׼�汾ҪС
*
*
* @see	tdr_get_meta_by_name
* @see	tdr_get_meta_by_id
*/
TDR_API int tdr_alter_table_file(IN LPTDRDBMS a_pstDBMS, IN LPTDRMETA a_pstMeta, IN const char *a_pszSqlFile, IN int a_iVersion);


/** @}*/ /*defgroup TDR_CREATE_TABLE TDR_DB_����Ԫ���������������ݶ�������DDL*/



/** @defgroup TDR_ORMAPPING TDR_DB_�����ϵӳ��
* @{
*/

/**����DBMS����Ϣ��DB������
*@param[out] a_phDBHandle ����DBMS��������ָ��
*@param[in]	a_pstDBMS ����DBMSxt��Ϣ�Ľṹ��ָ��
*@param[in] a_pszErrMsg ���������Ϣ�Ļ�����ָ�룬����˲�����ΪNULL��������򿪾��ʧ��ʱ����ͨ���˲�����ȡ������Ϣ
*
*@note ͨ�����ӿڴ򿪵ľ�����������tdr_close_dbhandle�ӿڹرվ������������ڴ�й©
*@note ���ӿڴ����ľ��ֻ����ͬһ���߳���ʹ�ã�������̲߳��ܹ���ͬһ��Э�鴦����
*
* @pre \e a_phDBHandle ����Ϊ NULL
* @pre \e a_pstDBMS ����Ϊ NULL
* @pre \e a_pszErrMsgΪNULLʱ�������������Ϣ��������Ϣ��Ӧ��buff��С��������Ϊ1024�ֽڣ�����̫С���ܻ���Խ��ķ���
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_NO_MEMORY	�������ʱ�ڴ治��
*	- TDR_ERROR_DB_FAILD_TO_CONNECT_SERVER	�������ݿ������ʧ��
*
* @see	tdr_close_dbhandle
*/
TDR_API int tdr_open_dbhanlde(OUT TDRDBHANDLE *a_phDBHandle, IN LPTDRDBMS a_pstDBMS, IN char *a_pszErrMsg);


/**����DBMS����Ϣ����DB������
*@param[out] a_phDBHandle ����DBMS��������ָ��
*@param[in]	a_pstDBMS ����DBMS��Ϣ�Ľṹ��ָ��
*@param[in] a_pszErrMsg ���������Ϣ�Ļ�����ָ�룬����˲�����ΪNULL��������򿪾��ʧ��ʱ����ͨ���˲�����ȡ������Ϣ
*@param[in] a_iSize ���������Ϣ�Ļ�������С
*
*@note ͨ�����ӿڴ����ľ�����������tdr_close_dbhandle�ӿڹرվ������������ڴ�й©
*@note ���ӿڴ����ľ��ֻ����ͬһ���߳���ʹ�ã�������̲߳��ܹ���ͬһ��Э�鴦����
*@note ���ӿڴ����ľ��Ĭ������MYSQL_OPT_READ_TIMEOUT��MYSQL_OPT_WRITE_TIMEOUTֵΪ������
*
* @pre \e a_phDBHandle ����Ϊ NULL
* @pre \e a_pstDBMS ����Ϊ NULL
* @pre \e a_pszErrMsgΪNULLʱ�������������Ϣ
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_NO_MEMORY	�������ʱ�ڴ治��
*
* @see	tdr_close_dbhandle
*/
TDR_API int tdr_new_dbhanlde(OUT TDRDBHANDLE *a_phDBHandle, IN LPTDRDBMS a_pstDBMS, INOUT char *a_pszErrMsg, IN size_t a_iSize);

/**ΪDBMS����������mysqlѡ��MYSQL_OPT_CONNECT_TIMEOUT
*@param[in] a_hDBHandle DBMS������
*@param[in] a_dwTimeOut ���ӳ�ʱʱ��ֵ
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
* @note ����ѡ�����ý�����TDR�����ϣ�ֻ���ڵ���tdr_connect_db��Ż���MYSQL����������Ч
*
* @see tdr_connect_db
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_connect_timeout(IN TDRDBHANDLE a_hDBHandle, IN unsigned int a_dwTimeOut);

/**ΪDBMS����������mysqlѡ��MYSQL_OPT_READ_TIMEOUT
*@param[in] a_hDBHandle DBMS������
*@param[in] a_dwTimeOut ����ʱʱ��ֵ
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
* @note ����ѡ�����ý�����TDR�����ϣ�ֻ���ڵ���tdr_connect_db��Ż���MYSQL����������Ч
*
* @see tdr_connect_db
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_read_timeout(IN TDRDBHANDLE a_hDBHandle, IN unsigned int a_dwTimeOut);

/**ΪDBMS����������mysqlѡ��MYSQL_OPT_WRITE_TIMEOUT
*@param[in] a_hDBHandle DBMS������
*@param[in] a_dwTimeOut д��ʱʱ��ֵ
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
* @note ����ѡ�����ý�����TDR�����ϣ�ֻ���ڵ���tdr_connect_db��Ż���MYSQL����������Ч
*
* @see tdr_connect_db
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_write_timeout(IN TDRDBHANDLE a_hDBHandle, IN unsigned int a_dwTimeOut);

/**ΪDBMS����������mysqlѡ��MYSQL_OPT_RECONNECT
 * DBAǿ�ҽ��鿪���߲�Ҫ��������ѡ���ҵ������ʵ�������߼�
*@param[in] a_hDBHandle DBMS������
*@param[in] a_chBool ����ѡ��ֵ
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
* @note ����ѡ�����ý�����TDR�����ϣ�ֻ���ڵ���tdr_connect_db��Ż���MYSQL����������Ч
*
* @see tdr_connect_db
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_reconnect(IN TDRDBHANDLE a_hDBHandle, IN char a_chBool);

/**ΪDBMS�������ڽ���mysql_real_connectʱ���ñ�־λCLIENT_FOUND_ROWS
 * ���ϣ��UPDATE���affected-numsֵΪƥ�䵽�ģ�found����������ʵ�ʸı�ģ�changed��
 * ��ʹ�øýӿ����÷�0ѡ��ֵ��ҵ�����������Ҫ�����ô�ѡ��
*@param[in] a_hDBHandle DBMS������
*@param[in] a_chBool update affected-nums �Ƿ���CLIENT_FOUND_ROWS��������ο�mysql�ֲ�
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
* @note ����ѡ�����ý�����TDR�����ϣ�ֻ���ڵ���tdr_connect_db��Ż���MYSQL����������Ч
*
* @see tdr_connect_db
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_foundrows(IN TDRDBHANDLE a_hDBHandle, IN char a_chBool);

/**��������ѹ��ʱ��Ҫ�Ļ�������С
*@param[in] a_hDBHandle DBMS������
*@param[in] a_dwSize ��Ҫ�趨�Ļ�������С�����С��1M����ʹ��1M
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
*
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*/
TDR_API int tdr_db_opt_compress_buffer(IN TDRDBHANDLE a_hDBHandle, IN uint32_t a_dwSize);


/**����DBMS����������DBMS
*@param[in] a_hDBHandle DBMS������
*@param[in] a_pszErrMsg ���������Ϣ�Ļ�����ָ�룬����˲�����ΪNULL��������򿪾��ʧ��ʱ����ͨ���˲�����ȡ������Ϣ
*@param[in] a_iSize ���������Ϣ�Ļ�������С
*
* @pre \e a_hDBHandle ����Ϊ NULL����Ҫע���������ΪTDRDBHANDLE
* @pre \e a_pszErrMsgΪNULLʱ�������������Ϣ
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_SET_DB_OPTIONS	�������ݿ�ѡ�����
*	- TDR_ERROR_DB_FAILD_TO_CONNECT_SERVER	�������ݿ������ʧ��
*/
TDR_API int tdr_connect_db(IN TDRDBHANDLE a_hDBHandle, INOUT char *a_pszErrMsg, IN size_t a_iSize);

/** �������Ӿ������һЩ����߼�
* 1)������Ӷ��ˣ����ؽ�����
*@param[out] a_hDBHandle ����DBMS��������ָ��
*
* @pre \e a_hDBHandle ����Ϊ NULL
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_DB_FAILD_TO_CONNECT_SERVER	�������ݿ������ʧ��
*
* @see	tdr_open_dbhanlde
*/
int tdr_keep_dbmsconnection(IN TDRDBHANDLE a_hDBHandle);


/**�ر�DB������
*@param[in,out] a_phDBHandle ����DBMS������
*
*/
TDR_API void tdr_close_dbhanlde(INOUT LPTDRDBHANDLE a_phDBHandle);

/**����Ԫ���������������ݲ������DML
*@param[in] a_hDBHandle ����DBMS������
*@param[in,out] a_pstSql ����SQL���Ļ������ṹָ��
*	- ����	a_pstSql.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstSql.iBuff	ָ���������Ĵ�С
*	- ���  a_pstSql.iBuff  ʵ��������Ϣ���ܳ���
*@param[in] a_iOPType DB�����ģ�Insert��Update��Delete��Select��ID,����ֵ�Ķ�����ο�TDRDBOPID����
*@param[in,out] a_pstObj ����������ݵĽṹ��ָ��
*	- ����	a_pstObj.pstMeta ָ�������ݵ�Ԫ���ݽṹָ��
*	- ����  a_pstObj.iVersion	ָ�������ݵļ��ð汾
*	- ����  a_pstObj.iObjSize  ָ�������ݵĴ�С
*	- ����  a_pstObj.pszObj  ָ�������ݵĻ�����ָ��
*	- ���  a_pstObj.iObjSize  ������ʵ�ʱ�����Ĵ�С
*@param[in] a_pszWhere �Զ���where����������˲���ΪNULL��մ�������������Ԫ������where����
*
*@note ���a_pstObj.iVersionΪ0���򽫰���߰汾���м���
*@note ���a_pszWhereΪNULL��մ�������������Ԫ������where����,����update,select,delete������������ĳ�Ա
*�����Ѿ���������Ч��ֵ
*@note update�������SET���ֲ���������������еı��ʽ
*
*@note insert���ĸ�ʽ�� INSERT   INTO tbl_name (col_name,...) VALUES (col_val,...)
*@note update���ĸ�ʽ�� UPDATE  tbl_name  SET col_name1=expr1 [, col_name2=expr2 ...]    WHERE where_definition
*@note select���ĸ�ʽ�� SELECT col_name,... FROM  tbl_name  WHERE where_definition��
*@note delete���ĸ�ʽ�� DELETE FROM tbl_name WHERE where_definition
*
*@note �������ʧ�ܣ���ȡ������Ϣ�ķ���:
	*	- ���ݷ���ֵ������tdr_error_string()���Ի�ȡ������Ϣ
    *	- ͨ����ӡ��a_pstSql.pszBuff�����ݣ����Ը����Բ鿴����ʱ��λ��
*
* @pre \e a_hDBHandle ������ͨ��tdr_open_dbhanlde�򿪵ľ��
* @pre \e a_pstSql ����Ϊ NULL
* @pre \e a_pstSql.pszBuff ����Ϊ NULL
* @pre \e a_pstSql.iBuff �������0
* @pre \e a_pstObj ����ΪNULL
*@pre \e a_pstObj->pstMeta ����ΪNULL
* @pre \e a_pstObj->pszObj ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ�������
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_INVALID_CUTOFF_VERSION	���ð汾����ȷ
*	- TDR_ERROR_NO_HOSTBUFF_SPACE ����SQL���Ļ������ռ䲻��
*	- TDR_ERROR_DB_NO_PRIMARYKEY	������������WHERE����ʧ�ܣ�����Ԫ����û��ָ���������ԣ�Ҳ���ܸü��ð汾�ƻ���������Ϣ��������
*
* @note ����ýӿڷ���ʧ�ܣ����Ե��� tdr_get_error_detail ȡ����ϸ�Ĵ�����Ϣ
*
*@see TDRDBOPID
*@see tdr_error_string
*@see tdr_get_error_detail
*/
TDR_API int tdr_obj2sql(IN TDRDBHANDLE a_hDBHandle, INOUT LPTDRDATA a_pstSql, IN int a_iOPType,
				INOUT  LPTDRDBOBJECT a_pstObj, IN const char *a_pszWhere);

/**ִ��SQL��䣬���SQL��ѯ���᷵�ؽ�������򽫸ý�������������أ���ͨ������������������
*@param[out] a_phDBResult ����DBMS�����������ָ��
*@param[in] a_hDBHandle ����DBMS������
*@param[in] a_pstSql ����SQL���Ļ������ṹָ��
*	- ����	a_pstSql.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstSql.iBuff	ָ���������Ĵ�С
*
*@note һ������˶Խ�����Ĳ������������tdr_free_dbresult�ͷŽ�����,���ͷ���Դ
* @pre \e a_phDBResult ����ΪNULL
* @pre \e a_hDBHandle ������ͨ��tdr_open_dbhanlde�򿪵ľ��
* @pre \e a_pstSql ����Ϊ NULL
* @pre \e a_pstSql.iBuff �������0
* @pre \e a_pstObj ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*
*@see tdr_free_dbresult
*@see tdr_dbms_errno
*/
TDR_API int tdr_query(INOUT TDRDBRESULTHANDLE *a_phDBResult, IN TDRDBHANDLE a_hDBHandle, IN LPTDRDATA a_pstSql);

/**ִ��SQL��䣬�����SQL��ѯ���᷵�ؽ��������ִֻ�г�ʼ������������ȡ���κ�������
*ͨ���������ؽ�������
*@param[out] a_phDBResult ����DBMS�����������ָ��
*@param[in] a_hDBHandle ����DBMS������
*@param[in] a_pstSql ����SQL���Ļ������ṹָ��
*	- ����	a_pstSql.pszBuff ָ�����汾�ؽṹ��Ϣ�Ļ�������ʼ��ַ
*	- ����  a_pstSql.iBuff	ָ���������Ĵ�С
*
*@note һ������˶Խ�����Ĳ������������tdr_free_dbresult�ͷŽ�����,���ͷ���Դ
*@note ���ӿڲ���tdr_query()�����������ʵ�ʶ�ȡ���ͻ��ˣ�����ȡ�������еĹ���ͨ��һϵ�е�tdr_fetch_now������ɣ�
* ���tdr_query_quickִ���ٶȸ������ʹ�õ��ڴ�Ҳ���١�����tdr_query_quick���������ݿ�������ĸ���������������ѽ������
*�����ݱ��浽�ͻ��˳�������Ƕ��������ͻ���Ϊֹ�����tdr_query_quick���ʺ������¼��ֿͻ��˳����е��ã�
*	- �����û���������������й����ݵĽ���ʽ�ͻ��˳���
*	- ��ǰ�������������м�������֮����Ҫ��������Ŀͻ��˳���
*
* @pre \e a_phDBResult ����ΪNULL
* @pre \e a_hDBHandle ������ͨ��tdr_open_dbhanlde�򿪵ľ��
* @pre \e a_pstSql ����Ϊ NULL
* @pre \e a_pstSql.iBuff �������0
* @pre \e a_pstObj ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*
*@see tdr_free_dbresult
*@see tdr_dbms_errno
*/
TDR_API int tdr_query_quick(INOUT TDRDBRESULTHANDLE *a_phDBResult, IN TDRDBHANDLE a_hDBHandle, IN LPTDRDATA a_pstSql);


/**��ȡ������еļ�¼��
*@param[in] a_hDBResult ���������ľ��
*
*@note tdr_num_rows()��ʹ��ȡ�����Ƿ������tdr_query()��tdr_query_quick()�����ؽ������
*���ʹ����tdr_query()���������̵���tdr_num_rows()�����ʹ����tdr_query_quick()��tdr_num_rows()��������ȷ��ֵ��ֱ�������˽�����е�������Ϊֹ��
*
*@return ������е�����
*@pre \e a_hDBResult ����ΪNULL
*/
TDR_API unsigned long tdr_num_rows(IN TDRDBRESULTHANDLE a_hDBResult);


/**�����ϴ�UPDATE���ĵ��������ϴ�DELETEɾ�������������ϴ�INSERT�����������
*����UPDATE��DELETE��INSERT��䣬����tdr_query()�����̵��á�
*����SELECT��䣬tdr_affected_row()�Ĺ�����ʽ��tdr_num_rows()����
*
*@param[in] a_hDBResult ���������ľ��
*
*@note tdr_affected_rows()��ʹ��ȡ�����Ƿ������tdr_query()��tdr_query_quick()�����ؽ������
*���ʹ����tdr_query()���������̵���tdr_affected_rows()�����ʹ����tdr_query_quick()��tdr_affected_rows()��������ȷ��ֵ��ֱ�������˽�����е�������Ϊֹ��
*
*@return ����0������������Ӱ��������������
* - 0	��ʾUPDATE���δ���¼�¼���ڲ�ѯ��û����WHEREƥ����У���δִ�в�ѯ��
* - -1 ��ʾ��ѯ���ش��󣬻��ߣ�����SELECT��ѯ���ڵ���tdr_query()֮ǰ������tdr_affected_rows()��
**@pre \e a_hDBResult ����ΪNULL
*@see tdr_num_rows
*/
TDR_API long tdr_affected_rows(IN TDRDBRESULTHANDLE a_hDBResult);

/** ����Ԫ���������������������һ�����ݣ�������ָ�����ð汾�����ݶ�ȡ��ָ����������
*@param[in] a_hDBResult ����DB����������ľ��
*@param[in,out] a_pstObj ��������������ݵĽṹ��ָ��
*	- ����	a_pstObj.pstMeta ָ�������ݵ�Ԫ���ݽṹָ��
*	- ����  a_pstObj.iVersion	ָ�������ݵļ��ð汾
*	- ����  a_pstObj.iObjSize  ָ����������ݻ������Ĵ�С
*	- ����  a_pstObj.pszObj  ָ����������ݻ�������ָ��
*	- ���  a_pstObj.iObjSize  ������ʵ�ʵĴ�С
*
*@note ���a_pstObj.iVersionΪ0�����յ�ǰ��߰汾���м���
*@note ���ĳ��������Ŀռ䲻�����Լ����������ݽ��нضϴ���
*@note ����ṹ��ĳ����Ա�����ݼ�¼��ΪNULL���򽫴�����������Ϊȱʡֵ
*
* @pre \e a_hDBResult ������ͨ��tdr_query��tdr_query_quick��ȡ�Ľ�����
* @pre \e a_pstObj ����ΪNULL
* @pre \e	a_pstObj.pstMeta ����ΪNULL
* @pre \e   a_pstObj.iObjSize������ڻ��������Ԫ��������Ĵ�ȡ�ռ�
* @pre \e	a_pstObj.pszObj  ����ΪNULL
*
*@retval 0   ����ɹ�
*@retval >0  ����ɹ���������ĳЩ���������
*	- TDR_SUCWARN_TRUNCATE_DATE ��������ʱ���������ݽض�
*@retval <0  ����ʧ�ܣ����ر�ʾ������Ϣ�Ĵ�����룺
*	- TDR_ERROR_INVALID_CUTOFF_VERSION	���ð汾����ȷ
*	- TDR_ERROR_DB_NO_RESULT_SET �����Ϊ�գ���ȷ���˽ӿ���tdr_query��tdr_query_quick֮��ʹ��
*	- TDR_ERROR_DB_NO_RECORD_IN_RESULTSET û�и���Ҫ�������л�����˴���(��dr_query_quick֮��ʹ��)
*	- TDR_ERROR_DB_NO_EXPECTED_FIELD �ض���Աû�г��������ݼ�¼�У���ȷ��
*	- TDR_ERROR_INVALID_REFER_VALUE	Ԫ�ص�refer����ֵ����ȷ
*	- TDR_ERROR_DB_UNSUPPORTED_COMPOSITE_ARRAY ��֧��struct����Ĵ�ȡ
*	- TDR_ERROR_TOO_COMPLIEX_META	Ԫ���ݸ��ϲ��̫��
*	- TDR_ERROR_NO_HOSTBUFF_SPACE	�������ݵĻ���������
*/
TDR_API int tdr_fetch_row(IN  TDRDBRESULTHANDLE a_hDBResult, INOUT  LPTDRDBOBJECT a_pstObj);

/**�ͷŽ�����
*@param[in] a_phDBResult ����DBMS�����������ָ��
*@see tdr_free_dbresult
*/
TDR_API void tdr_free_dbresult(IN TDRDBRESULTHANDLE *a_phDBResult);

/**��ȡDBMS�ײ�����Ĵ������
*@param[in] a_hDBHandle ����DBMS������
*
*@note tdr_query�Ȳ���ִ��ʧ��ʱ�����Ի�ȡDBMS��Ĵ�����Ϣ
*
*@return ���ʧ�ܣ������ϴ�mysql_xxx()���õĴ�����롣��0����ʾδ���ִ���
*@pre \e a_hDBHandle ����ΪNULL
*/
TDR_API int tdr_dbms_errno(IN TDRDBHANDLE a_hDBHandle);

/**��ȡDBMS�ײ�����Ĵ�����Ϣ
*@param[in] a_hDBHandle ����DBMS������
*
*@note tdr_query�Ȳ���ִ��ʧ��ʱ�����Ի�ȡDBMS��Ĵ�����Ϣ
*
*@return ������������ġ���Null�ս���ַ��������δ���ִ��󣬷��ؿ��ַ�����
*@pre \e a_hDBHandle ����ΪNULL
*/
TDR_API const char *tdr_dbms_error(IN TDRDBHANDLE a_hDBHandle);


/**���ش˴����������һ��ִ�����ݿ��ѯ���������ɵ�AUTO_INCREMENTֵ���ڰ���AUTO_INCREMENT�ֶεı�
*��ִ����INSERT����Ӧʹ�øú�����ȡ����AUTO_INCREMENT�е�ֵ��

*@param[in] a_hDBHandle ����DBMS������
*
*@note ���ǰ������δʹ��AUTO_INCREMENT��mysql_insert_id()����0�������Ҫ����ֵ��������ֵ������
������̵���tdr_dbms_insert_id()
*@pre \e a_hDBHandle ����ΪNULL
*
*@return ǰ���Ѿ������Ѿ�����
*/
TDR_API tdr_ulonglong tdr_dbms_insert_id(IN TDRDBHANDLE a_hDBHandle);

/**��ȡ���ݱ�������ָ����ѯ�����ļ�¼����
*@param[in] a_hDBHandle  ����DBMS������
*@param[in] a_pszTableName  �������ݿ�������ַ���ָ��
*@param[in] a_pszWhereDef ��ѯ����������ָ��
*@param[out] a_pdwCount  ��ȡ�ܼ�¼����
*
*@pre \e a_hDBHandle����ΪNULL
*@pre \e a_pszTableName����ΪNULL
*@pre \e a_pdwCount ����ΪNULL
*@note	���a_pszWhereDefΪNULL���򱾽ӿڽ���ѯ�����ݿ���м�¼�е�������
*	���൱��ָ������"where 1"
*
*@return 0: success,  !0: failed
*@pre \e a_hDBHandle ����ΪNULL

*/
TDR_API int  tdr_get_records_count(IN TDRDBHANDLE a_hDBHandle, IN const char *a_pszTableName,
								   IN const char *a_pszWhereDef, OUT unsigned int *a_pdwCount);



/** @}*/ /*defgroup TDR_CREATE_TABLE TDR_DB_�����ϵӳ��*/
#ifdef __cplusplus
}
#endif


#endif /* TSF_G_TDR_SQL_H */
