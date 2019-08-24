/*
 * =====================================================================================
 * 
 *       Filename:  minidump.h
 *    Description:
 *        Created:  01/18/2012 04:47:46 PM CST
 *         Author:  andyzhu , andyzhu@tencent.com
 * 
 * =====================================================================================
 */

#ifndef  MINIDUMP_INC
#define  MINIDUMP_INC

// 注: 请在tlog初始化后调用该函数，这样minidump本身的错误可以记录在tlog中
void minidump_handle(int pid);

#endif   /* ----- #ifndef MINIDUMP_INC  ----- */

