#include "stdafx.h"
#include "control_module.h"
#include "control_message_handler.h"

#define SEPERATOR       " \t\r\n"
#define JOIN_SEP " "

MG_REGISTER_MODULE(CControlModule);

enum CONTROL_TARGET_TYPE
{
	cttInvalid,

	cttTbusAddr,
	cttRoleID,

	cttTotal
};

BOOL CControlModule::_parse_command(std::string& sCmdType, int32_t& nTargetType, uint64_t& qwTarget, std::string& sCmdStr)
{
	int32_t nRetCode = 0;
	std::vector<std::string> vSepResult;
	std::vector<std::string> vTbusSplit;

	vSepResult = strsep(m_szCmdBuff, " ");
	LOG_PROCESS_ERROR(vSepResult.size() >= 2);

	sCmdType = vSepResult[0];

	vTbusSplit = strsep(vSepResult[1], ".");
	if (vTbusSplit.size() == 4)
	{
		int32_t nTbusAddr = 0;
		nTargetType = cttTbusAddr;

		replace(vSepResult[1].begin(), vSepResult[1].end(), '*', '0');
		nRetCode = tbus_addr_aton(vSepResult[1].c_str(), &nTbusAddr);
		LOG_PROCESS_ERROR(nRetCode == 0);

		qwTarget = nTbusAddr;
	}
	else
	{
		nTargetType = cttRoleID;
		qwTarget = strtoull(vSepResult[1].c_str(), 0, 10);
		if (qwTarget > 0)
			nTargetType = cttRoleID;
		else
			nTargetType = cttInvalid;
	}

	if (vSepResult.size() > 2)
	{
		for (int32_t i = 2; i < vSepResult.size(); i++)
		{
		    sCmdStr += vSepResult[i] + " ";
		}
	}
	else
	{
		sCmdStr = "";
	}

	INF("parse command success, origin command %s, command type %s command target type %d command target %lld command str %s",
		m_szCmdBuff, sCmdType.c_str(), nTargetType, qwTarget, sCmdStr.c_str());

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CControlModule::_proc_command(std::string& sCmdType, int32_t nTargetType, uint64_t qwTarget, std::string& sCmdStr)
{
    int32_t nRetCode = 0;
    std::string strCmd;

	switch (nTargetType)
	{
		case cttTbusAddr:
		{
			nRetCode = co_msg_send_cmd_by_addr(sCmdType.c_str(), sCmdStr.c_str(), qwTarget);
			LOG_PROCESS_ERROR(nRetCode);

			break;
		}
		case cttRoleID:
		{
			nRetCode = co_msg_send_cmd_by_roleid(sCmdType.c_str(), sCmdStr.c_str(), qwTarget);
			LOG_PROCESS_ERROR(nRetCode);
		}
		default:
			LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CControlModule::init(BOOL bResume)
{
    int32_t nRetCode = 0;

    nRetCode = _msg_handler_init();
    LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    return FALSE;
}

BOOL CControlModule::uninit(void)
{
    int32_t nRetCode = 0;

    nRetCode = _msg_handler_uninit();
    LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
    return FALSE;
}

const char* CControlModule::help_info(void)
{
    static const char* pszHelp;

    pszHelp = "CENTER Command List: \n"

        "reload\t\t# reload server res config and script\n"
        "\t\t\t# format: reload [server addr]\n"
        "\t\t\t# example: reload 1.0.2.1 , Means notify the server which server addr is 1.0.2.1 to reload.\n"
        "\t\t\t# example: reload 1.0.2.* , Means notify the server which server type is 2 to reload\n"
        "\t\t\t# example: reload 1.0.*.* , Means notify all the server to reload\n"
        
		"script\t\t# functions execute gm or other script in target server\n"
		"\t\t\t# format: script [server_tbus(* means match all) or roleid(0 means server execute)] [scripts detail...] \n"
        "\t\t\t# example: script 2.0.3.* settime 3600\n"
        "\t\t\t# example: script 10001 settime 3600\n"

        ;

    return pszHelp;
}

static int32_t str_join(int32_t argc, const char** argv, const char* sep, char* pOutBuf, uint32_t dwBufSize)
{
    pOutBuf[0] = '\0';
    uint32_t dwTotalLen = 0;
    uint32_t dwSepLen = strlen(sep);

	LOG_PROCESS_ERROR(argv);
	LOG_PROCESS_ERROR(pOutBuf);
	LOG_PROCESS_ERROR(sep);
	LOG_PROCESS_ERROR(dwSepLen > 0);

    for (int32_t i = 0; i < argc; i++)
    {
		uint32_t dwCurrLen = 0;
		LOG_PROCESS_ERROR(argv[i]);
        
		dwCurrLen = strlen(argv[i]);
		LOG_PROCESS_ERROR(dwCurrLen + dwSepLen + dwTotalLen < dwBufSize);

        strcat(pOutBuf, argv[i]);
        strcat(pOutBuf, sep);
        dwTotalLen += dwCurrLen + dwSepLen;
    }

	pOutBuf[dwTotalLen - 1] = '\0';

    return TRUE;
Exit0:
	return FALSE;
}

BOOL CControlModule::proc_cmdline(unsigned short argc, const char** argv)
{
	int32_t nRetCode = 0;
	
	nRetCode = str_join(argc, argv, JOIN_SEP, m_szCmdBuff, MAX_GM_COMMAND_LEN);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _do_process();
	LOG_PROCESS_ERROR(nRetCode);

    tappctrl_send_string("execute commands success!");
	return TRUE;
Exit0:
    tappctrl_send_string("execute commands fail!");
	return FALSE;
}

BOOL CControlModule::_do_process()
{
	int32_t nRetCode = 0;
    std::string sCmdType;
	int32_t nTargetType;
	uint64_t qwTarget;
	std::string sCmdStr;

    INF("process cmd: command <%s>, command_len <%d>", m_szCmdBuff, strlen(m_szCmdBuff));

	nRetCode = _parse_command(sCmdType, nTargetType, qwTarget, sCmdStr);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _proc_command(sCmdType, nTargetType, qwTarget, sCmdStr);
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    tappctrl_send_string(help_info());
	return FALSE;
}
