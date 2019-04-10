/**
 *
 * @file     shtable.h
 * @brief    ��ϣ��ʵ��
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

#ifndef SHTABLE_H
#define SHTABLE_H

/*************************************************************
 #FUNCTION:	static hash table defintion.
 #AUTHOR:	Niezhiming
 #DATE:		14-7-2005
 *************************************************************/
#include <stdio.h>
#include "pal/ttypes.h"

#if defined(_WIN32) || defined(_WIN64)

#endif/*#if defined(_WIN32) || defined(_WIN64)*/

#ifdef __cplusplus
extern "C"
{
#endif

struct tagSHtable;
typedef struct tagSHtable *LPSHTABLE;

typedef int (*SHT_CMP)(const void* pvData1, const void* pvData2);
typedef unsigned int (*SHT_HASHCODE)(const void* pvData);
typedef int (*SHT_PRINT)(FILE* fp, const void* pvData);

/** The default method for converting string to int.
 *@param[in]	 szKey  a string end with '\0'

 *@retval  hash code
 */
unsigned int sht_get_code(const char* szKey);

/**����shtable�Ĵ�С
 *@param[in]	 iBuck  Ͱ��
 *@param[in]  iMax   Ԫ�ظ���
 *@param[in]  iUnit  ÿ��Ԫ�صĴ�С
 *@retval  0  ʧ��

 */
size_t sht_get_size(size_t iBuck, size_t iMax, size_t iUnit);

/**����ڴ��Ƿ����shtable�ĸ�ʽ��һ����   ����attach shtable shm ʱʹ��
 *@param[in]	 pvBuff  �ڴ��ַָ��
 *@param[in]  iBuff   �ڴ��С
 *@param[in]  iBucket  Bucket num
 *@param[in]  iMax    Max item num
 *@param[in]  iUnit   ÿ��item�Ĵ�С
 *@retval  0  �ɹ�
 -1  ʧ��
 */
int sht_check(void* pvBuff, size_t iBuff, size_t iBucket, size_t iMax,
		size_t iUnit);

/**���ڴ�������һ��shtable
 *@param[in]      iBucket  Bucket num
 *@param[in]      iMax    Max item num
 *@param[in]      iUnit   ÿ��item�Ĵ�С
 *@param[in,out]  piBuff  ָ�����ɵ�shtable�Ĵ�С��ָ��

 *@retval  not NULL  �ɹ�shtable ָ��
 NULL  ʧ��

 *@note  �˺�����sht_destroy ���ʹ��
 */
LPSHTABLE sht_create(size_t iBucket, size_t iMax, size_t iUnit, size_t* piBuff);

/**��ʼ��shtable �ڴ�
 *@param[in]	 pvBuff  �ڴ��ַָ��
 *@param[in]  iBuff   �ڴ��С
 *@param[in]  iBucket  Bucket num
 *@param[in]  iMax    Max item num
 *@param[in]  iUnit   ÿ��item�Ĵ�С

 *@retval  not NULL  �ɹ�shtable ָ��
 NULL  ʧ��
 */
LPSHTABLE
sht_init(void* pvBuff, size_t iBuff, size_t iBucket, size_t iMax, size_t iUnit);

/**attach shtable �ڴ棬ʵ������ִ��sht_check����ڴ��ٷ���shtable ָ��
 *@param[in]	 pvBuff  �ڴ��ַָ��
 *@param[in]  iBuff   �ڴ��С
 *@param[in]  iBucket  Bucket num
 *@param[in]  iMax    Max item num
 *@param[in]  iUnit   ÿ��item�Ĵ�С

 *@retval  not NULL  �ɹ�shtable ָ��
 NULL  ʧ��
 */
LPSHTABLE sht_attach(void* pvBuff, size_t iBuff, size_t iBucket, size_t iMax, size_t iUnit);

/**insert unique item into shtable
 *@param[in]	 pstTab  shtable ָ��
 *@param[in]  pvData  insert item data ָ��
 *@param[in]  pfnCmp  �Ƚ�����item data �ĺ���ָ��, �˱ȽϺ�����
 ����ֵΪ=0 ��item data��ͬ, <0 ��>0 ��item data����ͬ
 *@param[in]  pfnHashCode  ����item data ����hash code �ĺ���ָ��

 *@retval  not NULL  �ɹ���shtable �ڴ��data�ĵ�ַָ��
 NULL      ʧ��
 *@note    �˺������óɹ���ֻ����shtable�ڷ�����һ���ڴ棬��Ҫ�ⲿ
 ��Ҫ�����������copy��shtable�У��˷�ʽ��Ϊ����������ԣ�
 �ɲμ��ĵ��еĴ���ʾ��
 */
void* sht_insert_unique(LPSHTABLE pstTab, const void* pvData, SHT_CMP pfnCmp,
		SHT_HASHCODE pfnHashCode); /* insert a item into hashtable, keep it unique.*/

/** insert a item into hashtable, do not keep unique.*/
void* sht_insert_multi(LPSHTABLE pstTab, const void* pvData,
		SHT_HASHCODE pfnHashCode);

/**find item in shtable
 *@param[in]	 pstTab  shtable ָ��
 *@param[in]  pvData  find item data ָ��
 *@param[in]  pfnCmp  �Ƚ�����item data �ĺ���ָ��, �˱ȽϺ�����
 ����ֵΪ=0 ��item data��ͬ, <0 ��>0 ��item data����ͬ
 *@param[in]  pfnHashCode  ����item data ����hash code �ĺ���ָ��

 *@retval  not NULL  �ɹ���shtable �ڴ��data�ĵ�ַָ��
 NULL      ʧ��

 *@note   pvDataֻ��Ҫ������pfnCmp��pfnHashCode ��صĲ��ּ���
 */
void* sht_find(LPSHTABLE pstTab, const void* pvData, SHT_CMP pfnCmp, SHT_HASHCODE pfnHashCode);

/**remove item in shtable
 *@param[in]	 pstTab  shtable ָ��
 *@param[in]  pvData  remove item data ָ��
 *@param[in]  pfnCmp  �Ƚ�����item data �ĺ���ָ��, �˱ȽϺ�����
 ����ֵΪ=0 ��item data��ͬ, <0 ��>0 ��item data����ͬ
 *@param[in]  pfnHashCode  ����item data ����hash code �ĺ���ָ��

 *@retval  not NULL  �ɹ���shtable �ڴ��data�ĵ�ַָ��
 NULL      ʧ��

 *@note   pvDataֻ��Ҫ������pfnCmp��pfnHashCode ��صĲ��ּ���
 */
void* sht_remove(LPSHTABLE pstTab, const void* pvData, SHT_CMP pfnCmp, SHT_HASHCODE pfnHashCode);

/**remove item in shtable by pos
 *@param[in]	 pstTab  shtable ָ��
 *@param[in]  iPos    remove item data ��shtable ��item list �е�����λ��

 *@retval  not NULL  �ɹ���shtable �ڴ��data�ĵ�ַָ��
 NULL      ʧ��

 *@note   item list ��һ�����飬������>=0 , <pstTab->iMax
 */
void* sht_remove_by_pos(LPSHTABLE pstTab, int iPos);

int sht_remove_by_dataptr(LPSHTABLE pstTab, void *pvData);

/**get item in shtable by iPos
 *@param[in]	 pstTab  shtable ָ��
 *@param[in]  iPos    item data ��shtable ��item list �е�����λ��
 *@param[in,out] pfValid  ��ʶ��item data �Ƿ���Ч, *pfValid = 1 ��Ч��=0 ��Ч

 *@retval  not NULL  �ɹ���shtable �ڴ��data�ĵ�ַָ��
 NULL      ʧ��

 *@note   item list ��һ�����飬������>=0 , <pstTab->iMax

 */
void* sht_pos(LPSHTABLE pstTab, int iPos, int* pfValid);

size_t sht_get_max_items(LPSHTABLE pstTab);
/*
 * get the num of used items in the sht
 * @param[in]	 pstTab  shtable ָ��
 * @retval Num of used items in the sht.
 * */
size_t sht_get_used_items(LPSHTABLE pstTab);

/**��sht_create���ʹ�ã��ͷ���Դ
 *@param[in,out]	 ppstTab  shtable ָ���ָ��
 *@retval NULL �ɹ�
 not NULL ʧ�ܣ����᷵��ʧ�ܣ�
 */
int sht_destroy(LPSHTABLE* ppstTab);

/**�ؽ�item ���������bucket ��������ʹ��
 *@param[in,out]	 pstTab  shtable ָ��
 *@retval NULL �ɹ�
 not NULL ʧ�ܣ����᷵��ʧ�ܣ�
 */
int sht_rebuild(LPSHTABLE pstTab);

/**�ж�shtable item data�ռ��Ƿ�Ϊ��, ����1 Ϊ��, 0 ����*/
int sht_is_empty(LPSHTABLE pstTab);

/**�ж�shtable item data�ռ��Ƿ�����, ����1 ����, 0 δ��*/
int sht_is_full(LPSHTABLE pstTab);

/**����shtable��item list �е��������ÿһ��valid item data ����pfnPrint(fp, pvData)*/
int sht_dump_all(LPSHTABLE pstTab, FILE* fp, SHT_PRINT pfnPrint);

/**����shtable��item list �е�����valid item���ÿһ��item data ����pfnPrint(fp, pvData)*/
int sht_dump_valid(LPSHTABLE pstTab, FILE* fp, SHT_PRINT pfnPrint);

/**��֪item hashֵǿ�Ʋ��룬���������ʹ�ã�by kent 2008.1.18*/
void *sht_insert_force(LPSHTABLE pstTab, unsigned int uCode);

void *sht_get_by_pos(LPSHTABLE pstTab, size_t iIdx);

/**ͨ�����ݻ�ȡԪ��pos
 *@param pvData  sht_insert* ���ص�ָ��
 *@retval 0 �ɹ��� -1 ʧ��
 */
int sht_get_pos(LPSHTABLE pstTab, void* pvData);

#ifdef __cplusplus
}
#endif

#endif /* SHTABLE_H */
