/// @file:   tgcpapi.h
/// @brief:  TGCPЭ��API����Ҫ��Ϊ����API����ʹ��
/// @author: laynehu
/// @date:   2013-10-15

#ifndef TGCP_API_H_
#define TGCP_API_H_

#include "tgcpapi_error.h"

#define TGCP_MAX_ID_LENGTH 256
#define TGCP_DATA_LEN 16

#if (defined(_WIN32) || defined(_WIN64)) && _MSC_VER < 1300
typedef unsigned __int64 tgcp_ulonglong;
#else
typedef unsigned long long tgcp_ulonglong;
#endif


/*
 * ��Ȩ��ʽ
 */
typedef enum tagAuthType {
    TGCP_AUTH_NONE = 0,         //��ʹ�ü�Ȩ
    TGCP_AUTH_WX_O2_CODE,       //΢�� OAuth 2.0 CODE
    TGCP_AUTH_WX_O2_RTK,        //΢�� OAuth 2.0 refresh token
    TGCP_AUTH_QQ_O2_ATK,        //��Q  OAuth 2.0 acess token
    TGCP_AUTH_QQ_CT_LOGIN,      //QQ CTLogin��Ȩ��ʽ
    TGCP_AUTH_ITOP = 0x1000,     //itop��Ȩ��ʽ
    TGCP_AUTH_PC = 0x1004,     //pc��Ȩ��ʽ
    TGCP_AUTH_WEGAME = 0x1005,     //wegame auth type

    TGCP_AUTH_VER_2 = 0x7fff,      //�µ��ʺż�Ȩ��ʽ��2014.03.07
} eAuthType;

/*
 * ���ܷ���
 */
typedef enum tagEncryptMethod {
    TGCP_ENCRYPT_METHOD_NONE = 0,   //������
    TGCP_ENCRYPT_METHOD_TEA  = 1,   //�ݲ�֧��
    TGCP_ENCRYPT_METHOD_QQ   = 2,   //�ݲ�֧��
    TGCP_ENCRYPT_METHOD_AES  = 3,   //AES����
    TGCP_ENCRYPT_METHOD_AES2 = 4,   //AES2����
} eEncryptMethod;

/*
 * ��Կ���ɷ���
 */
typedef enum tagKeyMaking {
    TGCP_KEY_MAKING_NONE = 0,       //��ʹ����Կ����ʱ��Ϊͨ�����ݲ����ܣ�
    TGCP_KEY_MAKING_INAUTH,         //��Կ�����ڼ�Ȩ��Ϣ
    TGCP_KEY_MAKING_INSVR,          //��Կ��TCONND�������·�
    TGCP_KEY_MAKING_RAW_DH,         //ԭʼDH�㷨��Կ
    TGCP_KEY_MAKING_ENC_DH,         //���ܹ���DH�㷨��Կ
} eKeyMaking;

/*
 * ƽ̨��Ϣ
 */
typedef enum tagPlatformID {
    TGCP_PC = 0,
    TGCP_ANDROID = 101,
    TGCP_IOS = 102,
} ePlatformID;

/*
 * ѹ����Ϣ
 */
typedef enum
{
    TGCP_COMPR_NONE = 0,
    TGCP_COMPR_LZ4,
    TGCP_COMPR_ZLIB,
}eCompressMethod;

/*
 * �Ŷ���Ϣ
 * */
typedef struct {
    unsigned int iPos;          // �Ŷ�λ��
    unsigned int iQueueLen;     // ���г���
    unsigned int iEstimateTime; // Ԥ���Ŷ�ʱ��,����Ϊ��λ
} QUEUENOTIFY, *LPQUEUENOTIFY;


/*
 * �˺�����
 */
typedef enum tagAccountType {
    TGCP_ACCOUNT_TYPE_NONE = 0,      //�Զ����ʽ
    TGCP_ACCOUNT_TYPE_QQ_UIN,        //QQ��
    TGCP_ACCOUNT_TYPE_QQ_OPENID,     //��QOpenID
    TGCP_ACCOUNT_TYPE_WX_OPENID,     //΢��OpenID
	TGCP_ACCOUNT_TYPE_IOS_GUEST = 0x2001,     //�ο�ģʽ
    TGCP_ACCOUNT_TYPE_FACEBOOK = 0x8001,
    TGCP_ACCOUNT_TYPE_KAKAO = 0x8010,
    TGCP_ACCOUNT_TYPE_GARENA = 0x8011,
} eAccountType;

typedef enum tagAccountFormat {
    TGCP_ACCOUNT_FORMAT_QQ_32 = 0,
    TGCP_ACCOUNT_FORMAT_QQ_64,
    TGCP_ACCOUNT_FORMAT_STRING,
} eAccountFormat;

typedef union tagTGCPAccountValue {
    tgcp_ulonglong uin;         //Uin
    char szID[TGCP_MAX_ID_LENGTH];  //String���͵�ID
} TGCPACCOUNTVALUE;

struct tagTGCPAccount {
    unsigned short uType;            //�˺�����,itop��Ȩģʽ��,��Ҫʹ��eAccountType,����ֱ��ʹ��������
    eAccountFormat bFormat;          //�˺Ÿ�ʽ
    TGCPACCOUNTVALUE stAccountValue; //�˺���Ϣ
    tgcp_ulonglong llUid;            //�ʺ�ӳ���uid
    tagTGCPAccount()
        :uType(0)
        ,bFormat(TGCP_ACCOUNT_FORMAT_QQ_32)
        ,llUid(0)
    {
    }
};
typedef struct tagTGCPAccount TGCPACCOUNT;
typedef struct tagTGCPAccount *LPTGCPACCOUNT;

struct tagTGCPApiHandle;
typedef struct tagTGCPApiHandle *HTGCPAPI;

/*
 * ��Ⱥ����½����
 */
typedef enum
{
    TGCP_ROUTE_NONE = 0,          //�Ǽ�Ⱥ����½
    TGCP_ROUTE_SPECIFY_ZONE,      //��Ⱥ��ָ��������½
    TGCP_ROUTE_SPECIFY_SERVER,    //��Ⱥ��ָ��ServerID��½
    TGCP_ROUTE_LOGIN_POSITION,    //��Ⱥ��½�㷢����Ϣ
}eTGCPRouteType;

typedef enum
{
    TGCP_NOT_ALLOW_LOST = 0,   //������ʧ��Ϣ����ʧ��ر�����
    TGCP_ALLOW_LOST,           //����ʧ��Ϣ����ʧ�󲻹ر�����
}eTGCPAllowLost;


/*
 * ��Ⱥ��ָ��������½��Ϣ
 */
typedef struct
{
    unsigned int iTypeID;
    unsigned int iZoneID;
}TGCPSPECIFYZONE, *LPTGCPSPECIFYZONE;

/*
 * ��Ⱥ��ָ��ServerID��½��Ϣ
 */
typedef struct
{
    tgcp_ulonglong ullServerID;
}TGCPSPECIFYSERVER, *LPTGCPSPECIFYSERVER;

/*
 * ��Ⱥ����½��Ϣ
 */
typedef union
{
    TGCPSPECIFYZONE stZone;
    TGCPSPECIFYSERVER stServer;
}TGCPROUTE, *LPTGCPROUTE;

/*
 * �����ת��������(��Ⱥ�����������ṹ����һ������ֱ�Ӹ���)
 */
typedef struct
{
    unsigned int dwJumpPosition;
    char szConnData[TGCP_DATA_LEN];
}TGCPJUMP, *LPTGCPJUMP;

// windows �����³�ʼ��socket����
#if defined(_WIN32) || defined(_WIN64)
int tgcpapi_WSA_startup(int a_iMaj, int a_iMin);
int tgcpapi_WSA_cleanup();
#endif

/// @defgroup GCP_API_MANAGE TGCP_�������
/// @{

/// @brief  ����TGCP���
///
/// @param[in, out]  a_ppHandle tgcpapi���ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
/// @Warning  ��tgcapi_ex.h����Ľӿڳ�ͻ��
///           �Ͻ��ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
int tgcpapi_create(HTGCPAPI* a_ppHandle);


/// @brief  ����TGCP���
///
/// @param[in, out]  a_ppHandle tgcpapi���ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_destroy(HTGCPAPI* a_ppHandle);

/// @brief  ��ʼ��TGCP�����Դ
///
/// @param[in]  a_pHandle       tgcpapi���
/// @param[in]  a_iServiceID    ҵ��ID��TConnd3.1.7�����ϰ汾�����ֶ������塣��ȡֵ�ᱻTConnd�����ļ���BusinessID���ǡ�
/// @param[in]  a_iPlatformID   ƽ̨��Ϣ
/// @param[in]  a_iAuthType     ��Ȩ����,��ʹ�� tagAuthType ö��ֵ
/// @param[in]  a_iEncMethod    ���ݼ��ܷ�ʽ,��ʹ�� tagEncryptMethod ö��ֵ
/// @param[in]  a_iKeyMode      ��ԿЭ�̷�ʽ,��ʹ�� tagKeyMaking ö��ֵ
/// @param[in]  a_iBuffLen      �û����ݻ���������
///
/// @return =0, success,
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note init�ӿڵ�����Ϻ���Ҫ����һϵ�е�set�ӿڽ������ã��ſ��Է������ӡ�
///       a_iAuthType����ʹ��tagAuthTypeö��ֵ������֧�ָ�ö��ֵ�����ֵ�������û�
///       �Զ����Ȩ��TCONND��֧�ֵĵ�����ƽ̨��Ȩ��ʽ��API��͸�����ֶε�ֵ���û��Զ����Ȩ
///       ����������0~32767��TCONND����ֵ���û��Զ����Ȩ���͵�ֵֻ����ʹ��32768~65535��
/// @note ����ӿ�a_iAuthTypeֻ֧��TGCP_AUTH_NONE��TGCP_AUTH_WX_O2_CODE��TGCP_AUTH_WX_O2_RTK����TGCP_AUTH_QQ_O2_ATK�ȡ�
/// @Warning  ��tgcapi_ex.h����Ľӿڳ�ͻ��
///           �Ͻ��ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
///
/// @see tgcpapi_set_appid()
/// @see tgcpapi_set_token()
/// @see tgcpapi_set_account()
int tgcpapi_init(HTGCPAPI a_pHandle,
                 int a_iServiceID,
                 ePlatformID a_iPlatformID,
                 unsigned short a_iAuthType,
                 eEncryptMethod a_iEncMethod,
                 eKeyMaking a_iKeyMode,
                 int a_iBuffLen);


/// @brief  ����TGCP�����Դ
/// @param[in]  a_pHandle       tgcpapi���
///
/// @return =0, success,
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
int tgcpapi_fini(HTGCPAPI a_pHandle);

/// @}


/// @defgroup GCP_API_PROTOCOL TGCP_ѡ������
/// @{

/// @defgroup Common  ���нӿ�
/// @{
///
/// @brief  ���socket�ļ�������
/// @param[in]  a_pHandle    tgcpapi���
/// @param[out] a_piFd       socket fd���ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_get_socket(HTGCPAPI a_pHandle, int* a_piFd);
///
/// @}


/// @defgroup OAuth  ʹ��OAuth��ʽ��Ȩ��ʽ���нӿ�
/// @{
///
/// @brief  ����AppID
/// @param[in]  a_pHandle   tgcpapi���
/// @param[in]  a_pszOpenID AppIDָ��
/// @param[in]  a_iLen      AppID����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ��������ĵ�AppID
/// @Warning  ��tgcapi_ex.h����Ľӿ��ظ���
///           �������ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
int tgcpapi_set_appid(HTGCPAPI a_pHandle, const char* a_pszAppID, int a_iLen);

/// @brief  ���ü�Ȩ��Ϣ
///
/// @param[in]  a_pHandle   tgcpapi���
/// @param[in]  a_pszToken  Tokenָ��
/// @param[in]  a_iLen      Token����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note a_pszToken�������Դ���OAuth�е�Code��Ҳ���Դ���Token����ʹ��Code���м�Ȩ��������
///       Ӧ��ת��Ϊʹ��Token����ΪToken�������Ȩ��ص���Ϣ
///       ������Q��OAuth��Ȩ��ʽ����Ҫ����AcessToken
///       ����΢�ŵ�OAuth��Ȩ��ʽ����Ҫ����RefreshToken
///       ����CTLogin��Ȩ��ʽ����Ҫ����STƱ��
/// @Warning  ��tgcapi_ex.h����Ľӿڳ�ͻ��
///           �Ͻ��ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
int tgcpapi_set_token(HTGCPAPI a_pHandle, const char* a_pszToken, int a_iLen);

/// @brief  ����Start����չ�ֶ�
///
/// @param[in]  a_pHandle   tgcpapi���
/// @param[in]  a_iExt      ��չ�ֶ�
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
int tgcpapi_set_synreserved(HTGCPAPI a_pHandle, int a_iExt);


/// @brief  ����AcessToken����Ч��
/// @param[in]  a_pHandle   tgcpapi���
/// @param[in]  a_iExpire   ʣ����Чʱ��,����Ϊ��λ
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ô���ԭ��
///
/// @note ʹ��QQ��Ȩģʽʱ�����԰�AcessToken����Ч�ڴ��������Ϊ�ο�
int tgcpapi_set_acesstoken_expire(HTGCPAPI a_pHandle, unsigned short a_iExpire);


/// @brief  ��ȡAcessToken
///
/// @param[in]       a_pHandle   tgcpapi���
/// @param[in, out]  a_pszToken  ����AcessToken��������ָ��
/// @param[in, out]  a_piLen     ����AcessToken���������ȵ�ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ʹ��WXCode��Ȩ�ķ�ʽʱ�����Ե��øýӿڻ��AcessToken
int tgcpapi_get_acesstoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);


/// @brief  ��ȡRefreshToken
///
/// @param[in]       a_pHandle   tgcpapi���
/// @param[in, out]  a_pszToken  Tokenָ��
/// @param[in, out]  a_iLen      Token����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ʹ��WXCode��Ȩ�ķ�ʽʱ�����Ե��øýӿڻ��RefreshToken
int tgcpapi_get_refreshtoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);


/// @brief  �����˺���Ϣ
/// @param[in]  a_pHandle       tgcpapi���
/// @param[in]  a_pstAccount    �˺Žṹ��ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ����API��Uin��OpenID�������˺���Ϣ����ʶ�û���Ψһ��
/// @Warning  ��tgcapi_ex.h����Ľӿ��ظ���
///           �������ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
int tgcpapi_set_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);


/// @brief  ��ȡ�˺���Ϣ
/// @param[in]  a_pHandle       tgcpapi���
/// @param[in]  a_pstAccount    �˺Žṹ��ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @see tgcpapi_update()
int tgcpapi_get_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);


/// @brief  ��ȡOpenID
/// @param[in]       a_pHandle      tgcpapi���
/// @param[in, out]  a_pszOpenID    ����OpenID��������ָ��
/// @param[in, out]  a_piLen        ����OpenID�������ĳ���ָ��
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ʹ��WXCode��Ȩ�ķ�ʽʱ�����Ե��øýӿ�ֱ�ӻ��OpenID
int tgcpapi_get_openid(HTGCPAPI a_pHandle, char* a_pszOpenID, int* a_piLen);


/// @brief  ����DH������Կ�㷨������Ϣ
/// @param[in]      a_pHandle       tgcpapi���
/// @param[in]      a_DH_p_hex      ������p��Hexֵ
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ʹ��DH�㷨������Կʱ����Ҫ���øýӿ�����DH��Կ
/// @Warning  ��tgcapi_ex.h����Ľӿ��ظ���
///           �������ڵ���tgcpapi_create_and_init�ĵط����ô˽ӿ�
int tgcpapi_set_dh_info(HTGCPAPI a_pHandle, const char* a_DH_p_hex);


/// @brief  ���ü�Ⱥ����½ʱ��·����Ϣ
/// @param[in]      a_pHandle        tgcpapi���
/// @param[in]      a_iRouteType     ·������
/// @param[in]      a_pstRouteInfo   ·����Ϣ
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note 1.Ĭ��ΪTGCP_ROUTE_NONE���Ǽ�Ⱥ����½
///       2.�����뼯Ⱥ��TCONND��½ʱ�����Ե��øýӿ�ָ����Ⱥ����¼��Ϣ
///       3.�ýӿ���init(��ʼ��)֮��connection(��������)֮ǰ���ÿ������õǼ�Ⱥʱѡ��GAMESVR�ķ�ʽ��
///         ����ѡ�� TGCP_ROUTE_SPECIFY_ZONE��TGCP_ROUTE_SPECIFY_SERVER ������ֵ
int tgcpapi_set_route_info(HTGCPAPI a_pHandle, eTGCPRouteType a_iRouteType, const LPTGCPROUTE a_pstRouteInfo);

/// @}


/// @defgroup GCP_API_CONNECTION TGCP_ͨ�Ź���
/// @{

/// @brief  �ͻ��˷�����tconnd�����ӡ�����(SYN/ACK)����Ȩ(����)��ͬ���ȴ���Ӧ��
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pszUrl   tconnd url(��ʽʾ����tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout �������ӡ����֡���֤�����������ʱʱ��ֵ(��λ��ms)
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///          ���У�����TGCP_ERR_STAY_IN_QUEUE���������Ŷӣ�ͨ��tgcpapi_query_wait_notify
///          ��ȡ������Ŷ���Ϣ���˽ӿ�ͬʱ����ȷ���Ƿ�����Ŷӣ�����ŶӺ�Ϳ�������ͨ��
///
///          ����TGCP_ERR_PEER_STOPPED_SESSIONʱ������tgcpapi_query_stopped��ѯԭ��
///
/// @note �ڵ���start_connectionǰ��ҵ������ȵ���tgcpapi_init������Ӧ�ĳ�ʼ������
///       ͬʱ��Ҫ����tgcpapi_set_*�Ƚӿ�������ص�OAUTH��Ϣ
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
/// @see  tgcpapi_query_stopped()
int tgcpapi_start_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);


/// @brief  �ͻ��˹ر���tconnd������
///
/// @param[in]  a_pHandle tgcpapi���
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_close_connection(HTGCPAPI a_pHandle);


/// @brief  �ͻ�������ͨ�ŻỰ
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pszUrl   tconnd url(��ʽʾ����tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout ���������������ʱʱ��ֵ(��λ��ms)
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///          ���У�����TGCP_ERR_STAY_IN_QUEUE���������Ŷӣ�ͨ��tgcpapi_query_wait_notify
///          ��ȡ������Ŷ���Ϣ���˽ӿ�ͬʱ����ȷ���Ƿ�����Ŷӣ�����ŶӺ�Ϳ�������ͨ��
///
/// @note �ڴ�֮ǰ��ҵ��֤�Ѿ�����tgcpapi_init������Ӧ�ĳ�ʼ������
///       ͬʱ�Ѿ�����tgcpapi_set_*�Ƚӿ�������ص�OAUTH
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
///
/// @note ��Ҫ˵�������鵱�ҽ���tgcpapi_recv �� tgcpapi_peek����TGCP_ERR_NETWORK_EXCEPTIONʱ����
int tgcpapi_relay_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);


/// @biref �����ת
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pszUrl   tconnd url(��ʽʾ����tcp://127.0.0.1:8090)
/// @param[in]  a_iTimeout ���������������ʱʱ��ֵ(��λ��ms)
/// @param[in]  a_pExtData �����ת�������Ϣ 
/// @param[in]  a_pServerId ָ��Ҫ��ת������id(�Ǽ�ȺģʽΪ��ָ�뼴��)
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///          ���У�����TGCP_ERR_STAY_IN_QUEUE���������Ŷӣ�ͨ��tgcpapi_query_wait_notify
///          ��ȡ������Ŷ���Ϣ���˽ӿ�ͬʱ����ȷ���Ƿ�����Ŷӣ�����ŶӺ�Ϳ�������ͨ��
///
/// @note �ڴ�֮ǰ��ҵ��֤�Ѿ�����tgcpapi_init������Ӧ�ĳ�ʼ������
///       ͬʱ�Ѿ�����tgcpapi_set_*�Ƚӿ�������ص�OAUTH
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_query_wait_notify()
///
int tgcpapi_jump_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout,
        const LPTGCPJUMP a_pExtData, const tgcp_ulonglong* a_pServerId);
///  @}


/// @brief ��tgcpapi_jump_connection��tgcpapi_start_connection���óɹ�����pos��ident�����Ϣ
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///
/// @see  tgcpapi_jump_connection()��tgcpapi_start_connection()
///
/// @note ��TGCP_ERR_NONE != tgcpapi_jump_connection()
///   ��  ��TGCP_ERR_NONE != tgcpapi_start_connection()ʱ����
int tgcpapi_get_iden_info(HTGCPAPI a_pHandle, LPTGCPJUMP a_pIdentInfo);

// �첽ͨ��ģʽ
//
// ���߳�ģʽ�£�����ģʽ��ȫ�첽��ͬʱ������߳�ģʽ����Ҫʹ�������첽ͨ��ģʽ�Ļ���
// һ��Ҫ��֤�����շ�ֻ��һ���߳������
enum {
    TGCP_EVENT_SSTOPED      = 0x01,     // ����������رջỰ
    TGCP_EVENT_WAITING      = 0x02,     // �����Ŷ���
    TGCP_EVENT_DATA_IN      = 0x04,     // ���ջ��������ɶ����յ�����һ����������
    TGCP_EVENT_DATA_OUT     = 0x08,     // ���ͻ�������д
    TGCP_EVENT_ATK          = 0x10,     // ATK ����֪ͨ
    TGCP_EVENT_SVR_IS_FULL  = 0x20,     // ����������֪ͨ
    TGCP_EVENT_ROUTE_CHANGE = 0x40,     // ·����ת֪ͨ
};

typedef struct tagGcpEvent {
    unsigned int iEvtNum;
    unsigned int iEvents;
} GCPEVENT, *LPGCPEVENT;


/// @defgroup GCP_API_CONNECTION_ASYNC TGCP_�첽ģʽͨ�Ź���
/// @{

/// @brief �������˷����ͨ�ŻỰ
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pszUrl   tconnd url(��ʽʾ����tcp://127.0.0.1:8090)
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note �ڵ���tgcpapi_startǰ��ҵ������ȵ���tgcpapi_init������Ӧ�ĳ�ʼ������
///       ͬʱ��Ҫ����tgcpapi_set_*�Ƚӿ�������ص�OAUTH��Ϣ
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_update()
/// @see  tgcpapi_stop()
int tgcpapi_start(HTGCPAPI a_pHandle, const char* a_pszUrl);

/// @brief ˢ�����˷����ͨ�ŻỰ״̬
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pstEvent ���ϲ㷴���¼���SSTOP��WAITING��DATAIN��DATAOUT��ATK��
///                        ���a_pstEvent->iEvtNumֵ����0ʱ��ͨ�����a_pstEvent->iEvents��ȷ���¼�Դ
///                        �����ж� a_pstEvent->iEvents & TGCP_EVENT_SSTOPED�Ƿ�Ϊtrue��ȷ���Ƿ���SSTOP�¼�
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ��Ҫ�ϲ㲻�ϵ���ȥ��������ͨ�ŻỰ
/// @note ������TGCP_ERR_PEER_CLOSED_CONNECTION��TGCP_ERR_NETWORK_EXCEPTIONʱ����Ȼ��Ҫͨ����iEvents & TGCP_EVENT_DATAIN�����ж��Ƿ�ɶ�
///
/// @see  tgcpapi_init()
/// @see  tgcpapi_set_appid()
/// @see  tgcpapi_set_token()
/// @see  tgcpapi_set_account()
/// @see  tgcpapi_start()
/// @see  tgcpapi_resume()
int tgcpapi_update(HTGCPAPI a_pHandle, LPGCPEVENT a_pstEvent);

/// @brief �ָ������������˷����ͨ�ŻỰ��һ��������ͨ�Ź�������������쳣�����
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_pszUrl   tconnd url(��ʽʾ����tcp://127.0.0.1:8090)
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @see  tgcpapi_start()
/// @see  tgcpapi_update()
/// @see  tgcpapi_stop()
int tgcpapi_resume(HTGCPAPI a_pHandle, const char* a_pszUrl);

/// @brief ��ֹ���˷����ͨ�ŻỰ
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @see  tgcpapi_start()
/// @see  tgcpapi_update()
/// @see  tgcpapi_resume()
int tgcpapi_stop(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ���Ŷ�֪ͨ�¼��󣬻�ȡ�Ŷ�״̬
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  !NULL, success
///          NULL, fail
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_WAITING)Ϊtrueʱ����
const QUEUENOTIFY* tgcpapi_get_queue(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ������˹رջỰ�¼��󣬻�ȡ�ر�ԭ��
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_SSTOPED)Ϊtrueʱ����
int tgcpapi_get_sstop_reason(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ��socket�쳣�Ͽ�ʱ��ͨ���˽ӿڻ�ȡsocket������
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note ��tgcpapi_update����TGCP_ERR_NETWORK_EXCEPTIONʱ����
int tgcpapi_get_last_socket_error(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ������˹رջỰ�¼��󣬻�ȡtconnd��Ӧ��ip��port��id
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[out] a_pszTconndIp  ��ȡ�õ���ip�ַ���ͷָ��
/// @param[in, out] a_pszTconndIpSize  ����ʱ������TconndIp�ռ䳤��(һ��Ϊ16),
///  ����ǻ�õ���ʵ���ݳ���
/// @param[out] a_pszTconndId  ��ȡ�õ���id�ַ���ͷָ��
/// @param[in, out] a_pszTconndIdSize  ����ʱ������TconndId�ռ䳤��(һ��Ϊ32)��
///  ����ǻ�õ���ʵ���ݳ���
/// @param[out] a_wTconndPort ��ȡ�õ���Tconnd�˿�
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_SSTOPED)Ϊtrueʱ����
int tgcpapi_get_sstop_ip_port_id(HTGCPAPI a_pHandle, char* a_pszTconndIp,
        int& a_iTconndIpSize, char* a_pszTconndId, int& a_iTconndIdSize, unsigned short& a_wTconndPort);

/// @brief �ڳɹ��������ӻ�update��ȡ��·���л���֪ͨ�󣬻�ȡ�ض�����GAMESVR��ServerID
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_ROUTE_CHANGE)Ϊtrueʱ����
tgcp_ulonglong tgcpapi_get_route_serverid(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ������˹رջỰ�¼��󣬻�ȡ�رյ���ϸԭ��
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_SSTOPED)Ϊtrueʱ����
/// @note ��stopreason=7(gamesvr�����ر�����)ʱ��excode��ʾgamesvr��stopreason
/// @note ��stopreason=8(��Ȩʧ��ʱ),excode��ʾ�����Ȩʧ�ܵ�ԭ������
/// @note     0x10000001����ʾ��Ȩ������񲻿���
/// @note     0x10000002����ʾ��Ȩ����Ϊ0��TCONND��֧��
/// @note     ����ֵ����ʾ��ǰƱ���Ѿ�ʧЧ���ͻ�����Ҫ������ȡƱ��
/// @note ��stopreasonΪ����ֵ�ǣ�excode������
int tgcpapi_get_sstop_excode(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ�������������¼��󣬻�ȡ������
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///          else, ����
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_DATA_IN)Ϊtrueʱ����
int tgcpapi_recved_pkglen(HTGCPAPI a_pHandle);

/// @brief ��update��ȡ���ɷ��Ͱ��¼��󣬻�ȡ���ÿռ�
///
/// @param[in]  a_pHandle  tgcpapi���
///
/// @return  -1, invalid
///          else, ���û������ռ��С
///
/// @see  tgcpapi_update()
///
/// @note ��(events & TGCP_EVENT_DATA_OUT)Ϊtrueʱ����
int tgcpapi_sendable_buflen(HTGCPAPI a_pHandle);

///  @}

/// @defgroup TGCP_API_SEND_RECV TGCP_�����շ�
/// @{

/// @brief  ����ҵ�����ݣ���֤����ҵ�����ݰ�����Ϊֹ
///
/// @param[in]  a_pHandle    tgcpapi���
/// @param[in]  a_pszBuffIn  ҵ�����ݻ�����ָ��
/// @param[in]  a_iSize      ҵ�����ݴ�С
/// @param[in]  a_iTimeout   ����ҵ�����ݰ����ʱֵ(��λ��ms), ��������Ϊ0
///
/// @return  =0, ���ͳɹ�
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ����ֵΪTGCP_ERR_SEND_UNCOMPLETEʱ��˵����������״�����ѣ�ʣ��δ����ֵsocket������
///       �����ھ���Ļ������У�������һ�ε���tgcpapi_sendʱһ��copy��socket�����У�
///       Ҳ����ѡ���ٴε���tgcpapi_flush���Ծ������ꡣһ������£�����ͻ��˴�ʱ��Ҫ�ر����ӡ�
/// @note �˽ӿڼ�ʹ���ش���Ҳ��Ӱ��tgcpapi_recv��tgcpapi_peek�Ľ���
///
/// @see tgcpapi_flush
int tgcpapi_send(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout);

/// @brief  ��ָ��·����Ϣ����ҵ�����ݣ���֤����ҵ�����ݰ�����Ϊֹ
///
/// @param[in]  a_pHandle         tgcpapi���
/// @param[in]  a_pszBuffIn       ҵ�����ݻ�����ָ��
/// @param[in]  a_iSize           ҵ�����ݴ�С
/// @param[in]  a_iTimeout        ����ҵ�����ݰ����ʱֵ(��λ��ms), ��������Ϊ0
/// @param[in]  a_iRouteType      ·������
/// @param[in]  a_pstRouteInfo    ·����Ϣ
/// @param[in]  a_iAllowLost  �Ƿ�����ʧ��Ϣ
///
/// @return  =0, ���ͳɹ�
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ����ֵΪTGCP_ERR_SEND_UNCOMPLETEʱ��˵����������״�����ѣ�ʣ��δ����ֵsocket������
///       �����ھ���Ļ������У�������һ�ε���tgcpapi_sendʱһ��copy��socket�����У�
///       Ҳ����ѡ���ٴε���tgcpapi_flush���Ծ������ꡣһ������£�����ͻ��˴�ʱ��Ҫ�ر����ӡ�
/// @note �˽ӿڼ�ʹ���ش���Ҳ��Ӱ��tgcpapi_recv��tgcpapi_peek�Ľ���
/// @note ��a_iAllowLost����Ϊ TGCP_NOT_ALLOW_LOST������Ҳ���ָ��Gamesvrʱ�����ӻ�رգ�
///       ��a_iAllowLost����Ϊ TGCP_ALLOW_LOST������Ҳ���ָ��Gamesvrʱ�����Ӳ���رա�
///
/// @see tgcpapi_flush
int tgcpapi_send_with_route(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout,
                            eTGCPRouteType a_iRouteType, const LPTGCPROUTE a_pstRouteInfo, eTGCPAllowLost a_iAllowLost);

/// @brief  flush������ͻ�������ʣ�����������绺����
///
/// @param[in]   a_pHandle   tgcpapi���
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///          ����ֵΪTGCP_ERR_FLUSH_UNCOMPLETEDʱ����������flushδ��ȫ��ϴ�ɾ�
///
/// @note ����ֵΪTGCP_ERR_SEND_UNCOMPLETEʱ��˵�����ݰ�û����ȫ���������磬���Ե���tgcpapi_flush����
///       �ȴ�tgcpapi_send��������һ����һ���͡�
/// @note �����������Ƶ�ʲ��ߵ�����£�tgcpapi_send����TGCP_ERR_SEND_UNCOMPLETEʱ����������tgcpapi_flush
///       ȥ��ϴδ�������ݰ���������Ҫע����ǣ�ͨ�����һ������δ��ȫ����ʱ����Ҫ���ô˽ӿڱ�֤����������
/// @note �˽ӿڼ�ʹ���ش���Ҳ��Ӱ��tgcpapi_recv��tgcpapi_peek�Ľ���
///
/// @see tgcpapi_send
int tgcpapi_flush(HTGCPAPI a_pHandle);


/// @brief  ����ҵ�����ݣ���֤����һ������ҵ���
///
/// @param[in]   a_pHandle     tgcpapi���
/// @param[out]  a_pszBuffOut  ����ҵ�����ݻ������ռ�ָ��
/// @param[in]   a_piSize      ����ҵ�����ݻ������ռ��С��ָ�룩
/// @param[out]  a_piSize      ʵ�ʽ��յ���ҵ�����ݴ�С��ָ�룩
/// @param[in]   a_iTimeout    ����ҵ�����ݰ����ʱʱ��ֵ(��λ��ms), ��������Ϊ0
///
/// @return  =0, ���ճɹ�
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ����ֵΪTGCP_ERR_PEER_STOPPED_SESSIONʱ�����������������ֹ�Ự
/// @note ����ֵΪTGCP_ERR_PKG_NOT_COMPLETEʱ�������հ��������������ϲ���ô�ʱ��Ҫ�ر�����
/// @note ����ֵΪTGCP_HAVE_RECV_GCP_INNER_MSGʱ�������յ�gcp�ڲ���Ϣ���Ѿ����ڲ�����������Ǻ������ܻ���ҵ�����Ҫ�������Խ���
/// @note ��ʹtgcpapi_send��tgcpapi_flush����ʧ�ܣ����ô˽ӿ���Ȼ���Խ������ݡ�
///
/// @see tgcpapi_query_stopped
int tgcpapi_recv(HTGCPAPI a_pHandle, char* a_pszBuffOut, int* a_piSize, int a_iTimeout);


/// @brief  ��tgcpapi����л�ȡ����ҵ����ĵ�ַ������
///
/// @param[in]   a_pHandle   tgcpapi���
/// @param[out]  a_ppszBuff  ��������ҵ�����ַ��ָ��ֵ
/// @param[out]  a_piSize    ��������ҵ������ȵ�ָ��ֵ
/// @param[in]   a_iTimeout  ����ҵ�����ݰ����ʱʱ��ֵ(��λ��ms), ��������Ϊ0
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note �ر�ע�⣺����tgcpapi_peek���Ա��ⲻ��Ҫ�Ŀ��������Ǳ�����peekȡ������ָ������ϴ���
///       ��Ϊ�ٴε���tgcpapi_peek �� tgcpapi_recv���ָ��ָ������ݸ��ǵ�
/// @note ����ֵΪTGCP_ERR_PEER_STOPPED_SESSIONʱ�����������������ֹ�Ự
/// @note ����ֵΪTGCP_ERR_PKG_NOT_COMPLETEʱ�������հ���������һ�㽨���ϲ�����յ����ʱ��Ҫ�ر�����
/// @note ��ʹtgcpapi_send��tgcpapi_flush����ʧ�ܣ����ô˽ӿ���Ȼ���Խ������ݡ�
int tgcpapi_peek(HTGCPAPI a_pHandle, const char** a_ppszBuff, int* a_piSize, int a_iTimeout);


/// @brief  �ͻ�����tconnd������ֹͨ�ŻỰ
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[in]  a_iTimeout ������ֹͨ�ŻỰ�����ʱֵ(��λ��ms)
///
/// @return =0, ���ճɹ�
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_stop_session(HTGCPAPI a_pHandle, int a_iTimeout);

///  @}

/// @defgroup TGCP_API_EVENT TGCP_�¼�̽ѯ
/// @{

/// @brief  ̽ѯ������Ƿ���ֹͨ�ŻỰ
///
/// @param[in]  a_pHandle  tgcpapi���
/// @param[out] a_piResult ��ѯ�����0(�Զ�δ��ֹ), ��0(�Զ���ֹ)
/// @param[out] a_piReason ��ֹ�Ựԭ�������NULL����
///
/// @return =0 success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note �Ƽ�������ֵΪTGCP_ERR_PEER_STOPPED_SESSIONʱ���ϲ���ô˽ӿڲ�ѯԭ��
///       Ҳ�����������ط����ô˽ӿڲ�ѯ�Ƿ���ֹ�Լ���ֹԭ��������ֹͨ�ŻỰʱ�������ͨ�Ž���
///       ��β�����󣬵���tgcpapi_close_connection�ر�socket���ӡ�
///       һ����˵���ڷ���TGCP_ERR_PEER_STOPPED_SESSIONʱ�Ϳ���ȷ���������Ѿ�������ֹ�Ự��
///       �������Ҫ����������β�����������鵽�������ֵ��ֱ�ӹر�socket���ӡ�
///
/// @see tgcpapi_start_connection()
///      tgcpapi_recv()
///      tgcpapi_peek()
///      tgcpapi_close_connection()
int tgcpapi_query_stopped(HTGCPAPI a_pHandle, int* a_piResult, int* a_piReason);

/// @brief ̽ѯ��������е��Ŷ�֪ͨ
///
/// @param[in]  a_pHandle      tgcpapi���
/// @param[out] a_piBingo      �Ƿ�����Ŷӵı�־��!0(����Ŷӣ���������ͨ��), 0(��Ȼ�����Ŷ�״̬)
/// @param[out] a_pstQueNotify �����Ŷ���Ϣ����*a_piBingo����0ʱ������
/// @param[in]  a_iTimeout     ��ȡ�ŶӼ�����Ŷ�֪ͨ�����ʱʱ��ֵ(��λ��ms)
///
/// @return =0 success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @see tgcpapi_start_connection
///
/// @note �˽ӿ�ֻ����tgcpapi_start_connection����TGCP_ERR_STAY_IN_QUEUE����ò������壬
///       ���Ի�ȡ���µ��Ŷ���Ϣ���Լ��Ƿ��Ѿ�����Ŷ�(����ŶӺ�ſ��Խ��������������շ�)
int tgcpapi_query_wait_notify(HTGCPAPI a_pHandle, int* a_piBingo,
                              LPQUEUENOTIFY a_pstQueNotify, int a_iTimeout);
///  @}

/// @defgroup TGCP_LAST_ERROR  TGCP-������Ϣ���
/// @{

/// @brief  ��ȡ���һ��TDR�����Ƚӿڴ���������Ϣ����λ����ʱ������Ϣ��
///
/// @param[in]  a_pHandle tgcpapi���
///
/// @return  ���������ַ���ָ�룬���ΪNULL������û�����tdr����������Ϣ
///
/// @note ��Ҫ˵�������ҽ�������ֵΪTGCP_ERR_PACK_FAILED �� TGCP_ERR_UNPACK_FAILEDʱ����������
const char* tgcpapi_get_tdr_last_error_string(HTGCPAPI a_pHandle);


/// @brief   ��ȡ���һ�η�Ԥ��������ֵ����λ����ʱ������Ϣ��
///
/// @param[in]  a_pHandle
///
/// @return  ��Ԥ��command������ֵ
///          -1, error
///
/// @note ��Ҫ˵������ǰ��������ֵΪTGCP_ERR_UNEXPECTED_COMMANDʱ����������
int tgcpapi_get_last_unexpected_cmd(HTGCPAPI a_pHandle);

///  @}


/// @defgroup TGCP_OPERATE  TGCP-����
/// @{
///
/// @brief ��������AcessToken
///
/// @param[in] a_pHandle    tgcpapi���
/// @param[in] a_iTimeout   ��ʱʱ��(��λ��ms)
///
/// @return =0 success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note �˽ӿ���Ϊ΢��OAuth��Ȩ��ƣ�΢�ż�Ȩ�е�AccessToken��Ч��ԼΪ2��Сʱ����AcessTokenʧЧǰ��
///       �ɵ��ñ��ӿ�ΪAcessToken���ڣ���Q��Ȩ�е�AcessToken���ں���Ҫ��ȡ�µ�AcessToken
int tgcpapi_refresh_acesstoken(HTGCPAPI a_pHandle, int a_iTimeout);



/// @brief �Ƿ���Ҫ�û�������Ȩ
///
/// @param[in] a_pHandle   tgcpapi���
///
/// @return >0, ��Ҫ�û�������Ȩ
///         =0, ����Ҫ�û�������Ȩ
///         <0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
///
/// @note �����³�������Ҫ���øýӿ�ȷ���Ƿ���Ҫ�û�������Ȩ
///         ͬ��ģʽ���� tgcpapi_start_connection �������Ӻ󣬷���ֵΪ TGCP_ERR_PEER_STOPPED_SESSION
///         �첽ģʽ���� tgcpapi_update �������� TGCP_EVENT_SSTOPED �¼�
///
/// @note ͨ��ͨ������ MSDK �� WGLogin �ӿ�ȥ�����û�������Ȩ
///
int tgcpapi_need_user_reauth(HTGCPAPI a_pHandle);
/// @}

int tgcpapi_report_access_token(HTGCPAPI a_pHandle, const char* a_pszAccessToken, unsigned int expire);


/// @brief  ����GCPЭ�����İ汾��
///
/// @param[in]   a_pHandle   tgcpapi���
/// @param[in]   a_iHeadVersion   Э��Head Version������GCPЭ������δ���������Ҫ�ṩ�����汾��
/// @param[in]   a_iBodyVersion   Э��Body Version
/// @note ˵�� �Զ�tconnd�汾��3.1.9-3.1.16ʱ��������HeadVersion=5,BodyVersion=8
///
/// @return  =0, success
///          !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ˵������TCONND�汾�ȿͻ��˸߻�����ͬʱ������Ҫ���ô˽ӿڣ��ڿͻ��˰汾��TCONND�汾��ʱ������Ҫ����
/// @note ˵����GCPĬ�ϻ�ʹ�õ�ǰTDRЭ�����߰汾�ţ��ڵ��øýӿ����ð汾�ź�ϵͳ��ʹ�����õİ汾�Ž���TDRЭ��Ĵ���
/// 
/// @note ����ʱ����tgcpapi_init��tgcpapi_create_and_init֮��
///
int tgcpapi_set_gcp_protocol_version(HTGCPAPI a_pHandle, int a_iHeadVersion, int a_iBodyVersion);

/// @brief  ��ȡ�ͻ��˵İ汾��
const char* tgcpapi_get_version();

#endif
