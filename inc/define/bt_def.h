#ifndef _BT_DEF_H_
#define _BT_DEF_H_

#include "object_def.h"

#define MAX_BT_CHILD_COUNT		(64)
#define MAX_CALL_FRAME			(16)
#define MAX_BT_PARAM			(6)
#define BT_NODE_NAME_LEN		(32)
#define MAX_BT_REPEAT_TIME		(32)
#define BT_LOCAL_VAR_COUNT		(4)
#define MAX_BT_TRIGGER_VAR_COUNT	(2)
#define MAX_BT_EVENT_VAR_COUNT		(2)

#define BT_COMMON_LEN			(128)
#define BT_ACTION_NAME_LEN		(32)
#define MAX_BT_TEXT_LEN			(128)
#define BT_DEBUG_INFO_LEN		(256)
#define MAX_BT_NODE_COUNT		(1024 * 512)
#define MAX_BT_TREE_COUNT		(1024 * 4)

#define LOAD_BT_FUNC_NAME		"load_behavior_tree"

#define GET_RETURN_STR(__value__)	(s_szRetValueNameList[__value__ - brvError])

#define REG_BT_C_ACTION(__type__, __func__)  \
	nRetCode = CBTMgr::instance().register_c_action(#__type__, __type__, __func__); \
	LOG_PROCESS_ERROR(nRetCode);

#define REG_BT_OWNER(__type__, __class_name__, __get_owner_func__, __get_owner_var_func__) \
	nRetCode |= CBTMgr::instance().register_owner_data(__type__, __class_name__, __get_owner_func__, __get_owner_var_func__); \
	LOG_PROCESS_ERROR(nRetCode)

//tolua_begin

enum BT_PARAM_TYPE
{
	bptInvalid = -1,

	bptNormal,
	bptLuaFunc,
	bptLocalEventVar,
	bptLocalTriggerVar,
	bptRoleVar,
	bptSceneVar,

	bptTotal
};

enum BT_RETURN_VALUE
{
	brvError = 100,

	brvFail,
	brvSuccess,
	brvRunning,

	brvTotal
};

enum BT_NODE_TYPE
{
	btInvalid = 0,

	btSelector,
	btSequencer,
	btParallel,
	btNot,
	btDebug,
	btRollBack,
	btRandom,
	btRepeat,

	btUserDefine,

	btTotal = 1024
};

enum BT_TARGET
{
	tarInvalid,

	tarOwner,
	tarTarget,

	tarTotal
};

enum BT_OPERATOR
{
	boLT,
	boLTE,
	boGT,
	boGTE,
	boEQ,
	boNEQ,
};

enum BT_ACTION_TYPE
{
	batInvalid = btInvalid,
	batSelector = btSelector,
	batSequencer = btSequencer,
	batParallel = btParallel,
	batNot = btNot,
	batDebug = btDebug,
	batRollback = btRollBack,
	batRandom = btRandom,
	batRepeat = btRepeat,

	batBegin = btUserDefine,

	batReturnSuccess,
	batReturnFail,
	batPrint,

	batSetLocalVar,
	batGetLocalVar,
	batCheckRoleLevel,
	batRoleRun,

	batEnd
};

//tolua_end

#pragma pack(push, 1)
struct BT_NODE
{
	unsigned	uType				: 16;
	unsigned	uChildrenCount		: 8;
	unsigned	uReturnValue		: 8;
	unsigned	uVarType			: 32;

	int32_t		nParam[MAX_BT_PARAM];
	uint32_t	dwChild;
	uint32_t	dwSibling;
};

struct BT_CTX
{
	struct CALL_FRAME
	{
		uint32_t	dwNode;
		uint32_t	dwChild;
	};
	CALL_FRAME	CallFrame[MAX_CALL_FRAME];

	void*		pOwner;
	uint64_t	qwOwnerID;
	CLuaScript* pScript;
	uint64_t	qwScriptNameHash;
	uint8_t		byReturnValue;
	uint8_t		byCallFrameCount;
	uint8_t		byRepeatCounter;
	uint8_t		byReserved;
	int32_t		nTreeID;
	int64_t		llEventVarList[MAX_BT_EVENT_VAR_COUNT];
	int64_t		llTriggerVarList[MAX_BT_TRIGGER_VAR_COUNT];
	int32_t		nResult;
	int32_t		nWaitTimer;

	unsigned	uOwnerType		: 6;
	unsigned	bDebugEnabled	: 1;
	unsigned	bRollback		: 1;
	unsigned	uReserved		: 24;
};
#pragma pack(pop)

typedef int32_t(*BT_ACTION_FUNC)(BT_CTX& rCtx, int32_t nParam[]);

struct BT_ACTION_FUNC_DEF
{
	int32_t			nActionID;
	char			szName[BT_ACTION_NAME_LEN];
	BT_ACTION_FUNC	pFunc;
	uint16_t		bLuaFunc;
};

struct BT_TREE
{
	int32_t nTreeID;
	uint32_t dwRootNode;
	uint32_t dwCRC;
	uint64_t qwScriptNameHash;
	CLuaScript* pScript;
};

struct BT_MGR_DATA
{
	int32_t		nActionIDGenerator;
	BT_CTX*		pCurrentCtx;
	BOOL		bDebugEnabled;
	int32_t		nBtCtxIDGenerator;
	char		szText[MAX_BT_TEXT_LEN];
		
	int32_t		nBtNodeUsedCount;
	BT_NODE		stBtNode[MAX_BT_NODE_COUNT];

	int32_t		nBtTreeUsedCount;
	BT_TREE		stBtTree[MAX_BT_TREE_COUNT];
	
	BT_ACTION_FUNC_DEF ActionDefList[btTotal];
};

typedef void* (*GET_OWNER)(uint64_t qwOwnerID);
typedef int32_t (*GET_OWNER_VAR)(uint64_t qwOwnerID, int32_t nVarType, int32_t nVarIndex);
typedef int32_t (*REG_BT_OWNER_FUNC)(void);

#endif
