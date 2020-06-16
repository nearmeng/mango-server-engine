#ifndef _TBUS_INTENAL_H_
#define _TBUS_INTENAL_H_

#include "tbus/tbus.h"

#pragma pack(push, 1)

struct tagChannelLastCheckVar
{
	int iLastSeq;
	int iLastHeadSeq;
	uint64_t ullRecordTime;
};

struct tagChannelVar
{
	unsigned int dwSize;                   	
	volatile unsigned int dwHead;			
	volatile unsigned int dwTail;			
	int iSeq;								
	unsigned int dwGet;						
	int iRecvSeq;                       	
	char chAuth;                          	
	char chGStart;                        	
	char chPStart;                        	
    char chPadding;                       	
    unsigned int dwCtrlChnnlSize;           
    volatile unsigned int dwCtrlChnnlHead;  
    volatile unsigned int dwCtrlChnnlTail;  
    int iCtrlMsgSeq;                        
    struct tagChannelLastCheckVar stLastCheck;        
	char sReserve[256 - sizeof(int)*11-sizeof(struct tagChannelLastCheckVar)];
};
#pragma pack(pop)

typedef struct tagChannelVar                       	CHANNELVAR;
typedef struct tagChannelVar                       	*LPCHANNELVAR;

#define TBUS_CHANNEL_HEAD_RESERVE_NUM   (4)

struct tagChannelHead
{
	TBUSADDR astAddr[2];          
	CHANNELVAR astQueueVar[2];    
	unsigned int dwAlignLevel;    
	HANDLE iShmID;	              
	unsigned int dwMagic;		  
    unsigned int dwVersion;       
	char sReserve[TBUS_CHANNEL_HEAD_RESERVE_NUM]; 
};

typedef struct tagChannelHead                      	CHANNELHEAD;
typedef struct tagChannelHead                      	*LPCHANNELHEAD;

struct tagTbusChannel
{
	unsigned int dwFlag;
	int iRecvSide;      
	int iSendSide;
	HANDLE iShmID;	

	CHANNELHEAD *pstHead;

	char *pszQueues[2];

    char *pszCtrlQueues[2];

	time_t tBeginDisable;		
    int iHandle;                
};

#define HANDLE_COUNT                             	2049
#define TBUS_MAX_CHANNEL_COUNT_PREHANDLE         	TBUS_MAX_CHANNEL_NUM_PREHOST
#define TBUS_MAX_BIND_ADDR_NUM_PREHANDLE			32

struct tagTbusProcessState
{
	long long llLastBeatTime;
	int iIsAlive;
	int iIsAvailable;
	int iTimeOutGap;
	TBUSADDR iPeerAddr;
	int iBindedChnnlNum;
};

typedef struct tagTbusProcessState                      TBUSPROCESSSTATE;
typedef struct tagTbusProcessState                   *LPTBUSPROCESSSTATE;

struct tagTbusHandle
{
	unsigned int dwFlag;
	unsigned int dwChannelCnt;
	int aiChannelTableIdx[TBUS_MAX_CHANNEL_COUNT_PREHANDLE];
	void* pstChannelShtable;

	unsigned int dwPeerCnt;
	TBUSPROCESSSTATE astPeerState[TBUS_MAX_CHANNEL_COUNT_PREHANDLE];

	int iRecvPos;
	unsigned int dwRecvPkgCnt;
};

typedef struct tagTbusHandle                        	TBUSHANDLE;

struct tagTbusGlobal
{
	unsigned int dwInitStatus;
	pthread_mutex_t tMutex;
	int	iBussId;
	int iFlag;
	unsigned int dwDyedMsgID;
	unsigned int dwHandleCnt;

	TBUSHANDLE *pastTbusHandle[HANDLE_COUNT];
	volatile unsigned int dwGCIMVersion;
};

typedef struct tagTbusGlobal                       	TBUSGLOBAL;
typedef struct tagTbusGlobal                       	*LPTBUSGLOBAL;

extern "C" TBUSGLOBAL g_stBusGlobal;
#define TBUS_IDX_INVALID                         	-1
#define TBUS_RECV_POS_INVALID                    	-1
#define  TBUS_GET_HANDLE(a_iHandle)  	((a_iHandle) < 1 || ((a_iHandle) >= HANDLE_COUNT) ?\
                                        NULL : g_stBusGlobal.pastTbusHandle[(a_iHandle)])

#endif	// _TBUS_INTENAL_H_
