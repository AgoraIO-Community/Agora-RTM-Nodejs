// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// base logging utilities

#include "syslog_log.h"

#include <syslog.h>

namespace agora {
namespace logging {

static int g_enabled_level = kInfoLog;
static uint64_t g_logging_count = 0;

static uint32_t g_logging_out_streak = 2;
static const uint32_t kDropInterval = 50000;

void EnableDebugLogging(bool enabled) {
  if (enabled) {
    g_enabled_level = kDebugLog;
  } else {
    g_enabled_level = kInfoLog;
  }
}

bool SetLoggingStreak(uint32_t streak) {
  if (streak > kDropInterval) {
    g_logging_out_streak = kDropInterval;
    return false;
  }

  g_logging_out_streak = streak;
  return true;
}

bool IsLoggingEnabled(Severity level) {
  if (level <= g_enabled_level) {
    return true;
  }

  ++g_logging_count;
  return (g_logging_count % kDropInterval < g_logging_out_streak);
}

LogGuard::LogGuard() {
  ::openlog(nullptr, LOG_PID | LOG_NDELAY, LOG_USER | LOG_DAEMON);
}

LogGuard::~LogGuard() {
  ::closelog();
}

}  // namespace logging
}  // namespace agora


