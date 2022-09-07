#include "stdafx.h"
#include "bt/bt_mgr.h"

#include "lua/script_mgr.h"
#include "math/crc32c_fast.h"

#include "config/global_config.h"

#include "event/server_event.h"

#ifdef _linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

CBTMgr CBTMgr::ms_Instance;

extern int32_t bat_return_success(BT_CTX& rCtx, int32_t nParam[]);
extern int32_t bat_return_fail(BT_CTX& rCtx, int32_t nParam[]);

static const char* s_szRetValueNameList[] = 
{
	"Error",
	"Failed",
	"Success",
	"Running",
};

BOOL CBTMgr::init(REG_BT_OWNER_FUNC pBtOwnerFunc, BOOL bResume)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pBtOwnerFunc);
	
	nRetCode = m_BtCtx.init(stdBtCtx, g_ServerConfig.Common.nInitBtCtxCount, bResume);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_BtMgrData.init(stdBtMgrData, bResume);
	LOG_PROCESS_ERROR(nRetCode);
	
	m_pMgrData = m_BtMgrData.get_obj();
	LOG_PROCESS_ERROR(m_pMgrData);

	nRetCode = (*pBtOwnerFunc)();
	LOG_PROCESS_ERROR(nRetCode);

	m_pMgrData->pCurrentCtx = NULL;

	if (!bResume)
	{
		m_pMgrData->nBtNodeUsedCount = 1;
		m_pMgrData->nBtTreeUsedCount = 1;
		m_pMgrData->bDebugEnabled = FALSE;
		m_pMgrData->nBtCtxIDGenerator = 0;
		m_pMgrData->nActionIDGenerator = batEnd;
	}
	else
	{
		TRAVERSE_BT_CTX_RESUME TraverseBtCtxResume;
		TraverseBtCtxResume.pOwnerDataList = m_OwnerDataList;
		m_BtCtx.traverse(TraverseBtCtxResume);

		for (int32_t i = 0; i < m_pMgrData->nBtTreeUsedCount; i++)
		{
			m_pMgrData->stBtTree[i].pScript = CScriptMgr::instance().find_script(m_pMgrData->stBtTree[i].qwScriptNameHash);
		}
	}

	//build the index
	for (int32_t nIndex = 0; nIndex < btTotal; nIndex++)
	{
		if (m_pMgrData->ActionDefList[nIndex].szName[0])
		{
			m_ActionName2Func.insert(std::make_pair(
				m_pMgrData->ActionDefList[nIndex].szName, m_pMgrData->ActionDefList + nIndex));
		}
	}

	for (int32_t nIndex = 0; nIndex < m_pMgrData->nBtTreeUsedCount; nIndex++)
	{
		uint32_t dwCRC = 0;
		_get_crc(m_pMgrData->stBtTree[nIndex].dwRootNode, 0, dwCRC);

		m_CRC2Tree[dwCRC] = m_pMgrData->stBtTree + nIndex;
	}

	nRetCode = _init_c_action();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBTMgr::uninit(void)
{
	int32_t nRetCode = 0;

	return TRUE;
Exit0:
	return FALSE;
}

BT_NODE* CBTMgr::bt_create_node(uint32_t& dwNodeID)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(m_pMgrData);
	LOG_PROCESS_ERROR(m_pMgrData->nBtNodeUsedCount < MAX_BT_NODE_COUNT);

	dwNodeID = m_pMgrData->nBtNodeUsedCount;
	m_pMgrData->nBtNodeUsedCount++;

	return ((m_pMgrData->stBtNode) + dwNodeID);

Exit0:
	return NULL;
}

BT_NODE* CBTMgr::bt_get_node(uint32_t dwNodeID)
{
	PROCESS_ERROR(dwNodeID > 0);
	LOG_PROCESS_ERROR(dwNodeID < MAX_BT_NODE_COUNT);
	
	return ((m_pMgrData->stBtNode) + dwNodeID);

Exit0:
	return NULL;
}

BOOL CBTMgr::bt_add_child(uint32_t dwParent, uint32_t dwChild)
{
	BT_NODE* pParent = NULL;
	BT_NODE* pChild = NULL;
	BT_NODE* pNode = NULL;
	int32_t nChildIndex = 0;

	pParent = bt_get_node(dwParent);
	LOG_PROCESS_ERROR(pParent);

	pChild = bt_get_node(dwChild);
	LOG_PROCESS_ERROR(pChild);

	if (pParent->dwChild == 0)
	{
		LOG_PROCESS_ERROR(pParent->uChildrenCount == 0);
		pParent->dwChild = dwChild;
	}
	else
	{
		pNode = bt_get_node(pParent->dwChild);

		nChildIndex = 0;
		while (pNode)
		{
			if (pNode->dwSibling == 0)
			{
				LOG_PROCESS_ERROR(nChildIndex + 1 == pParent->uChildrenCount);

				pNode->dwSibling = dwChild;
				break;
			}

			++nChildIndex;
			LOG_PROCESS_ERROR(nChildIndex < MAX_BT_CHILD_COUNT);

			pNode = bt_get_node(pNode->dwSibling);
		}
	}

	pParent->uChildrenCount++;

	return TRUE;
Exit0:
	return FALSE;
}

int	lua_create_bt_node(lua_State* L)
{
	int32_t nRetCode = 0;
	uint32_t dwNode = 0;
	uint32_t dwParent = 0;
	BT_NODE* pNode = NULL;

	dwParent = (uint32_t)lua_tointeger(L, 1);
	int32_t nType = (int32_t)lua_tointeger(L, 2);
	int32_t nParam0 = (int32_t)lua_tointeger(L, 3);
	int32_t nParam1 = (int32_t)lua_tointeger(L, 4);
	int32_t nParam2 = (int32_t)lua_tointeger(L, 5);
	int32_t nParam3 = (int32_t)lua_tointeger(L, 6);
	int32_t nParam4 = (int32_t)lua_tointeger(L, 7);
	int32_t nParam5 = (int32_t)lua_tointeger(L, 8);
	int32_t nVarParam = (int32_t)lua_tointeger(L, 9);

	LOG_PROCESS_ERROR(nType > btInvalid && nType < btTotal);

	pNode = CBTMgr::instance().bt_create_node(dwNode);
	LOG_PROCESS_ERROR(pNode);

	pNode->uType = (uint16_t)nType;
	pNode->nParam[0] = nParam0;
	pNode->nParam[1] = nParam1;
	pNode->nParam[2] = nParam2;
	pNode->nParam[3] = nParam3;
	pNode->nParam[4] = nParam4;
	pNode->nParam[5] = nParam5;
	pNode->uReturnValue = 0;
	pNode->uVarType = nVarParam;
	pNode->uChildrenCount = 0;
	pNode->dwChild = 0;
	pNode->dwSibling = 0;

	if (dwParent)
	{
		nRetCode = CBTMgr::instance().bt_add_child(dwParent, dwNode);
		LOG_PROCESS_ERROR(nRetCode);
	}

	lua_pushinteger(L, dwNode);

	return 1;
Exit0:
	return 0;
}

int lua_set_node_ret_value(lua_State* L)
{
	uint32_t dwNode = (uint32_t)lua_tointeger(L, 1);
	int32_t nRetValue = (int32_t)lua_tointeger(L, 2);

	BT_NODE* pNode = CBTMgr::instance().bt_get_node(dwNode);
	LOG_PROCESS_ERROR(pNode);

	pNode->uReturnValue = nRetValue;
Exit0:
	return 0;
}

int lua_create_bt_tree(lua_State* L)
{
	int32_t nRetCode = 0;
	uint32_t dwRootNode = 0;
	CLuaScript* pScript = NULL;

	dwRootNode = (uint32_t)lua_tointeger(L, 1);
	LOG_PROCESS_ERROR(dwRootNode > 0);

	pScript = CLuaScript::get_script_from_state(L);
	LOG_PROCESS_ERROR(pScript);

	nRetCode = CBTMgr::instance().create_bt_tree(dwRootNode, pScript);
	LOG_PROCESS_ERROR(nRetCode > 0);

	lua_pushinteger(L, nRetCode);
	return 1;

Exit0:
	return 0;
}

int32_t lua_print_node(lua_State* L)
{
	uint32_t dwRoot = (uint32_t)lua_tointeger(L, 1);

	CBTMgr::instance().print(dwRoot, 0);

	return 0;
}

int32_t lua_find_bt_action_id(lua_State* L)
{
	const char* pcszName = lua_tostring(L, 1);

	int32_t nActionID = CBTMgr::instance().get_action_id(pcszName);
	PROCESS_ERROR(nActionID != btInvalid);

	lua_pushinteger(L, nActionID);

	return 1;
Exit0:
	return 0;
}

int32_t lua_register_bt_action(lua_State* L)
{
	const char* pcszName = lua_tostring(L, 1);
	int32_t nActionID = CBTMgr::instance().register_lua_action(pcszName);

	lua_pushinteger(L, nActionID);

	return 1;
Exit0:
	return 0;
}

uint64_t CBTMgr::load(CLuaScript* pScript, const char* pcszTreeName)
{
	int32_t nRetCode = 0;
	int32_t nScriptRetCode = 0;
	uint64_t dwTreeID = 0;

	LOG_PROCESS_ERROR(pScript);
	LOG_PROCESS_ERROR(pcszTreeName);

	nRetCode = pScript->call_function(LOAD_BT_FUNC_NAME, "s:dd", pcszTreeName, &dwTreeID, &nScriptRetCode);
	LOG_PROCESS_ERROR(nRetCode);
	LOG_PROCESS_ERROR(nScriptRetCode);

	return dwTreeID;
Exit0:
	return 0;
}

uint32_t CBTMgr::get_used_node_count(void)
{
	return m_pMgrData->nBtNodeUsedCount;
Exit0:
	return 0;
}

inline BOOL bt_push_node(BT_CTX& rCtx, uint32_t dwNode)
{
	LOG_PROCESS_ERROR(rCtx.byCallFrameCount < MAX_CALL_FRAME);

	rCtx.CallFrame[rCtx.byCallFrameCount].dwNode = dwNode;
	rCtx.CallFrame[rCtx.byCallFrameCount].dwChild = 0;
	rCtx.byCallFrameCount++;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL bt_pop_node(BT_CTX& rCtx)
{
	LOG_PROCESS_ERROR(rCtx.byCallFrameCount > 0);
	rCtx.byCallFrameCount--;

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CBTMgr::start_run(BT_CTX& rCtx, int32_t nTreeID, int32_t nOwnerType, void* pOwner, 
	uint64_t qwOwnerID, int64_t llEventVar0 /* = 0 */, int64_t llEventVar1 /* = 0 */, int64_t llTriggerVar0 /* = 0 */, int64_t llTriggerVar1 /* = 0 */, BOOL bRollback /* = FALSE */)
{
	int32_t nRetCode = 0;
	EVENT_INFO* pEvent = NULL;
	CLuaScript* pScript = m_pMgrData->stBtTree[nTreeID].pScript;

	LOG_PROCESS_ERROR(nTreeID > 0 && nTreeID < MAX_BT_TREE_COUNT);
	LOG_PROCESS_ERROR(pScript);

	rCtx.CallFrame[0].dwNode = m_pMgrData->stBtTree[nTreeID].dwRootNode;
	rCtx.CallFrame[0].dwChild = 0;
	rCtx.qwOwnerID = qwOwnerID;
	rCtx.qwScriptNameHash = pScript->get_name_hash();
	rCtx.byReturnValue = brvError;
	rCtx.byCallFrameCount = 1;
	rCtx.nWaitTimer = 0;
	rCtx.nResult = -1;
	rCtx.uOwnerType = nOwnerType; 
	rCtx.bRollback = bRollback;
	rCtx.bDebugEnabled = FALSE;
	rCtx.llEventVarList[0] = llEventVar0;
	rCtx.llEventVarList[1] = llEventVar1;
	rCtx.llTriggerVarList[0] = llTriggerVar0;
	rCtx.llTriggerVarList[1] = llTriggerVar1;

	if (m_pMgrData->bDebugEnabled)
	{
		//if(pEventBase)
			//INF("run event %s owner %llX rollback", pMgrData->neve)
	}

	nRetCode = _run(rCtx, pOwner, pScript);
	if (nRetCode == brvRunning)
	{
		BT_CTX* pCtx = m_BtCtx.new_object(m_pMgrData->nBtCtxIDGenerator);
		LOG_PROCESS_ERROR(pCtx);

		memcpy(pCtx, &rCtx, sizeof(rCtx));
	}

	return nRetCode;
Exit0:
	return brvError;
}

int32_t CBTMgr::_run(BT_CTX& rCtx, void* pOwner, CLuaScript* pScript)
{
	int32_t nRetCode = 0;
	BT_NODE* pNode = NULL;
	BT_NODE* pChild = NULL;

#if defined(_DEBUG)
	char szTab[BT_DEBUG_INFO_LEN];
#endif

	rCtx.pOwner = pOwner;
	rCtx.pScript = pScript;

	while (rCtx.byCallFrameCount > 0)
	{
		BT_CTX::CALL_FRAME& callFrame = rCtx.CallFrame[rCtx.byCallFrameCount - 1];

		pNode = bt_get_node(callFrame.dwNode);
		LOG_PROCESS_ERROR(pNode);

#if defined(_DEBUG)
		if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
        {
			int32_t i = 0;
            for (; i < (rCtx.byCallFrameCount - 1) * 3; i++)
                szTab[i] = ' ';
			szTab[i] = 0;
        }
#endif	// _DEBUG

		switch (pNode->uType)
		{
		//sys node process
		case btSelector:
		{
			if (callFrame.dwChild == 0)
			{
				if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
					INF("%sbatSelector : run children count %d", _get_debug_space_info(rCtx.byCallFrameCount),pNode->uChildrenCount);

				if (pNode->dwChild)
				{
					callFrame.dwChild = pNode->dwChild;
					rCtx.byReturnValue = brvError;

					nRetCode = bt_push_node(rCtx, pNode->dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					rCtx.byReturnValue = brvFail;
				
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatSelector : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				pChild = bt_get_node(callFrame.dwChild);
				LOG_PROCESS_ERROR(pChild);

				if (pChild->dwSibling && rCtx.byReturnValue == brvFail)
				{
					callFrame.dwChild = pChild->dwSibling;

					nRetCode = bt_push_node(rCtx, callFrame.dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatSelector : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			break;
		}
		case btSequencer:
		{
			if (callFrame.dwChild == 0)
			{
				if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
					INF("%sbatSequencer : children count %d", _get_debug_space_info(rCtx.byCallFrameCount), pNode->uChildrenCount);

				if (pNode->dwChild)
				{
					callFrame.dwChild = pNode->dwChild;
					rCtx.byReturnValue = brvError;

					nRetCode = bt_push_node(rCtx, pNode->dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					rCtx.byReturnValue = brvError;
				
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatSequncer : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				pChild = bt_get_node(callFrame.dwChild);
				LOG_PROCESS_ERROR(pChild);

				if (pChild->dwSibling && rCtx.byReturnValue == brvSuccess)
				{
					callFrame.dwChild = pChild->dwSibling;
					nRetCode = bt_push_node(rCtx, callFrame.dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatSequencer : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			break;
		}
		case btParallel:
		{
			if (callFrame.dwChild == 0)
			{
				if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
					INF("%sbatParallel : children count %d", _get_debug_space_info(rCtx.byCallFrameCount), pNode->uChildrenCount);

				if (pNode->dwChild)
				{
					callFrame.dwChild = pNode->dwChild;
					rCtx.byReturnValue = brvError;

					nRetCode = bt_push_node(rCtx, pNode->dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					rCtx.byReturnValue = brvSuccess;
				
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatParallel : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				pChild = bt_get_node(callFrame.dwChild);
				LOG_PROCESS_ERROR(pChild);

				if (pChild->dwSibling)
				{
					callFrame.dwChild = pChild->dwSibling;
					nRetCode = bt_push_node(rCtx, callFrame.dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					rCtx.byReturnValue = brvSuccess;
					
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatParallel : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			break;
		}
		case btDebug:
		{
			if (callFrame.dwChild == 0)
			{
				LOG_PROCESS_ERROR(pNode->uChildrenCount == 1);
				callFrame.dwChild = pNode->dwChild;
				rCtx.byReturnValue = brvError;
				rCtx.bDebugEnabled = TRUE;

				nRetCode = bt_push_node(rCtx, pNode->dwChild);
				LOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				rCtx.bDebugEnabled = FALSE;
				nRetCode = bt_pop_node(rCtx);
				LOG_PROCESS_ERROR(nRetCode);
			}
			break;
		}
		case btRollBack:
		{
			if (callFrame.dwChild == 0)
			{
				LOG_PROCESS_ERROR(pNode->uChildrenCount == 1);
				callFrame.dwChild = pNode->dwChild;
				rCtx.byReturnValue = brvError;
				rCtx.bRollback = TRUE;

				nRetCode = bt_push_node(rCtx, pNode->dwChild);
				LOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				rCtx.bRollback = FALSE;
				nRetCode = bt_pop_node(rCtx);
				LOG_PROCESS_ERROR(nRetCode);
			}
			break;
		}
		case btNot:
		{
			if (callFrame.dwChild == 0)
			{
				LOG_PROCESS_ERROR(pNode->uChildrenCount == 1);

				callFrame.dwChild = pNode->dwChild;
				rCtx.byReturnValue = brvError;

				nRetCode = bt_push_node(rCtx, pNode->dwChild);
				LOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				if (rCtx.byReturnValue == brvSuccess)
					rCtx.byReturnValue = brvFail;
				else if (rCtx.byReturnValue == brvFail)
					rCtx.byReturnValue = brvSuccess;
				else
					LOG_PROCESS_ERROR(FALSE);

				nRetCode = bt_pop_node(rCtx);
				LOG_PROCESS_ERROR(nRetCode);
			}
			break;
		}
		case btRepeat:
		{
			if (callFrame.dwChild == 0)
			{
				if (pNode->uChildrenCount > 0)
				{
					int32_t nRepeatTimes = pNode->nParam[0];

					LOG_PROCESS_ERROR(nRepeatTimes > 0 && nRepeatTimes <= MAX_BT_REPEAT_TIME);
					LOG_PROCESS_ERROR(pNode->uChildrenCount == 1);
					LOG_PROCESS_ERROR(rCtx.byRepeatCounter == 0);

					rCtx.byRepeatCounter = nRepeatTimes;

					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatRepeated : times %d", _get_debug_space_info(rCtx.byCallFrameCount), pNode->nParam[0]);
					rCtx.byReturnValue = brvError;

					nRetCode = bt_push_node(rCtx, pNode->dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					rCtx.byReturnValue = brvSuccess;
					
					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbatRepeat : no child node return %s", _get_debug_space_info(rCtx.byCallFrameCount), pNode->nParam[0], GET_RETURN_STR(rCtx.byReturnValue));

					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				rCtx.byRepeatCounter--;

				if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
					INF("%sbatRepeated : times [%d|%d] return %s", _get_debug_space_info(rCtx.byCallFrameCount), pNode->nParam[0] - rCtx.byRepeatCounter, pNode->nParam[0], GET_RETURN_STR(rCtx.byReturnValue));

				if (rCtx.byRepeatCounter > 0 && rCtx.byReturnValue == brvSuccess)
				{
					callFrame.dwChild = pNode->dwChild;
					nRetCode = bt_push_node(rCtx, pNode->dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			break;
		}
		case btRandom:
		{
            if (callFrame.dwChild == 0)
            {
                if (pNode->uChildrenCount >= 1 && pNode->dwChild)
                {
                    int32_t nTotalWeight = 0;
                    BT_NODE* pChild = NULL;
					uint32_t dwChild = 0;
					int32_t nRandom = 0;
					int32_t nCounter = 0;

					for (int32_t i = 0; i < pNode->uChildrenCount; i++)
                    {
                        nTotalWeight += pNode->nParam[i];
                    }
                    LOG_PROCESS_ERROR(nTotalWeight > 0);

					nRandom = rand_ex(nTotalWeight);

					dwChild = pNode->dwChild;
                    while (dwChild)
                    {
                        nRandom -= pNode->nParam[nCounter];
                        if (nRandom < 0)
                            break;

						pChild = bt_get_node(dwChild);
						LOG_PROCESS_ERROR(pChild);

						dwChild = pChild->dwSibling;
                        nCounter++;
                    }

					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbtRandom : choose child %d on (%d/%d)", _get_debug_space_info(rCtx.byCallFrameCount), nCounter, 
						pNode->nParam[nCounter], nTotalWeight);

                    callFrame.dwChild = dwChild;
                    rCtx.byReturnValue = brvError;

                    nRetCode = bt_push_node(rCtx, dwChild);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                else
                {
                    rCtx.byReturnValue = brvFail;

					if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
						INF("%sbtRandom : no child to choose return %s", _get_debug_space_info(rCtx.byCallFrameCount),
							GET_RETURN_STR(rCtx.byReturnValue));

                    nRetCode = bt_pop_node(rCtx);
                    LOG_PROCESS_ERROR( nRetCode );
                }
            }
            else
            {
				if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
					INF("%sbtRandom : %s", _get_debug_space_info(rCtx.byCallFrameCount), GET_RETURN_STR(rCtx.byReturnValue));

				nRetCode = bt_pop_node(rCtx);
                LOG_PROCESS_ERROR( nRetCode );
            }
			break;
		}
		default:
			// custom node process
			if (callFrame.dwChild == 0)
			{
				rCtx.byReturnValue = call_node_func(rCtx, pNode);
				LOG_PROCESS_ERROR(rCtx.byReturnValue != brvError);
				PROCESS_SUCCESS(rCtx.byReturnValue == brvRunning);

				callFrame.dwChild = pNode->dwChild;
				pChild = bt_get_node(callFrame.dwChild);
				while (pChild && pChild->uReturnValue != rCtx.byReturnValue)
				{
					callFrame.dwChild = pChild->dwSibling;
					pChild = bt_get_node(callFrame.dwChild);
				}

				if (callFrame.dwChild)
				{
					callFrame.dwChild = pNode->dwChild;
					rCtx.byReturnValue = brvError;

					nRetCode = bt_push_node(rCtx, callFrame.dwChild);
					LOG_PROCESS_ERROR(nRetCode);
				}
				else
				{
					nRetCode = bt_pop_node(rCtx);
					LOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				pChild = bt_get_node(callFrame.dwChild);
				LOG_PROCESS_ERROR(pChild);

				rCtx.byReturnValue = pChild->uReturnValue;

				nRetCode = bt_pop_node(rCtx);
				LOG_PROCESS_ERROR(nRetCode);
			}
			break;
		}
	}

	PROCESS_ERROR(rCtx.byReturnValue == brvSuccess);

Exit1:
	return rCtx.byReturnValue;

Exit0:
	if (rCtx.byReturnValue == brvError)
	{
		//CRI("failed to run bt tree %d %s")

		for (int32_t i = rCtx.byCallFrameCount - 1; i >= 0; i--)
		{
			BT_NODE* pNode = bt_get_node(rCtx.CallFrame[i].dwNode);
			if (pNode)
				CRI("[%d] node [%s] param {%d %d %d %d %d %d}", i, get_action_name(pNode->uType), 
					pNode->nParam[0], pNode->nParam[1], pNode->nParam[2], pNode->nParam[3], pNode->nParam[4], pNode->nParam[5]);
			else
				CRI("[%d] unknown node", i);
		}
	}

	rCtx.byCallFrameCount = 0;
	return rCtx.byReturnValue;
}

void CBTMgr::_get_crc(uint32_t dwNode, int32_t nLayer, uint32_t &rdwCRC)
{
	int32_t nChildIndex = 0;
	BT_NODE* pNode = NULL;

	PROCESS_ERROR(dwNode > 0);
	LOG_PROCESS_ERROR(nLayer < MAX_CALL_FRAME);

	pNode = bt_get_node(dwNode);
	LOG_PROCESS_ERROR(pNode);

	rdwCRC = crc32c_append(rdwCRC, (const uint8_t*)pNode, sizeof(*pNode) - 8);

	if (pNode->dwChild)
	{
		_get_crc(pNode->dwChild, nLayer + 1, rdwCRC);
	}
	if (pNode->dwSibling)
	{
		_get_crc(pNode->dwSibling, nLayer, rdwCRC);
	}

Exit0:
	return;
}

BOOL CBTMgr::TRAVERSE_BT_CTX_RESUME::operator()(uint32_t dwBtCtx, BT_CTX* pCtx)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pOwnerDataList);

	pCtx->pOwner = pOwnerDataList[pCtx->uOwnerType].pGetOwnerFunc(pCtx->qwOwnerID);
	LOG_PROCESS_ERROR(pCtx->pOwner);

	pCtx->pScript = CScriptMgr::instance().find_script(pCtx->qwScriptNameHash);
	LOG_PROCESS_ERROR(pCtx->pScript);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBTMgr::TRAVERSE_BT_CTX_PROCESS::operator()(uint32_t dwBtCtx, BT_CTX* pCtx)
{
	int32_t nRetCode = 0;

	m_pCtx = pCtx;

	nRetCode = CBTMgr::instance()._process_bt_ctx(pCtx);
	if (!nRetCode)
		CBTMgr::instance().m_BtCtx.delete_object(pCtx);

	m_pCtx = NULL;

	return TRUE;
}

BOOL CBTMgr::process_bt_ctx(void)
{
	int32_t nRetCode = 0;
	TRAVERSE_BT_CTX_PROCESS TraverseBtCtx;

	m_BtCtx.traverse(TraverseBtCtx);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBTMgr::_init_c_action(void)
{
	int32_t nRetCode = 0;
	
	REG_BT_C_ACTION(batSelector, NULL);
	REG_BT_C_ACTION(batSequencer, NULL);
	REG_BT_C_ACTION(batParallel, NULL);
	REG_BT_C_ACTION(batNot, NULL);
	REG_BT_C_ACTION(batDebug, NULL);
	REG_BT_C_ACTION(batRollback, NULL);
	REG_BT_C_ACTION(batRandom, NULL);
	REG_BT_C_ACTION(batRepeat, NULL);
	REG_BT_C_ACTION(batReturnSuccess, bat_return_success);
	REG_BT_C_ACTION(batReturnFail, bat_return_fail);

	return TRUE;
Exit0:
	return FALSE;
}

const char* CBTMgr::_get_debug_space_info(int32_t nDepth)
{
	for (int32_t i = 0; i < (nDepth - 1) * 3; i++)
		m_szSpaceInfo[i] = ' ';
	m_szSpaceInfo[(nDepth - 1) * 3] = 0;

	return m_szSpaceInfo;
}

BOOL CBTMgr::_process_bt_ctx(BT_CTX* pCtx)
{
	int32_t nRetCode = 0;
	void* pOwner = NULL;
	CLuaScript* pScript = NULL;

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(m_OwnerDataList[pCtx->uOwnerType].pGetOwnerFunc);

	pOwner = m_OwnerDataList[pCtx->uOwnerType].pGetOwnerFunc(pCtx->qwOwnerID);
	LOG_PROCESS_ERROR(pOwner);

	pScript = CScriptMgr::instance().find_script(pCtx->qwScriptNameHash);
	LOG_PROCESS_ERROR(pScript);

	nRetCode = CBTMgr::instance()._run(*pCtx, pOwner, pScript);
	PROCESS_ERROR(nRetCode == brvRunning);

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CBTMgr::call_node_func(BT_CTX& rCtx, BT_NODE* pNode)
{
	int32_t nResult = brvError;
	int32_t nRetCode = 0;
	int32_t nParam[MAX_BT_PARAM] = { 0 };

	LOG_PROCESS_ERROR(pNode);

	LOG_PROCESS_ERROR(pNode->uType > btInvalid && pNode->uType < btTotal);
	LOG_PROCESS_ERROR(rCtx.pScript);

	m_pMgrData->pCurrentCtx = &rCtx;

	// set params 
	for (int32_t nIndex = 0; nIndex < MAX_BT_PARAM; nIndex++)
	{
		//every var 5bit 
		uint32_t uVarType = (pNode->uVarType >> (5 * nIndex)) & 0x1F;
		switch (uVarType)
		{
			case bptNormal:
			{
				nParam[nIndex] = pNode->nParam[nIndex];
				break;
			}
			case bptLuaFunc:
			{
				char szFuncName[BT_COMMON_LEN];
				snprintf(szFuncName, BT_COMMON_LEN, "ParamFunc%u", pNode->nParam[nIndex]);

				nRetCode = rCtx.pScript->call_function(szFuncName, "o:d",
					rCtx.pOwner, m_OwnerDataList[rCtx.uOwnerType].szName, nParam + nIndex);
				LOG_PROCESS_ERROR(nRetCode);
				break;
			}
			case bptRoleVar:
			case bptSceneVar:
			{
				LOG_PROCESS_ERROR(m_OwnerDataList[rCtx.uOwnerType].pGetOwnerVarFunc);
				nParam[nIndex] = m_OwnerDataList[rCtx.uOwnerType].pGetOwnerVarFunc(rCtx.qwOwnerID, uVarType, pNode->nParam[nIndex]);
				break;
			}
			case bptLocalEventVar:
			{
				LOG_PROCESS_ERROR(m_pMgrData->pCurrentCtx);
				LOG_PROCESS_ERROR(pNode->nParam[nIndex] >= 0 && pNode->nParam[nIndex] < MAX_BT_EVENT_VAR_COUNT);
				nParam[nIndex] = m_pMgrData->pCurrentCtx->llEventVarList[pNode->nParam[nIndex]];
				break;
			}
			case bptLocalTriggerVar:
			{
				LOG_PROCESS_ERROR(m_pMgrData->pCurrentCtx);
				LOG_PROCESS_ERROR(pNode->nParam[nIndex] >= 0 && pNode->nParam[nIndex] < MAX_BT_TRIGGER_VAR_COUNT);
				nParam[nIndex] = m_pMgrData->pCurrentCtx->llTriggerVarList[pNode->nParam[nIndex]];
				break;
			}
			default:
				break;
		}
	}
		
	//call action function
	if (m_pMgrData->ActionDefList[pNode->uType].bLuaFunc &&
		m_pMgrData->ActionDefList[pNode->uType].szName[0])
	{
		nRetCode = rCtx.pScript->call_function(m_pMgrData->ActionDefList[pNode->uType].szName, "odddddd:d",
			rCtx.pOwner, m_OwnerDataList[rCtx.uOwnerType].szName, nParam[0], nParam[1], nParam[2], nParam[3], nParam[4], nParam[5], &nResult);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		LOG_PROCESS_ERROR(m_pMgrData->ActionDefList[pNode->uType].pFunc);

		nResult = m_pMgrData->ActionDefList[pNode->uType].pFunc(rCtx, nParam);
	}

	if (rCtx.bDebugEnabled || m_pMgrData->bDebugEnabled)
	{
		INF("%s%s (%d %d %d %d %d %d) : %s", _get_debug_space_info(rCtx.byCallFrameCount), get_action_name(pNode->uType),
			nParam[0], nParam[1], nParam[2], nParam[3], nParam[4], nParam[5], GET_RETURN_STR(nResult));
	}

Exit0:
	m_pMgrData->pCurrentCtx = NULL;

	if (nResult == brvError)
	{
		if (pNode)
			WRN("failed to run bt node %s(%d %d %d %d %d %d) result %d",
				get_action_name(pNode->uType), nParam[0], nParam[1], nParam[2], nParam[3], nParam[4], nParam[5], nResult);
		else
			WRN("failed to run bt node, node NULL");
	}
	return nResult;
}

const char* CBTMgr::get_action_name(int32_t nActionID)
{
	BT_ACTION_FUNC_DEF* pAction = NULL;

	LOG_PROCESS_ERROR(nActionID > 0 && nActionID < btTotal);
	pAction = &(m_pMgrData->ActionDefList[nActionID]);

	LOG_PROCESS_ERROR(pAction->nActionID == nActionID);
	LOG_PROCESS_ERROR(pAction->szName[0] != '\0');

	return pAction->szName;	
Exit0:
	return "Unknown action";
}

int32_t CBTMgr::get_action_id(const char* pcszActionName)
{
	MAP_ACTION_NAME_2_FUNC::iterator it;

	LOG_PROCESS_ERROR(pcszActionName);

	it = m_ActionName2Func.find(pcszActionName);
	PROCESS_ERROR(it != m_ActionName2Func.end());
	PROCESS_ERROR(it->second);

	return it->second->nActionID;
Exit0:
	return btInvalid;
}

void CBTMgr::print(uint32_t dwNode, int32_t nLayer)
{
	char szTab[BT_COMMON_LEN] = { 0 };
	char szParam[BT_COMMON_LEN][BT_COMMON_LEN] = { 0 };
	const char* pcszName = NULL;
	BT_NODE* pNode = NULL;

	PROCESS_ERROR(dwNode > 0);
	LOG_PROCESS_ERROR(nLayer < MAX_CALL_FRAME);

	pNode = bt_get_node(dwNode);
	LOG_PROCESS_ERROR(pNode);

	memset(szTab, ' ', nLayer * 4);

	for (int32_t nIndex = 0; nIndex < MAX_BT_PARAM; nIndex++)
	{
		uint32_t uVarType = (pNode->uVarType >> (5 * nIndex)) & 0x1F;
		switch (uVarType)
		{
			case bptNormal:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "%d", pNode->nParam[nIndex]);
				break;
			}
			case bptLocalEventVar:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "LocalEvent%d", pNode->nParam[nIndex]);
				break;
			}
			case bptLocalTriggerVar:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "LocalTrigger%d", pNode->nParam[nIndex]);
				break;
			}
			case bptLuaFunc:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "luaFunc%d", pNode->nParam[nIndex]);
				break;
			}
			case bptRoleVar:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "RoleVar%d", pNode->nParam[nIndex]);
				break;
			}
			case bptSceneVar:
			{
				snprintf(szParam[nIndex], BT_COMMON_LEN, "SceneVar%d", pNode->nParam[nIndex]);
				break;
			}
			default:
				break;
		}
	}

	if (pNode->uReturnValue == brvSuccess)
		INF("%s brvSuccess: %s (%s %s %s %s %s %s)", szTab, get_action_name(pNode->uType),
			szParam[0], szParam[1], szParam[2], szParam[3], szParam[4], szParam[5]);
	else if (pNode->uReturnValue == brvFail)
		INF("%s brvFail: %s (%s %s %s %s %s %s)", szTab, get_action_name(pNode->uType),
			szParam[0], szParam[1], szParam[2], szParam[3], szParam[4], szParam[5]);
	else if (pNode->uType <= btRollBack)
		INF("%s %s", szTab, get_action_name(pNode->uType));
	else
		INF("%s %s (%s %s %s %s %s %s)", szTab, get_action_name(pNode->uType),
			szParam[0], szParam[1], szParam[2], szParam[3], szParam[4], szParam[5]);

	if (pNode->dwChild)
		print(pNode->dwChild, nLayer + 1);

	if (pNode->dwSibling)
		print(pNode->dwSibling, nLayer);

Exit0:
	return;
}
	
BOOL CBTMgr::register_c_action(const char* pcszName, int32_t nActionID, BT_ACTION_FUNC pFunc)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pcszName);
	LOG_PROCESS_ERROR_DETAIL(nActionID > btInvalid && nActionID < btTotal, "invalid action %d", nActionID);

	LOG_PROCESS_ERROR(m_pMgrData);
	
	m_pMgrData->ActionDefList[nActionID].nActionID = nActionID;
	m_pMgrData->ActionDefList[nActionID].pFunc = pFunc;
	m_pMgrData->ActionDefList[nActionID].bLuaFunc = FALSE;
	strxcpy(m_pMgrData->ActionDefList[nActionID].szName, pcszName, sizeof(m_pMgrData->ActionDefList[nActionID].szName));

	return TRUE;
Exit0:
	return FALSE;
}

int32_t CBTMgr::register_lua_action(const char* pcszName)
{
	std::pair<MAP_ACTION_NAME_2_FUNC::iterator, bool> InsRet;

	LOG_PROCESS_ERROR(pcszName);

	m_pMgrData->ActionDefList[m_pMgrData->nActionIDGenerator].nActionID = m_pMgrData->nActionIDGenerator;
	m_pMgrData->ActionDefList[m_pMgrData->nActionIDGenerator].bLuaFunc = TRUE;
	m_pMgrData->ActionDefList[m_pMgrData->nActionIDGenerator].pFunc = NULL;
	strxcpy(m_pMgrData->ActionDefList[m_pMgrData->nActionIDGenerator].szName, pcszName, BT_ACTION_NAME_LEN);

	InsRet = m_ActionName2Func.insert(std::make_pair(m_pMgrData->ActionDefList[m_pMgrData->nActionIDGenerator].szName, m_pMgrData->ActionDefList + m_pMgrData->nActionIDGenerator));
	LOG_PROCESS_ERROR(InsRet.second);

	return m_pMgrData->nActionIDGenerator++;
Exit0:
	return btInvalid;
}

int32_t CBTMgr::create_bt_tree(uint32_t dwRootNodeID, CLuaScript* pScript)
{
	int32_t nRetCode = 0;
	uint32_t dwCRC = 0;

	_get_crc(dwRootNodeID, 0, dwCRC);
	dwCRC = crc32c_append(dwCRC, (const uint8_t*)pScript->get_script_name(), strnlen(pScript->get_script_name(), SCRIPT_NAME_LEN));

	MAP_CRC_2_TREE::iterator it = m_CRC2Tree.find(dwCRC);
	if (it != m_CRC2Tree.end() && it->second->dwCRC == dwCRC)
	{
		ERR("create bt tree success script %s for tree %d has exist", pScript->get_script_name(), it->second->nTreeID);
		return it->second->nTreeID;
	}
	else
	{
		int32_t& rnTreeCount = m_pMgrData->nBtTreeUsedCount;
		
		m_pMgrData->stBtTree[rnTreeCount].nTreeID = rnTreeCount;
		m_pMgrData->stBtTree[rnTreeCount].dwRootNode = dwRootNodeID;
		m_pMgrData->stBtTree[rnTreeCount].dwCRC = dwCRC;
		m_pMgrData->stBtTree[rnTreeCount].pScript = pScript;
		m_pMgrData->stBtTree[rnTreeCount].qwScriptNameHash = pScript->get_name_hash();

		m_CRC2Tree[dwCRC] = m_pMgrData->stBtTree + rnTreeCount;
		rnTreeCount++;

		return rnTreeCount - 1;
	}

Exit0:
	return 0;
}

uint32_t CBTMgr::find_bt_tree_node(int32_t nTreeID)
{
	LOG_PROCESS_ERROR(nTreeID >= 0 && nTreeID < m_pMgrData->nBtTreeUsedCount);

	return m_pMgrData->stBtTree[nTreeID].dwRootNode;
Exit0:
	return 0;
}

BOOL CBTMgr::register_owner_data(int32_t nOwnerType, const char* pcszName, GET_OWNER pGetOwnerFunc, GET_OWNER_VAR pGetOwnerVarFunc)
{
	int32_t nRetCode = 0;

	LOG_PROCESS_ERROR(pcszName);
	LOG_PROCESS_ERROR(pGetOwnerFunc);
	LOG_PROCESS_ERROR(pGetOwnerVarFunc);

	LOG_PROCESS_ERROR(nOwnerType > otInvalid && nOwnerType < otTotal);

	strxcpy(m_OwnerDataList[nOwnerType].szName, pcszName, BT_COMMON_LEN);
	m_OwnerDataList[nOwnerType].pGetOwnerFunc = pGetOwnerFunc;
	m_OwnerDataList[nOwnerType].pGetOwnerVarFunc = pGetOwnerVarFunc;

	return TRUE;
Exit0:
	return FALSE;
}
