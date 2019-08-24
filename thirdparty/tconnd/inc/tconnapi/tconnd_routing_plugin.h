/************************************************************
 *  File Module:    tconnd_routing_plugin.h
 *  File Description:
 *   定义路由扩展接口
 *  File History:
 *  <author>  <time>            <version >   <desc>
 *  jiaweixu    2011/08/26         1.0           create this moudle
 *  alancxu     2012/07/09         1.1           add function interface 'tconnd_router_getAllRoutingInfo'
 *  thomaspan   2013/12/26         1.2           add moudle to tconnd v3
 ***********************************************************/
#ifndef _TCONND_ROUTING_PLUGIN_H
#define _TCONND_ROUTING_PLUGIN_H

#ifndef IN
#define  IN
#endif

#ifndef OUT
#define  OUT
#endif

#ifndef INOUT
#define  INOUT
#endif

#define MAX_ROUTING_ERR_MSG_LEN         1024
#define MAX_ROUTING_ID_LENGTH           256 	/* 字符串账号最大长度 */
#define MAX_ROUTING_CLT_MSG_LEN         1024
#define MAX_OP_ROUTING_TABLE_MSG_LEN    (1024*1024)

typedef struct tagTRoutingCtx TROUTINGCTX;
typedef struct tagTRoutingCtx *LPTROUTINGCTX;

typedef struct tagTSerializerInfo TSERIALIZERINFO;
typedef struct tagTSerializerInfo *LPTSERIALIZERINFO;

typedef struct tagTPluginInfo TPLUGININFO;
typedef struct tagTPluginInfo *LPTPLUGININFO;

typedef struct tagTRoutingInfo TROUTINGINFO;
typedef struct tagTRoutingInfo *LPTROUTINGINFO;

union tagTROUTINGIDValue;
typedef union  tagTROUTINGIDValue                                	TROUTINGIDVALUE;
typedef union  tagTROUTINGIDValue                                	*LPTROUTINGIDVALUE;

struct tagTROUTINGAccount;
typedef struct tagTROUTINGAccount                                	TROUTINGACCOUNT;
typedef struct tagTROUTINGAccount                                	*LPTROUTINGACCOUNT;

/* TROUTING_ID*/
enum tagTROUTING_ID 
{
    TROUTING_ID_NONE = 0, 
    TROUTING_ID_UINT32 = 1, 
    TROUTING_ID_UINT64 = 2, 
    TROUTING_ID_STRING = 3, 
};

/* TROUTING_ACCOUNT*/
enum tagTROUTING_ACCOUNT 
{
    TROUTING_ACCOUNT_NONE = 0, 
    TROUTING_ACCOUNT_QQ_UIN = 1, 
    TROUTING_ACCOUNT_WX_OPENID = 4098, 
    TROUTING_ACCOUNT_QQ_OPENID = 4099, 
};

/*Base class of tconnd's routing plugin*/
class TRouterBase
{
    public:
        virtual ~TRouterBase(){}
        /**
         * @brief Initializing routing configuration. This is called when tconnd is initialized.
         * @param pszPluginInfo[IN] Some information tconnd keeps.
         * @retval Returns 0 for success, otherwise for failure.
         */
        virtual int Init(IN const LPTPLUGININFO pstPluginInfo) = 0;
		
		/**
		* @brief Reload routing configuration. 
		*        This is called when tconnd is reloading.
		* @param pszPluginInfo[IN] new tconnd context.
		* @retval Returns 0 for success, otherwise for failure.
		*/
	    virtual int Reload(IN const LPTPLUGININFO pszPluginInfo) = 0;

	    /**
	    * @brief Validate new configuration.
	    */
	    virtual void Commit() = 0;
	
	    /**
	    * @brief Rollback new configuration.
	    */
	    virtual void Rollback() = 0;
	
        /**
         * @brief Acquiring routing list by some information(e.g. tframe head command, packet and so on). This is called before tconnd sends packet to logic server.
         * @param pstCtx[IN] Routing context
         * @param pstRoutingInfo[OUT] Routing information.
         * @retval Returns 0 for success, otherwise for failure.
         */
        virtual int GetRoutingList(IN const LPTROUTINGCTX pstCtx, 
								OUT LPTROUTINGINFO pstRoutingInfo) = 0;
        /**
         * @brief Registering routing information which come from logic server into routing table. This is called when tconnd recieves TFRAMEHEAD_CMD_REGISTER_ROUTING message from logic server.
         * @param iPeerAddr[IN] Logic server's address which sends TFRAMEHEAD_CMD_REGISTER_ROUTING message. See tframe head protocol about this message.
         * @param pszRoutingArgs[IN] Routing arguments. According to these, tconnd generates routing tables.
         * @param iArgsLen[IN] The length of routing arguments.
         * @retval Returns 0 for success, otherwise for failure.
         */
        virtual int RegisterRoutingInfo(IN int iPeerAddr, 
				IN const char *pszRoutingArgs, 
				IN int iArgsLen) = 0;
};

/* ID */
union tagTROUTINGIDValue
{
    uint32_t dwUINT32;                                	/* TROUTING_ID_UINT32,  uint32格式 */
    uint64_t ullUINT64;                               	/* TROUTING_ID_UINT64,  uint64格式 */
    char szSTRING[MAX_ROUTING_ID_LENGTH];          	/* TROUTING_ID_STRING,  字符串格式 */
};

/* 用户账号 */       
struct tagTROUTINGAccount
{
    uint16_t wType;                                   	/*   账号类型 ,0 表示无账号信息，1表示QQ，其它表示不同账号平台的值，参考TROUTING_ACCOUNT Bind Macrosgroup:TROUTING_ACCOUNT,*/
    uint8_t bFormat;                                  	/*   账号格式 Bind Macrosgroup:TROUTING_ID,*/
    TROUTINGIDVALUE stValue;                        	/*   账号值 */
};

struct tagTRoutingCtx
{
    /*tconnd index*/
    int iConnIdx;
    /*TFrame head command, see tframe head protocol for detail.*/
    int iFrameCmd;
    /*The size of up-link packet.*/
    int iDataLen;
    /*The up-link packet. The application can use it to implement routing based on level-7.*/
    const char *pszData;
    /*Account Info*/
    TROUTINGACCOUNT stAccount;
    /*GID, 仅作用于GCP模式下，是ACCOUNT经过账号映射后的ID*/
    uint64_t ullGID;    
    /*client ip*/
    uint32_t ulIp;
    /*service id，对于TDR模式，这个值永为0，对于GCP和QQ模式，这个值是客户端设置的值*/
    uint32_t ulServiceID;	
};

/*The status of a serializer*/
struct tagTSerializerInfo
{
    int iSerIdx; /*the serializer's index, read only*/
    int iIsPeerActive; /*the logic server's status: 1 specify active, otherwiese dead. read only*/
    int iPeerAddr; /*the logic server's address*/
    time_t tLastUpdateState; /*The last time of updating serializer status*/
};

/*This is used by routing plugin, which keeps tconnd's information*/
struct tagTPluginInfo
{
    unsigned int dwMaxFD;  /*max connection #*/
    int iLocalAddr; /*tconnd's address*/
    const char *pszConfPath; /*the path of routing configuration file. it may be null.*/
    int iSerCount;    /*the number of all serializers*/
    LPTSERIALIZERINFO pstSerInfoList; /*the status of all serializers*/
    void *pstTdrMeta; /*对应于TDR模式中填的元数据Pkg对于的TDR META*/
};

/*Routing infomation, which is returned by routing plugin. According to this,
 * tconnd will route message to these serializers.
 */
struct tagTRoutingInfo
{
    /*The number of serializers which the tconnd will send message to. It shuold be less than or equal to tagTPluginInfo.iSerCount.*/
    int iSerCount;
    /*A array of the index of serializers and its size is equal to tagTPluginInfo.iSerCount. Its memory is allocated by the tconnd.*/
    int *serIndexList;
    /*routing error message*/
    char szErrMsg[MAX_ROUTING_ERR_MSG_LEN];
	
    /*
      Message's length When failing to select routing.
      It denotes no need to send message client if the iCltMsgLen is less than 0. 
      The default value is 0.
    */
    int iCltMsgLen;
    /*
      Message content when failing to select routing.
    */
    char szCltMsg[MAX_ROUTING_CLT_MSG_LEN];
};


#ifdef __cplusplus
extern "C"
{
#endif

    typedef TRouterBase* (*PFNTCONND_ROUTER_CREATE)(void);
    typedef void (*PFNTCONND_ROUTER_DESTROY)(TRouterBase* router);
    typedef int (*PFNTCONND_ROUTER_GetAllRoutingInfo)(IN TRouterBase* router, 
				INOUT char *pszRoutingInfo, 
				INOUT int *iRoutingInfoLen);


    /*Create concrete router object*/
    TRouterBase* tconnd_router_create(void);

    /*Destroy the router object*/
    void tconnd_router_destroy(TRouterBase* router);

    /**
     *@brief Get all routing infomation, not must be realized
     *@param router[IN] the pointer which pointers to router object
     *@param pszRoutingInfo[INOUT] the buffer which stores all routing infomation
     *      - IN pszRoutingInfo the first address which starts store routing infomation
     *      - OUT pszRoutingInfo the first address which starts store routing infomation
     *@param iRoutingInfoLen[INOUT] the pointer which pointers to value is the length of storing all routing infomation buffer
     *      - IN iRoutingInfoLen the maximize length of storing all routing infomation buffer
     *      - OUT iRoutingInfoLen the used length of storing all routing infomation, used length not greater than maximize length
     *
     *@pre \e router not NULL
     *@pre \e pszRoutingInfo not NULL
     *@pre \e iRoutingInfoLen not NULL and pointer to value is greater than 0
     *
     *@retval 0 success
     *@retval other fail
     */
    int tconnd_router_getAllRoutingInfo(IN TRouterBase* router, 
			INOUT char *pszRoutingInfo, 
			INOUT int *iRoutingInfoLen);

#ifdef __cplusplus
}
#endif

#endif

