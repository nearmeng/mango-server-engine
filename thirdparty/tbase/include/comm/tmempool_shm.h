/**
*
* @file     tmempool_shm.h  
* @brief   ���ڹ����ڴ�� �ڴ��
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
#ifndef TMEMPOOL_SHM_H
#define TMEMPOOL_SHM_H

#include "comm/tmempool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**�ڹ����ڴ��ϴ���mempool
*@param[in,out]	 ppstPool  �ڴ��ָ���ָ��
*@param[in]  uiKey    �����ڴ�Key
*@param[in]  iMax     �ڴ��������ɵ�Ԫ�ظ���
*@param[in]  iUnit    ÿ��Ԫ�صĴ�С

*@retval  >=0  �ɹ�����ppstPool�з���mempoolָ��
          <0      ʧ��
          
*@note   ��Ҫ�жϷ���ֵ
  
*/
int tmempool_new_shm(TMEMPOOL** ppstPool, key_t uiKey, int iMax, int iUnit);

#ifdef __cplusplus
}
#endif

#endif  /* TMEMPOOL_SHM_H */

