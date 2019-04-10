/**
*
* @file     shtable_shm.h
* @brief   ���ڹ����ڴ�Ĺ�ϣ��ʵ��
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

#ifndef SHTABLE_SHM_H

#define SHTABLE_SHM_H

#include "comm/shtable.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**�ڹ����ڴ��ϴ�����ϣ��
*@param[in]	 iBucket  ��ϣ��Ͱ��
*@param[in]  iMax     ��ϣ��������ɵ�Ԫ�ظ���
*@param[in]  iUnit    ÿ��Ԫ�صĴ�С
*@param[in]  uiKey    �����ڴ�Key
*@retval  not NULL  �ɹ�����ϣ��ָ��
          NULL      ʧ��

*@note   ��Ҫ�жϷ���ֵ

*/
LPSHTABLE sht_create_shm(size_t iBucket, size_t iMax, size_t iUnit, key_t uiKey);
#ifdef __cplusplus
}
#endif

#endif /* SHTABLE_SHM_H */
