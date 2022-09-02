#include "stdafx.h"
#include "tbus/tbus_wrapper.h"
#include "tbus/tbus_internal.h"

#define MAX_TBUS_RECV_PER_LOOP (1024)

static int32_t g_nTbusAddr;
static int32_t g_nTbusHandle;
static TBUS_MSG_HANDLE g_pTbusMsgHandler;
static char	g_szTbusMsgBuff[1024 * 1024];

BOOL tbus_system_init(int32_t nTbusAddr, int32_t nTbusHandle, TBUS_MSG_HANDLE pTbusMsgHandler, BOOL bResume)
{
	int32_t nRetCode = 0;

	g_nTbusAddr = nTbusAddr;
	g_nTbusHandle = nTbusHandle;
	g_pTbusMsgHandler = pTbusMsgHandler;

	/*
	if (!bResume)
	{
		nRetCode = tbus_clear_data();
		LOG_CHECK_ERROR(nRetCode);
	}
	*/

	return TRUE;
Exit0:
	return FALSE;
}

BOOL tbus_system_uninit()
{
	return TRUE;
}

static BOOL get_tbusid(int32_t nTbusAddr, int32_t* pArea, int32_t* pZone, int32_t* pFunc, int32_t* pInst)
{
	int32_t nRetCode = 0;
	char addr[64] = { 0 };
	int32_t nArea = 0;
	int32_t nZone = 0;
	int32_t nFunc = 0;
	int32_t nInst = 0;

	tbus_addr_ntop(nTbusAddr, addr, sizeof(addr));

	nRetCode = sscanf(addr, "%d.%d.%d.%d", &nArea, &nZone, &nFunc, &nInst);
	LOG_PROCESS_ERROR(nRetCode == 4);
	
	if (pArea) *pArea = nArea;
	if (pZone) *pZone = nZone;
	if (pFunc) *pFunc = nFunc;
	if (pInst) *pInst = nInst;

	return TRUE;
Exit0:
	return FALSE;
}

int32_t tbus_get_addr(void)
{
	return g_nTbusHandle;
}

/*
int32_t tbus_get_area(int32_t nTbusAddr)
{
	int32_t nArea = 0;
	int32_t nRetCode = 0;

	nRetCode = get_tbusid(nTbusAddr, &nArea, NULL, NULL, NULL);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return nArea;
}

int32_t tbus_get_zone(int32_t nTbusAddr)
{
	int32_t nFunc = 0;
	int32_t nRetCode = 0;

	nRetCode = get_tbusid(nTbusAddr, NULL, NULL, &nFunc, NULL);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return nFunc;

}

int32_t tbus_get_type(int32_t nTbusAddr)
{
	int32_t nFunc = 0;
	int32_t nRetCode = 0;

	nRetCode = get_tbusid(nTbusAddr, NULL, NULL, &nFunc, NULL);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return nFunc;
}

int32_t tbus_get_inst(int32_t nTbusAddr)
{
	int32_t nInst = 0;
	int32_t nRetCode = 0;

	nRetCode = get_tbusid(nTbusAddr, NULL, NULL, NULL, &nInst);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return nInst;
}
*/

const char* tbus_get_str(int32_t nTbusAddr)
{
	int32_t nRetCode = 0;
	static char szTbusStr[64];

	tbus_addr_ntop(nTbusAddr, szTbusStr, sizeof(szTbusStr));
Exit0:
	return szTbusStr;
}

int32_t tbus_make_addr(int32_t nArea, int32_t nZone, int32_t nType, int32_t nInst)
{
	int32_t nRetCode = 0;
	int32_t nTbusAddr = 0;

	char szAddr[64] = { 0 };
	snprintf(szAddr, sizeof(szAddr), "%d.%d.%d.%d", nArea, nZone, nType, nInst);

	nRetCode = tbus_addr_aton(szAddr, &nTbusAddr);
	LOG_PROCESS_ERROR(nRetCode == 0);

Exit0:
	return nTbusAddr;
}

BOOL tbus_send_data(int32_t nDstTbusAddr, const char* pBuff, int32_t nDataLen)
{
	int32_t nRetCode = 0;

	nRetCode = tbus_send(g_nTbusHandle, &g_nTbusAddr, &nDstTbusAddr, pBuff, nDataLen, 0);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	CRI("tbus send failed from %llx to %llx ret %d", g_nTbusAddr, nDstTbusAddr, nRetCode);
	return FALSE;
}

BOOL tbus_sendv_data(int32_t nDstTbusAddr, const struct iovec* pVec, const int32_t nVecCount)
{
	int32_t nRetCode = 0;

	nRetCode = tbus_sendv(g_nTbusHandle, &g_nTbusAddr, &nDstTbusAddr, pVec, nVecCount, 0);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	CRI("tbus send failed from %llx to %llx ret %d", g_nTbusAddr, nDstTbusAddr, nRetCode);
	return FALSE;
}

BOOL tbus_recv_data(void)
{
	int32_t nRetCode = 0;
	int32_t nLeftRecvCount = MAX_TBUS_RECV_PER_LOOP;
	
	while (nLeftRecvCount > 0)
	{
		int32_t nTbusSrc = TBUS_ADDR_ALL;
		int32_t nTbusDst = 0;
		size_t nDataSize = sizeof(g_szTbusMsgBuff);

		nRetCode = tbus_recv(g_nTbusHandle, &nTbusSrc, &nTbusDst, g_szTbusMsgBuff, &nDataSize, 0);
		if (nRetCode >= 0)
		{
			g_pTbusMsgHandler(g_szTbusMsgBuff, nDataSize, nTbusSrc);
            nLeftRecvCount--;
		}
		else
			break;
	}

	if (nLeftRecvCount == 0)
		WRN("wow, too many tbus msg to process!");

	return nLeftRecvCount < MAX_TBUS_RECV_PER_LOOP;
Exit0:
	return FALSE;
}

BOOL tbus_clear_data(void)
{
	int32_t nRetCode = 0;
	int32_t nTbusSrc = TBUS_ADDR_ALL;
	int32_t nTbusDst = 0;

	while (true)
	{
		size_t nDataSize = sizeof(g_szTbusMsgBuff);
		nRetCode = tbus_recv(g_nTbusHandle, &nTbusSrc, &nTbusDst, g_szTbusMsgBuff, &nDataSize, 0);
		if (nRetCode < 0)
			break;

		nTbusSrc = TBUS_ADDR_ALL;
		nTbusDst = 0;
	}

	return TRUE;
}