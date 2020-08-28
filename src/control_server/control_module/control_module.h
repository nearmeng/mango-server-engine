#ifndef  _CONTROL_MODULE_H_
#define  _CONTROL_MODULE_H_

#include "define/str_def.h"
#include "module/server_module.h"

typedef BOOL(*CONTROL_HANDLE_PTR)(std::string& sCmdType, int32_t nTargetType, uint64_t qwTarget, std::string& sCmdStr);

class CControlModule : public CServerModule
{
public:
    CControlModule() {};
	virtual ~CControlModule() {};

	virtual BOOL init(BOOL bResume);
	virtual BOOL uninit(void);

    const char* help_info(void);
    BOOL proc_cmdline(unsigned short argc, const char** argv);

private:
    BOOL _do_process();
	BOOL _parse_command(std::string& sCmdType, int32_t& nTargetType, uint64_t& qwTarget, std::string& sCmdStr);
	BOOL _proc_command(std::string& sCmdType, int32_t nTargetType, uint64_t qwTarget, std::string& sCmdStr);

    BOOL _msg_handler_init(void);
    BOOL _msg_handler_uninit(void);

private:
    char m_szCmdBuff[MAX_GM_COMMAND_LEN];
};

#endif   /* ----- #ifndef _CONTROL_MODULE_H_  ----- */
