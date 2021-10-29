#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "shm_log.h"

#define MAX_LOG_BUF_SIZE 4096

static void PrintLog(ShmLogLevel ll, const char *buf, size_t /* size */) {
    const char *ll_strs[] = {"trace", "debug", "info", "warn", "error", "fatal"};

    char tmbuf[32] = {0};
    time_t now;
    now = time(NULL);
#if defined(WIN32) || defined(WIN64)
    struct tm *ptm = localtime(&now);  // NOLINT
#else
    struct tm tm;
    struct tm *ptm = &tm;
    localtime_r(&now, ptm);
#endif
    snprintf(tmbuf, sizeof(tmbuf), "[%04d%02d%02d %02d:%02d:%02d]", ptm->tm_year + 1900,
             ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    fprintf(stderr, "%s [%s] %s\n", tmbuf, ll_strs[static_cast<int>(ll)], buf);
}

static ShmLogLevel g_ll = ShmLogLevel::LL_DEBUG;
static std::function<void(ShmLogLevel, const char *, size_t)> g_printer = PrintLog;

void shm_logging_set_level(ShmLogLevel ll) { g_ll = ll; }

void shm_logging_set_printer(const std::function<void(ShmLogLevel, const char *, size_t)> &printer) {
g_printer = printer;
}

void shm_logging_log(ShmLogLevel ll, const char *fmt, ...) {
    if (ll < g_ll) return;

    char buf[MAX_LOG_BUF_SIZE] = {0};
    va_list va;
    va_start(va, fmt); /*lint !e530 */
    size_t size = vsnprintf(buf, sizeof(buf) - 1, fmt, va);
    va_end(va);

    if (g_printer) {
        g_printer(ll, buf, size);
    }
}