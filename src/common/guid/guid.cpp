#include "stdafx.h"

#include "guid/guid.h"
#include "define/guid_def.h"

#define MAX_GUID_SERIAL_NUM		(0xFFFF)
#define MAX_GUID_TYPE_NUM		(0x3F)
#define MAX_UNIQ_ID_NUM			(0xFFF)

union GUID_DEF
{
	uint64_t qwGuid;
	struct
	{
		uint64_t nSerialID	: 16;
		uint64_t nType		: 6;
		uint64_t nTimeStamp : 30;
		uint64_t nUniqID	: 12;
	};
};

struct GUID_GENERATOR
{
	uint32_t	dwLastSec;
	uint32_t	dwLastSerial;
};

struct GUID_GEN_MGR
{
	GUID_GENERATOR stGen[gtdTotal];
};

static int32_t g_nUniqID = 0;
CShmObject<GUID_GEN_MGR> g_ShmGenMgr;
GUID_GEN_MGR*			 g_pShmGenMgr;

BOOL guid_init(int32_t nUniqID, BOOL bResume)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(nUniqID < MAX_UNIQ_ID_NUM);

	nRetCode = g_ShmGenMgr.init(stdGuidMgr, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	g_pShmGenMgr = g_ShmGenMgr.get_obj();
	LOG_PROCESS_ERROR(g_pShmGenMgr);

	g_nUniqID = nUniqID;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL guid_uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE; 
}

uint64_t guid_alloc(int32_t nType)
{
	uint64_t qwGuid = 0;
	uint32_t dwCurrTime = 0;
	GUID_GENERATOR* pGen = NULL;

	LOG_PROCESS_ERROR(g_pShmGenMgr);
	LOG_PROCESS_ERROR(nType > gtdInvalid && nType < gtdTotal);

	pGen = &(g_pShmGenMgr->stGen[nType]);
	LOG_PROCESS_ERROR(pGen);

	dwCurrTime = time(0);
	if (dwCurrTime != pGen->dwLastSec)
	{
		pGen->dwLastSec = dwCurrTime;
		pGen->dwLastSerial = 0;
	}

	if (pGen->dwLastSerial >= MAX_GUID_SERIAL_NUM)
		return 0;

	GUID_DEF guid = { (uint64_t)0 };
	guid.nSerialID = (pGen->dwLastSerial)++;
	guid.nTimeStamp = pGen->dwLastSec;
	guid.nType = nType;
	guid.nUniqID = g_nUniqID;

	qwGuid = ((uint64_t)guid.qwGuid);

Exit0:
	return qwGuid;
}