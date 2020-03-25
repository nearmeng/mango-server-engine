#ifndef TGCP_API_ERROR_H
#define TGCP_API_ERROR_H

typedef enum {
    TGCP_ERR_NONE                       = 0,
    TGCP_ERR_INVLD_HANDLE               = -1,
    TGCP_ERR_INVLD_ARGUMENT             = -2,
    TGCP_ERR_ALLOC_FAIL                 = -3,
    TGCP_ERR_HANDLE_NOT_INITED          = -4,
    TGCP_ERR_STR_LEN_TOO_BIG            = -5,
    TGCP_ERR_CONNECT_FAILED             = -6,
    TGCP_ERR_AUTH_FAIL                  = -7,
    TGCP_ERR_SYN_NOT_FINISHED           = -8,
    TGCP_ERR_NETWORK_EXCEPTION          = -9,
    TGCP_ERR_PEER_CLOSED_CONNECTION     = -10,
    TGCP_ERR_PEER_STOPPED_SESSION       = -11,
    TGCP_ERR_PKG_NOT_COMPLETE           = -12,
    TGCP_ERR_UNEXPECTED_MAGIC           = -13,
    TGCP_ERR_UNEXPECTED_COMMAND         = -14,
    TGCP_ERR_PKG_ILLEGAL                = -15,
    TGCP_ERR_SEND_BIGGER_BUSS_DATA      = -16,
    TGCP_ERR_PACK_FAILED                = -17,
    TGCP_ERR_UNPACK_FAILED              = -18,
    TGCP_ERR_ASSERT_FAILED              = -19,
    TGCP_ERR_UNKNOW_AUTH_TYPE           = -20,
    TGCP_ERR_SHORT_BUF_FOR_WRITE        = -21,
    TGCP_ERR_GET_ACCOUNT_FAIL           = -22,
    TGCP_ERR_SEND_FAILTURE              = -23,
    TGCP_ERR_SEND_BUFFER_FULL           = -24,
    TGCP_ERR_SEND_UNCOMPLETED           = -25,
    TGCP_ERR_NO_TOKEN                   = -26,
    TGCP_ERR_NO_QUEUE_INFO              = -27,
    TGCP_ERR_STAY_IN_QUEUE              = -28,
    TGCP_ERR_UNSUPPORTED_ENC_METHOD     = -29,
    TGCP_ERR_UNSUPPORTED_KEY_MODE       = -30,
    TGCP_ERR_INVLD_SECRET_KEY           = -31,
    TGCP_ERR_AES_ENCRYPT                = -32,
    TGCP_ERR_AES_DECRYPT                = -33,
    TGCP_ERR_NO_OPENID                  = -34,
    TGCP_ERR_NO_APPID                   = -35,
    TGCP_ERR_UNSUPPORTED_ACCOUNT_TYPE   = -36,
    TGCP_ERR_AUTH_REFRESH_FAIL          = -37,
    TGCP_ERR_SERVER_IS_FULL             = -38,
    TGCP_ERR_GEN_DH_KEY_FAILED          = -39,
    TGCP_ERR_CUM_DH_KEY_FAILED          = -40,
    TGCP_ERR_DH_KEY_LEN_TOO_BIG         = -41,
    TGCP_ERR_GEN_RSA_KEY_FAILED         = -42,
    TGCP_ERR_ENC_DH_FAILED              = -43,
    TGCP_ERR_FLUSH_UNCOMPLETED          = -44,
    TGCP_ERR_NO_TCP_CONNECTION          = -45,
    TGCP_ERR_NOT_INIT_DH_INFO           = -46,
	TGCP_ERR_TOKEN_INVALID				= -47,//无效或过期的Token，需要让用户重新登录。
	TGCP_ERR_TOKEN_SVR_ERROR			= -48,//鉴权服务器错误（可以重试，不需要让用户重新登录）
    TGCP_ERR_COMPR_ERROR                = -49,
    TGCP_ERR_DECOMPR_ERROR              = -50,
    TGCP_ERR_UNSUPPORTED_METHOD_IN_CURRENT_MODE = -51, // 当前模式（由初始化时选择的不同接口决定）不支持该函数。
    TGCP_ERR_NEED_KEY                   = -52,
    TGCP_ERR_CREATE_SIG_FAIL            = -53,
    TGCP_ERR_INVLD_ROUTE_TYPE           = -54,
    TGCP_ERR_INVLD_PROTOCOL_VERSION     = -55,
    //[-56, -65] is reserved by lwip tgcp api
    TGCP_ERR_NOT_SUPPORT				= -56,
    TGCP_ERR_ALREADY_INITED				= -57,
    TGCP_ERR_FAILED_TO_ALLOCATE_SOCKET  = -58,
    TGCP_ERR_FAILED_TO_CONNECT_SOCKET   = -59,
    TGCP_ERR_INVALID_SOCKET             = -60,
    TGCP_ERR_FAILED_TO_SEND_UDP			= -61,

    TGCP_ERR_MERGE_PACKET_TOO_BIG       = -66,//收到的合并包太大
    TGCP_HAVE_RECV_GCP_INNER_MSG        = -67,//收到GCP协议内部消息，压测工具在收到这个返回码时要尝试继续收包
} TGCP_ERROR;


const char* tgcpapi_error_string(int iErrorCode);

#endif
