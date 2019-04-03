/*
**  @file $RCSfile: tagentapi.h,v $
**  general description of this module
**  $Id: tagentapi.h,v 1.16 2009-06-29 06:17:05 sean Exp $
**  @author $Author: sean $
**  @date $Date: 2009-06-29 06:17:05 $
**  @version $Revision: 1.16 $
**  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
**  @note Platform: Linux
*/

#ifndef __TAGENT_API__
#define __TAGENT_API__


#if defined(_WIN32) || defined(_WIN64)
# ifdef _DEBUG
#  define TSF4G_LIB_D    "_d"
# else
#  define TSF4G_LIB_D
# endif /* _DEBUG */



#if defined(_WIN32) || defined(_WIN64)
#ifdef TSF4G_SMALL_LIB
#undef _TSF4G_LARGE_LIB_
#endif

#ifdef _TSF4G_LARGE_LIB_
#ifdef _DEBUG
	#pragma comment(lib, "libtsf4g_d.lib")
#else
	#pragma comment(lib, "libtsf4g.lib")
#endif
#else

#if defined(TSF4G_LIB_D)
#pragma comment(lib, "libtaa"TSF4G_LIB_D".lib" )
#else
#pragma comment(lib, "libtaa.lib")
#endif


#endif
#endif


#endif/*#if defined(_WIN32) || defined(_WIN64)*/

#ifdef __cplusplus
extern "C"
{
#endif

#define EXCHANGESIZE			1024*1024
#define EXCBLOCKSIZE			4096

#define EXCHANGEKEY			    0xFFEE0001
#define EXCHEADSIZE 			64

#define TAGENT_DEFAULT_BUSINESS_ID	0	  //default business id

#define ID_APPID_CENTERD		(0x00)    //Ĭ��centerd 	appid
#define ID_APPID_DIRTY			(0x01)    //Ĭ��dirtyd  	appid
#define ID_APPID_BUSCONFIG		(0x02)    //Ĭ��busconfig appid
#define ID_APPID_TCONND		    (0x03)    //Ĭ��tconnd    appid
#define ID_APPID_PROCMNG		(0x04)    //process manage  appid
#define ID_APPID_COLLECT		(0x05)    //TDR-URI and TBUS �����ռ�

#define ID_APPID_TMAC			(0xff)	  //Ĭ��tmac		appid

#define ID_BUSSID_BROADCAST -1

typedef struct tagExchnageHead
{
	int		iExchangeSize;
	int		iBlockSize;
	int		iCreateTime;
	int		iMagic;
	int		iUsed;
	int		iVersion;

	char	szDesc[EXCHEADSIZE-24];
} EXCHANGEHEAD, *LPEXCHANGEHEAD;

typedef struct tagExchangeBlock
{
	unsigned int	uiAppid;
	unsigned int	uiBusid;
} EXCHANGEBLOCK, *	LPEXCHANGEBLOCK;


typedef struct tagExchangeMng
{
	char*	pShm;
	void*	aSem;

	EXCHANGEHEAD*	pstExcHead;
} EXCHANGEMNG, *LPEXCHANGEMNG;

/*********************************************************************
  Fuction         	:  agent_api_init
  Descption       	:  ��ʼ��agent APIs
  Input           	:  ppstExcMng		-- ���ָ��ָ��
  Output          	:  ppstExcMng		-- ���ؾ��ʵ��
  Return          	:  0            	-- ע��ɹ�
                      -1           		-- �ڴ������
  Note            	:  ppstExcMng ��ʼ��ΪNULL��agent_api_init�����
  					   �ڴ�
*********************************************************************/
int	agent_api_init(LPEXCHANGEMNG *ppstExcMng);

/*********************************************************************
  Fuction         	:  agent_api_register
  Descption       	:  ע�ᵽbusAgent
  Input           	:  pstExcMng		-- ���ָ��
                       uiAppid			-- Ӧ��ID
			   		   uiBusid			-- ҵ��ID
  Output          	:  ppstExcMng		-- ���ؾ��ʵ��
  Return          	:  0            	-- ע��ɹ�
                      -1           		-- ע��ʧ��
  Note            	:  ppstExcMng ��ʼ��ΪNULL
*********************************************************************/
int	agent_api_register(LPEXCHANGEMNG pstExcMng, unsigned int uiAppid, unsigned int uiBusid);
/*********************************************************************
  Fuction         	:  agent_api_unregister
  Descption       	:  ȡ��ע��
  Input           	:  pstExcMng		-- ���ָ��
                       uiAppid			-- Ӧ��ID
			   		   uiBusid			-- ҵ��ID
  Output          	:  ppstExcMng		-- ���ؾ��ʵ��
  Return          	:  0            	-- ȡ��ע��ɹ�
                      -1           		-- ȡ��ע��ʧ��
  Note            	:  ppstExcMng ��ʼ��ΪNULL
*********************************************************************/
int	agent_api_unregister(LPEXCHANGEMNG pstExcMng, unsigned int uiAppid, unsigned int uiBusid);

/*********************************************************************
  Fuction         	:  agent_api_get_bussid
  Descption       	:  ����appid���bussid
  Input           	:  ppstExcMng		-- ���ָ��
                       uiAppid			-- Ӧ��ID
			   		   astBussID		-- ҵ��ID����
			   		   piSize			-- ҵ��ID����ָ��
  Output          	:  astBussID		-- ҵ��ID
			   		   piBussSize		-- ҵ�� ID����
  Return          	:  0            	-- ��ȡ�ɹ�
                       -1       		-- ��ȡʧ�ܣ�����ԭ����pstExcMng
                       					   û�г�ʼ��
  Note            	:
*********************************************************************/
int	agent_api_get_bussid(LPEXCHANGEMNG pstExcMng, unsigned int uiAppid, unsigned int astuiBusid[], int *piLen);

/*********************************************************************
  Fuction         	:  agent_api_get_blocks
  Descption       	:  ��ȡ�Ѿ�ע���blocks��Ϣ
  Input           	:  ppstExcMng		-- ���ָ��
                       ppstBlock		-- block����ָ���ָ��
			   		   piBlockSize		-- block����
  Output          	:  ppstBlock		-- block����ָ���ָ��
			   		   piBlockSize		-- block����
  Return          	:  0            	-- ��ȡ�ɹ�
                       -1       		-- ��ȡʧ�ܣ�����ԭ����pstExcMng
                       					   û�г�ʼ��
  Note            	:  ppstBlock ��Ҫfree
*********************************************************************/
int	agent_api_get_blocks(LPEXCHANGEMNG pstExcMng, LPEXCHANGEBLOCK pstBlock[], int *piLen);

/*********************************************************************
  Fuction         	:  agent_api_get_freebufptr
  Descption       	:  ����appid��bussid����freebufָ��
  Input           	:  ppstExcMng		-- ���ָ��
                       uiAppid			-- Ӧ��ID
			   		   uBussSize		-- ҵ��ID
  Output          	:
  Return          	:  0            	-- ��ȡ�ɹ�
                       -1       		-- ��ȡʧ��
  Note            	:
*********************************************************************/
char * agent_api_get_freebufptr(LPEXCHANGEMNG pstExcMng, unsigned int uiAppid, unsigned int uiBusid);


/*********************************************************************
	Fuction         	:  agent_api_destroy
	Descption       	:  ע�����
	Input           	:  pstExcMng		-- ���
	Output          	:
	Return          	:
	Note            	:
*********************************************************************/

void agent_api_destroy(LPEXCHANGEMNG pstExcMng);


#ifdef __cplusplus
}
#endif

#endif /* __TAGENT_API__*/
