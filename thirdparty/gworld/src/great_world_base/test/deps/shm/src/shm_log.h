//
// Created by davidppwang on 2020/11/19.
//

#pragma once

#include <errno.h>
#include <string.h>
#include <functional>


enum class ShmLogLevel {
    LL_TRACE = 0, LL_DEBUG, LL_INFO, LL_WARN, LL_ERROR, LL_FATAL
};

void shm_logging_set_level(ShmLogLevel ll);

// set print fucntion, default print to stderr
// buf is 'pure' log message (no time & level)
// size  if size == -1, should use strlen(buf) to calculate string length
void shm_logging_set_printer(const std::function<void(
        ShmLogLevel ll, const char *buf, size_t size)> &printer);

void shm_logging_log(ShmLogLevel ll, const char *fmt, ...)
#ifdef __GNUC__
__attribute__((__format__(__printf__, 2, 3)))
#endif
;  // NOLINT

#ifdef _MSC_VER
// VC
#define SHMLOG_TRACE(fmt, ...) shm_logging_log(ShmLogLevel::LL_TRACE, \
                                                       "[%s:%d:%s]" fmt, \
                                                       __FILE__, __LINE__, \
                                                       __FUNCTION__,  __VA_ARGS__)

#define SHMLOG_DEBUG(fmt, ...) shm_logging_log(ShmLogLevel::LL_DEBUG, \
                                                       "[%s:%d:%s]" fmt, \
                                                       __FILE__, __LINE__, \
                                                       __FUNCTION__,  __VA_ARGS__)

#define SHMLOG_INFO(fmt, ...) shm_logging_log(ShmLogLevel::LL_INFO, \
                                                      "[%s:%d:%s]" fmt, \
                                                      __FILE__, __LINE__, \
                                                      __FUNCTION__, __VA_ARGS__)

#define SHMLOG_WARN(fmt, ...) shm_logging_log(ShmLogLevel::LL_WARN, \
                                                      "[%s:%d:%s]" fmt, \
                                                      __FILE__, __LINE__, \
                                                      __FUNCTION__, __VA_ARGS__)

#define SHMLOG_ERROR(fmt, ...) shm_logging_log(ShmLogLevel::LL_ERROR, \
                                                       "[%s:%d:%s]" fmt, \
                                                       __FILE__, __LINE__, \
                                                       __FUNCTION__, __VA_ARGS__)

#define SHMLOG_FATAL(fmt, ...) shm_logging_log(ShmLogLevel::LL_FATAL, \
                                                       "[%s:%d:%s]" fmt, \
                                                       __FILE__, __LINE__, \
                                                       __FUNCTION__, __VA_ARGS__)
#else
// GCC
#define SHMLOG_TRACE(fmt, args...) shm_logging_log(ShmLogLevel::LL_TRACE, \
                                                           "[%s:%d:%s]" fmt, \
                                                           __FILE__, __LINE__, \
                                                           __FUNCTION__, ##args)

#define SHMLOG_DEBUG(fmt, args...) shm_logging_log(ShmLogLevel::LL_DEBUG, \
                                                           "[%s:%d:%s]" fmt, \
                                                           __FILE__, __LINE__, \
                                                           __FUNCTION__, ##args)

#define SHMLOG_INFO(fmt, args...) shm_logging_log(ShmLogLevel::LL_INFO, \
                                                          "[%s:%d:%s]" fmt, \
                                                          __FILE__, __LINE__, \
                                                          __FUNCTION__, ##args)

#define SHMLOG_WARN(fmt, args...) shm_logging_log(ShmLogLevel::LL_WARN, \
                                                          "[%s:%d:%s]" fmt, \
                                                          __FILE__, __LINE__, \
                                                          __FUNCTION__, ##args)

#define SHMLOG_ERROR(fmt, args...) shm_logging_log(ShmLogLevel::LL_ERROR, \
                                                           "[%s:%d:%s]" fmt, \
                                                           __FILE__, __LINE__, \
                                                           __FUNCTION__, ##args)

#define SHMLOG_FATAL(fmt, args...) shm_logging_log(ShmLogLevel::LL_FATAL, \
                                                           "[%s:%d:%s]" fmt, \
                                                           __FILE__, __LINE__, \
                                                           __FUNCTION__, ##args)
#endif  // __MSVC__



