#ifndef _BT_MGR_H_
#define _BT_MGR_H_

#include "define/bt_def.h"

class CLuaScript;

class CBTMgr
{
public:
	CBTMgr() {};
	virtual ~CBTMgr() {};

	BOOL init(REG_BT_OWNER_FUNC pFunc, BOOL bResume);
	BOOL uninit(void);
	inline static CBTMgr& instance(void);

	inline void set_debug_mode(BOOL bEnabled);
	inline BOOL is_debug_mode(void);
	void print(uint32_t dwNode, int32_t nLayer);

	int32_t register_lua_action(const char* pcszName);
	BOOL register_c_action(const char* pcszName, int32_t nActionID, BT_ACTION_FUNC pFunc);
	const char* get_action_name(int32_t nActionID);
	int32_t get_action_id(const char* pcszName);

	uint64_t load(CLuaScript* pScript, const char* pcszTreeName);
	int32_t create_bt_tree(uint32_t dwRootNodeID, CLuaScript* pScript);
	uint32_t find_bt_tree_node(int32_t nTreeID);

	uint32_t get_used_node_count(void);
	int32_t start_run(BT_CTX& rCtx, int32_t nTreeID, int32_t nOwnerType,
		void* pOwner, uint64_t qwOwnerID, int64_t llVar0 = 0, int64_t llVar1 = 0,
		int64_t llVar2 = 0, int64_t llVar3 = 0, BOOL bRollback = FALSE);

	BOOL process_bt_ctx(void);
	inline BT_CTX* get_current_ctx(void);

	int32_t call_node_func(BT_CTX& rCtx, BT_NODE* pNode);
	inline void set_bt_text(const char* pcszText);
	inline const char* get_bt_text(void);
	
	BT_NODE* bt_create_node(uint32_t& dwNodeID);
	BT_NODE* bt_get_node(uint32_t dwNodeID);
	BOOL bt_add_child(uint32_t dwParent, uint32_t dwChild);

	BOOL register_owner_data(int32_t nOwnerType, const char* pcszName, GET_OWNER pGetOwnerFunc, GET_OWNER_VAR pGetOwnerVarFunc);

private:
	int32_t _run(BT_CTX& rCtx, void* pOwner, CLuaScript* pScript);
	void _get_crc(uint32_t dwNode, int32_t nLayer, uint32_t& rdwCRC);
	BOOL _process_bt_ctx(BT_CTX* pCtx);
	BOOL _init_c_action(void);
	const char* _get_debug_space_info(int32_t nDepth);

private:
	static CBTMgr ms_Instance;

	struct BT_ACTION_LTSTR
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strncmp(s1, s2, BT_ACTION_NAME_LEN) < 0;
		}
	};

	typedef std::map<const char*, BT_ACTION_FUNC_DEF*, BT_ACTION_LTSTR>	MAP_ACTION_NAME_2_FUNC;
	MAP_ACTION_NAME_2_FUNC	m_ActionName2Func;

	typedef std::map<uint32_t, BT_TREE*> MAP_CRC_2_TREE;
	MAP_CRC_2_TREE			m_CRC2Tree;

	struct TRAVERSE_BT_CTX_PROCESS
	{
		BT_CTX* m_pCtx; //用于执行出问题的时候的定位
		BOOL operator()(uint32_t dwBtCtx, BT_CTX* pCtx);
	};

	CShmObjectPool<uint32_t, BT_CTX>	m_BtCtx;
	CShmObject<BT_MGR_DATA>	m_BtMgrData;

	struct OWNER_DATA
	{
		char			szName[BT_COMMON_LEN];
		GET_OWNER		pGetOwnerFunc;
		GET_OWNER_VAR	pGetOwnerVarFunc;
	};
	OWNER_DATA m_OwnerDataList[otTotal];
	
	struct TRAVERSE_BT_CTX_RESUME
	{
		OWNER_DATA* pOwnerDataList;
		BOOL operator()(uint32_t dwBtCtx, BT_CTX* pCtx);
	};


	BT_MGR_DATA*	m_pMgrData;	
	char m_szSpaceInfo[BT_DEBUG_INFO_LEN];
};

inline CBTMgr& CBTMgr::instance(void)
{
	return ms_Instance;
}

inline BT_CTX* CBTMgr::get_current_ctx(void)
{
	LOG_PROCESS_ERROR(m_pMgrData);

	return m_pMgrData->pCurrentCtx;
Exit0:
	return NULL;
}

inline void CBTMgr::set_debug_mode(BOOL bEnabled)
{
	LOG_PROCESS_ERROR(m_pMgrData);

	m_pMgrData->bDebugEnabled  = bEnabled;
Exit0:
	return;
}

inline BOOL CBTMgr::is_debug_mode(void)
{
	LOG_PROCESS_ERROR(m_pMgrData);

	return m_pMgrData->bDebugEnabled;
Exit0:
	return FALSE;
}

inline void CBTMgr::set_bt_text(const char * pcszText)
{
	LOG_PROCESS_ERROR(pcszText);
	LOG_PROCESS_ERROR(m_pMgrData);

	strxcpy(m_pMgrData->szText, pcszText, MAX_BT_TEXT_LEN);
Exit0:

	return;
}

inline const char * CBTMgr::get_bt_text(void)
{
	LOG_PROCESS_ERROR(m_pMgrData);

	return m_pMgrData->szText;
Exit0:
	return NULL;
}

#endif
