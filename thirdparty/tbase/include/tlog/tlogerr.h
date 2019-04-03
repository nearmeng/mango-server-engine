/*
**  @file $RCSfile: tlogerr.h,v $
**  general description of this module
**  $Id: tlogerr.h,v 1.1 2009-03-27 06:18:25 kent Exp $
**  @author $Author: kent $
**  @date $Date: 2009-03-27 06:18:25 $
**  @version $Revision: 1.1 $
**  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
**  @note Platform: Linux
*/

#ifndef TLOG_ERROR_H
#define TLOG_ERROR_H

#include "comm/tmodid.h"
#include "comm/terr.h"


/** @defgroup TLOG_ERR TLOG_������
* @{
*@note TLOG������Ļ���˼��:
*  - ʹ��һ���������洢���������Ϣ��������ֵ�ĺ�������:
*      -# <0 ��ʾ����������ֵΪ�������Ĵ���
*      -# 0 ��ʾ�ɹ�
*      -# >0 ��ʾ�ɹ�����������ĳЩ��������
*  - �������洢��4�ֽڵ������У������������:
*      -# ���󼶱�: 1���ֽڣ�����Ϊ��λ�ĵ�1���ֽ�
*      -# ģ��ID:  1�ֽڣ�����Ϊ��λ�ĵ�2���ֽ�
*      -# �����:  2�ֽڣ�0-1023�ű�ʾͨ�ô��󣬸�ģ�鶨������1024��ʼ
* 
*/
	
#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */
	
	
#define TLOG_SUCCESS				0
#define TLOG_ERR_ERROR				-1
	
#define TLOG_ERR_IS_ERROR(error)		TERR_IS_ERROR(error)

#define TLOG_ERR_START_ERRNO		1024
#define TLOG_ERR_MAKE_ERROR(err)    TERR_MAKE_ERROR(TMODID_LOG, (TLOG_ERR_START_ERRNO + err))

		
		typedef enum
		{
			TLOG_ERROR_NONE = 0, 				 /**< û�д���*/
			TLOG_ERROR_FILE_OPEN,	             /**< ��־�ļ��򿪳��� */
			TLOG_ERROR_NET_CONN,                 /**< ����������ʧ�� */
			TLOG_ERROR_LOOP,                     /**< forward ��ϵ��ѭ��*/
			TLOG_ERROR_COUNT,                    /**< ������������־*/ 
		}TLOGERROR;

		/**
		* ���ݴ�������ȡ������Ϣ
		* @param[in] iErrorCode �������
		*
		* @return  ������Ϣ����ָ��
		*/
		extern char const*	tlog_error_string( int iErrorCode);
	
		/** @} */ // TLOG_ERR TLOG������
	
#ifdef __cplusplus
	}
#endif
	
	
	
	
#endif /* TLOG_ERROR_H */

