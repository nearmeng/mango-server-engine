#ifndef _ROBOT_INTERACT_
#define _ROBOT_INTERACT_

#include "robot_user.h"

class CRobotInteractMgr
{
public:
	CRobotInteractMgr() {};
	~CRobotInteractMgr() {};

	BOOL init();
	BOOL uninit();
	inline static CRobotInteractMgr& instance();

	inline void set_interact_user(CRobotUser* pUser);
	inline CRobotUser* get_interact_user(void);

	void mainloop(void);
	const char* read_data(void);

private:
	BOOL _create_thread(void);
	BOOL _destroy_thread(void);

	void _process_user_input(const char* pInput);

private:
	static CRobotInteractMgr ms_Instance;
	
	CRobotUser* m_InteractUser;
	CLuaScript* m_pScript;
};

inline CRobotInteractMgr& CRobotInteractMgr::instance()
{
	return ms_Instance;
}

inline void CRobotInteractMgr::set_interact_user(CRobotUser* pUser)
{
	m_InteractUser = pUser;
}

inline CRobotUser* CRobotInteractMgr::get_interact_user(void)
{
	return m_InteractUser;
}

#endif