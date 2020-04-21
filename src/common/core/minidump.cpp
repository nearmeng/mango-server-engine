#ifdef __linux__
#include "stdafx.h"
#include "core/minidump.h"

#include <execinfo.h>                // for backtrace, backtrace_symbols
#include <libgen.h>                  // for basename
#include <stdio.h>                   // for fprintf, FILE, sprintf, fclose
#include <stdlib.h>                  // for exit
#include <string.h>                  // for memset
#include <time.h>                    // for tm, localtime, time, time_t
#include <unistd.h>                  // for pid_t, getpid, readlink
#include <errno.h>
#include <signal.h>

#include "define/version.h"

struct REG_SET
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
};

// 这个不能用函数，会改变bp/sp
#define RECORD_REGISTERS(reg)\
do {\
    asm ("movq %%rax, %0;" : "=m"(reg.rax));\
    asm ("movq %%rbx, %0;" : "=m"(reg.rbx));\
    asm ("movq %%rcx, %0;" : "=m"(reg.rcx));\
    asm ("movq %%rdx, %0;" : "=m"(reg.rdx));\
    asm ("movq %%rsi, %0;" : "=m"(reg.rsi));\
    asm ("movq %%rdi, %0;" : "=m"(reg.rdi));\
    asm ("movq %%rbp, %0;" : "=m"(reg.rbp));\
    asm ("movq %%rsp, %0;" : "=m"(reg.rsp));\
} while(0)\

static void dump_registers(FILE *fp_dump, const REG_SET *reg)
{
    fprintf(fp_dump, "[registers]\n");
    fprintf(fp_dump, "rax %016lx\n", reg->rax);
    fprintf(fp_dump, "rbx %016lx\n", reg->rbx);
    fprintf(fp_dump, "rcx %016lx\n", reg->rcx);
    fprintf(fp_dump, "rdx %016lx\n", reg->rdx);
    fprintf(fp_dump, "rsi %016lx\n", reg->rsi);
    fprintf(fp_dump, "rdi %016lx\n", reg->rdi);
    fprintf(fp_dump, "rbp %016lx\n", reg->rbp);
    fprintf(fp_dump, "rsp %016lx\n", reg->rsp);
    fprintf(fp_dump, "\n");
}

static void dump_proc_section(FILE *fp_dump, pid_t pid, const char *section_name)
{
    fprintf(fp_dump, "[%s]\n", section_name);
    char path[128];
    sprintf(path, "/proc/%d/%s", pid, section_name);
 
    FILE *fp_proc = fopen(path, "r");
    if (fp_proc == NULL)
    {
        fprintf(fp_dump, "read section %s failed", section_name);
        fprintf(fp_dump, "\n");
        return;
    }
    
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    size_t num_read = fread(buff, 1, sizeof(buff)-1, fp_proc);
    fwrite(buff, 1, num_read, fp_dump);

    if ((num_read > 0) && (buff[num_read - 1] != '\n'))
    {
        fprintf(fp_dump, "\n\n");
    }
    else
    {
        fprintf(fp_dump, "\n");
    }
    fclose(fp_proc);
}

static void dump_backtrace(FILE *fp_dump)
{
    fprintf(fp_dump, "[backtrace]\n");
    void *array[16];
    size_t size = backtrace(array, 16);
    char **strings = backtrace_symbols(array, size);
	if(strings == NULL)
	{
		CRI("backtrace_symbols failed");
		return;
	}
    
    for (size_t i=0; i<size; ++i)
    {
        fprintf(fp_dump, "%s\n", strings[i]);
    }
    fprintf(fp_dump, "\n");

	free(strings);
}

FILE* create_minidump_file(pid_t pid)
{
    char path[256];
    sprintf(path, "/proc/%d/exe", pid);
    
    char buff[512];
    memset(buff, 0, sizeof(buff));
    readlink(path, buff, sizeof(buff));    
    
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
	if (local == NULL)
		return NULL;

	char dir[128];
	sprintf(dir, "log/core_dump/");
    
    sprintf(path, "log/core_dump/%s_%d_%04d%02d%02d_%02d%02d%02d_%d.minidump"
        , basename(buff)
		, pid
        , local->tm_year + 1900
        , local->tm_mon + 1
        , local->tm_mday
        , local->tm_hour
        , local->tm_min
        , local->tm_sec
		, VERSION_NUMBER);

	if (access(dir, 0) != 0)
	{
		int ret = mkdir(dir, S_IRWXU | S_IRWXG);
		if (ret != 0)
		{
			return NULL;
		}
	}
    return fopen(path, "w+");
}

void minidump_handle(int pid) 
{
    // 注意: 寄存器的保留必须在所有代码之前
    //     因为任何逻辑代码都可能改写寄存器
    REG_SET reg;
    RECORD_REGISTERS(reg);

    FILE *fp_dump = create_minidump_file(pid);
    if (fp_dump == NULL)
    {
		CRI("failed to create minidump file");
        return;
    }

	fprintf(fp_dump, "mini dump by signal \n\n");
    dump_proc_section(fp_dump, pid, "cmdline");
    dump_proc_section(fp_dump, pid, "stat");
    dump_proc_section(fp_dump, pid, "statm");
    dump_backtrace(fp_dump);
    dump_registers(fp_dump, &reg);
    fclose(fp_dump);
}
#endif
