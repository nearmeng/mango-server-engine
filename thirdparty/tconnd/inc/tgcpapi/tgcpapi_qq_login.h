/*
 * Copyright (c) 2013 Tencent all rights reserved
 *
 * time:   2014-04-22
 * file:   tgcpapi_qq_login.h
 * desc:   ΪQQ��½ģʽר�õĽӿ�
 *
 */
/// �û���ʹ��QQ��½ģʽ��ʱ�򣬷�Ϊ���¼������裺
///         1.��ʼ����2.���ӣ�3.�շ���4.�˳�
///
///         1.��ʼ�����裺
///             1.1 tgcpapi_create: �������
///             1.2 tgcpapi_qq_login_st_init: ��ʼ��
///
///         2.���ӣ�
///           tgcpapi������ʹ��ģʽ��ͬ��ģʽ���첽ģʽ��
///             2.1 ͬ��ģʽ�漰�ӿڣ�
///                 2.1.1 tgcpapi_start_connection���ͻ��˷�����tconnd������
///                 2.1.2 tgcpapi_close_connection���ر���tconnd������
///                 2.1.3 tgcpapi_relay_connection������ͨ�ŻỰ
///                 2.1.4 tgcpapi_query_wait_notify��̽ѯ��������е��Ŷ�֪ͨ
///                 2.1.5 tgcpapi_query_stopped��̽ѯ������Ƿ���ֹͨ�ŻỰ
///
///             2.2 �첽ģʽ�漰�ӿڣ�
///                 2.2.1 tgcpapi_start���ͻ��˷�����tconnd������
///                 2.2.2 tgcpapi_update��ˢ��tgcpapi�ڲ���״̬���������˷����ͨ��״̬
///                 2.2.3 tgcpapi_resume������ͨ�ŻỰ
///                 2.2.4 tgcpapi_stop���ر���tconnd������
///                 2.2.5 tgcpapi_get_queue����update��ȡ��TGCP_EVENT_WAITING�¼��󣬻�ȡ�Ŷ�״̬
///                 2.2.6 tgcpapi_get_sstop_reason����update��ȡ��TGCP_EVENT_SSTOPED�¼��󣬻�ȡ�ر�ԭ��
///                 2.2.7 tgcpapi_recved_pkglen����update��ȡ��TGCP_EVENT_DATA_IN�¼��󣬻�ȡ������
///                 2.2.8 tgcpapi_sendable_buflen����update��ȡ��TGCP_EVENT_DATA_OUT�¼��󣬻�ȡ���ÿռ�
///                 2.2.9 tgcpapi_get_acesstoken: ��update��ȡ��TGCP_EVENT_ATK�¼��󣬻�ȡ�µ�AccessToken
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
///             3.1 tgcpapi_close_connection(�����ͬ��ģʽ��
///             3.2 tgcpapi_stop��������첽ģʽ��
///             3.3 tgcpapi_fini����init��ԣ�
///             3.4 tgcpapi_destroy����create��ԣ�
#ifndef TGCPAPI_QQ_LOGIN_H_
#define TGCPAPI_QQ_LOGIN_H_

#include "tgcpapi.h"


#define TGCP_QQ_UNIFIED_MAX_SIGN_LEN 256
#define TGCP_QQ_LOGIN_KEY_LEN 16
#define GSPH_QQDECRYPT_KEY      ("aaaaaaaaaaaaaaaa")


/*
 * CTLoginģʽ��Ȩ��Ϣ
 */
struct tagTGCPQQUnifiedAuthInfo
{
    unsigned long long Uin;         //Uin
    char szLoginKey[TGCP_QQ_LOGIN_KEY_LEN];     //��½��Կ����Ӧ��ͨƱ�ݵ� STKey
    unsigned short wLen;    //ǩ������
    char szSigInfo[TGCP_QQ_UNIFIED_MAX_SIGN_LEN];   //ǩ����Ϣ����Ӧ��ͨƱ�ݵ� ST

    tagTGCPQQUnifiedAuthInfo() : Uin(0), wLen(0)
    {

    }
};

typedef struct tagTGCPQQUnifiedAuthInfo TGCPQQUnifiedAuthInfo;


/// @brief ��ʼ��QQ STƱ��ģʽ�µľ��
///
/// @param[in]  a_pHandle       TCPAPI���
/// @param[in]  a_iServiceID    ����ID
/// @param[in]  a_iPlatformID   �豸��Ϣ
/// @param[in]  a_iBuffLen      �û����ݻ���������
/// @param[in]  a_iAppID        QQҵ��ID
/// @param[in]  a_pstAuthInfo   ��Ȩ��Ϣ
/// @param[in]  a_iEncMethod    ���ݼ��ܷ�ʽ,��ʹ�� tagEncryptMethod ö��ֵ
///
/// @return =0, success,
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note ��ʹ��CTLoginģʽ��Ȩʱ����ʼ��ֻ���ñ��������ɣ�Ȼ����÷������ӵĺ���
int tgcpapi_qq_login_st_init(HTGCPAPI a_pHandle, int a_iServiceID, ePlatformID a_iPlatformID, int a_iBuffLen,
                             int a_iAppID, TGCPQQUnifiedAuthInfo* a_pstAuthInfo, eEncryptMethod a_iEncMethod);


/// @brief ����ctlogin��Ȩģʽ�µĲ���ǩ��
///
/// @param[in] a_pstUnifyAuthInfo ���ǩ���Ľṹ��ָ��
/// @param[in] a_uUin   QQ��
/// @param[in] a_pszSvrKey  �ӽ�����Կ
/// @param[in] a_pszSessionKey  �Ự��Կ
/// @param[in] a_uCltIP     �ͻ���IP
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ����ԭ��
///
/// @note �˽ӿ����ɵ�ǩ��ֻ��TGCP_AUTH_QQ_CT_LOGIN��Ȩģʽ��Ч
///         a_pszSvrKey�ǿͻ�����TConndͨ�ţ�ҵ����ļ�����Կ����Կ��TConndҲҪ�ø�ֵ���ܣ��ſ��Ի�ȡa_pszSessionKey
///         a_pszSessionKey�ǿͻ�����gamesvrͨ�ţ�ҵ����ļ�����Կ
///         ͨ������tgcpapi_create_test_signature()�������ɲ���ǩ��
int tgcpapi_make_qqunify_authinfo(TGCPQQUnifiedAuthInfo* a_pstUnifyAuthInfo,
                                  unsigned long long a_uUin, char* a_pszSvrKey, char* a_pszSessionKey, unsigned int a_uCltIP);

/// @brief ����ctlogin��Ȩģʽ�µĹ̶�����ǩ��
///
/// @param[in] a_pstUnifyAuthInfo ���ǩ���Ľṹ��ָ��
/// @param[in] a_uUin   QQ��
/// @param[in] a_uCltIP     �ͻ���IP
///
/// @return =0, success
///         !0, fail, ����tgcpapi_error_string()��ȡ��ԭ��
///
/// @note ���ɵĲ���ǩ����������ͨ����ԿΪ GSPH_QQDECRYPT_KEY��TConnd��Ӧ��������ͬ��ֵ
///         ͨ�����ñ��������ɲ���ǩ��
int tgcpapi_create_test_signature(TGCPQQUnifiedAuthInfo* a_pstUnifyAuthInfo, unsigned long long a_uUin, unsigned int a_uCltIP);

#endif

