#include "stdafx.h"
#include "robot_interact.h"

#include "windows.h"
#include "process.h"

#include "message/client_message_handler.h"

CRobotInteractMgr CRobotInteractMgr::ms_Instance;
char   g_nbstdin_buffer[2][1024];
HANDLE g_input[2];
HANDLE g_process[2];

BOOL CRobotInteractMgr::init()
{
	int32_t nRetCode = 0;

	m_InteractUser = NULL;
	m_pScript = NULL;
	
	nRetCode = _create_thread();
	LOG_PROCESS_ERROR(nRetCode);

	m_pScript = CScriptMgr::instance().new_script("robot/main.lua");
	LOG_PROCESS_ERROR(m_pScript);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotInteractMgr::uninit()
{
	int32_t nRetCode = 0;

	nRetCode = CScriptMgr::instance().del_script(m_pScript);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _destroy_thread();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

void CRobotInteractMgr::mainloop()
{
	int32_t nRetCode = 0;
	const char* pReadData = NULL;

	pReadData = read_data();
	if (pReadData)
	{
		INF("read user input %s", pReadData);
		_process_user_input(pReadData);
	}

Exit0:
	return;
}
	
DWORD WINAPI console_input(LPVOID lpParameter)
{
	for (;;) {
		int i;
		for (i = 0; i < 2; i++) {
			fgets(g_nbstdin_buffer[i], 1024, stdin);
			SetEvent(g_input[i]);
			WaitForSingleObject(g_process[i], INFINITE);
		}
	}
	return 0;
}

BOOL CRobotInteractMgr::_create_thread(void)
{
	int32_t nRetCode = 0;
	DWORD tid;

	CreateThread(NULL, 1024, &console_input, 0, 0, &tid);
	for (int32_t i = 0; i < 2; i++) 
	{
		g_input[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		g_process[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		g_nbstdin_buffer[i][0] = '\0';
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CRobotInteractMgr::_destroy_thread(void)
{
	return TRUE;
}
	
const char* CRobotInteractMgr::read_data(void)
{
	DWORD n = WaitForMultipleObjects(2, g_input, FALSE, 0);
	if (n == WAIT_OBJECT_0 || n == WAIT_OBJECT_0 + 1) 
	{
		n = n - WAIT_OBJECT_0;
		SetEvent(g_process[n]);
		return g_nbstdin_buffer[n];
	}
	else 
	{
		return NULL;
	}
}

void CRobotInteractMgr::_process_user_input(const char* pInput)
{
	int32_t nRetCode = 0;

	if (pInput[0] == '.')
	{
		if (m_InteractUser == NULL)
		{
			CRI("execute server gm, but user was not created");
			return;
		}

		nRetCode = CClientMessageHandler::instance().do_gm_command(pInput + 1, m_InteractUser);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		if (m_InteractUser)
		{
			tolua_pushusertype(m_pScript->get_lua_state(), m_InteractUser, "CRobotUser");
			lua_setglobal(m_pScript->get_lua_state(), "user");
		}
		else
		{
			lua_pushnil(m_pScript->get_lua_state());
			lua_setglobal(m_pScript->get_lua_state(), "user");
		}

		nRetCode = m_pScript->call_function("gmcommand", "s", pInput);
		LOG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return;
}