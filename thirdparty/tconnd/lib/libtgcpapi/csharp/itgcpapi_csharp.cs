using System;
using System.Runtime.InteropServices;


namespace Apollo
{

    // 加密方法
    public enum eEncryptMethod
    {
        TGCP_ENCRYPT_METHOD_NONE    = 0,   //不加密
        TGCP_ENCRYPT_METHOD_TEA     = 1,   //暂不支持
        TGCP_ENCRYPT_METHOD_QQ      = 2,   //暂不支持
        TGCP_ENCRYPT_METHOD_AES     = 3,   //AES加密
        TGCP_ENCRYPT_METHOD_AES2    = 4,   //AES2加密
    };

    // 密钥生成方法
    public enum eKeyMaking
    {
        TGCP_KEY_MAKING_NONE = 0,       //不使用秘钥（此时认为通信数据不加密）
        TGCP_KEY_MAKING_INAUTH,         //秘钥包含在鉴权信息
        TGCP_KEY_MAKING_INSVR,          //秘钥由TCONND服务器下发
        TGCP_KEY_MAKING_RAW_DH,         //原始DH算法秘钥
        TGCP_KEY_MAKING_ENC_DH,         //加密过的DH算法秘钥
    };

    // 鉴权方式
    public enum eAuthType
    {
        TGCP_AUTH_NONE = 0,         //不使用鉴权
        TGCP_AUTH_WX_O2_CODE,       //微信 OAuth 2.0 CODE
        TGCP_AUTH_WX_O2_RTK,        //微信 OAuth 2.0 refresh token
        TGCP_AUTH_QQ_O2_ATK,        //手Q  OAuth 2.0 acess token

        TGCP_AUTH_VER_2 = 0x7fff,      //新的帐号鉴权方式，2014.03.07
    };

    // 平台信息
    public enum ePlatformID
    {
        TGCP_PC = 0,
        TGCP_ANDROID = 101,
        TGCP_IOS = 102,
    };

    //压缩选项
    public enum eCompressMethod
    {
        TGCP_COMPR_NONE = 0,        //不压缩
        TGCP_COMPR_LZ4,             //使用LZ4方法压缩
    };

    // 账号类型
    public enum eAccountType
    {
        TGCP_ACCOUNT_TYPE_NONE = 0,      //自定义格式
        TGCP_ACCOUNT_TYPE_QQ_UIN,        //QQ号
        TGCP_ACCOUNT_TYPE_QQ_OPENID,     //手QOpenID
        TGCP_ACCOUNT_TYPE_WX_OPENID,     //微信OpenID
		TGCP_ACCOUNT_TYPE_IOS_GUEST = 0x2001,     //游客模式
    };

    public enum eAccountFormat
    {
        TGCP_ACCOUNT_FORMAT_QQ_32 = 0,
        TGCP_ACCOUNT_FORMAT_QQ_64,
        TGCP_ACCOUNT_FORMAT_STRING,
        
    };

    public enum eTGCPEvent
    {
        TGCP_EVENT_SSTOPED      = 0x01,     //服务器主动关闭会话
        TGCP_EVENT_WAITING      = 0x02,     //正在排队
        TGCP_EVENT_DATA_IN      = 0x04,     //接收缓冲区包可读(至少收到一个完整包)
        TGCP_EVENT_DATA_OUT     = 0x08,     //发送缓冲区可写
        TGCP_EVENT_ATK          = 0x10,     //ATK续期通知
        TGCP_EVENT_SVR_IS_FULL  = 0x20,     //服务器爆满通知
    };
    
    /*
    * 与连接相关的排队信息结构体
    * */
    [StructLayout(LayoutKind.Sequential)]
    public struct QueueNotify
    {
        public uint pos;               //当前位置
        public uint queue_len;         //队列长度
        public uint estimate_time;     //预计排队时长
    }

    /*
     * 与异步操作相关的事件信息结构体
     * */
    [StructLayout(LayoutKind.Sequential)]
    public struct TGCPEvent
    {
        public uint event_num;     //事件个数
        public uint events;        //事件位标志
    }

	/*
     * LOG级别
     * */
	public enum T_LOG_PRIORITY
	{
		TLOG_Pri_Debug,
		TLOG_Pri_Info,
		TLOG_Pri_Warning,
		TLOG_Pri_Event,
		TLOG_Pri_Error,
		TLOG_Pri_None,
	};
	
    interface ITGCPAPI
    {
        int Create();

        int Destroy();

        int Init(int service_id, ePlatformID platform_id, ushort auth_type, eEncryptMethod enc_method, eKeyMaking key_mode, int buff_len);

        int Fini();


        /*
         * 使用OAuth鉴权方式特有接口
         * */
        int GetSocket(ref int socket_fd);

        int SetAppID(byte[] app_id, int len);

        int SetToken(byte[] token, int len);

        int SetAccessTokenExpire(ushort expire);

        int GetAccessToken(byte[] token, ref int len);

        int GetRefreshToken(byte[] token, ref int len);

        int SetOpenIDAccount(eAccountType type, string openid);

        int SetOpenUidAccount(eAccountType type, eAccountFormat format, UInt64 uid);

        int GetMapid(ref UInt64 mapid);

        int GetOpenID(byte[] open_id, ref int len);

        int SetDHInfo(string dh_p_hex);

        int StartConnection(string url, int timeout);

        int CloseConnection();

        int RelayConnection(string url, int timeout);


        int Start(string url);

        int Update(ref TGCPEvent events);

        int Resume(string url);

        int Stop();

        QueueNotify GetQueue();

        int GetServerStopReason();

        int RecvedPkgLen();

        int SendableBufLen();

        int Send(byte[] buff, int size, int timeout);

        int Flush();

        int Recv(byte[] buff, ref int size, int timeout);

        int StopSession(int timeout);

        int QueryStopped(ref int result, ref int reason);

        int QueryWaitNotify(ref int bingo, ref QueueNotify notify, int timeout);

        string GetTdrLastErrorString();

        int GetLastUnexpectCmd();

        int RefreshAccessToken(int timeout);

		void SetLogLevel(T_LOG_PRIORITY pri);

		//void SetTLogCallback(TLogCallback callback);
		int NeedUserReauth();

		int create_and_init(int service_id,byte[] app_id, int len,int buff_len,eAccountType type,UInt64 uid,byte[] atk, int atk_len);

		int create_and_init_string(int service_id,byte[] app_id, int len,int buff_len,eAccountType type,string openid,byte[] atk, int atk_len);

		int set_security_info(eEncryptMethod enc_method, eKeyMaking key_mode,string dh_p_hex);

		int set_authtype(eAuthType auth_type);

		int set_refresh_token(byte[] token, int len, int expire);

		int set_refresh_token_expire(int expire);
    }
}
