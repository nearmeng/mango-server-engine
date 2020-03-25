#ifndef TGCPAPI_SSTOP_REASON_H_
#define TGCPAPI_SSTOP_REASON_H_

/* TFRAMEHEAD_STOP_REASON*/
enum tagTFRAMEHEAD_STOP_REASON
{
    TFRAMEHEAD_REASON_NONE = 0, 	/* success */
    TFRAMEHEAD_REASON_IDLE_CLOSE = 1, 	/* 连接空闲关闭 */
    TFRAMEHEAD_REASON_PEER_CLOSE = 2, 	/* 客户端关闭 */
    TFRAMEHEAD_REASON_NETWORK_FAIL = 3, 	/* 网络异常关闭 */
    TFRAMEHEAD_REASON_BAD_PKGLEN = 4, 	/* 请求包长度异常关闭 */
    TFRAMEHEAD_REASON_EXCEED_LIMIT = 5, 	/* 客户端发送速度超过限制 */
    TFRAMEHEAD_REASON_TCONND_SHUTDOWN = 6, 	/* tconnd重启 */
    TFRAMEHEAD_REASON_SELF_CLOSE = 7, 	/* 服务器主动下发关闭 */
    TFRAMEHEAD_REASON_AUTH_FAIL = 8, 	/* 客户端连接验证失败 */
    TFRAMEHEAD_REASON_SYNACK_FAIL = 9, 	/* 客户端三次握手验证失败 */
    TFRAMEHEAD_REASON_WR_BLOCKED = 10, 	/* socket发送缓冲区写满 */
    TFRAMEHEAD_REASON_SEQUENCE_INVALID = 11, 	/* 上行包序列号非法 */
    TFRAMEHEAD_REASON_TRANS_RELAY = 12, 	/* 预占由于重连释放 */
    TFRAMEHEAD_REASON_TRANS_LOST = 13, 	/* 连接已经释放 */
    TFRAMEHEAD_REASON_RELAY_FAIL = 14, 	/* 客户端重连验证失败 */
    TFRAMEHEAD_REASON_SESSION_RENEW_FAIL = 15, 	/* session key 续期失败 */
    TFRAMEHEAD_REASON_RECV_BUFF_FULL = 16, 	/* 接收缓存满 */
    TFRAMEHEAD_REASON_UNPACK_FAIL = 17, 	/* tconnd 客户端协议解码失败 */
    TFRAMEHEAD_REASON_INVALID_PKG = 18, 	/* 数据包非法 */
    TFRAMEHEAD_REASON_INVALID_SKEY = 19, 	/* SKEY失效，用户非登录状态。 */
    TFRAMEHEAD_REASON_VERIFY_DUP = 20, 	/* 其他连接已在校验，当前连接暂时不能校验。 */
    TFRAMEHEAD_REASON_CLIENT_CLOSE = 21, 	/* 客户端主动请求关闭 */
    TFRAMEHEAD_REASON_PRE_RELAY_FAIL = 22, 	/* 客户端跨服跳转端重连失败 */
    TFRAMEHEAD_REASON_SYSTEM_ERROR = 23, 	/* 系统异常 */
    TFRAMEHEAD_REASON_CLIENT_RECONNECT = 24, 	/* 客户端发生重连操作，需要关闭原来链接 */
    TFRAMEHEAD_REASON_GEN_KEY_FAIL = 25, 	/* DH方法生成KEY失败 */
    TFRAMEHEAD_REASON_COUNT = 26, 	/* 加错误码后记得改tconnapi.c和tconnapi.cpp里的tconnd_close_string[] */
};

#endif

