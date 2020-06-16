#ifndef  _CONTROL_HANDLER_H_
#define  _CONTROL_HANDLER_H_

#include "define/str_def.h"

typedef BOOL(*CONTROL_HANDLE_PTR)(std::string& sCmdType, int32_t nTargetType, uint64_t qwTarget, std::string& sCmdStr);

class CControlHandler
{
public:
    CControlHandler() {};
	virtual ~CControlHandler() {};

	BOOL init(void);
	BOOL uninit(void);
	inline static CControlHandler& instance(void);

    const char* help_info(void);
    BOOL proc_cmdline(unsigned short argc, const char** argv);

private:
    BOOL _do_process();
	BOOL _parse_command(std::string& sCmdType, int32_t& nTargetType, uint64_t& qwTarget, std::string& sCmdStr);
	BOOL _proc_command(std::string& sCmdType, int32_t nTargetType, uint64_t qwTarget, std::string& sCmdStr);

private:
    static CControlHandler      ms_Instance;

    char m_szCmdBuff[MAX_GM_COMMAND_LEN];
};

inline CControlHandler& CControlHandler::instance(void)
{
	return ms_Instance;
}

#endif   /* ----- #ifndef _CONTROL_HANDLER_H_  ----- */
