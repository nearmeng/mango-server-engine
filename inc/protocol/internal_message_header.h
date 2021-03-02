#ifndef _INTERNAL_MESSAGE_HEADER_
#define _INTERNAL_MESSAGE_HEADER_

#pragma pack(push, 1)
#pragma warning(disable : 4200)
//tolua_begin

#define MAX_INTERNAL_MESSAGE_SIZE	(256 * 1024)

enum INTERNAL_MESSAGE_ID
{
	internal_message_begin = 0x100,

	common_message_begin = internal_message_begin,
	common_message_end = 0x0150,

	router_message_begin = common_message_end,
	router_message_end = 0x200,

	service_manage_message_begin = router_message_end,
	service_manage_message_end = 0x250,

    conn_message_begin = service_manage_message_end,
    conn_message_end = 0x300,

    db_proxy_message_begin = conn_message_end,
    db_proxy_message_end = 0x400,

    game_online_message_begin = db_proxy_message_end,
    game_online_message_end = 0x500,

	internal_message_end
};

struct INTERNAL_MESSAGE_HEADER
{
    INTERNAL_MESSAGE_HEADER()
    {
        wMsg = 0;
        nMsgSrcAddr = 0;
        qwCoroID = 0;
    }

    void init_by_header(INTERNAL_MESSAGE_HEADER* pHeader)
    {
        wMsg = 0;
        nMsgSrcAddr = 0;
        qwCoroID = pHeader->qwCoroID;
    }

	uint16_t wMsg;
	int32_t  nMsgSrcAddr;
    uint64_t qwCoroID;
};

// tolua_end
#pragma pack(pop)
#endif	// _INTERNAL_MESSAGE_HEADER_
