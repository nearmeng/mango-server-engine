#ifndef _SERVICE_DEF_H_
#define _SERVICE_DEF_H_

#include "define/server_def.h"
#include "tbus/tbus_wrapper.h"

#define MAX_SERVER_COUNT_PER_SERVICE	(512)
#define MAX_SERVER_NODE_COUNT			(6000)
#define MAX_SERVICE_COUNT				(25)
#define MAX_TRANS_CMD_COUNT				(100)
#define MAX_SERVICE_MGR_COUNT			(16)

// tolua_begin

enum SERVICE_STATE_TYPE
{
	svcstInvalid,

	svcstInit,			///< ��ǰ�����ʼ��״̬
	svcstInService,		///< ��ǰ������������
	svcstTransfer,		///< ��ǰ������״̬Ǩ����
	svcstEndService,	///< ��ǰ������ֹ

	svcstTotal
};

enum SERVER_STATE_TYPE
{
	svstInvalid,

	svstInit,			///< ��ǰ�ڵ��ʼ���׶�
	svstAvailable,		///< ��ǰ�ڵ����,��δ��ʹ��
	svstInService,		///< ��ǰ�ڵ��ڷ�����
	svstTransfer,		///< ��ǰ�ڵ㴦��״̬ת���� ��״̬����Ϣ��������
	svstDown,			///< ��ǰ�ڵ������
	svstEndService,		///< ��ǰ�ڵ���ֹ����

	svstTotal
};

enum SERVER_OP_PHASE
{
	sopInvalid,

	sopPrepare,			///< ����׼���׶�
	sopExecute,			///< ����ִ�н׶�
	sopFinish,			///< ������ɽ׶�

	sopTotal
};

enum SERVER_EVENT_TYPE
{
	setInvalid,

	setServerInit,				///< ����ڵ��ʼ���¼�
	setServerEndService,		///< ����ڵ���ֹ�����¼�
	setServerDown,				///< ����ڵ�����¼�
	setServerRecover,			///< ����ڵ�ָ��¼�
	setServerExpand,			///< ����ڵ������¼�
	setServerReduce,			///< ����ڵ������¼�
	setServerInfoNtf,			///< ����ڵ���Ϣ֪ͨ�¼� 
	setServerResume,			///< ����ڵ�Resume�¼�

	setTotal
};


struct SERVER_INFO
{
	int32_t		nTbusAddr;		///<  ����tbus��ַ
	int32_t		nState;			///<  �ڵ�״̬
	int32_t		nRouterAddr;    ///<  ��ǰ�ڵ�����·�ɽڵ��ַ
	int32_t		nLoadValue;		///<  ��ǰ�ڵ㸺��ֵ Ŀǰ֧�ֵ�������ֵ 
};

struct SERVICE_INFO
{
	int32_t			nState;												///< ����ǰ״̬
	int32_t			nServerCount;										///< ������ܽڵ���
	int32_t			nServiceServerCount;								///< ����ĵ�ǰ����ڵ���
	uint16_t		ServerInfoIndex[MAX_SERVER_COUNT_PER_SERVICE];		///< ����ڵ���Ϣ����(��ʡ���ڴ�)
};

struct FULL_SERVICE_DATA
{
	SERVICE_INFO	ServiceInfo[MAX_SERVICE_COUNT];
	
	int32_t			nServerCount;
	SERVER_INFO		ServerInfo[MAX_SERVER_NODE_COUNT];

	inline BOOL add_server_node(SERVER_INFO ServerInfo);
	inline BOOL del_server_node(int32_t nServerAddr);
	inline SERVER_INFO* get_server_info_by_addr(int32_t nServerAddr);
	inline SERVER_INFO* get_server_info_by_index(int32_t nServerInfoIndex);
	inline BOOL set_in_service(void);
};

//tolua_end

//note server index is instid of server
inline BOOL FULL_SERVICE_DATA::add_server_node(SERVER_INFO stServerInfo) 
{
	int32_t nRetCode = 0;
	int32_t nServiceType = tbus_get_type(stServerInfo.nTbusAddr);
	int32_t nServerIndex = tbus_get_inst(stServerInfo.nTbusAddr);

	LOG_PROCESS_ERROR(nServiceType > 0 && nServerIndex < MAX_SERVICE_COUNT);
	LOG_PROCESS_ERROR(nServerIndex > 0 && nServerIndex <= MAX_SERVER_COUNT_PER_SERVICE);

	if (ServiceInfo[nServiceType].nServerCount < nServerIndex)
	{
		//expand service info if the server index is too large
		for (int32_t nIndex = ServiceInfo[nServiceType].nServerCount; nIndex < nServerIndex; nIndex++)
		{
			ServiceInfo[nServiceType].ServerInfoIndex[nIndex] = -1;
		}
		ServiceInfo[nServiceType].nServerCount = nServerIndex;
	}

	// set server info
	ServerInfo[nServerCount] = stServerInfo;

	// update service info
	ServiceInfo[nServiceType].ServerInfoIndex[nServerIndex - 1] = nServerCount;
	nServerCount++;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL FULL_SERVICE_DATA::del_server_node(int32_t nServerAddr)
{
	int32_t nRetCode = 0;
	int32_t nServerInfoIndex = 0;
	int32_t nServiceType = tbus_get_type(nServerAddr);
	int32_t nServerIndex = tbus_get_inst(nServerAddr);


	LOG_PROCESS_ERROR(nServiceType > 0 && nServerIndex < MAX_SERVICE_COUNT);
	LOG_PROCESS_ERROR(nServerIndex > 0 && nServerIndex <= MAX_SERVER_COUNT_PER_SERVICE);
	
	//ensure that the deleted server node is the last of the service
	LOG_PROCESS_ERROR(nServerIndex == ServiceInfo[nServiceType].nServerCount);

	nServerInfoIndex = ServiceInfo[nServiceType].ServerInfoIndex[nServerIndex - 1];
	LOG_PROCESS_ERROR(nServerInfoIndex >= 0);

	// update other node index info
	for (int32_t nIndex = nServerInfoIndex + 1; nIndex < nServerCount; nIndex++)
	{
		int32_t nCurrServiceType = tbus_get_type(ServerInfo[nIndex].nTbusAddr);
		int32_t nCurrServerIndex = tbus_get_inst(ServerInfo[nIndex].nTbusAddr);

		ServerInfo[nIndex - 1] = ServerInfo[nIndex];
		ServiceInfo[nCurrServiceType].ServerInfoIndex[nCurrServerIndex - 1]--;
	}
	nServerCount--;

	// update service info
	ServiceInfo[nServiceType].ServerInfoIndex[nServerIndex - 1] = -1;
	ServiceInfo[nServiceType].nServerCount--;

	return TRUE;
Exit0:
	return FALSE;
}
	
inline SERVER_INFO* FULL_SERVICE_DATA::get_server_info_by_addr(int32_t nServerAddr)
{
	int32_t nServerInfoIndex = 0;
	int32_t nServiceType = tbus_get_type(nServerAddr);
	int32_t nServerInst = tbus_get_inst(nServerAddr);

	if (nServiceType <= 0 || nServiceType >= MAX_SERVICE_COUNT)
		CRI("invalid service type %d", nServiceType);

	LOG_PROCESS_ERROR(nServiceType > 0 && nServiceType < MAX_SERVICE_COUNT);
	PROCESS_ERROR(nServerInst <= ServiceInfo[nServiceType].nServerCount);

	nServerInfoIndex = ServiceInfo[nServiceType].ServerInfoIndex[nServerInst - 1];
	return get_server_info_by_index(nServerInfoIndex);

Exit0:
	return NULL;
}

inline SERVER_INFO* FULL_SERVICE_DATA::get_server_info_by_index(int32_t nServerInfoIndex)
{
	PROCESS_ERROR(nServerInfoIndex >= 0 && nServerInfoIndex < MAX_SERVER_COUNT_PER_SERVICE);

	return &(ServerInfo[nServerInfoIndex]);
Exit0:
	return NULL;
}
	
inline BOOL FULL_SERVICE_DATA::set_in_service(void)
{
	for (int32_t i = 0; i < svrTotal; i++)
	{
		if (ServiceInfo[i].nServerCount > 0)
			ServiceInfo[i].nState = svcstInService;
		
		//set init service server count
		ServiceInfo[i].nServiceServerCount = ServiceInfo[i].nServerCount;
	}

	for (int32_t i = 0; i < nServerCount; i++)
	{
		if (ServerInfo[i].nState == svstAvailable)
			ServerInfo[i].nState = svstInService;
	}

	return TRUE;
Exit0:
	return FALSE;
}

#endif