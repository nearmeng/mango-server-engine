#include "stdafx.h"
#include "profile/profile.h"

_PROFILE_POINT g_ProfileTable[MAX_PROFILE_SID_COUNT][MAX_PROFILE_POINT_COUNT];
INT32 g_nProfileIndex[MAX_PROFILE_SID_COUNT];

#if defined(WIN32) | defined(WIN64)

#else

unsigned long long RDTSC(void)
{
    unsigned long long int l, h;
    __asm__ volatile ("rdtsc" : "=a" (l), "=d" (h));

	return (h << 32) + l;
}

#endif	// WIN32