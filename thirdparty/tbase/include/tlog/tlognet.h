/**
*
* @file    tlognet.h  
* @brief   ��־����ģ��
* 
* @author steve
* @version 1.0
* @date 2007-04-05 
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/

#ifndef TLOGNET_H
#define TLOGNET_H

#include "tlogio.h"
#include "tlognetdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct tagTLogDevNetInst
{
	int s;
	int iType; 
	int iCurRetry;
	char *pszBuff;
	int iIsSendNetSucc; /*�����ж���־�Ƿ��͵�SOCKET�������еı�־λ������:���ͳɹ������ʾʧ��*/
};

typedef struct tagTLogDevNetInst	TLOGDEVNETINST;
typedef struct tagTLogDevNetInst	*LPTLOGDEVNETINST;

struct tagTLogNet
{
	TLOGDEVNET* pstDev;
	TLOGDEVNETINST stInst;
};

typedef struct tagTLogNet		TLOGNET;
typedef struct tagTLogNet		*LPTLOGNET;

/**
* ��ʼ��������
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pstDev ��־����ģ������
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_init(TLOGNET* a_pstLogNet, TLOGDEVNET* a_pstDev);

/**
* ��������д����־����
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pszBuff ��Ҫд����ַ�������
* @param[in] a_iBuff ��Ҫд����ַ�������
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_write(TLOGNET* a_pstLogNet, const char* a_pszBuff, int a_iBuff);

/**
* �ͷ�������
* @param[in,out] a_pstLogNet ��־����ģ����
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_fini(TLOGNET* a_pstLogNet);

/**
* ��������д������
* @param[in,out] a_pstLogNet ��־����ģ����
* @param[in] a_pstIOVec д����ַ�����������
* @param[in] a_iCount д����ַ������������С
*
* @return  0 �ɹ�
*			��0 ʧ��
*/
int tlognet_writev(TLOGNET* a_pstLogNet, const TLOGIOVEC* a_pstIOVec, int a_iCount);

#ifdef __cplusplus
}
#endif

#endif /* TLOGNET_H */


