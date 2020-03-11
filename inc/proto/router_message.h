#ifndef  _ROUTER_MESSAGE_H_
#define  _ROUTER_MESSAGE_H_

#include "proto/internal_message_header.h"

#pragma pack(push, 1)

enum ROUTER_MESSAGE_ID
{
	router_load_report = router_message_begin,
	router_send_by_routerid,
	router_send_by_service_type,
	router_send_by_service_inst,
	router_send_by_addr,
	router_send_by_objid,
	router_send_by_load,
	router_send_to_mgr,
	router_server_event_ack,
	router_ntf_msg_clear_ack,
	router_get_service_info,

	router_alive_report,
	router_obj_report,
	router_resume_report,
	router_unregister_report,

	router_event_prepare_report,
	router_event_execute_report,
	router_event_finish_report,

	router_ntf_server_event,
	router_ntf_msg_clear,
};

struct ROUTER_LOAD_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t		nLoadValue;
};

struct ROUTER_SEND_BY_ROUTERID : INTERNAL_MESSAGE_HEADER
{
	uint64_t	qwRouterID;
	int32_t		nServiceType;
	int32_t		nDataSize;
	int32_t		nTransferHandleType;
	char		szData[0];
};

struct ROUTER_SEND_BY_SERVICE_TYPE : INTERNAL_MESSAGE_HEADER
{
	int32_t		nServiceType;
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_SEND_BY_SERVICE_INST: INTERNAL_MESSAGE_HEADER
{
	int32_t		nServiceType;
	int32_t		nServiceInst;
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_SEND_BY_ADDR : INTERNAL_MESSAGE_HEADER
{
	int32_t		nDstServerAddr;
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_SEND_BY_OBJID : INTERNAL_MESSAGE_HEADER
{
	uint64_t	qwObjID;
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_SEND_BY_LOAD : INTERNAL_MESSAGE_HEADER
{
	int32_t		nServiceType;
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_SEND_TO_MGR : INTERNAL_MESSAGE_HEADER
{
	int32_t		nDataSize;
	char		szData[0];
};

struct ROUTER_ALIVE_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t					nReportCount;
	struct REPORT_INFO
	{
		int32_t		nTbusAddr;
		int32_t		nLoadValue;
		uint8_t		byAlive;
	};
	REPORT_INFO	ReportInfo[0];
};

struct ROUTER_OBJ_REPORT : INTERNAL_MESSAGE_HEADER
{
	uint64_t			qwObjID;
	int32_t				nTbusAddr;
	BOOL				bRegister;
};

struct ROUTER_RESUME_REPORT : INTERNAL_MESSAGE_HEADER 
{
	uint64_t			qwObjID;
	int32_t				nTbusAddr;
};
 

struct ROUTER_EVENT_EXECUTE_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_EVENT_FINISH_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t				nEventType;
	int32_t				nEventParam;
};
 
struct ROUTER_EVENT_PREPARE_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_NTF_SERVER_EVENT : INTERNAL_MESSAGE_HEADER 
{
	int32_t				nEventType;
	int32_t				nEventParam;
	int32_t				nExtraParam;
};

struct ROUTER_NTF_MSG_CLEAR: INTERNAL_MESSAGE_HEADER
{
	int32_t				nServiceType;
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_NTF_MSG_CLEAR_ACK : INTERNAL_MESSAGE_HEADER
{
	int32_t				nServiceType;
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_UNREGISTER_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t				nServerAddr;
};

struct ROUTER_SERVER_EVENT_ACK : INTERNAL_MESSAGE_HEADER
{
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_SERVER_PREPARE_REPORT : INTERNAL_MESSAGE_HEADER
{
	int32_t				nEventType;
	int32_t				nEventParam;
};

struct ROUTER_GET_SERVICE_INFO : INTERNAL_MESSAGE_HEADER
{
	int32_t				nServiceType;
};

#pragma pack(pop)

#endif   /* ----- #ifndef _ROUTER_MESSAGE_H_  ----- */

