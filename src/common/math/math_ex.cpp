#include "stdafx.h"
#include "math/math_ex.h"

uint32_t g_dwRandSeed = 0;

void srand_ex(uint32_t dwSeed)
{
	g_dwRandSeed = dwSeed;
}

uint32_t get_srand_seed_ex()
{
	return g_dwRandSeed;
}

int64_t rand_ex(uint32_t dwRange)
{
	if (dwRange)
	{
		g_dwRandSeed = (uint32_t)(g_dwRandSeed * 0x08088405) + 1;
		return (int64_t)((g_dwRandSeed * (uint64_t)dwRange) >> 32);
	}
	else
		return 0;
}