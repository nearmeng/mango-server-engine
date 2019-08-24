#ifndef SIGNAL_MGR_H
#define SIGNAL_MGR_H

#include <map>

typedef void (*SIGNAL_HANDLE_FUNC)(void);

struct SIGNAL_HANDLER
{
	SIGNAL_HANDLER()
	{
		nSignal = 0;
		pFunc = NULL;
		bHasToDo = FALSE;
	}
	int32_t				nSignal;
	SIGNAL_HANDLE_FUNC	pFunc;
	BOOL				bHasToDo;
};

class CSignalMgr
{
public:
	CSignalMgr() {};
	~CSignalMgr() {};

	inline static CSignalMgr& get_instance(void);
	BOOL init(void);
	BOOL uninit(void);
	void mainloop();

	BOOL reg_sig_handler(int32_t nSignal, SIGNAL_HANDLE_FUNC pFunc);
	inline SIGNAL_HANDLER* get_signal_handler(int32_t nSignal);

private:
	static void common_signal_handle_func(int32_t nSignal);
	BOOL _reg_sig(int32_t nSignal);

	std::map<int32_t, SIGNAL_HANDLER> 	m_handler_map;
	static CSignalMgr ms_Instance;
};

inline CSignalMgr& CSignalMgr::get_instance(void)
{
	return ms_Instance;
}

inline SIGNAL_HANDLER* CSignalMgr::get_signal_handler(int32_t nSignal)
{
	std::map<int32_t, SIGNAL_HANDLER>::iterator it = m_handler_map.find(nSignal);
	LOG_PROCESS_ERROR(it != m_handler_map.end());

	return &(it->second);
Exit0:
	return NULL;
}

#endif

