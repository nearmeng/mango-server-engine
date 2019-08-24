using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Apollo
{
    public class TGCPAPI : ITGCPAPI
    {

		#if UNITY_IPHONE || UNITY_XBOX360
		
		// On iOS and Xbox 360 plugins are statically linked into
		// the executable, so we have to use __Internal as the
		// library name
		
		private const string plugin_name = "__Internal";
		
		#elif UNITY_ANDROID
		
		private const string plugin_name = "tgcpapi";
		
		#else
		// Other platforms load plugins dynamically, so pass the name
		// of the plugin's dynamic library.
		private const string plugin_name = "libtgcpapi";
		
		#endif


        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_create(ref IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_destroy(ref IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_init(IntPtr handle, int service_id, ePlatformID platform_id,
            ushort auth_type, eEncryptMethod enc_method, eKeyMaking key_mode, int buff_len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_fini(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_socket(IntPtr handle, ref int fd);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_appid(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] app_id, int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_token(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] token, int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_acesstoken_expire(IntPtr handle, ushort expire);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_acesstoken(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] token, ref int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_refreshtoken(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] token, ref int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_openid_account(IntPtr handle, eAccountType type, [MarshalAs(UnmanagedType.LPStr)]String openid);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_uid_account(IntPtr handle, eAccountType type, eAccountFormat format, UInt64 uid);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_mapid(IntPtr handle, ref UInt64 mapid);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_openid(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] open_id, ref int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_dh_info(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)]String dh_p_hex);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_start_connection(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)]String url, int timeout);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_close_connection(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_relay_connection(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)]String url, int timeout);



        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_start(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)]String url);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_update(IntPtr handle, IntPtr event_ptr);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_resume(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)]String url);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_stop(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tgcpapi_csharp_get_queue(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_sstop_reason(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_recved_pkglen(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_sendable_buflen(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_send(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] buff, int size, int timeout);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_flush(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_recv(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] buff, ref int size, int timeout);

        //C#不提供peek操作

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_stop_session(IntPtr handle, int timeout);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_query_stopped(IntPtr handle, ref int result, ref int reason);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_query_wait_notify(IntPtr handle, ref int bingo, IntPtr notify_ptr, int timeout);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tgcpapi_csharp_get_tdr_last_error_string(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_get_last_unexpected_cmd(IntPtr handle);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_refresh_acesstoken(IntPtr handle, int timeout);

        //tgcpapi_error.h
        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tgcpapi_csharp_error_string(int error_code);

		[DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
		private static extern int tgcpapi_csharp_need_user_reauth(IntPtr handle);


        
        //tgcpapi_ex
		[DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
		private static extern int tgcpapi_csharp_create_and_init(ref IntPtr handle, int service_id, [MarshalAs(UnmanagedType.LPArray)]byte[]app_id, int app_id_len, int buff_len,
		                                                                eAccountType type, UInt64 uid, [MarshalAs(UnmanagedType.LPArray)]byte[] atk, int atk_len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_create_and_init_string(ref IntPtr handle, int service_id, [MarshalAs(UnmanagedType.LPArray)]byte[]app_id, int app_id_len, int buff_len,
		                                                         eAccountType type, string openid, [MarshalAs(UnmanagedType.LPArray)]byte[] atk, int atk_len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_security_info(IntPtr handle, eEncryptMethod enc_method, eKeyMaking key_mode, [MarshalAs(UnmanagedType.LPStr)]String dh_p_hex);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_authtype(IntPtr handle, eAuthType auth_type);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_refresh_token(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] token, int len, int expire);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_refresh_token_expire(IntPtr handle, int expire);
		/*
        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_auth_code(IntPtr handle, [MarshalAs(UnmanagedType.LPArray)]byte[] code, int len);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_set_account_ex(IntPtr handle, [MarshalAs(UnmanagedType.LPStruct)]TGCPAccount account, [MarshalAs(UnmanagedType.LPArray)]byte[] atk, int len);
        */


        //for debug
        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tgcpapi_csharp_test();

		#if UNITY_STANDALONE_WIN
        //for windows
        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_WSA_startup(int maj, int min);

        [DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
        private static extern int tgcpapi_csharp_WSA_cleanup();
		#endif

		[DllImport(plugin_name, CallingConvention = CallingConvention.Cdecl)]
		private static extern void SetLogLevel_csharp (T_LOG_PRIORITY pri);


        //私有变量
        private IntPtr handle_;
        private IntPtr events_ptr_;
        private IntPtr queue_ptr_;

        public TGCPAPI()
        {
            handle_ = IntPtr.Zero;
            events_ptr_ = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(TGCPEvent)));
            queue_ptr_ = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(QueueNotify)));
        }

        ~TGCPAPI()
        {
            if (IntPtr.Zero != handle_)
            {
              Destroy();
            }

            Marshal.FreeHGlobal(events_ptr_);
            Marshal.FreeHGlobal(queue_ptr_);
        }


        public int Create()
        {
            return tgcpapi_csharp_create(ref handle_);
        }

        public int Destroy()
        {
            int ret = tgcpapi_csharp_destroy(ref handle_);
            if (0 == ret)
            {
                handle_ = IntPtr.Zero;
            }

            return ret;
        }

        public int Init(int service_id, ePlatformID platform_id, UInt16 auth_type, eEncryptMethod enc_method, eKeyMaking key_mode, int buff_len)
        {
            return tgcpapi_csharp_init(handle_, service_id, platform_id, auth_type, enc_method, key_mode, buff_len);
        }

        public int Fini()
        {
            return tgcpapi_csharp_fini(handle_);
        }

        //end

        public int GetSocket(ref int fd)
        {
            return tgcpapi_csharp_get_socket(handle_, ref fd);
        }

        public int SetAppID(byte[] app_id, int len)
        {
            return tgcpapi_csharp_set_appid(handle_, app_id, len);
        }

        public int SetToken(byte[] token, int len)
        {
            return tgcpapi_csharp_set_token(handle_, token, len);
        }

        public int SetAccessTokenExpire(ushort expire)
        {
            return tgcpapi_csharp_set_acesstoken_expire(handle_, expire);
        }

        public int GetAccessToken(byte[] token, ref int len)
        {
            return tgcpapi_csharp_get_acesstoken(handle_, token, ref len);
        }

        public int GetRefreshToken(byte[] token, ref int len)
        {
            return tgcpapi_csharp_get_refreshtoken(handle_, token, ref len);
        }

        public int SetOpenIDAccount(eAccountType type, string openid)
        {
            return tgcpapi_csharp_set_openid_account(handle_, type, openid);
        }

        public int SetOpenUidAccount(eAccountType type, eAccountFormat format, UInt64 uid)
        {
            return tgcpapi_csharp_set_uid_account(handle_, type, format, uid);
        }

        public int GetMapid(ref UInt64 mapid)
        {
            return tgcpapi_csharp_get_mapid(handle_, ref mapid);
        }

        public int GetOpenID(byte[] open_id, ref int len)
        {
            return tgcpapi_csharp_get_openid(handle_, open_id, ref len);
        }

        public int SetDHInfo(string dh_p_hex)
        {
            return tgcpapi_csharp_set_dh_info(handle_, dh_p_hex);
        }

        public int StartConnection(string url, int timeout)
        {
            return tgcpapi_csharp_start_connection(handle_, url, timeout);
        }

        public int CloseConnection()
        {
            return tgcpapi_csharp_close_connection(handle_);
        }

        public int RelayConnection(string url, int timeout)
        {
            return tgcpapi_csharp_relay_connection(handle_, url, timeout);
        }



        //异步模式
        public int Start(string url)
        {
            return tgcpapi_csharp_start(handle_, url);
        }

        public int Update(ref TGCPEvent events)
        {
            int ret = tgcpapi_csharp_update(handle_, events_ptr_);
            events = (TGCPEvent)Marshal.PtrToStructure(events_ptr_, typeof(TGCPEvent));
            return ret;
        }

        public int Resume(string url)
        {
            return tgcpapi_csharp_resume(handle_, url);
        }

        public int Stop()
        {
            return tgcpapi_csharp_stop(handle_);
        }

        public QueueNotify GetQueue()
        {
            return (QueueNotify)Marshal.PtrToStructure(tgcpapi_csharp_get_queue(handle_), typeof(QueueNotify));
        }

        public int GetServerStopReason()
        {
            return tgcpapi_csharp_get_sstop_reason(handle_);
        }

        public int RecvedPkgLen()
        {
            return tgcpapi_csharp_recved_pkglen(handle_);
        }

        public int SendableBufLen()
        {
            return tgcpapi_csharp_sendable_buflen(handle_);
        }

        public int Send(byte[] buff, int size, int timeout)
        {
            return tgcpapi_csharp_send(handle_, buff, size, timeout);
        }

        public int Flush()
        {
            return tgcpapi_csharp_flush(handle_);
        }

        public int Recv(byte[] buff, ref int size, int timeout)
        {
            return tgcpapi_csharp_recv(handle_, buff, ref size, timeout);
        }

        //c#不提供peek操作

        public int StopSession(int timeout)
        {
            return tgcpapi_csharp_stop_session(handle_, timeout);
        }

        public int QueryStopped(ref int result, ref int reason)
        {
            return tgcpapi_csharp_query_stopped(handle_, ref result, ref reason);
        }

        public int QueryWaitNotify(ref int bingo, ref QueueNotify notify, int timeout)
        {
            int ret = tgcpapi_csharp_query_wait_notify(handle_, ref bingo, queue_ptr_, timeout);
            notify = (QueueNotify)Marshal.PtrToStructure(queue_ptr_, typeof(QueueNotify));
            return ret;
        }

        public string GetTdrLastErrorString()
        {
            return Marshal.PtrToStringAnsi(tgcpapi_csharp_get_tdr_last_error_string(handle_));
        }

        public int GetLastUnexpectCmd()
        {
            return tgcpapi_csharp_get_last_unexpected_cmd(handle_);
        }

        public int RefreshAccessToken(int timeout)
        {
            return tgcpapi_csharp_refresh_acesstoken(handle_, timeout);
        }

		public int NeedUserReauth()
		{
			return tgcpapi_csharp_need_user_reauth(handle_);
		}

        //ex接口不提供

        //tgcpapi_error.h
        public static string ErrorString(int error_code)
        {
          return Marshal.PtrToStringAnsi(tgcpapi_csharp_error_string(error_code));
        }


        //debug接口
        public static string CallTest()
        {
            return Marshal.PtrToStringAnsi(tgcpapi_csharp_test());
        }

        #if UNITY_STANDALONE_WIN
        //for windows
        public int WSAStartUp(int maj, int min)
        {
            return tgcpapi_csharp_WSA_startup(maj, min);
        }

        public int WSACleanUp()
        {
            return tgcpapi_csharp_WSA_cleanup();
        }
        #endif	


		public void SetLogLevel(T_LOG_PRIORITY pri)
		{
			SetLogLevel_csharp(pri);
		}


		public int create_and_init(int service_id, byte[] app_id, int len, int buff_len, eAccountType type, UInt64 uid, byte[] atk, int atk_len)
		{
			return tgcpapi_csharp_create_and_init (ref handle_, service_id, app_id, len, buff_len, type, uid, atk, atk_len);
		}
		
		public int create_and_init_string(int service_id, byte[] app_id, int len, int buff_len, eAccountType type, string openid, byte[] atk, int atk_len)
		{
			return tgcpapi_csharp_create_and_init_string (ref handle_, service_id, app_id, len, buff_len, type, openid, atk, atk_len);
		}

		public int set_security_info(eEncryptMethod enc_method, eKeyMaking key_mode,string dh_p_hex)
		{
			return tgcpapi_csharp_set_security_info (handle_, enc_method, key_mode, dh_p_hex);

		}

		public int set_authtype(eAuthType auth_type)
		{

			return tgcpapi_csharp_set_authtype (handle_, auth_type);
		}

		public int set_refresh_token(byte[] token, int len, int expire)
		{
			return tgcpapi_csharp_set_refresh_token(handle_, token, len, expire);

		}
		
		public int set_refresh_token_expire(int expire)
		{

			return tgcpapi_csharp_set_refresh_token_expire(handle_, expire);
		}
    }
}
