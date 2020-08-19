
/**
*
* @file     tcm_config_api.h 
* @brief   so�������ݽӿ�
* 
* @author vinsonzuo
* @version 1.0
* @date 2009-5-13
*
*
* Copyright (c)  2009, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/


#ifndef __TCM_CONFIG_API__
#define __TCM_CONFIG_API__


#ifdef __cplusplus
extern "C" 
{
#endif

#include "tcm_config_plunin.h"

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

#ifndef IN
#define IN
#endif



#define TCMAPI_MAX_ADDR_STR_LEN 256
#define TCMAPI_MAX_HOST_NAME_LEN 128
#define TCMAPI_MAX_IP_LEN 64
#define TCMAPI_MAX_OUTER_IP_NUM 8
#define TCMAPI_NODE_NAME_LEN 64
#define TCMAPI_GROUP_NAME_LEN 64
#define TCMAPI_BUSS_CUSTOM_DATA_SIZE 1024
#define TCMAPI_MAX_PROC_NAME_LEN 64
#define TCMAPI_MAX_PATH_LEN  256
#define TCMAPI_BUS_MAX_KEY_STR_LEN 256
#define TCMAPI_MAX_FUNC_NAME_LEN 32

/*������Ϣ*/
struct tagAPIProcInst
{
    unsigned int iID;	/*����ҵ��ϵͳprocunitΨһid��ʾ */
    char szAddr[TCMAPI_MAX_ADDR_STR_LEN]; /*���ʮ���Ʊ�ʾ��ַ��*/
    char szHostName[TCMAPI_MAX_HOST_NAME_LEN];	/*����������������*/
    char szLayerName[TCMAPI_NODE_NAME_LEN];	   /*�������ڲ�����*/
    char szProcName[TCMAPI_MAX_PROC_NAME_LEN]; 	/*   ���̳�����,ȱʡȡ��������������Ϊ������ */
    char szWorkPath[TCMAPI_MAX_PATH_LEN];    	/*   �˽��̵Ĺ���Ŀ¼ */
    char szGroupName[TCMAPI_GROUP_NAME_LEN];    	/*   ���������Ĳ������� */
	char szFuncName[TCMAPI_MAX_FUNC_NAME_LEN];	/* �������������� */
};
typedef struct tagAPIProcInst APIPROCINST;
typedef struct tagAPIProcInst *LPAPIPROCINST;

/*������Ϣ*/
struct tagAPIHost
{
    char szHostName[TCMAPI_MAX_HOST_NAME_LEN]; 	/*   ������������ */
    char szInnerIP[TCMAPI_MAX_IP_LEN];   /*������������ip*/
	int iOuterIpNum;
    char szOuterIP[TCMAPI_MAX_OUTER_IP_NUM][TCMAPI_MAX_IP_LEN];	/*������������ip*/
    char szGCIMShmKey[TCMAPI_BUS_MAX_KEY_STR_LEN];
    char szCustomAttr[TCMAPI_BUSS_CUSTOM_DATA_SIZE];
};
typedef struct tagAPIHost APIHOST;
typedef struct tagAPIHost *LPAPIHOST;


/*���ݽ���id��ý�����Ϣ�ṹ��proc��Ҫ������Чָ��*/
int tcmapi_get_proc_inst(IN HAPIHANDLE handle, INOUT LPAPIPROCINST proc_inst, IN int proc_id);
/*���ݽ���id��ȡ������Ϣ�ṹʱ���ȡ��FuncName�ֶ�, add by edenzhong at 20190828*/
int tcmapi_get_proc_inst2(IN HAPIHANDLE handle, INOUT LPAPIPROCINST proc_inst, IN int proc_id);

/*���ݽ���id������ڲ������Զ�����Ϣ*/
const char* tcmapi_get_group_customattr(IN HAPIHANDLE handle, IN int proc_id);

/*���ݽ���id������ڲ������Զ�����Ϣ*/
const char* tcmapi_get_layer_customattr(IN HAPIHANDLE handle, IN int proc_id);

/*�����������ƻ��������Ϣ�ṹ��host��Ҫ������Чָ��*/
int tcmapi_get_host_by_name(IN HAPIHANDLE handle, INOUT APIHOST* host, IN const char* host_name);


//��ȡ��¼��־�� ��־����ָ��
void *tcmapi_get_logcatinst(IN HAPIHANDLE handle);

/**��ȡ��ָ������  ͨ��tbus����ͨ�ŵ����������б�
@param[in] a_handle ������������
@param[in] a_iProcID ָ������ID
@param[in] a_iProcs ������ؽ���id������
@param[in,out] a_piCount ���ֽ�����Ŀ��ָ��
	-	in ָ��a_iProcs����ĳ���
	-	out	��ȡʵ�ʵĽ�����Ŀ
@return zero: succ;  nonzero: failed
*/
int tcmapi_get_procs_with_buscommunication(IN HAPIHANDLE a_handle, IN int a_iProcID, IN int a_iProcs[],
									   INOUT int *a_piCount);


/*�������ϵͳ����������*/
int tcmapi_get_host_num(IN HAPIHANDLE handle);

/*��������idx���������Ϣ��host��Ҫ������Чָ��
@param[in] handle ������������
@param[in] idx ����idx��0 <= idx <= tcmapi_get_host_num(...)
@param[in,out] host��������Ϣ
@return zero: succ;  nonzero: failed
*/
int tcmapi_get_host_by_idx(IN HAPIHANDLE handle, INOUT APIHOST* host, IN int idx);


/*�������ϵͳ�Ľ�������*/
int tcmapi_get_proc_num(IN HAPIHANDLE handle);

/*���ݽ���idx��ý�����Ϣ�ṹ��proc��Ҫ������Чָ��
@param[in] handle ������������
@param[in] idx��0 <= idx <= tcmapi_get_proc_num(...)
@param[in,out] proc_inst��������Ϣ
@return zero: succ;  nonzero: failed
*/
int tcmapi_get_proc_inst_by_idx(IN HAPIHANDLE handle, INOUT LPAPIPROCINST proc_inst, IN int idx);
/*���ݽ���idx��ȡ������Ϣ�ṹʱ���ȡ��FuncName�ֶ�, add by edenzhong at 20190828*/
int tcmapi_get_proc_inst_by_idx2(IN HAPIHANDLE handle, INOUT LPAPIPROCINST proc_inst, IN int idx);


/*���ƥ�����a_pszProcStr(����: *.*.5.*) �Ľ��̵����� */

int tcmapi_get_procset_num_by_str(IN HAPIHANDLE handle,IN  char *a_pszProcStr) ;

/*���ƥ�����a_pszProcStr(����: *.*.5.*)����id����,Ҫ����Proc_id����������СΪ����
@param[in] handle ������������
@param[in] a_pszProcStr��(����: *.*.*.*, ������AddrTemplate����һ��)
@param[in] a_iProcs ������ؽ���id������
@param[in,out] a_piCount ���ֽ�����Ŀ��ָ��
	-	in ָ��a_iProcs����ĳ���
	-	out	��ȡʵ�ʵĽ�����Ŀ

@return zero: succ;  nonzero: failed
*/

int tcmapi_get_procset_by_str(IN HAPIHANDLE handle, IN  char *a_pszProcStr, 
		IN int a_iProcs[], INOUT int *a_piCount);

/*���ݽ���id��ý�������·��*/
const char* tcmapi_get_proc_configpath(IN HAPIHANDLE handle, IN int proc_id);

#ifdef __cplusplus
}
#endif

#endif /* __TCM_CONFIG_API__*/


