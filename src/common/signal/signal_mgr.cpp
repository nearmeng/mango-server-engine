#include "stdafx.h"
#include <signal.h>
#include "signal/signal_mgr.h"

CSignalMgr CSignalMgr::ms_Instance;

BOOL CSignalMgr::init(void)
{
	m_handler_map.clear();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSignalMgr::uninit(void)
{
	m_handler_map.clear();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSignalMgr::reg_sig_handler(int32_t nSignal, SIGNAL_HANDLE_FUNC pFunc)
{
	int32_t nRetCode = 0;
	SIGNAL_HANDLER* pHandler = NULL;

	std::map<int32_t, SIGNAL_HANDLER>::iterator it = m_handler_map.find(nSignal);
	LOG_PROCESS_ERROR(it == m_handler_map.end());

	pHandler = &(m_handler_map[nSignal]);
	LOG_PROCESS_ERROR(pHandler);

	pHandler->nSignal = nSignal;
	pHandler->pFunc = pFunc;
	pHandler->bHasToDo = FALSE;

	nRetCode = _reg_sig(nSignal);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	CRI("reg_sig_handler failed, signal %d", nSignal);
	return FALSE;
}

void CSignalMgr::mainloop()
{
	for (std::map<int32_t, SIGNAL_HANDLER>::iterator it = m_handler_map.begin(); it != m_handler_map.end(); ++it)
	{
		SIGNAL_HANDLER &handler = it->second;
		if (!handler.bHasToDo)
			continue;

		if (handler.pFunc == NULL)
			continue;

		handler.pFunc();
		handler.bHasToDo = FALSE;
	}
}

void CSignalMgr::common_signal_handle_func(int32_t nSignal)
{
	SIGNAL_HANDLER* pSignalHander = NULL;

	//INF("recv signal [%d]", nSignal);

	pSignalHander = CSignalMgr::get_instance().get_signal_handler(nSignal);
	LOG_PROCESS_ERROR(pSignalHander);

	pSignalHander->bHasToDo = TRUE;

Exit0:
	return;
}

int32_t CSignalMgr::_reg_sig(int32_t nSignal)
{
	int32_t nRetCode = 0;

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = common_signal_handle_func;

	nRetCode = sigaction(nSignal, &act, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

