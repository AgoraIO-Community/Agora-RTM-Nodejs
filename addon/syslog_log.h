// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// base logging utilities
// TODO(liuyong): A more detailed description is required.

#pragma once  // NOLINT(build/header_guard)

#include <inttypes.h>
#include <syslog.h>

#include <cassert>
#include <cstdarg>
#include <cstdint>

namespace agora {
namespace logging {

enum Severity {
  kDebugLog = LOG_DEBUG,  // 7 debug-level messages
  kInfoLog = LOG_INFO,  // 6 informational
  kNoticeLog = LOG_NOTICE,  // 5 normal but significant condition
  kWarnLog = LOG_WARNING,  // 4 warning conditions
  kErrorLog = LOG_ERR,  // 3 error conditions
  kFatalLog = LOG_CRIT,  // 2 critical conditions
};

void EnableDebugLogging(bool enabled);
bool SetLoggingStreak(uint32_t streak);
bool IsLoggingEnabled(Severity level);

inline void OpenLog() {
  ::openlog(nullptr, LOG_PID | LOG_NDELAY, LOG_USER | LOG_DAEMON);
}

inline void Log(Severity level, const char* format, ...) {
  if (!IsLoggingEnabled(level)) {
    return;
  }

  va_list args;
  va_start(args, format);
  ::vsyslog(level, format, args);
  va_end(args);
}

class LogGuard {
 public:
  LogGuard();
  ~LogGuard();

 private:
  LogGuard(const LogGuard &) = delete;
  LogGuard(LogGuard &&) = delete;

  LogGuard& operator=(const LogGuard &) = delete;
  LogGuard& operator=(LogGuard &&) = delete;
};

}  // namespace logging
}  // namespace agora

#define DO_LOG2(level, fmt, ...) agora::logging::Log( \
    agora::logging::k##level##Log, \
    "%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_IF2(level, cond, ...) \
  if (cond) { \
    DO_LOG2(level,  __VA_ARGS__); \
  }

#define LOG2(level, ...) LOG_IF2(level, agora::logging::IsLoggingEnabled( \
    agora::logging::k##level##Log), __VA_ARGS__)

#define LOG_FAST2 LOG2

#define LOG_EVERY_N2(level, N, ...) \
  {  \
    static unsigned int count = 0; \
    if (++count % N == 0) \
      LOG2(level, __VA_ARGS__); \
  }

//#include "safe_log.h"
