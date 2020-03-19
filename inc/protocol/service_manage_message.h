#ifndef  _SERVICE_MANAGE_MESSAGE_H_
#define  _SERVICE_MANAGE_MESSAGE_H_

#include "protocol/internal_message_header.h"
#include "define/service_def.h"

#pragma pack(push, 1)

enum SERVICE_MANAGE_MESSAGE_ID
{
	service_mgr_sync_data_begin = service_manage_message_begin,
	service_mgr_sync_server_info,
	service_mgr_sync_obj,
	service_mgr_sync_router,
	service_mgr_sync_data_end,
	service_mgr_server_info_ntf,
	service_mgr_obj_ntf,
	service_mgr_in_service,
	service_mgr_end_service,
	service_mgr_ntf_router_down,

	service_mgr_ntf_server_event,
	service_mgr_ntf_mgr_leader,

	service_mgr_raft_msg,
};

struct SERVICE_MANAGE_SYNC_DATA_BEGIN : INTERNAL_MESSAGE_HEADER
{
	int32_t				nLeaderMgrAddr;
};

struct SERVICE_MANAGE_SYNC_SERVER_INFO : INTERNAL_MESSAGE_HEADER
{
	int32_t				nServerCount;
	SERVER_INFO			ServerInfo[0];
};

struct SERVICE_MANAGE_SYNC_OBJ: INTERNAL_MESSAGE_HEADER
{
	struct OBJ_INFO
	{
		uint64_t	qwObjID;
		int32_t		nTbusAddr;
		uint32_t	dwLastUpdateTime;
	};
	int32_t		nObjCount;
	OBJ_INFO	ObjInfo[0];
};

struct SERVICE_MANAGE_SYNC_ROUTER : INTERNAL_MESSAGE_HEADER
{
	struct ROUTER_INFO
	{
		int32_t		nRouterAddr;
		uint32_t	dwLastUpdateTime;
	};
	int32_t		nRouterCount;
	ROUTER_INFO	RouterInfo[0];
};

struct SERVICE_MANAGE_SYNC_DATA_END : INTERNAL_MESSAGE_HEADER
{
};

struct SERVICE_MANAGE_SERVER_INFO_NTF : INTERNAL_MESSAGE_HEADER
{
	int32_t			nServerInfoCount;
	SERVER_INFO		ServerInfo[0];
};

struct SERVICE_MANAGE_OBJ_NTF : INTERNAL_MESSAGE_HEADER
{
	uint64_t		qwObjID;
	int32_t			nTbusAddr;
	BOOL			bRegister;
};

struct SERVICE_MANAGE_IN_SERVICE : INTERNAL_MESSAGE_HEADER
{

};

struct SERVICE_MANAGE_END_SERVICE : INTERNAL_MESSAGE_HEADER
{

};

struct SERVICE_MANAGE_NTF_SERVER_EVENT : INTERNAL_MESSAGE_HEADER
{
	int32_t			nEventType;
	int32_t			nEventParam;
	int32_t			nPhaseType;
};

struct SERVICE_MANAGE_RAFT_MSG : INTERNAL_MESSAGE_HEADER
{
	int32_t			nMsgType;
	int32_t			nMsgSize;
	char			szMsgData[0];
};

struct SERVICE_MANAGE_NTF_MGR_LEADER : INTERNAL_MESSAGE_HEADER
{
	int32_t			nMgrLeader;
};

struct SERVICE_MANAGE_NTF_ROUTER_DOWN : INTERNAL_MESSAGE_HEADER
{
	int32_t			nRouterAddr;
	int32_t			nWatchMgrAddr;
};

#pragma pack(pop)

#endif   /* ----- #ifndef _ROUTER_MESSAGE_H_  ----- */

