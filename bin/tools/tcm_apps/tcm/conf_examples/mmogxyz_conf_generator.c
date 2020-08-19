
/**
*
* @file     mmogxyz_conf_generator.c 
* @brief   生成mmogxyz的相关配置文件
* 
* @author jackyai
* @version 1.0
* @date 2009-5-26
*
*
* Copyright (c)  2009, 腾讯科技有限公司互动娱乐研发部
* All rights reserved.
*
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "tlog/tlog.h"
#include "tdr/tdr.h"
#include "gamesvr_def.h"
#include "pal/pal.h"
#include "tsecbasedef.h"
#include "tconnddef.h"
#include "tcmapi/tcm_config_plunin.h"
#include "tcmapi/tcm_config_api.h"

#define MMOGXYZ_GET_FUNC_ID(id)		(((id)>>8) & 0xFF)		//倒数第二个字节为功能ID

#define MMOGXYZ_FUNC_ID_ROLE_DB			1	//Role DB 的功能ID
#define	MMOGXYZ_FUNC_ID_GAMESVR			10	
#define MMOGXYZ_FUNC_ID_TCONND			11

#define MMOGXYZ_GAME_CONF_TEMPLET_PATH	"../conf_tmplt"

#define MMOGXYZ_GAME_CONF_TEMPLET_FILE	"gamesvrd.xml"
#define MMOGXYZ_GAME_CONNECT_CONF_TEMPLET_FILE	"tconnd.xml"
#define MMOGXYZ_GAME_CONNECT_DEFAULT_PROT	8500

typedef struct SoEnv
{
	int status;        /*just for test, add your own data*/
	HAPIHANDLE handle; /*maintain the api handle*/
	LPTDRMETA pstGamesvrMeta;
	LPTDRMETA pstTconndMeta;
	TCONND stTconnd;	
	XYZ_GAMESVR stGamesvr;
}SoEnv;

SoEnv mydata;



LPTLOGCATEGORYINST g_pstMmogxyzConfCat = NULL;	/*日志类*/
extern unsigned char g_szMetalib_xyz_gamesvr[];
extern unsigned char g_szMetalib_tconnd[];

//////////////////////////////////////////////
/*
Fuction            :  mmogxyz_conf_getname
Descption          :  获取so名称
Input              :  p             -- 插件指针
Return             :  0             -- 成功
其他          -- 失败
*/
char* mmogxyz_conf_getname(struct config_plunin *p)
{
	return "mmogxyz";
}


/*
Fuction            :  mmogxyz_conf_getversion
Descption          :  获取so版本
Input              :  p             --插件指针
Return             :  0             -- 成功
其他          -- 失败
*/
char* mmogxyz_conf_getversion(struct config_plunin *p)
{
	printf("enter %s\n", __FUNCTION__);

	return "1.0.0";
}

/*
Fuction            :  mmogxyz_conf_init
Descption          :  so 自身的初始化函数，server在config_plunin_init初始化插件成功后会调用
Input              :  p            --插件指针
Return             :  0            -- 成功
其他         -- 失败
*/
int mmogxyz_conf_init(struct config_plunin *p)
{
	LPTDRMETALIB pstLib;

	SoEnv* pstEnv = (SoEnv*)p->data;

	pstEnv->status = 1;
	g_pstMmogxyzConfCat = (LPTLOGCATEGORYINST)tcmapi_get_logcatinst(mydata.handle);

	pstLib = (LPTDRMETALIB)&g_szMetalib_xyz_gamesvr[0];
	pstEnv->pstGamesvrMeta = tdr_get_meta_by_name(pstLib, "xyz_gamesvr");
	if (NULL == pstEnv->pstGamesvrMeta)
	{
		tlog_error(g_pstMmogxyzConfCat, 0,0, "failed to get meta by name \"xyz_gamesvr\"");
		return -1;
	}

	pstLib = (LPTDRMETALIB)&g_szMetalib_tconnd[0];
	pstEnv->pstTconndMeta = tdr_get_meta_by_name(pstLib, "tconnd");
	if (NULL == pstEnv->pstTconndMeta)
	{
		tlog_error(g_pstMmogxyzConfCat, 0,0, "failed to get meta by name \"tconnd\"");
		return -1;
	}
	

	tlog_info(g_pstMmogxyzConfCat,0,0, "mmogxyz_conf_init successfully!!!");

	return 0;
}


/*
Fuction            :  mmogxyz_conf_cleanup
Descption          :  so 自身的初始化函数，server在释放插件之前调用
Input              :  插件指针
Return             :  0          -- 成功
其他       -- 失败
*/
int mmogxyz_conf_cleanup(struct config_plunin *p)
{
	SoEnv* pstEnv = (SoEnv*)p->data;

	pstEnv->status = 0;

	return 0;

}

int mmogxyz_generate_gamesvr_conf(char* dir, int id)
{
	TDRDATA stHost;
	int iRet = 0;
	APIPROCINST stGameProc;
	int aiOtherPids[4];
	int iProcCount;
	int iFindTconnd;
	int iFindTorm;
	int i;
	 char szFilePath[TCMAPI_MAX_PATH_LEN];    

	if (NULL == dir)
	{
		return -1;
	}
	
	//读取配置文件模版中的数据
	stHost.iBuff = sizeof(mydata.stGamesvr);
	stHost.pszBuff = (char *)&mydata.stGamesvr;
	snprintf(szFilePath, sizeof(szFilePath), "%s%c%s", 
		MMOGXYZ_GAME_CONF_TEMPLET_PATH, TOS_DIRSEP, MMOGXYZ_GAME_CONF_TEMPLET_FILE);
	iRet = tdr_input_file(mydata.pstGamesvrMeta, &stHost, &szFilePath[0], 
		0, TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to input file %s， %s",
			szFilePath,tdr_error_string(iRet));
		return -2;
	}

	APIPROCINST proc_inst;
	memset(&proc_inst,0x00,sizeof(proc_inst));
	tcmapi_get_proc_inst(mydata.handle,&proc_inst,id);
	
	memset(&proc_inst,0x00,sizeof(proc_inst));
	tcmapi_get_proc_inst_by_idx(mydata.handle,&proc_inst,0);
	
	tcmapi_get_group_customattr(mydata.handle,id);
	
	tcmapi_get_layer_customattr(mydata.handle,id);
	
	APIHOST host;
	memset(&host,0x00,sizeof(host));
	iRet = tcmapi_get_host_by_name(mydata.handle,&host,"Host97");
	if(0 == iRet)
	{
		tlog_debug(g_pstMmogxyzConfCat,0,0,"%s:%s ,CustomAttr:%s,GCIMKEY:%s",host.szHostName,host.szInnerIP,host.szCustomAttr,host.szGCIMShmKey);
	}

	iRet = tcmapi_get_host_by_idx(mydata.handle,&host,0);
	if(0 == iRet)
	{
		tlog_debug(g_pstMmogxyzConfCat,0,0,"idx:%d,%s:%s ,CustomAttr:%s,GCIMKEY:%s",0,host.szHostName,host.szInnerIP,host.szCustomAttr,host.szGCIMShmKey);
	}
	
	tcmapi_get_layer_customattr(mydata.handle,id);
	
	iProcCount = sizeof(aiOtherPids)/sizeof(aiOtherPids[0]);
	iRet = tcmapi_get_procs_with_buscommunication(mydata.handle, id, &aiOtherPids[0], &iProcCount);
	tlog_debug(g_pstMmogxyzConfCat,0,0, "get proc num: %d", iProcCount);

	iFindTconnd = 0;
	iFindTorm = 0;
	for (i = 0; i < iProcCount; i++)
	{
		if (MMOGXYZ_GET_FUNC_ID(aiOtherPids[i]) == MMOGXYZ_FUNC_ID_TCONND)
		{
			iRet = tcmapi_get_proc_inst(mydata.handle, &stGameProc, aiOtherPids[i]);
			if (0 != iRet)
			{
				tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get proc(id:%u)'s configure info, iRet:%d",
					aiOtherPids[i], iRet);
				break;
			}
			STRNCPY(mydata.stGamesvr.szConnectorAddr, stGameProc.szAddr, sizeof(mydata.stGamesvr.szConnectorAddr));
			iFindTconnd = 1;
		}
		if (MMOGXYZ_GET_FUNC_ID(aiOtherPids[i]) == MMOGXYZ_FUNC_ID_ROLE_DB)
		{
			iRet = tcmapi_get_proc_inst(mydata.handle, &stGameProc, aiOtherPids[i]);
			if (0 != iRet)
			{
				tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get proc(id:%u)'s configure info, iRet:%d",
					aiOtherPids[i], iRet);
				break;
			}
			STRNCPY(mydata.stGamesvr.szTormsvrAddr, stGameProc.szAddr, sizeof(mydata.stGamesvr.szTormsvrAddr));
			iFindTorm = 1;
		}
	}/*for (i = 0; i < iProcCount; i++)*/

	if ((0 != iRet) ||(!iFindTconnd)||(!iFindTorm))
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get tconnd's addr(%d) or tormsvr's addr(%d)",
			iFindTconnd, iFindTorm);
		return -2;
	}


	//输出配置文件
	stHost.iBuff = sizeof(mydata.stGamesvr);
	stHost.pszBuff = (char *)&mydata.stGamesvr;
	snprintf(szFilePath, sizeof(szFilePath), "%s%c%s", 
		dir, TOS_DIRSEP, MMOGXYZ_GAME_CONF_TEMPLET_FILE);
	iRet = tdr_output_file(mydata.pstGamesvrMeta, &szFilePath[0], &stHost, 
		0, TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to input file %s， %s",
			szFilePath,tdr_error_string(iRet));
	}

	return iRet;
}

int mmogxyz_generate_tconnd_listenurl(LPLISTENER a_pstListener , int a_iProcID)
{
	int iRet = 0;
	APIPROCINST stProc;
	APIHOST stHost;
	char szCustomAttr[1024];
	const char *pszCustomAttr;
	char *pch;
	int iPort;

	assert(NULL!= a_pstListener);

	iRet = tcmapi_get_proc_inst(mydata.handle, &stProc, a_iProcID);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get proc(id:%u)'s configure info, iRet:%d",
			a_iProcID, iRet);
		return -1;
	}
	iRet = tcmapi_get_host_by_name(mydata.handle, &stHost, stProc.szHostName);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get host info by name %s",
			stProc.szHostName);
		return -1;
	}
	if (0 >= stHost.iOuterIpNum)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "host[%s] has not outerip",
			stProc.szHostName);
		return -1;
	}
	//test call tcmapi_get_proc_inst_by_idx
	tcmapi_get_proc_inst_by_idx(mydata.handle, &stProc,0);
	//test call tcmapi_get_host_by_idx
	tcmapi_get_host_by_idx(mydata.handle, &stHost,0);

	tcmapi_get_group_customattr(mydata.handle,a_iProcID);

	tcmapi_get_layer_customattr(mydata.handle,a_iProcID);

	tcmapi_get_host_num(mydata.handle);

	tcmapi_get_proc_num(mydata.handle);

	int iProcSet[64] = {0};
	int iProcNum = sizeof(iProcSet)/sizeof(iProcSet[0]);

	int iNum = 0;
	
	iNum = tcmapi_get_procset_num_by_str(mydata.handle,"*.*.11.*");
	
	tlog_trace(g_pstMmogxyzConfCat,0,0,"iProcNum(%d) = tcmapi_get_procset_num_by_str(%s)",iNum,"*.*.11.*");
	iRet = tcmapi_get_procset_by_str(mydata.handle,"10.1.11.1",iProcSet,&iProcNum);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to tcmapi_get_procset_by_str(%s), iRet:%d",
			"10.1.11.1", iRet);
		return -1;
	}	

	int i;
	for(i = 0;i < iProcNum;i++)
	{
		tlog_trace(g_pstMmogxyzConfCat,0,0,"get procset by(%s),Proc[%d]: %d","10.1.11.1",i,iProcSet[i]);
	}

	iProcNum = sizeof(iProcSet)/sizeof(iProcSet[0]);
	iRet = tcmapi_get_procset_by_str(mydata.handle,"*.*.*.*",iProcSet,&iProcNum);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to tcmapi_get_procset_by_str(%s), iRet:%d",
			"*.*.*.*", iRet);
		return -1;
	}

	
	
	for(i = 0;i < iProcNum;i++)
	{
		tlog_trace(g_pstMmogxyzConfCat,0,0,"get procset by(%s),Proc[%d]: %d","*.*.*.*",i,iProcSet[i]);
	}

	iProcNum = sizeof(iProcSet)/sizeof(iProcSet[0]);
	iRet = tcmapi_get_procset_by_str(mydata.handle,"*.*.11.*",iProcSet,&iProcNum);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to tcmapi_get_procset_by_str(%s), iRet:%d",
			"*.*.11.*", iRet);
		return -1;
	}
	
	for(i = 0;i < iProcNum;i++)
	{
		tlog_trace(g_pstMmogxyzConfCat,0,0,"get procset by(%s),Proc[%d]: %d","*.*.11.*",i,iProcSet[i]);
	}	
	
	//分析端口
	iPort = MMOGXYZ_GAME_CONNECT_DEFAULT_PROT;
	pszCustomAttr = tcmapi_get_group_customattr(mydata.handle, a_iProcID);
	if (NULL != pszCustomAttr)
	{
		STRNCPY(szCustomAttr, pszCustomAttr, sizeof(szCustomAttr));
		pch = strstr(szCustomAttr, "tconndport:");
		if (NULL !=  pch)
		{
			pch += strlen("tconndport:");
			iPort = atoi(pch);
		}
	}/*if (NULL != pszCustomAttr)*/
	

	
	snprintf(a_pstListener->szUrl, sizeof(a_pstListener->szUrl), "tcp://%s:%d?reuse=1",
		stHost.szOuterIP[0], iPort);

	return iRet;
}

int mmogxyz_generate_game_connect_conf(char* dir, int id)
{
	TDRDATA stHost;
	int iRet = 0;
	APIPROCINST stGameProc;
	int aiOtherPids[4];
	int iProcCount;
	int iGamesvr;
	int i;
	char szFilePath[TCMAPI_MAX_PATH_LEN];    
	LPTCONND pstTconnd ;

	if (NULL == dir)
	{
		return -1;
	}

	//读取配置文件模版中的数据
	stHost.iBuff = sizeof(mydata.stTconnd);
	stHost.pszBuff = (char *)&mydata.stTconnd;
	snprintf(szFilePath, sizeof(szFilePath), "%s%c%s", 
		MMOGXYZ_GAME_CONF_TEMPLET_PATH, TOS_DIRSEP, MMOGXYZ_GAME_CONNECT_CONF_TEMPLET_FILE);
	iRet = tdr_input_file(mydata.pstTconndMeta, &stHost, &szFilePath[0], 
		0, TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to input file %s， %s",
			szFilePath,tdr_error_string(iRet));
		return -2;
	}
	

	iProcCount = sizeof(aiOtherPids)/sizeof(aiOtherPids[0]);
	iRet = tcmapi_get_procs_with_buscommunication(mydata.handle, id, &aiOtherPids[0], &iProcCount);
	tlog_debug(g_pstMmogxyzConfCat,0,0, "get proc num: %d", iProcCount);

	iGamesvr = 0;
	pstTconnd = &mydata.stTconnd;
	for (i = 0; i < iProcCount; i++)
	{
		if (MMOGXYZ_GET_FUNC_ID(aiOtherPids[i]) == MMOGXYZ_FUNC_ID_GAMESVR)
		{
			iRet = tcmapi_get_proc_inst(mydata.handle, &stGameProc, aiOtherPids[i]);
			if (0 != iRet)
			{
				tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get proc(id:%u)'s configure info, iRet:%d",
					aiOtherPids[i], iRet);
				break;
			}
			STRNCPY(pstTconnd->stSerializerList.astSerializers[0].szUrl, stGameProc.szAddr, 
				sizeof(pstTconnd->stSerializerList.astSerializers[0].szUrl));
			iGamesvr = 1;
		}
		
	}/*for (i = 0; i < iProcCount; i++)*/

	if ((0 != iRet) ||(!iGamesvr))
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to get gamesvr's addr");
		return -2;
	}

	//分析tconnd的监听端口
	iRet = mmogxyz_generate_tconnd_listenurl(&pstTconnd->stListenerList.astListeners[0], id);
	if (0 != iRet)
	{
		return iRet;
	}

	//输出配置文件
	stHost.iBuff = sizeof(mydata.stTconnd);
	stHost.pszBuff = (char *)&mydata.stTconnd;
	snprintf(szFilePath, sizeof(szFilePath), "%s%c%s", 
		dir, TOS_DIRSEP, MMOGXYZ_GAME_CONNECT_CONF_TEMPLET_FILE);
	iRet = tdr_output_file(mydata.pstTconndMeta, &szFilePath[0], &stHost, 
		0, TDR_XML_DATA_FORMAT_LIST_ENTRY_NAME);
	if (0 != iRet)
	{
		tlog_error(g_pstMmogxyzConfCat,0,0, "failed to input file %s， %s",
			szFilePath,tdr_error_string(iRet));
	}

	return iRet;
}

/*
Fuction            :  mmogxyz_conf_createconfig
Descption          :  生成配置实现函数
Input              :  p         --插件指针
id        --进程id
dir       --生成配置文件存放目录
error     --错误信息预留地址
maxlen    --错误信息可用最大空间，包括结尾空字符
Output             :  error     --错误信息
Return             :  0         -- 成功
其他      -- 失败
*/
int mmogxyz_conf_createconfig(struct config_plunin *p, int id, char* dir, char* error, int maxlen)
{
	int iRet = 0;
	
	switch(MMOGXYZ_GET_FUNC_ID(id))
	{
	case MMOGXYZ_FUNC_ID_ROLE_DB:
		break;
	case MMOGXYZ_FUNC_ID_GAMESVR:
		iRet = mmogxyz_generate_gamesvr_conf(dir, id);
		break;
	case MMOGXYZ_FUNC_ID_TCONND:
		iRet = mmogxyz_generate_game_connect_conf(dir, id);
		break;
	default:
		tlog_error(g_pstMmogxyzConfCat,0,0, "unsportted funcid(%d) by process id:%d",
			MMOGXYZ_GET_FUNC_ID(id), id);
		break;
	}
	

	return iRet;
}


/*
Fuction            :  config_plunin_init
Descption          :  初始化插件,so必须实现该函数
Input              :  p             --插件指针
handle        --api handle
Return             :  0             -- 成功
其他          -- 失败
*/
int config_plunin_init(struct config_plunin *p, HAPIHANDLE handle)
{
	printf("enter %s\n", __FUNCTION__);
	p->getname = mmogxyz_conf_getname;
	p->getversion = mmogxyz_conf_getversion;
	p->init = mmogxyz_conf_init;
	p->cleanup = mmogxyz_conf_cleanup;
	p->createconfig = mmogxyz_conf_createconfig;

	mydata.handle = handle;
	mydata.status = 0;

	p->data = (void*)&mydata;

	return 0;
}


