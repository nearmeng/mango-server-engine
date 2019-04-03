#ifndef  _PROCESS_ERROR_H_
#define  _PROCESS_ERROR_H_

#include "log/log.h"

#ifdef __linux__
	#define likely(x)    __builtin_expect (!!(x), 1)	
	#define unlikely(x)  __builtin_expect (!!(x), 0)
#else
	#define likely(x) (x)
	#define unlikely(x) (x)
#endif

#define PROCESS_ERROR(__condition__)    \
	do                                  \
	{                                   \
		if(likely(__condition__))         \
		{                               \
		}                               \
		else                            \
		{                               \
			goto Exit0;                 \
		}                               \
	}while(0)


#define PROCESS_SUCCESS(__condition__) \
	do                                 \
	{                                  \
		if(likely(__condition__))      \
		{                              \
			goto Exit1;                \
		}                              \
	}while(0) 

#define LOG_PROCESS_ERROR(__condition__) 	\
	do                                  	\
	{                                   	\
		if(likely(__condition__))         	\
		{                               	\
		}                               	\
		else                            	\
		{                               	\
			ERR("%s(%d): LOG_PROCESS_ERROR("#__condition__") in function %s()", __FILE__, __LINE__, __FUNCTION__);           \
			goto Exit0;                 	\
		}                               	\
	}while(0)

#define LOG_CHECK_ERROR(__condition__) 		\
	do                                  	\
	{                                   	\
		if(likely(__condition__))         	\
		{                               	\
		}                               	\
		else                            	\
		{                               	\
			ERR("%s(%d): LOG_CHECK_ERROR("#__condition__") in function %s()", __FILE__, __LINE__, __FUNCTION__);           \
		}                               	\
	}while(0)

#ifdef __linux__
	#define LOG_PROCESS_ERROR_DETAIL(__condition__, __fmt__, args...) \
		do                                  	\
		{                                   	\
			if(likely(__condition__))         	\
			{                               	\
			}                               	\
			else                            	\
			{                               	\
				ERR("%s(%d): LOG_PROCESS_ERROR("#__condition__") in function %s() detail: "#__fmt__, __FILE__, __LINE__, __FUNCTION__, ##args);           \
				goto Exit0;                 	\
			}                               	\
		}while(0)
#else
	#define LOG_PROCESS_ERROR_DETAIL(__condition__, __fmt__, ...) \
		do                                  	\
		{                                   	\
			if(likely(__condition__))         	\
			{                               	\
			}                               	\
			else                            	\
			{                               	\
				ERR("%s(%d): LOG_PROCESS_ERROR("#__condition__") in function %s() detail: "#__fmt__, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);     \
				goto Exit0;                 	\
			}                               	\
		}while(0)
#endif

#define PROCESS_ERROR_RET_CODE(__condition__, __result__)    \
	do                                                       \
	{                                                        \
		if(likely(__condition__))                            \
		{                                                    \
		}                                                    \
		else                                                 \
		{                                                    \
			result = __result__;                             \
			goto Exit0;                                      \
		}                                                    \
	}while(0)

#define LOG_PROCESS_ERROR_RET_CODE(__condition__, __result__)  \
	do                                                         \
	{                                                          \
		if(likely(__condition__))                              \
		{                                                      \
		}                                                      \
		else                                                   \
		{                                                      \
			result = __result__;                               \
			ERR("%s(%d): LOG_PROCESS_ERROR_RET_CODE("#__condition__") in function %s()", __FILE__, __LINE__, __FUNCTION__);           \
			goto Exit0;                                        \
		}                                                      \
	}while(0)

#define SAFE_FREE(__pointer__)    	\
	do                            	\
	{                               \
		if(__pointer__)             \
		{                           \
			free(__pointer__);      \
			(__pointer__) = NULL;   \
		}                           \
	}while(0);                      

#define SAFE_DELETE(__pointer__)    	\
	do                                	\
	{                                   \
		if(__pointer__)                 \
		{                               \
			delete (__pointer__);    \
			(__pointer__) = NULL;       \
		}                               \
	}while(0); 

#define SAFE_DELETE_ARRAY(__pointer__)    	\
	do                                		\
	{                                   	\
		if(__pointer__)                 	\
		{                               	\
			delete [] (__pointer__);    		\
			(__pointer__) = NULL;       	\
		}                               	\
	}while(0); 

#endif   /* ----- #ifndef _PROCESS_ERROR_H_  ----- */

