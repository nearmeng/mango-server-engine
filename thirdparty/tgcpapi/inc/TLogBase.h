#ifndef __TLogBase_h__
#define __TLogBase_h__

#include "TLogDefine.h"

class CTLogBase
{
public:
	CTLogBase(void);
	virtual ~CTLogBase(void);

public:
	virtual void Log(T_LOG_PRIORITY pri, const char* msg) = 0;

public:
	void Register();
	void Unregister();
    
public:
    T_LOG_PRIORITY GetLogPriority();
    void SetLogPriority(T_LOG_PRIORITY pri);
};

#endif

