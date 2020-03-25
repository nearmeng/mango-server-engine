//////////////////////////////////////////////////////////////////////////
/// @file:   tgcpapi_ex.h
/// @brief:  ���ļ�����Ľӿ��Ƕ�tgcpapi.h��һ�����߲�ķ�װ
/// @author: vforkk
/// @date:   2014-03-03
/// @desc:  �û���ʹ��tgcpapi��ʱ�򣬷�Ϊ���¼������裺
///         1.��ʼ����2.���ӣ�3.�շ���4.�˳�
///
///         1.��ʼ�����裺
///             1.1 tgcpapi_create_and_init:���������ͬʱ��ʼ�������������ʺ���Ϣ��AccessToken)
///             1.2 tgcpapi_set_security_info:���ü��ܷ�ʽ��Ϣ
///             1.3 tgcpapi_set_acesstoken_expire������AccessToken��Ч��(����еĻ���
///             1.4 tgcpapi_set_refresh_token������RefreshToken������еĻ���
///             1.5 tgcpapi_set_refresh_token_expire: ����RefreshToken(����еĻ���
///
///         2.���ӣ�
///           tgcpapi������ʹ��ģʽ��apiģʽ��Eventģʽ��
///             2.1 apiģʽ�漰�ӿڣ�
///                 2.1.1 tgcpapi_start_connection���ͻ��˷�����tconnd�����ӡ�����(SYN/ACK)����Ȩ(����)��ͬ���ȴ���Ӧ��
///                 2.1.2 tgcpapi_close_connection���ر���tconnd������
///                 2.1.3 tgcpapi_relay_connection������ͨ�ŻỰ���˽ӿڻ��������֡���Ȩ����
///                 2.1.4 tgcpapi_query_wait_notify��̽ѯ��������е��Ŷ�֪ͨ
///                 2.1.5 tgcpapi_query_stopped��̽ѯ������Ƿ���ֹͨ�ŻỰ
///
///             2.2 Eventģʽ�漰�ӿڣ�(Eventģʽ��ʹ�ù��̣������ļ�ĩβ��ע�Ͳ��֣�
///                 2.2.1 tgcpapi_start���������˷����ͨ�ŻỰ
///                 2.2.2 tgcpapi_update��ˢ��tgcpapi�ڲ���״̬���������˷����ͨ��״̬
///                 2.2.3 tgcpapi_resume���ָ������������˷����ͨ�ŻỰ��һ��������ͨ�Ź�������������쳣����ã�ֻ�����������ӣ�
///                 2.2.4 tgcpapi_stop�� ��ֹ���˷����ͨ�ŻỰ
///                 2.2.5 tgcpapi_get_queue����update��ȡ��TGCP_EVENT_WAITING�¼��󣬻�ȡ�Ŷ�״̬
///                 2.2.6 tgcpapi_get_sstop_reason����update��ȡ��TGCP_EVENT_SSTOPED�¼��󣬻�ȡ�ر�ԭ��
///                 2.2.7 tgcpapi_recved_pkglen����update��ȡ��TGCP_EVENT_DATA_IN�¼��󣬻�ȡ������
///                 2.2.8 tgcpapi_sendable_buflen����update��ȡ��TGCP_EVENT_DATA_OUT�¼��󣬻�ȡ���ÿռ�
///                 2.2.9 tgcpapi_refresh_acesstoken����������AcessToken����update��ȡ��TGCP_EVENT_ATK�¼��󣬵���tgcpapi_get_acesstoken���ȡ�µ�AccessToken
///
///         3.�շ�����
///           ������apiģʽ����Eventģʽ��ʹ�õĶ�����ͬ���շ��ӿڡ��������ڣ�
///           apiģʽ���շ��ӿ�����Ӧ�ó����Լ���ѭ�����������ã�(��ʹ��ϵͳsocket��send��recvһ����
///           Eventģʽ����Update֮���⵽����¼��󣬲ű����á�
///
///             3.1 tgcpapi_send����update��⵽TGCP_EVENT_DATA_OUT�¼�������д����͵����ݰ�������ô˽ӿڷ���
///             3.2 tgcpapi_recv����update��⵽TGCP_EVENT_DATA_IN�¼��󣬽����������ݰ�.��peek�������ǻ´�����ݡ�
///             3.3 tgcpapi_peek����update��⵽TGCP_EVENT_DATA_IN�¼��󣬽����������ݰ�.��recv�������ǲ��������ݣ�����Ҫ��ʱȡ�ߣ��������ݿ��ܻᱻ���
///             3.4 tgcpapi_flush����tgcpapi_send����TGCP_ERR_SEND_UNCOMPLETEʱ��������������tgcpapi_flushȥ��ϴδ�������ݰ���
///
///           ע����send��flush���ش��󣬲�Ӱ��tgcpapi_recv��tgcpapi_peek�Ľ��ա�Ӧ�ó�����Ӧ����������recv��peek�ӿڣ���ȡ���ջ����е���Ч���ݰ���
///
///         4.�˳�
///             3.1 tgcpapi_close_connection(�����Apiģʽ��
///             3.2 tgcpapi_stop�������Eventģʽ��
///             3.3 tgcpapi_fini����init��ԣ�
///             3.4 tgcpapi_destroy����create��ԣ�
///
///         5.�����ӿ�
///             4.1 tgcpapi_get_socket����ȡ�ڲ�socket���ļ��������������Ȳ����ѣ�����ǿ�ҽ��鲻Ҫ���÷��ʴ˽ӿڣ�
///             4.2 tgcpapi_get_acesstoken����ȡAccessToken
///             4.3 tgcpapi_get_refreshtoken����ȡRefreshToken
///             4.4 tgcpapi_get_account:��ȡAccount��Ϣ
///             4.5 tgcpapi_get_openid����ȡOpenid
///             4.6 tgcpapi_get_tdr_last_error_string����ȡ���һ��TDR�����Ƚӿڴ���������Ϣ����λ����ʱ������Ϣ��
///             4.7 tgcpapi_get_last_unexpected_cmd����ȡ���һ�η�Ԥ��������ֵ����λ����ʱ������Ϣ��
///             4.8 tgcpapi_error_string: ��ȡ��ϸ�Ĵ�������
//////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             FBI Warning!!!!
// ���ļ�����Ľӿڣ��Ƕ�tgcpapi.h�ĸ��߲�ķ�װ������һ���ֽӿں�tgcpapi.h�еĲ��ֽӿڲ���ͬʱʹ��.
// tgcpapi_ex.h��tgcpapi.h����ͬʱʹ�õĽӿ��У�
//          tgcpapi_ex.h                                        tgcpapi.h
// tgcpapi_create_and_init          VS       ��ͻ�ӿڣ�tgcpapi_create��tgcpapi_init��tgcpapi_set_token
//                                           �ظ��ӿڣ�tgcpapi_set_appid��tgcpapi_set_dh_info��tgcpapi_set_account(tgcpapi.h�ж���ĺ�����
//
// tgcpapi_set_security_info        VS       tgcpapi_init��tgcpapi_set_dh_info
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGCP_API_EX_H_
#define TGCP_API_EX_H_
#include "tgcpapi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ���½ӿڣ������滻tgcpapi_create��tgcpapi_init��tgcpapi_set_appid�ӿ�
/// @brief  ����TGCP���ͬʱ��ʼ��
///
/// @param[in, out]  a_ppHandle tgcpapi���ָ��
/// @param[in]  iServiceID    ҵ��ID��TConnd3.1.7�����ϰ汾�����ֶ������塣��ȡֵ�ᱻTConnd�����ļ���BusinessID���ǡ�
/// @param[in]  pszAppId      AppId������������QQ��΢�����뵽��AppId��QQ��΢�ŶԵ�����Ӧ�õ���ݱ�ʶ��
/// @param[in]  iMaxMessageBuffSize      �û����ݻ�������С�� ���ֵ����С��tgcpapi_send,tgcpapi_recv,tgcpapi_peek���ݰ������ܵ���󳤶ȡ�
/// @param[in]  pAccount     ��¼���ʺ���Ϣ
/// @param[in]  pszAccessToken     ��QQ��΢�ŷ��ص�AccessToken
/// @param[in]  iAccessTokenLen     pszAccessToken����Ч����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_create_and_init(HTGCPAPI* ppHandler,
    int iServiceID, 
    const char* pszAppId, int iAppIdLen,
    int iMaxMessageBuffSize,
    LPTGCPACCOUNT pAccount,
    const char* pszAccessToken, int iAccessTokenLen
    );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  ������֤�����Ϣ
///
/// @param[in, out]  a_pHandle tgcpapi���ָ��
/// @param[in]  iEncMethod    ���ݼ��ܷ�ʽ,��ʹ�� tagEncryptMethod ö��ֵ
/// @param[in]  iKeyMode      ��ԿЭ�̷�ʽ,��ʹ�� tagKeyMaking ö��ֵ
/// @param[in]  pszDH_P_Hex   ������p��Hexֵ���ô��������ɹ������ɣ�,��iKeyModeΪTGCP_KEY_MAKING_RAW_DHʱ����Ҫ��˲�����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_security_info(HTGCPAPI pHandler, 
    eEncryptMethod iEncMethod,
    eKeyMaking iKeyMode,
    const char* pszDH_P_HexString
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  ����AuthType�����ڱ�ʶAccessToken��RefreshToken��Code����Token�����������֤��ʽ��
///         ����û�ʹ�õ�����������Ʊ�ݣ�����Ҫ���ô˽ӿ�
///
/// @param[in, out]  a_pHandle tgcpapi���ָ��
/// @param[in]  authType   ��֤����
/// @note       ���� [0x7000, 0x7FFF] Ϊ�ڲ���������Ϊ��������
/// @note            0x7001 �Ѿ�����, ΢�ż�Ȩ, ͳһ CODE �� RTK ���ּ�Ȩ
/// @note            0x7FFF �Ѿ����䣬OAuth 2.0 ͳһ��Ȩʱ��ʾ "������"
/// @note            [0x8000, 0xFFFF] Ϊ�ⲿ����������Ŀ��ʹ��
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_authtype(HTGCPAPI pHandler, eAuthType authType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  ����RefreshToken
///
/// @param[in, out]  a_pHandle tgcpapi���ָ��
/// @param[in]  pszRefreshToken   Refresh Token����
/// @param[in]  iRefreshTokenLen   pszRefreshToken�ĳ���
/// @param[in]  iExpire  ��Ч�ڣ�����Ϊ��λ,û�еĻ�����0
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_refresh_token(HTGCPAPI pHandler, const char* pszRefreshToken, int iRefreshTokenLen, int iExpire = 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  ����RefreshToken����Ч�ڣ�û�еĻ�����Ҫ���ô˽ӿ�
///
/// @param[in, out]  a_pHandle tgcpapi���ָ��
/// @param[in]  iExpire  ��Ч�ڣ�����Ϊ��λ
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_refresh_token_expire(HTGCPAPI pHandler, int iExpire);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  ����Code
///
/// @param[in, out]  a_pHandle tgcpapi���ָ��
/// @param[in]  pszAuthCode   AuthCodeֵ
/// @param[in]  iCodeLen   pszAuthCode�ĳ���
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_auth_code(HTGCPAPI pHandler, const char* pszAuthCode, int iCodeLen);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  �������ʺţ�һ�������ʺ��л���
///
/// @param[in, out]  a_ppHandle tgcpapi���ָ��
/// @param[in]  pAccount     ��¼���ʺ���Ϣ
/// @param[in]  pszAccessToken     ��QQ��΢�ŷ��ص�AccessToken
/// @param[in]  iAccessTokenLen     iAccessTokenLen����Ч����
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
int tgcpapi_set_account_ex(HTGCPAPI pHandler,
    LPTGCPACCOUNT pAccount,
    const char* pszAccessToken,
    int iAccessTokenLen
    );

/*********************************************************************************************************
���½ӿ�ʹ��tgcpapi.h�Ķ���

// APIģʽ
int tgcpapi_start_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);
int tgcpapi_close_connection(HTGCPAPI a_pHandle);
int tgcpapi_relay_connection(HTGCPAPI a_pHandle, const char* a_pszUrl, int a_iTimeout);

int tgcpapi_query_wait_notify(HTGCPAPI a_pHandle, int* a_piBingo,
                              LPQUEUENOTIFY a_pstQueNotify, int a_iTimeout);
int tgcpapi_query_stopped(HTGCPAPI a_pHandle, int* a_piResult, int* a_piReason);


// �¼�ģʽ
int tgcpapi_start(HTGCPAPI a_pHandle, const char* a_pszUrl);
int tgcpapi_update(HTGCPAPI a_pHandle, LPGCPEVENT a_pstEvent);
int tgcpapi_resume(HTGCPAPI a_pHandle, const char* a_pszUrl);
int tgcpapi_stop(HTGCPAPI a_pHandle);

const QUEUENOTIFY* tgcpapi_get_queue(HTGCPAPI a_pHandle);
int tgcpapi_get_sstop_reason(HTGCPAPI a_pHandle);
int tgcpapi_recved_pkglen(HTGCPAPI a_pHandle);
int tgcpapi_sendable_buflen(HTGCPAPI a_pHandle);


// �շ��ӿ�
int tgcpapi_send(HTGCPAPI a_pHandle, const char* a_pszBuffIn, int a_iSize, int a_iTimeout);
int tgcpapi_recv(HTGCPAPI a_pHandle, char* a_pszBuffOut, int* a_piSize, int a_iTimeout);
int tgcpapi_peek(HTGCPAPI a_pHandle, const char** a_ppszBuff, int* a_piSize, int a_iTimeout);
int tgcpapi_flush(HTGCPAPI a_pHandle);

// ��������
int tgcpapi_refresh_acesstoken(HTGCPAPI a_pHandle, int a_iTimeout);

// Set�ӿ�
int tgcpapi_set_acesstoken_expire(HTGCPAPI a_pHandle, unsigned short a_iExpire);
int tgcpapi_set_route_info(HTGCPAPI a_pHandle, eClusterType a_iClusterType, LPCLUSTERINFO a_pstClusterInfo, eDataAllowLost a_iDataAllowLost);

// Get�ӿ�
int tgcpapi_get_socket(HTGCPAPI a_pHandle, int* a_piFd);
int tgcpapi_get_acesstoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);
int tgcpapi_get_refreshtoken(HTGCPAPI a_pHandle, char* a_pszToken, int* a_piLen);
int tgcpapi_get_account(HTGCPAPI a_pHandle, LPTGCPACCOUNT  a_pstAccount);
int tgcpapi_get_openid(HTGCPAPI a_pHandle, char* a_pszOpenID, int* a_piLen);

const char* tgcpapi_get_tdr_last_error_string(HTGCPAPI a_pHandle);
int tgcpapi_get_last_unexpected_cmd(HTGCPAPI a_pHandle);

// ��ȡ������Ϣ
const char* tgcpapi_error_string(int iErrorCode);

*********************************************************************************************************/

/*********************************************************************************************************
** ������Eventģʽ�Ĵ�Ź���

//���Է���һ���߳�ѭ����
if (tgcpapi_update(a_pHandle, &stEvent) == TGCP_ERR_NONE)
{

    if (stEvent.iEvtNum > 0)
    {
        if (stEvent.iEvents & TGCP_EVENT_SVR_IS_FULL)
        {
            return;
        }

        if (stEvent.iEvents & TGCP_EVENT_SSTOPED)
        {
            return;
        }

        // �Ŷ���Ϣ
        if (stEvent.iEvents & TGCP_EVENT_WAITING)
        {
            //ֻ��Ҫ��ʾ�û�,�������˳����򡣵ȳɹ������ˣ��Ϳ����ߺ������������ˣ�����Ҫ��������
        }

        // ����
        if (stEvent.iEvents & TGCP_EVENT_DATA_IN)
        {
            tgcpapi_peek();
        }

        if (stEvent.iEvents & TGCP_EVENT_ATK)
        {
            tgcpapi_get_acesstoken(a_pHandle, acc_token, &atk_size);
        }

        // ����
        if (stEvent.iEvents & TGCP_EVENT_DATA_OUT)
        {
            a���ӷ���Buffer�ж�һ�����������ݰ�
            b��tgcpapi_send(a_pHandle, buffer, sizeof(buffer), 0);
        }
    }
}
*********************************************************************************************************/

#endif
