/*
 **  @file $RCSfile: tconnapi.h,v $
 **  general description of this module
 **  $Id: tconnapi.h,v 1.7 2009-09-23 07:41:26 hardway Exp $
 **  @author $Author: hardway $
 **  @date $Date: 2009-09-23 07:41:26 $
 **  @version $Revision: 1.7 $
 **  @note Editor: Vim 6.1, Gcc 4.0.1, tab=4
 **  @note Platform: Linux
 */

#ifndef TCONNAPI_H
#define TCONNAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "tdr/tdr_types.h"
#include "tframehead.h"

#ifndef IN
#define  IN
#endif

#ifndef OUT
#define  OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/** @defgroup tconnapi
 * @{
 */

enum tagTconnAPIOptionName
{
    TCONNAPI_OPT_NAME_TBUS_EXCLUSIVE_CHANNELS = 1,   /*ֻ����connectͨ���İ�*/
    TCONNAPI_OPT_COUNT
};

typedef  intptr_t	TCONNDHANDLE;
typedef  intptr_t	*LPTCONNDHANDLE;

extern LPTDRMETA g_pstConnapiFrameHead;

#define TCONNAPI_FRAMEHEAD_HTON(net, host, ver)		tdr_hton(g_pstConnapiFrameHead, net,host, ver)
#define TCONNAPI_FRAMEHEAD_NTOH(host,net, ver)		tdr_ntoh(g_pstConnapiFrameHead, host, net, ver)
#define TCONNAPI_MAX_BUFF		0x10000

#define DEL_GUID_MASK        0x9fffffff  /* ���һ��������idx�е�GUID���� */


    /**  ���һ�����У�����ConnIdx������ĺ궨��
     *
     *  @note ��ǰ�˽�����ڶ��tconnd������£����ڿ����ת��Ԥռ���ӵ�Ӧ�����
     *        ������Ϸ�ͻ���������ConnIdx��Ҫ��GET_UNMASK_IDX()����ȡ����������ĺ��ֵ��
     *        ���磺 iIdx = GET_UNMASK_IDX(iConnIdx)
     */
#define GET_UNMASK_IDX(ConnIdx) ((ConnIdx) & DEL_GUID_MASK)




    /**
     *@brief ʹ��tconnapi֮ǰ�ȱ�����øú��������ʹ�ö�̬tbusģʽ�������tconnd_init_with_automatic_tbus
     *@brief ��ʼ��API��Ϣ
     *@param pszBuff[IN]  tbus share memory key,if set to 0,use 1688 by default
     *@retval   =0:success
     *@retval   <0:fail
     *@retval  -1 ��ʼ��bus ʧ��,
     *@retval   -2 ��ȡTFRAMEHEAD Metaʧ��
     */
    int tconnapi_init(IN int iKey);

    /**
     *@brief ʹ��tconnapi֮ǰ�ȱ�����øú�������̬tbusģʽר�ã��������ͨģʽ����ʹ��tconnapi_init
     *@brief ��ʼ��tbus�����Ϣ
     *@param a_pszTbusdUrl[IN]  tbusd�ĵ�ַ
     *ֻ֧��udp, ��ʽΪ"udp://127.0.0.1:xxxx"
     *-NULL��մ�("") ʹ��ȱʡֵ TBUS_DEFAULT_TBUSD_URL����tbusd
     *@param[in]    a_iFlag ��ʼ��������־λ:
     *-TBUS_INIT_FLAG_USE_SAVED_TIME
     *@param[in] a_iTimeout ��tbusd����ͨ��ʱ�ĳ�ʱʱ��,��λ����     
     *@retval   =0:success
     *@retval   <0:fail
     */
    int tconnapi_init_with_automatic_tbus(const char* a_pszTbusdUrl, int a_iFlag, int a_iTimeout);

    /**
     *@brief TFRAMEHEAD ���
     *@param pszBuff[IN]  		���Ŀ�Ļ�����
     *@param iBuff[IN]  		Ŀ�Ļ���������
     *@param pstHead[INOUT]	TFRAMEHEAD�ṹ��ַ
     *@param piHeadLen[OUT]	��������糤��
     *@retval   =0:success
     *@retval   <0:���ʧ��
     *@retval   =-1 ��������Ƿ�
     *@retval   =-2 tdr���ʧ��
     *@see
     */
    int tconnapi_decode(IN const char* pszBuff,IN  int iBuff,OUT TFRAMEHEAD* pstHead,OUT int* piHeadLen);
    int tconnapi_wap_decode(IN const char *pszBuff, IN int iBuff, OUT TWAPUPLOAD2 *pPkgBuff, INOUT int *piPkgLen);

    /**
     *@brief FrameHead ���
     *@param pszBuff[IN]  		���Ŀ�Ļ�����
     *@param piBuff[INOUT]  	IN:Ŀ�Ļ���������
     *                                         OUT:������糤��
     *@param pstHead[IN]		FrameHead��ַ
     *@retval   =0:success
     *@retval   <0:���ʧ��
     *@retval  =-1 ��������Ƿ�
     *@retval   =-2 tdr ���ʧ��
     *@see
     */
    int tconnapi_encode(IN char* pszBuff,INOUT  int* piBuff, IN TFRAMEHEAD* pstHead);
    int tconnapi_wap_encode(OUT char *pszBuff, INOUT int *piBuff, IN TWAPDOWNLOAD2 *pPkgBuff, IN int iPkgLen);

    /**
     *@brief  �ͷ�API��Ϣ,�����������ڽ���������ø�API�ͷ�
     */
    void tconnapi_fini(void);



    /**
     *@brief ����API����շ���Ϣ
     *@param iProcID[IN]  bus ID
     *@param ppstHandle[OUT]  ���ָ��
     *@retval     =0:success
     *@retval    <0:fail
     *@retval      = -1  �������ڴ�ʧ��
     *@retval      = -2  ����BUS ���ʧ��
     *@retval      = -3  ��BUS ͨ��ʧ��
     */
    int tconnapi_create(IN int iProcID, OUT TCONNDHANDLE *pstHandle);



    /**
     *@brief ���ݶԷ�BUS��ַ��ռ�ض���ͨ��(�ڵ���tconnapi_connect֮ǰ�����ȵ���tconnapi_set_handle_opt)
     *@param pstHandle[IN]  ���ָ��
     *@param iDst[IN]       �Զ˵�ַ
     *@retval    =0 success
     *@retval    <0 fail
     *@retval    =-1 �ײ�tbus_connect����ʧ��
     *@retval    =-2 ���øýӿڵĴ��������������tconnd�����ޣ�Ŀǰֻ֧��ǰ����ಿ��4��tconnd
     *@retval    =-3 ��������Ƿ�

     *@note      ������Ҫǰ�˽�����tconnd������������ε��ñ��ӿڣ����ҷֱ���tconnd��bus��ַ
     */
    int tconnapi_connect(IN TCONNDHANDLE iHandle,IN int iDst);

    /**
     *@brief  
     *@brief  ��bus���հ�
     *@param pstHandle[IN]  	���ָ��
     *@param piSrc[INOUT]  	busԴ��ַ
     *@param pszBuff[OUT]  	Ӧ����Ϣ�建������ʼ��ַ
     *@param piBuff[INOUT]       IN  :����������
     *						OUT:��Ϣ�峤��
     *@param pstHead[OUT]  	�����FrameHead
     *@retval     =0  �յ���
     *@retval    <0 û���յ���,See errno
     *@retval    -1 Tbusͨ��û�а�
     *@retval    -2 ��������Ƿ�
     *@retval    -3 TFrameHead �����
     *@retval    -4 Ӧ�û���������
     
     *
     *@note����ǰ�˽�����ڶ��tconnd������£����ڿ����ת��Ԥռ���ӵ�Ӧ�����
     *       ������Ϸ�ͻ���������ConnIdx��Ҫ��GET_UNMASK_IDX()����ȡ����������ĺ��ֵ
     *       ���磺 iIdx = GET_UNMASK_IDX(iConnIdx)
     */
    int tconnapi_recv(IN TCONNDHANDLE iHandle, INOUT int *piSrc, OUT char* pszBuff,INOUT int* piBuff,OUT TFRAMEHEAD* pstHead);


    /**
     *@brief ��bus�з���
     *@brief ��ʹ�ö��һ����ʱ��iDst��������Ч��tconnd�����framehead�е�iConnIdx����Զ˵�ַ
     *@brief ���ϣ��ʹ��iDst�������������޹ص���Ϣ�����ض�tconnd���뽫iConnIdx��Ϊ-1
     *@param iHandle[IN]  		���ָ��
     *@param iDst[IN]  			busĿ�ĵ�ַ
     *@param pszBuff[IN]  		Ӧ����Ϣ�建������ʼ��ַ
     *@param iBuff[IN  ]  	       ����������
     *@param pstHead[IN]  		�����FrameHead,��Ϊ�����;���ϴ��յ��İ�
     *@retval    =0 send package success
     *@retval    <0 ����ʧ��
     *@retval     = -1 TFRAMEHEAD�������
     *@retval     = -2 Tbus ����ʧ��
     *@retval     = -3 �㲥��Ϣ����(С��0��)�Ƿ��� iConnIdx ֵ
     *@retval     = -4 ��������Ƿ�
	 *@retval     = -10 TFRAMEHEAD�ṹ���зǷ�����
     */
    int tconnapi_send(IN TCONNDHANDLE iHandle, IN  int iDst, IN char* pszBuff,IN  int iBuff,IN TFRAMEHEAD* pstHead);



    /**
     *@brief �ͷ�API���
     *@param ppstHandle[OUT]  ���ָ��
     *@retval
     */
    void tconnapi_free(IN TCONNDHANDLE * piHandle);



    /**
     *@brief ����bus������ԣ���Ҫע����Ǹ�apiӦ����tconnapi_create֮��tconnapi_connect֮ǰ����
     *@brief ʾ��  int flag = 1; tconnapi_set_handle_opt(iHandle, TCONNAPI_OPT_NAME_TBUS_EXCLUSIVE_CHANNELS ,&flag,sizeof(flag))       
     *@param iHandle  				���ָ��
     *@param iOptionName   		������
     *@param pvOptionValue               ����ֵ,�ο�tagTconnAPIOptionName����
     *@param dwOptionLen                  ���Գ���
     *@retval      =0  success
     *@retval      <0 fail
     *@retval     =-1 ������� iHandle �Ƿ�
     *@retval     =-2 ������� iOptionName �Ƿ�
     */
    int tconnapi_set_handle_opt(IN TCONNDHANDLE iHandle,IN int iOptionName,IN const void *pvOptionValue,IN unsigned int dwOptionLen);




    /**
     *@brief  ���tconnd �ر�������ʾ��Ϣ
     *@param iReason  �ر�ԭ��
     *@retval   �������ӹر���ʾ��Ϣ
     */
    const char* tconnapi_get_closestring(IN int iReason);


    /**
     *@brief  ���tconnd �����汾
     *@retval   ���ذ汾������Ϣ
     */
    const char* tconnapi_get_version(void);


    /**
     *@brief  �ڲ��ӿ�
     */
    int tconnapi_initialize(const char *a_pszGCIMKey, int a_iBusinessid);


    /**
     *@brief  ��ȡtbus���
     *@retval >=0 tbus���
     *@retval =-1 ��������Ƿ�
     */
    int tconnapi_get_tbusid(IN TCONNDHANDLE iHandle);


    /**
     *@brief ��tframehead�ṹ��ת���ɿ��ӻ�buffer
     *@param pszBuff[IN]  		Ŀ�Ļ�����
     *@param piBuff[INOUT]  	IN:Ŀ�Ļ���������
     *                                         OUT:������糤��
     *@param pstHead[IN]		FrameHead��ַ
     *@retval   =0:success
     *@retval  =-1 ��������Ƿ�
     *@retval   =-2 buffer���Ȳ���
     *@retval   =-3 ��������
     *@see
     */
    int tconnapi_visualize_framehead(TFRAMEHEAD* pstHead, char* pszBuff, size_t* piBuff);
	
    /**
     *@brief ���÷��ʹ�ʱ���ѡ��
     *@param iHandle[IN]  ���ָ��
     *@param iFlag[IN]  �Ƿ��ʱ����ı�־λ, 0��ʾ����ʱ�������0��ʾ��ʱ���
     */
    void tconnapi_set_send_with_timestamp_flag(IN TCONNDHANDLE iHandle, IN int iFlag);	

    /**
     *@brief ��ȡ��һ����Ϣ�ӽ��� tbus ͨ������ǰʱ���ʱ����(��λ��ms)
     *@param iHandle[IN]  ���ָ��
     *@param pllMsTime[OUT]     ��һ����Ϣ�ӽ��� tbus ͨ������ǰʱ���ʱ����(��λ��ms)
     *@param pstCurrentTime[IN]  	��ǰϵͳ��ʱ�䣬�����ΪNULL��api �ڲ���ʹ�ô˲�����Ϊ��ǰʱ�䣻
     *                              ���ΪNULL��api �ڲ������ gettimeofday ��ȡ��ǰʱ��
     *
     *@retval   =0:success
     *@retval   =-1 ��������Ƿ�
     *@retval   =-2 ��ǰʱ�����һ����Ϣ���� tbus ͨ����ʱ�����
     *@retval   =-3 �յ�����һ����Ϣ��δ����ʱ���
     *@retval   =-4 ��������
     */
    int tconnapi_get_lastmsg_timestamp(IN TCONNDHANDLE iHandle, OUT long long* pllMsTime, IN struct timeval* pstCurrentTime);
    /** @} */

#ifdef __cplusplus
}
#endif


#endif /* TCONNAPI_H */
