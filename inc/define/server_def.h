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
	svrFS			=	5,

	svrTotal
};



#endif
