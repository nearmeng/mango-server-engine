#include "stdafx.h"
#include "core/core_dumper.h"
#include "time/time_mgr.h"

#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "define/server_def.h"
#include "define/version.h"
#include "config/global_config.h"

#ifdef __linux__
#include "core/minidump.h"
#include <unistd.h>
#include <sys/statfs.h>

#else
#include <windows.h>
#include <stdlib.h>
#include <DbgHelp.h>
#include <eh.h>
#include <psapi.h>

#pragma comment(lib, "Dbghelp.lib") 
#endif

#define SEGV_STACK_SIZE			(1024 * 1024)
#define MAX_SIGJMP_BUF_COUNT	(100)
#define CORE_FILTER_MASK_LEN     (10)
#define FREE_DISK_THRES1         (64 * 1024 * 1024)
#define FREE_DISK_THRES2         (32 * 1024 * 1024) 

enum CORE_DUMPER_STATUS
{
	cdsInvaid = 0,

	cdsInit = 1,
	cdsDumping = 2,
	cdsFinished = 3,

	csdTotal
};

static const char szDefaultFilterMask[CORE_FILTER_MASK_LEN] = "0x3f";
static const char szMiniFilterMask[CORE_FILTER_MASK_LEN] = "0x17";
static int32_t g_nDumperStatus = 0;

#ifdef __linux__
extern char *__progname;

struct LOCAL_CORE_DUMPER_MGR
{
	struct sigaltstack sigstk;
	struct sigaction sa;
	sigjmp_buf stBuf[MAX_SIGJMP_BUF_COUNT];
	int32_t nBufCount;
	int32_t nDumpCount;
	uint64_t qwLastDumpTime;
	int32_t nStatus;

	int32_t	nPid;
	uint64_t qwCurrTick;
	char    szFilterName[128];
	struct statfs stDiskInfo;  

	int32_t nSig;
	siginfo_t* pInfo;
};

static LOCAL_CORE_DUMPER_MGR g_stDumperMgr;

void sigseg_handler(int sig, siginfo_t *info, void *data)
{
	if (g_stDumperMgr.nStatus == cdsFinished)
		return;
	
	PROCESS_ERROR(g_stDumperMgr.nStatus == cdsInit);

	g_stDumperMgr.qwCurrTick = CTimeMgr::instance().get_server_tick();
	PROCESS_ERROR(g_stDumperMgr.qwLastDumpTime + g_ServerConfig.Common.DumpConfig.nDumpInterval < g_stDumperMgr.qwCurrTick);

	g_stDumperMgr.nDumpCount++;
	g_stDumperMgr.nStatus = cdsDumping;
	g_stDumperMgr.qwLastDumpTime = g_stDumperMgr.qwCurrTick;
	g_stDumperMgr.nSig = sig;
	g_stDumperMgr.pInfo = info;

    if(fork() == 0) 
	{
		statfs("/data/corefile", &g_stDumperMgr.stDiskInfo);  

		uint64_t freeDisk = (uint64_t)(g_stDumperMgr.stDiskInfo.f_bavail * g_stDumperMgr.stDiskInfo.f_bsize / 1024);
		if (g_ServerConfig.Common.DumpConfig.bIsMiniDump || (freeDisk < FREE_DISK_THRES1 && freeDisk > FREE_DISK_THRES2))
		{
			snprintf(g_stDumperMgr.szFilterName, sizeof(g_stDumperMgr.szFilterName), "/proc/%d/coredump_filter", getpid());

			int fd = open(g_stDumperMgr.szFilterName, O_WRONLY);
			if (fd != -1)
			{
				int size = write(fd, szMiniFilterMask, sizeof(szMiniFilterMask));
				close(fd);
			}
		}

		//INF("capture signal, sig %d, addr %p freeDisk %llu", sig, info->si_addr, freeDisk);

		if (freeDisk < FREE_DISK_THRES2)
		{
			_exit(0);
		}

		kill(getpid(), SIGILL);
    }

	g_stDumperMgr.nStatus = cdsInit;
	
Exit0:
	if(g_stDumperMgr.nStatus == cdsInit && g_stDumperMgr.nBufCount> 0 && g_stDumperMgr.nBufCount <= MAX_SIGJMP_BUF_COUNT)
	    siglongjmp(g_stDumperMgr.stBuf[g_stDumperMgr.nBufCount - 1], 1);

	return;
}

static BOOL init_stack()
{
	struct sigaltstack& sigstk = g_stDumperMgr.sigstk;
	sigstk.ss_sp = valloc(SEGV_STACK_SIZE);
	sigstk.ss_flags = 0;
	sigstk.ss_size = SEGV_STACK_SIZE;
	sigaltstack(&sigstk, NULL);

	return TRUE;
}

static BOOL init_core_filter()
{
	int32_t fd = 0;
	int32_t nSize = 0;
	char szCoreFilter[64];

	snprintf(szCoreFilter, sizeof(szCoreFilter), "/proc/%d/coredump_filter", getpid());

	fd = open(szCoreFilter, O_WRONLY);
	LOG_PROCESS_ERROR(fd != -1);
		
	nSize = write(fd, szDefaultFilterMask, sizeof(szDefaultFilterMask));
	close(fd);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL init_core_pattern()
{
	int32_t fd = 0;
	int32_t nSize = 0;
	int32_t nRetCode = 0;
	char szCorePattern[64];

	snprintf(szCorePattern, sizeof(szCorePattern), "/data/corefile/core_%%e_%%t_%d", VERSION_NUMBER);

	fd = open("/proc/sys/kernel/core_pattern", O_WRONLY);
	LOG_PROCESS_ERROR(fd != -1);
		
	nSize = write(fd, szCorePattern, sizeof(szCorePattern));
	close(fd);

	return TRUE;
Exit0:
	return FALSE;
}

static BOOL init_signal()
{
	int32_t nRetCode = 0;
	
	struct sigaction& sa = g_stDumperMgr.sa;
	bzero(&sa, sizeof(sa));
	sa.sa_flags = SA_SIGINFO | SA_STACK;
	sa.sa_sigaction = sigseg_handler;
	sigfillset(&sa.sa_mask);

	signal(SIGPIPE, SIG_IGN);
	
	nRetCode = sigaction(SIGSEGV, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = sigaction(SIGFPE, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = sigaction(SIGBUS, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = sigaction(SIGIOT, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
//	nRetCode = sigaction(SIGQUIT, &sa, NULL);
//	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGTRAP, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGXCPU, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGXFSZ, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGALRM, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGHUP, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = sigaction(SIGINT, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGPOLL, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGPROF, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGSYS, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGTERM, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	nRetCode = sigaction(SIGVTALRM, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = sigaction(SIGTSTP, &sa, NULL);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

#else

static inline BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	return FALSE;
}

void CreateMiniDump(PEXCEPTION_POINTERS pInfo, LPCTSTR sName)
{
	HANDLE hFile = CreateFile(sName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pInfo;
		mdei.ClientPointers = TRUE;

		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam = 0;

#if defined(_DEBUG)
		MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory | MiniDumpWithThreadInfo);
#else
		MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithThreadInfo);
#endif

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, mdt, (pInfo != 0) ? &mdei : 0, NULL, NULL);
		CloseHandle(hFile);
	}
}

static BOOL get_dump_path(char* pcszPath)
{
	LOG_PROCESS_ERROR(pcszPath);

	char szProcessName[128];
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));

	for (int32_t nIndex = 0; nIndex < strlen(szProcessName); ++nIndex)
	{
		if (szProcessName[nIndex] == '.')
		{
			szProcessName[nIndex] = '\0';
			break;
		}
	}

	sprintf(pcszPath, "log/core_dump/%s_%d_%s_%d.dmp"
		,szProcessName
		,GetCurrentProcessId()
		,CTimeMgr::get_time_str(time(NULL))
		,0);

	INF("get dump path is %s", pcszPath);
	return TRUE;
Exit0:
	return FALSE;
}

static LONG _stdcall MyExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (g_nDumperStatus == cdsFinished)
		return EXCEPTION_EXECUTE_HANDLER;

	if (IsDebuggerPresent())
		__debugbreak();
	else
	{
		g_nDumperStatus = cdsDumping;

		char szPath[256] = {0};
		get_dump_path(szPath);
		CreateMiniDump(pExceptionInfo, szPath);
		
		g_nDumperStatus = cdsInit;
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}

static void MyCommonHandler(void)
{
	INF("enter common_handler");
	RaiseException(EXCEPTION_EXECUTE_HANDLER, 0, 0, NULL);
	return;
}

static void myInvalidParameterHandler(const wchar_t* expression,
          const wchar_t* function, 
          const wchar_t* file, 
          unsigned int line, 
          uintptr_t pReserved)
{
	RaiseException(EXCEPTION_EXECUTE_HANDLER, 0, 0, NULL);
	return;
}

static void DisableSetUnhandlerExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibrary("kernel32.dll"), "SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

static BOOL init_win()
{
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter(MyExceptionHandler);
	DisableSetUnhandlerExceptionFilter();
	_set_purecall_handler(MyCommonHandler);
	set_terminate(MyCommonHandler);
	set_unexpected(MyCommonHandler);
	_set_invalid_parameter_handler(myInvalidParameterHandler);

	g_nDumperStatus = cdsInit;

	return TRUE;
}

int32_t seh_filter(unsigned int nCode, struct _EXCEPTION_POINTERS *pInfo)
{
	INF("coredump happen: enter seh_filter, nCode is %d", nCode);
	
	if (g_nDumperStatus == cdsFinished)
		return EXCEPTION_EXECUTE_HANDLER;

	if (nCode != EXCEPTION_STACK_OVERFLOW)
	{
		g_nDumperStatus = cdsDumping;

		char szPath[256] = {0};
		get_dump_path(szPath);
		CreateMiniDump(pInfo, szPath);

		g_nDumperStatus = cdsInit;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

static BOOL init_dir(void)
{
	char    szDir[128];
	int32_t nRetCode = 0;
	
	snprintf(szDir, sizeof(szDir), "log/core_dump/");
	if (access(szDir, 0) != 0)
	{
		nRetCode =  mkdir(szDir, S_IRWXU | S_IRWXG);	
		LOG_PROCESS_ERROR(nRetCode == 0);
	}
	return TRUE;
Exit0:
	return FALSE;
}

BOOL core_dumper_init()
{
	int32_t nRetCode = 0;
	
#ifdef __linux__
	//init stack
	nRetCode = init_stack();
	LOG_PROCESS_ERROR(nRetCode);

	//init signal
	nRetCode = init_signal();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = init_core_filter();
	LOG_PROCESS_ERROR(nRetCode);

	//nRetCode = init_core_pattern();
	//LOG_PROCESS_ERROR(nRetCode);

	g_stDumperMgr.nBufCount = 0;
	g_stDumperMgr.nDumpCount = 0;
	g_stDumperMgr.nStatus = cdsInit;
	g_stDumperMgr.qwLastDumpTime = 0;
	g_stDumperMgr.qwCurrTick = 0;
	g_stDumperMgr.nPid = getpid();

	INF("current process pid %d name %s", g_stDumperMgr.nPid, __progname);
#else
	nRetCode = init_dir();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = init_win();
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return TRUE;
Exit0:

#ifdef __linux__
	CRI("error %d", errno);
#endif	// __linux__

	return FALSE;
}

BOOL core_dumper_uninit()
{
#ifdef __linux__
	g_stDumperMgr.nStatus = cdsFinished;
#else
	g_nDumperStatus = cdsFinished;
#endif
	return TRUE;
}

#ifdef __linux__
sigjmp_buf& core_dumper_get_curr_buf()
{
	LOG_PROCESS_ERROR(g_stDumperMgr.nBufCount >=0 && g_stDumperMgr.nBufCount < MAX_SIGJMP_BUF_COUNT);
	
	return g_stDumperMgr.stBuf[g_stDumperMgr.nBufCount];

Exit0:
	return g_stDumperMgr.stBuf[MAX_SIGJMP_BUF_COUNT-1];
}

BOOL core_dumper_inc_buf_count()
{
	LOG_PROCESS_ERROR(g_stDumperMgr.nBufCount < MAX_SIGJMP_BUF_COUNT);
	g_stDumperMgr.nBufCount++;
	return TRUE;

Exit0:
	return FALSE;
}

BOOL core_dumper_dec_buf_count()
{
	LOG_PROCESS_ERROR(g_stDumperMgr.nBufCount > 0);
	g_stDumperMgr.nBufCount--;

Exit0:
	return FALSE;
}
#endif
