#include "stdafx.h"
#include "hash.h"

#include "define/router_def.h"
#include "define/service_def.h"
#include "math/hash.h"

#include "router_mgr/router_mgr.h"
#include "define/server_def.h"
	
typedef int32_t (*ROUTER_HASH_FUNC)(SERVICE_INFO* pServiceInfo, uint64_t qwRouterID, BOOL bCanCache);

ROUTER_HASH_FUNC ms_HashFunc[rhtTotal] = { 0 };
int32_t s_HashType[svrTotal] = { 0 };

static int32_t router_jump_consist_hash(SERVICE_INFO* pServiceInfo, uint64_t qwRouterID, int32_t nTransferHandleType)
{
	int32_t nRetCode = 0;
	SERVER_INFO* pServerInfo = NULL;
		
	LOG_PROCESS_ERROR(pServiceInfo);
	
    do {
		int32_t nHashIndex = jump_consist_hash(pServiceInfo->nServiceServerCount, qwRouterID);
		
		//INF("router by routerid %llu server_count %d hash_index %d", qwRouterID, pServiceInfo->nServiceServerCount, nHashIndex + 1);
		pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[nHashIndex]);
		LOG_CHECK_ERROR(pServiceInfo);

		if (pServerInfo)
		{
			if (pServerInfo->nState == svstInService)
				return pServerInfo->nTbusAddr;
			else if (pServerInfo->nState == svstTransfer)
			{
				if (nTransferHandleType != rtnhtChooseNew)
					return pServerInfo->nTbusAddr;
				else
				{
					int32_t nServiceCount = 0;
					for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
					{
						SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
						LOG_CHECK_ERROR(pServerInfo);

						if (pServerInfo->nState == svstInService)
							nServiceCount++;
					}
					LOG_PROCESS_ERROR(nServiceCount > 0);
				}
			}
		}

		qwRouterID++;

    } while (true);

Exit0:
	return 0;
}

BOOL router_hash_init(void)
{
	int32_t nRetCode = 0;

#define REG_ROUTER_HASH_FUNC(_type_, _func_) ms_HashFunc[_type_] = _func_;
    REG_ROUTER_HASH_FUNC(rhtJumpConsistHash, router_jump_consist_hash);

#define REG_ROUTER_HASH_TYPE(_service_type_, _type_) s_HashType[_service_type_] = _type_;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL router_hash_uninit(void)
{
	return TRUE;
}

int32_t router_hash_get_node(int32_t nHashType, int32_t nServiceType, uint64_t qwRouterID, BOOL bCanCache)
{
	int32_t nRetCode = 0;
	int32_t nNodeAddr = 0;
	int32_t nActiveNodeCount = 0;
	SERVICE_INFO* pServiceInfo = NULL;

	LOG_PROCESS_ERROR(nHashType > 0 && nHashType < rhtTotal);
	LOG_PROCESS_ERROR(ms_HashFunc[nHashType]);
	
	pServiceInfo = CRouterMgr::get_instance().get_service_info(nServiceType);
	LOG_PROCESS_ERROR(pServiceInfo);

	for (int32_t i = 0; i < pServiceInfo->nServiceServerCount; i++)
	{
		SERVER_INFO* pServerInfo = CRouterMgr::get_instance().get_server_info_by_index(pServiceInfo->ServerInfoIndex[i]);
		LOG_CHECK_ERROR(pServerInfo);

		if (pServerInfo->nState == svstInService || pServerInfo->nState == svstTransfer)
			nActiveNodeCount++;
	}
	LOG_PROCESS_ERROR(nActiveNodeCount > 0);

	nNodeAddr = ms_HashFunc[nHashType](pServiceInfo, qwRouterID, bCanCache);
	LOG_PROCESS_ERROR(nNodeAddr);

Exit0:
	//INF("service_type %d hash get node %s", nServiceType, get_tbus_str(nNodeAddr));
	return nNodeAddr;
}

int32_t router_hash_get_hash_type(int32_t nServiceType)
{
	int32_t nType = rhtJumpConsistHash;
	LOG_PROCESS_ERROR(nServiceType > svrInvalid && nServiceType < svrTotal);

	if (s_HashType[nServiceType] != 0)
		nType = s_HashType[nServiceType];
Exit0:
	return nType;
}