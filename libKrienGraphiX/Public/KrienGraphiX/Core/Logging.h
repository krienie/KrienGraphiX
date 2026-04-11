
#pragma once

#include <spdlog/spdlog.h>

#ifndef KGXLOG_TRACE
#define KGXLOG_TRACE SPDLOG_TRACE
#endif

#ifndef KGXLOG_DEBUG
#define KGXLOG_DEBUG SPDLOG_DEBUG
#endif

#ifndef KGXLOG_INFO
#define KGXLOG_INFO SPDLOG_INFO
#endif

#ifndef KGXLOG_WARN
#define KGXLOG_WARN SPDLOG_WARN
#endif

#ifndef KGXLOG_ERROR
#define KGXLOG_ERROR SPDLOG_ERROR
#endif

#ifndef KGXLOG_CRITICAL
#define KGXLOG_CRITICAL SPDLOG_CRITICAL
#endif

#ifndef KGXLOG_IF
#define KGXLOG_IF(level, condition, ...) \
if (condition) SPDLOG_LOGGER_CALL(spdlog::default_logger(), level, __VA_ARGS__)
#endif

#ifndef KGXLOG_TRACE_IF
#define KGXLOG_TRACE_IF(condition, ...) KGXLOG_IF(spdlog::level::trace, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_DEBUG_IF
#define KGXLOG_DEBUG_IF(condition, ...) KGXLOG_IF(spdlog::level::debug, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_INFO_IF
#define KGXLOG_INFO_IF(condition, ...) KGXLOG_IF(spdlog::level::info, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_WARN_IF
#define KGXLOG_WARN_IF(condition, ...) KGXLOG_IF(spdlog::level::warn, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_ERROR_IF
#define KGXLOG_ERROR_IF(condition, ...) KGXLOG_IF(spdlog::level::err, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_CRITICAL_IF
#define KGXLOG_CRITICAL_IF(condition, ...) KGXLOG_IF(spdlog::level::critical, condition, __VA_ARGS__)
#endif

#ifndef KGXLOG_LOG_ONCE
#define KGXLOG_LOG_ONCE(level, ...) \
static bool isLogged##__LINE__ = false; \
if (!isLogged##__LINE__) \
{ \
	isLogged##__LINE__ = true; \
	SPDLOG_LOGGER_CALL(spdlog::default_logger(), level, __VA_ARGS__); \
}
#endif

#ifndef KGXLOG_TRACE_ONCE
#define KGXLOG_TRACE_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::trace, __VA_ARGS__)
#endif

#ifndef KGXLOG_DEBUG_ONCE
#define KGXLOG_DEBUG_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::debug, __VA_ARGS__)
#endif

#ifndef KGXLOG_INFO_ONCE
#define KGXLOG_INFO_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::info, __VA_ARGS__)
#endif

#ifndef KGXLOG_WARN_ONCE
#define KGXLOG_WARN_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::warn, __VA_ARGS__)
#endif

#ifndef KGXLOG_ERROR_ONCE
#define KGXLOG_ERROR_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::err, __VA_ARGS__)
#endif

#ifndef KGXLOG_CRITICAL_ONCE
#define KGXLOG_CRITICAL_ONCE(...) KGXLOG_LOG_ONCE(spdlog::level::critical, __VA_ARGS__)
#endif



