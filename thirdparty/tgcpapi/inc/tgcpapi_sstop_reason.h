#ifndef TGCPAPI_SSTOP_REASON_H_
#define TGCPAPI_SSTOP_REASON_H_

/* TFRAMEHEAD_STOP_REASON*/
enum tagTFRAMEHEAD_STOP_REASON
{
    TFRAMEHEAD_REASON_NONE = 0, 	/* success */
    TFRAMEHEAD_REASON_IDLE_CLOSE = 1, 	/* ���ӿ��йر� */
    TFRAMEHEAD_REASON_PEER_CLOSE = 2, 	/* �ͻ��˹ر� */
    TFRAMEHEAD_REASON_NETWORK_FAIL = 3, 	/* �����쳣�ر� */
    TFRAMEHEAD_REASON_BAD_PKGLEN = 4, 	/* ����������쳣�ر� */
    TFRAMEHEAD_REASON_EXCEED_LIMIT = 5, 	/* �ͻ��˷����ٶȳ������� */
    TFRAMEHEAD_REASON_TCONND_SHUTDOWN = 6, 	/* tconnd���� */
    TFRAMEHEAD_REASON_SELF_CLOSE = 7, 	/* �����������·��ر� */
    TFRAMEHEAD_REASON_AUTH_FAIL = 8, 	/* �ͻ���������֤ʧ�� */
    TFRAMEHEAD_REASON_SYNACK_FAIL = 9, 	/* �ͻ�������������֤ʧ�� */
    TFRAMEHEAD_REASON_WR_BLOCKED = 10, 	/* socket���ͻ�����д�� */
    TFRAMEHEAD_REASON_SEQUENCE_INVALID = 11, 	/* ���а����кŷǷ� */
    TFRAMEHEAD_REASON_TRANS_RELAY = 12, 	/* Ԥռ���������ͷ� */
    TFRAMEHEAD_REASON_TRANS_LOST = 13, 	/* �����Ѿ��ͷ� */
    TFRAMEHEAD_REASON_RELAY_FAIL = 14, 	/* �ͻ���������֤ʧ�� */
    TFRAMEHEAD_REASON_SESSION_RENEW_FAIL = 15, 	/* session key ����ʧ�� */
    TFRAMEHEAD_REASON_RECV_BUFF_FULL = 16, 	/* ���ջ����� */
    TFRAMEHEAD_REASON_UNPACK_FAIL = 17, 	/* tconnd �ͻ���Э�����ʧ�� */
    TFRAMEHEAD_REASON_INVALID_PKG = 18, 	/* ���ݰ��Ƿ� */
    TFRAMEHEAD_REASON_INVALID_SKEY = 19, 	/* SKEYʧЧ���û��ǵ�¼״̬�� */
    TFRAMEHEAD_REASON_VERIFY_DUP = 20, 	/* ������������У�飬��ǰ������ʱ����У�顣 */
    TFRAMEHEAD_REASON_CLIENT_CLOSE = 21, 	/* �ͻ�����������ر� */
    TFRAMEHEAD_REASON_PRE_RELAY_FAIL = 22, 	/* �ͻ��˿����ת������ʧ�� */
    TFRAMEHEAD_REASON_SYSTEM_ERROR = 23, 	/* ϵͳ�쳣 */
    TFRAMEHEAD_REASON_CLIENT_RECONNECT = 24, 	/* �ͻ��˷���������������Ҫ�ر�ԭ������ */
    TFRAMEHEAD_REASON_GEN_KEY_FAIL = 25, 	/* DH��������KEYʧ�� */
    TFRAMEHEAD_REASON_COUNT = 26, 	/* �Ӵ������ǵø�tconnapi.c��tconnapi.cpp���tconnd_close_string[] */
};

#endif

