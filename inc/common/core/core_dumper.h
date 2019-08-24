#ifndef  _CORE_DUMPER_H_
#define  _CORE_DUMPER_H_

#include <setjmp.h>

#ifdef __linux__
#define CORE_DUMPER_BEGIN \
	nRetCode =  sigsetjmp(core_dumper_get_curr_buf(), 1); \
	if(nRetCode == 0) \
	{ core_dumper_inc_buf_count(); \
	  do  
#else
#define CORE_DUMPER_BEGIN \
	__try { \
	do
#endif

#ifdef __linux__
#define CORE_DUMPER_PROCESS \
	while(0); \
	} \
	else { \
		do 
#else
#define CORE_DUMPER_PROCESS \
	while(0); \
	} \
	__except (seh_filter(GetExceptionCode(), GetExceptionInformation())) { \
		do
#endif

#ifdef __linux__
#define CORE_DUMPER_END \
		while(0); } \
	core_dumper_dec_buf_count();
#else
#define CORE_DUMPER_END \
		while(0); }
#endif

#ifdef __linux__

sigjmp_buf& core_dumper_get_curr_buf(void);

BOOL core_dumper_inc_buf_count(void);

BOOL core_dumper_dec_buf_count(void);

#else

int32_t seh_filter(unsigned int nCode, PEXCEPTION_POINTERS pInfo);

#endif // linux

BOOL core_dumper_init(void);

BOOL core_dumper_uninit(void);


#endif   /* ----- #ifndef _CORE_DUMPER_H_  ----- */

