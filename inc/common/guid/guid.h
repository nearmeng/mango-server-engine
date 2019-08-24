#ifndef _GUID_H_
#define _GUID_H_

#include "define/guid_def.h"

BOOL guid_init(int32_t nUniqID, BOOL bResume);
BOOL guid_uninit(void);
uint64_t guid_alloc(int32_t nType);

#endif