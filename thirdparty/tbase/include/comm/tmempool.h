/**
*
* @file     tmempool.h
* @brief    �ڴ��ʵ��
*
* @author kent
* @version 1.0
* @date 2007-04-05
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/

#ifndef TMEMPOOL_H
#define TMEMPOOL_H
#include <stdio.h>
#include <stddef.h>
#include "pal/ttypes.h"

#if defined(_WIN32) || defined(_WIN64)

#endif/*#if defined(_WIN32) || defined(_WIN64)*/

#define TMEMPOOL_FIFO           0x00
#define TMEMPOOL_LIFO           0x01

enum tagTMEMPOOLOPT
{
	TMEMPOOL_SET_ALLOC_METHOD_FIFO,
	TMEMPOOL_SET_ALLOC_METHOD_LIFO,
	TMEMPOOL_SET_PAGE_PROTECT,
	TMEMPOOL_RESET_PAGE_PROTECT,
};

typedef enum tagTMEMPOOLOPT TMEMPOOLOPT;

#ifdef __cplusplus
extern "C"
{
#endif

#define IN
#define OUT
#define INOUT
#define TMEMPOOL_API

struct tagTMemPool;
typedef struct tagTMemPool		TMEMPOOL;
typedef struct tagTMemPool		*LPTMEMPOOL;

/**Caculate the size of mem needed by mempool
*@param[in]      iMax      mempool������
*@param[in]      iUnit     Ӧ�����ݵĴ�С
*@retval  the size of memory required by mempool
*/
TMEMPOOL_API size_t tmempool_calc_size(size_t iMax, size_t iUnit);

/**��ʼ��mempool�ڴ棬�γɿ�������
*@param[in,out]	 ppstPool  mempoolָ���ָ��
*@param[in]      iMax      mempool������
*@param[in]      iUnit     Ӧ�����ݵĴ�С
*@param[in]      pvBase    �����γ�mempool���ڴ��ָ��
*@param[in]      iSize     pvBase �ڴ�Ĵ�С

*@retval  0  �ɹ�
         -1  ʧ��
*/
TMEMPOOL_API int tmempool_init(INOUT TMEMPOOL** ppstPool,IN  int iMax,IN  size_t iUnit,IN void* pvBase,IN size_t iSize);

/**����mempool�ڴ沢��ʼ��
*@param[in,out]	 ppstPool  mempoolָ���ָ��
*@param[in]      iMax      mempool������
*@param[in]      iUnit     Ӧ�����ݵĴ�С

*@retval  0  �ɹ�
         -1  ʧ��

*@note   ��tmempool_destroy���ʹ��
*@note   ��ppstPoolΪNULLʱ, ������Ҫ���ڴ��С
*/
TMEMPOOL_API int tmempool_new(INOUT TMEMPOOL** ppstPool, IN size_t iMax,IN size_t iUnit);

/**attach һ���Ѵ��ڵ�mempool�ڴ棬������ڴ��ʽ
*@param[in,out]	 ppstPool  mempoolָ���ָ��
*@param[in]      iMax      mempool������
*@param[in]      iUnit     Ӧ�����ݵĴ�С
*@param[in]      pvBase    �����γ�mempool���ڴ��ָ��
*@param[in]      iSize     pvBase �ڴ�Ĵ�С

*@retval  0  �ɹ�
         -1  ʧ��
*/
TMEMPOOL_API int tmempool_attach(INOUT TMEMPOOL** ppstPool,IN int iMax,IN  int iUnit, IN void* pvBase, IN size_t iSize);

/**������tmempool_new�������ڴ��*/
TMEMPOOL_API int tmempool_destroy(INOUT TMEMPOOL** ppstPool);

/**����mempool��memblock���䷽ʽ
*@param[in]	     pstPool   mempoolָ��
*@param[in]      iMethod   TMEMPOOL_FIFO  �Ƚ��ȳ�,Ĭ�Ϸ�ʽ; TMEMPOOL_LIFO ����ȳ�

*@retval  0  �ɹ�
         -1  ʧ��
*/
TMEMPOOL_API int tmempool_set_method(IN TMEMPOOL* pstPool, IN int iMethod);

/**����������ȡӦ������
*@param[in]	     pstPool   mempoolָ��
*@param[in]      iIdx      memblock������

*@retval  not NULL  �ɹ�
          NULL      ʧ��
*/
TMEMPOOL_API void* tmempool_get(IN TMEMPOOL* pstPool,IN int iIdx);

/**get a user data by position
*@param[in]	     pstPool   mempoolָ��
*@param[in]      iPos      memblock ��������0~max-1

*@retval  not NULL  �ɹ�
          NULL      ʧ��
*/
TMEMPOOL_API void* tmempool_get_bypos(IN  TMEMPOOL* pstPool, IN  int iPos);

/**/
/**����һ��memblock
*@param[in]	     pstPool   mempoolָ��

*@retval  >=0     �ɹ�,   ����memblock������
          -1      ʧ��
*/
TMEMPOOL_API int tmempool_alloc(IN TMEMPOOL* pstPool);

/** Alloc data by pos.
*@param[in]	     pstPool   mempoolָ��
*@retval  >=0  �ɹ�
          -1      ʧ��
*/
TMEMPOOL_API int tmempool_alloc_bypos(IN TMEMPOOL* pstPool);

/** Free the data by it's position */
TMEMPOOL_API int tmempool_free_bypos(IN TMEMPOOL* pstPool,IN int iIdx);

/* Get the number of used items in the mempool */
TMEMPOOL_API int tmempool_get_used_items(IN TMEMPOOL* pstPool);

/* Get capacity of the mempool */
TMEMPOOL_API int tmempool_get_max_items(IN TMEMPOOL* pstPool);

/**�ͷ�һ������ΪiIdx��memblock */
TMEMPOOL_API int tmempool_free(IN TMEMPOOL* pstPool,IN int iIdx);

TMEMPOOL_API int tmempool_free_byptr(IN TMEMPOOL *pstPool,IN void *pvData);

/**�������ɿ�������һ����iStart!=iEndʱʹ��*/
TMEMPOOL_API int tmempool_fix(IN TMEMPOOL* pstPool);

/**������0��ʼ�ҵ���һ��ʹ���е�memblock
*@param[in]	     pstPool   mempoolָ��
*@param[out]     piPos     �洢memblock index

*@retval   0      �ɹ�
          -1      ʧ��
*/
TMEMPOOL_API int tmempool_find_used_first(IN TMEMPOOL* pstPool, OUT int* piPos);

/**�������һ����������һ��������ʼ�ҵ�һ��ʹ���е�memblock
*@param[in]	     pstPool   mempoolָ��
*@param[in,out]  piPos
                       in  ���������
                      out  >=0  �ҵ�������
                           -1   �Ѿ�û����һ��ʹ���е�memblock��

*@retval  >=0    ���ش������piPos��Ӧ��memblock��idx
          -1    ���ҹ�������������
*/
TMEMPOOL_API int tmempool_find_used_next(IN TMEMPOOL* pstPool,INOUT int* piPos);

/*Dump the used items in the mempool*/
TMEMPOOL_API int tmempool_dump_used(IN TMEMPOOL *pstPool,IN FILE *fp);

/*Get the size fo the mempool */
TMEMPOOL_API size_t tmempool_get_size(IN TMEMPOOL *pstPool);

/* is all the memblocks allocated in pstPool */
TMEMPOOL_API int tmempool_is_full(IN TMEMPOOL *pstPool);

/**
 * ���� mempool �е����нڵ�(�ѷ���ġ�δ�����)
 * ���� �ڵ㡢pvArg �ֱ�Ϊ��һ���ڶ��������� pfnTravel
 * @param[in]       pstPool   mempoolָ��
 * @param[in]       pfnTravel ��������ָ��
 * @param[in]       pvArg     ���ñ��������ĵڶ�������
 * @retval   ==0    �ɹ�
 * @retval   !=0    ʧ��
 * @pre             pstPool   ����ΪNULL
 * @pre             pfnTravel ����ΪNULL
 * @note    һ��������ĳ����㣬pfnTravelָ��ĺ�������ֵ!=0, tmempool_travel_all�ͷ��ظ�ֵ
 */
typedef int (*TMEMPOOL_TRAVEL_FUNC)(void* pvNode, void* pvArg);
TMEMPOOL_API int tmempool_travel_all(IN TMEMPOOL *pstPool, TMEMPOOL_TRAVEL_FUNC pfnTravel, void* pvArg);

/**
 * ���� mempool �е������ѷ���Ľڵ�
 * ���� �ڵ㡢pvArg �ֱ�Ϊ��һ���ڶ��������� pfnTravel
 * @param[in]       pstPool   mempoolָ��
 * @param[in]       pfnTravel ��������ָ��
 * @param[in]       pvArg     ���ñ��������ĵڶ�������
 * @retval   ==0    �ɹ�
 * @retval   !=0    ʧ��
 * @pre             pstPool   ����ΪNULL
 * @pre             pfnTravel ����ΪNULL
 * @note    һ��������ĳ����㣬pfnTravelָ��ĺ�������ֵ!=0, tmempool_travel_all�ͷ��ظ�ֵ
 * @note    �ڵ�ı��������ڴ�����˳����ڵ�ķ���˳���޹�
 */
TMEMPOOL_API int tmempool_travel_used(IN TMEMPOOL *pstPool, TMEMPOOL_TRAVEL_FUNC pfnTravel, void* pvArg);

int tmempool_ptr2idx(IN TMEMPOOL *pstPool, void *pvData);

#ifdef __cplusplus
}
#endif

#endif /* TMEMPOOL_H */

