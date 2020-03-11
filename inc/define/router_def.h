#ifndef _ROUTER_DEF_H_
#define _ROUTER_DEF_H_

#define MAX_SERVER_PER_ROUTER						(50)
#define MAX_ROUTER_LOAD_THRES						(200)
#define ROUTER_CLIENT_HEART_BEAT_INTERVAL			(1 * 1000)
#define MAX_ROUTER_CLIENT_INTERNAL_MSG_COUNT		(10)

typedef int32_t(*SERVER_LOAD_REPORT_FUNC)(void);
typedef BOOL (*MESSAGE_FUNC)(const char* pBuffer, size_t dwSize, int32_t nSrcAddr);
typedef void (*SERVER_EVENT_HANDLE)(int32_t nServiceType, int32_t nServerAddr, int32_t nParam1, int32_t nParam2);

// tolua_begin

enum ROUTER_HASH_TYPE
{
	rhtInvalid,

	rhtJumpConsistHash,

	rhtTotal
};

enum ROUTER_TRANSFER_NODE_HANDLE_TYPE
{
	rtnhtInvalid,

	rtnhtCache,						///< 缓存数据 伺机发送
	rtnhtStillSend,					///< 仍然发送数据
	rtnhtChooseNew,					///< 重新选择新的节点发送

	rtnhtTotal
};

// tolua_end

#endif