#ifndef _SERVER_DEF_H_
#define _SERVER_DEF_H_

#define MAX_SERVER_RELOAD_INTERVAL	(5 * 1000) // 5 seconds
#define GAME_FPS					(4)

enum SERVER_TYPE
{
	svrInvalid		=	0,

	svrTconnd		=	1,
	svrGS			=	2,
	svrRouter		=	3,
	svrServiceMgr	=	4,
	svrControl		=	5,
	svrTest			=	6,
    svrConn         =   7,

	svrTotal
};

#define MAKE_SESSION_ID(__tconnd_addr__, __conn_index__)	((((__tconnd_addr__) + 0ULL) << 32) | (__conn_index__))
#define GET_TCONND_ADDR(__session_id__)		                ((__session_id__) >> 32)
#define GET_CONN_INDEX(__session_id__)		                ((__session_id__) & 0xFFFFFFFF)

#endif
