#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/Integer.h>
#include <rad/Common/Float.h>
#include <rad/Common/Flags.h>
#include <rad/Common/Memory.h>
#include <rad/Common/RefCounted.h>
#include <rad/Container/SmallVector.h>
#include <rad/Container/Span.h>
#include <rad/IO/Logging.h>

#include <SDL3/SDL.h>

namespace rad
{

spdlog::logger* GetGuiLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_GUI(LogLevel, ...)                                                                 \
    SPDLOG_LOGGER_CALL(::rad::GetGuiLogger(), spdlog::level::LogLevel, __VA_ARGS__)

} // namespace rad
