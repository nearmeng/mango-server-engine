#ifndef _PROFILE_H_
#define _PROFILE_H_

#if defined(_WIN32) | defined(_WIN64)

inline unsigned long long RDTSC(void)
{
    return __rdtsc();
}

#else

unsigned long long RDTSC(void) __attribute__((noinline));

#endif

#define PROFILE_START()     \
    do                      \
    {                       \
        profile_start();    \
    } while (0)

#define PROFILE_POINT(__sid__)                                                  \
    do                                                                          \
    {                                                                           \
        profile_point(__sid__, __LINE__,                                        \
            "[PROFILE %2d] %8lld @ line %d file \"" __FILE__ "\"");				\
    } while (0)

#define PROFILE_END()   \
    do                  \
    {                   \
        profile_end();  \
    } while (0)

/************************************************************************/

#define MAX_PROFILE_POINT_COUNT     (64)
#define MAX_PROFILE_SID_COUNT       (16)

struct _PROFILE_POINT
{
    UINT64          qwTime;
    INT32           nLine;
    const char*     pcszMessage;
};

extern _PROFILE_POINT g_ProfileTable[MAX_PROFILE_SID_COUNT][MAX_PROFILE_POINT_COUNT];
extern INT32 g_nProfileIndex[MAX_PROFILE_SID_COUNT];

inline void profile_start(void)
{
    for (INT32 nSid = 0; nSid < MAX_PROFILE_SID_COUNT; nSid++)
    {
        g_nProfileIndex[nSid] = 0;
        for (INT32 nIndex = 0; nIndex < MAX_PROFILE_POINT_COUNT; nIndex++)
        {
            g_ProfileTable[nSid][nIndex].qwTime = 0;
            g_ProfileTable[nSid][nIndex].nLine = 0;
            g_ProfileTable[nSid][nIndex].pcszMessage = NULL;
        }
    }
}

inline void profile_point(INT32 nSid, INT32 nLine, const char* pcszMessage)
{
    g_ProfileTable[nSid][g_nProfileIndex[nSid]].qwTime = RDTSC();
    g_ProfileTable[nSid][g_nProfileIndex[nSid]].nLine = nLine;
    g_ProfileTable[nSid][g_nProfileIndex[nSid]++].pcszMessage = pcszMessage;
}

inline void profile_end(void)
{
    for (INT32 nSid = 0; nSid < MAX_PROFILE_SID_COUNT; nSid++)
    {
        if (g_nProfileIndex[nSid] > 0)
        {
            INF("[PROFILE %2d]", nSid);
			INF(g_ProfileTable[nSid][0].pcszMessage, nSid, 0LL, g_ProfileTable[nSid][0].nLine);
        }

        for (INT32 nIndex = 1; nIndex < g_nProfileIndex[nSid]; nIndex++)
        {
			INF(g_ProfileTable[nSid][0].pcszMessage, nSid,
                g_ProfileTable[nSid][nIndex].qwTime - g_ProfileTable[nSid][nIndex - 1].qwTime, 
                g_ProfileTable[nSid][nIndex].nLine);
        }
    }
}

#endif