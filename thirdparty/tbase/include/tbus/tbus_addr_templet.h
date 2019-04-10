/**
*
* @file     tbus_addr_templet.h
* @brief    BUSͨ�ŵ�ַģ���������ؽӿ�
* Busϵͳ����ͨ�ŵ�ַ�����ֱ�ʾ������
*	1.�ڲ���ʾ��32λ�޷�������
*	2.�ⲿ��ʾ�����ñ�ʾ��ʮ���Ƶ�ֱ�ʾ���ַ��� xx.xx.xx.xxx
*
* @author jackyai
* @version 1.0
* @date 2008-09-10
*
*
* Copyright (c)  2008, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/
#ifndef TBUS_ADDR_TEMPLET_H
#define	TBUS_ADDR_TEMPLET_H

#include <stdio.h>
#include "tbus/tbus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TBUS_ADDR_TEMPLET_MAX_BITS_NUM	32
#define TBUS_MAX_ADDR_SEGMENT_NUM	32	/*Tbusͨ�ŵ�ַʮ���Ʊ�ʾ�����������Ŀ*/
#define TBUS_SPLIT_SEGMENT_CHARACTER	'.'	/*ʮ���Ƶ�ַ��зָ�ÿһ�ڵķָ��ַ�*/
#define TBUS_SPLIT_SEGMENT_DESCRITION_CHARACTER	':'	/*ʮ���Ƶ�ַ��зָ�ÿһ���������������ֵķָ��ַ�*/



typedef struct tagTbusAddrSeg TBUSHANDLEEG;
typedef struct tagTbusAddrSeg *LPTBUSHANDLEEG;
typedef struct tagTbusAddrTemplet TBUSADDRTEMPLET;
typedef struct tagTbusAddrTemplet *LPTBUSADDRTEMPLET;

typedef struct tagTbusAddrSegRange TBUSADDRSEGRANGE;
typedef struct tagTbusAddrSegRange *LPTBUSADDRSEGRANGE;
typedef struct tagTbusAddrInstance TBUSADDRINSTANCE;
typedef struct tagTbusAddrInstance *LPTBUSADDRINSTANCE;


struct tagTbusAddrSeg
{
	unsigned char bStartBit;	/*��һ����ռ��bit�ӵڼ�λ��ʼ���趨��31λΪ���λ����0λΪ���λ*/
	unsigned char bBitNum;		/*��һ����ռ��bit��*/
	unsigned int dwMaxVal;		/*��һ���ܱ�ʾ�����ֵ,����һ����ֵΪ0��dwMaxVal��dwMaxVal=2^bBitNum - 1*/
};

/*ͨ�ŵ�ַʮ���Ƶ�ַ�ģ����*/
struct tagTbusAddrTemplet
{
	int iSegNum;	/**/
	TBUSHANDLEEG astSeg[TBUS_MAX_ADDR_SEGMENT_NUM];
};


/*ͨ�ŵ�ַһ�εı�ʾ��Χ*/
struct tagTbusAddrSegRange
{
    int iBegin;
    int iEnd;
};

/*ͨ�ŵ�ַ*/
struct tagTbusAddrInstance
{
    int iSegNum;
    TBUSADDRSEGRANGE astSegRange[TBUS_MAX_ADDR_SEGMENT_NUM];
};


/**/



/**����ͨ�ŵ�ַʮ���Ƶ��ģ�崮������ͨ�ŵ�ַģ���ڲ��ṹ
* ���ʮ���Ʊ�ʾ����ģ��Ϊ��"segmentdesc:bitnum.segmendesc:binnum.....segmentdesc:bitnum",����
* "world:8.zone:8.func:8.instance:8"
*@param[out] a_pstAddrTemplet ����ͨ�ŵ�ַ��ַ���ʾģ����Ϣ�����ݽṹָ��
*@param[in] a_pszAddTemplet ʮ���Ƶ�ַ���ʾ��ͨ�ŵ�ַģ��
*@retval 0	����ɹ�
*@retval <0 ����ʧ��
*/
int tbus_parse_addrtemplet(OUT LPTBUSADDRTEMPLET a_pstAddrTemplet, IN const char *a_pszAddTemplet);

void tbus_dump_addrtemplet(IN LPTBUSADDRTEMPLET a_pstAddrTemplet, IN FILE *a_fpDump);

/**���ݵ�ַģ�����ý��ڲ�ͨ�ŵ�ַ ת���ɵ��ʮ���Ʊ�ʾ�ĵ�ַ��
*@param[in] a_pstTemplet ͨ�ŵ�ַģ��ṹ��ָ��
*@param[in] a_iAddr	ͨ�ŵ�ַ
*@return �ɵ��ʮ���Ʊ�ʾ�ĵ�ַ��
*@note ���صĳɵ��ʮ���Ʊ�ʾ�ĵ�ַ��������һ����̬�������У���һ�ε��ûḲ��ǰһ�ε���ʱ��ȡ����Ϣ
*/
char *tbus_addr_nota_by_addrtemplet(IN LPTBUSADDRTEMPLET a_pstTemplet,IN TBUSADDR a_iAddr) ;

/**���ݵ�ַģ�����ý��ڲ�ͨ�ŵ�ַ ת���ɵ��ʮ���Ʊ�ʾ�ĵ�ַ��
*@param[in] a_pstTemplet ͨ�ŵ�ַģ��ṹ��ָ��
*@param[in] a_iAddr	ͨ�ŵ�ַ
*@param[out] a_pszDstBuff ���������
*@param[in] a_dwBuffSize ���뻺��������
*@return �ɵ��ʮ���Ʊ�ʾ�ĵ�ַ��
*/
char *tbus_addr_notp_by_addrtemplet(LPTBUSADDRTEMPLET a_pstTemplet, TBUSADDR a_iAddr,
                                    char *a_pszDstBuff, unsigned int a_dwBuffSize);

/**����ͨ�ŵ�ַģ�彫���ʮ���Ʊ�ʾ�ĵ�ַ��ת�����ڲ���ʾͨ�ŵ�ַ
*@param[in] a_pstTemplet ͨ�ŵ�ַģ��ṹ��ָ��
*@param[in] a_pszAddr ���ʮ���Ʊ�ʾ�ĵ�ַ������ȥָ��
*@param[out] a_piAddr ����ͨ�ŵ�ַ��ָ��
*@retval	0 �ɹ�
*@retval <0	ʧ��
*/
int tbus_addr_aton_by_addrtemplet(IN LPTBUSADDRTEMPLET a_pstTemplet, IN const char *a_pszAddr, OUT LPTBUSADDR a_piAddr) ;


#ifdef __cplusplus
}
#endif
#endif /*TBUS_ADDR_TEMPLET_H*/
