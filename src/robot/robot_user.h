#ifndef _USER_H_
#define _USER_H_

#define RECONNECT_RESET_URL_THR 5
#define MAX_RECONNECT_COUNT 10
#define MAX_USER_COUNT 10000

#include <vector>
#include "robot_conn.h"
#include "google/protobuf/message.h"

class CRobotUser
{
public:
    CRobotUser() {};
    ~CRobotUser() {};

    BOOL init(const char* pcszName);
    BOOL fini(void);

    BOOL connect(const char* pcszSvrUrl);
    BOOL disconnect(void);

    BOOL send(int32_t nMsgId, lua_State* L);
    BOOL send(int32_t nMsgID, google::protobuf::Message& Msg);
    BOOL set_lua_wait_msgid(int32_t nMsgId1, int32_t nMsgId2);
    BOOL set_local_wait_msgid(int32_t nMsgId1, int32_t nMsgId2, WAIT_MSG_CALLBACK Callback);

    inline const char* get_coname(void);
    inline void set_coname(const char* pcszCoName);
    inline void clear_coname(void);

    inline const char* get_user_name(void);
    inline void set_user_name(const char* pcszUserName);

    inline int32_t get_connid(void);
	inline ROBOT_CONNECTION* get_conn(void);
	inline void set_conn(ROBOT_CONNECTION*);

    inline int32_t get_user_id(void);
    int32_t generate_user_id(void);

    inline int32_t get_conn_count(void);
    inline void set_conn_count(int32_t nConnCount);

    inline const char* get_svr_url(void);
    inline void set_svr_url(const char* szSvrUrl);

protected:
    char m_szUserName[128];
    int32_t m_nUserID;
    int32_t m_nConnID;
	char m_szCoName[128];

    char m_szSvrUrl[128];
    int32_t m_nConnCount;
	
	ROBOT_CONNECTION* m_pConnection;
};

class CRobotUserMgr
{
public:
    CRobotUserMgr() {};
    ~CRobotUserMgr() {};

    inline static CRobotUserMgr& instance(void);

    BOOL init(void);
    BOOL fini(void);

    CRobotUser* create_user(const char* pcszUserName);
    BOOL destroy_user(CRobotUser* pUser);
    CRobotUser* find_user(int32_t nUserID);
    CRobotUser* find_user(const std::string& sUserName);
	CRobotUser* rand_user();

    void mainloop(void);

	int32_t get_user_count();

private:
    std::map<int32_t, CRobotUser*> mUsersID;
    std::map<std::string, CRobotUser*> mUsersName;

    static CRobotUserMgr        ms_Instance;
};

inline CRobotUserMgr& CRobotUserMgr::instance(void)
{
    return ms_Instance;
}

inline int32_t CRobotUser::get_user_id()
{
    return m_nUserID;
}

inline const char* CRobotUser::get_coname(void)
{
    return m_szCoName;
}

inline void CRobotUser::set_coname(const char* pcszCoName)
{
    strxcpy(m_szCoName, pcszCoName, sizeof(m_szCoName));
}

inline void CRobotUser::clear_coname(void)
{
    m_szCoName[0] = '\0';
}

inline const char* CRobotUser::get_user_name(void)
{
    return m_szUserName;
}

inline void CRobotUser::set_user_name(const char* pcszUserName)
{
    strxcpy(m_szUserName, pcszUserName, sizeof(m_szUserName));
}

inline int32_t CRobotUser::get_connid(void)
{
    return m_nConnID;
}

inline ROBOT_CONNECTION* CRobotUser::get_conn(void)
{
	return m_pConnection;
}

inline void CRobotUser::set_conn(ROBOT_CONNECTION* pConnection)
{

	m_pConnection = pConnection;
}

inline int32_t CRobotUser::get_conn_count(void)
{
    return m_nConnCount;
}

inline void CRobotUser::set_conn_count(int32_t nConnCount)
{
    this->m_nConnCount = nConnCount;
}

inline const char* CRobotUser::get_svr_url(void)
{
    return m_szSvrUrl;
}

inline void CRobotUser::set_svr_url(const char* szSvrUrl)
{
    strxcpy(this->m_szSvrUrl, szSvrUrl, sizeof(this->m_szSvrUrl));
}

#endif
