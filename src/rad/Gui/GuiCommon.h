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

#include <format>
#include <source_location>
#include <stdexcept>

namespace rad
{

spdlog::logger* GetGuiLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_GUI(LogLevel, ...)                                                                 \
    SPDLOG_LOGGER_CALL(::rad::GetGuiLogger(), spdlog::level::LogLevel, __VA_ARGS__)

inline bool SdlCheck(bool result, const char* expr,
                     std::source_location sourceLoc = std::source_location::current())
{
    if (!result)
    {
        RAD_LOG_GUI(err, "{} failed: {} (at {}:{} in {})", expr, SDL_GetError(),
                    sourceLoc.file_name(), sourceLoc.line(), sourceLoc.function_name());
    }
    return result;
}

inline bool SdlCheckThrow(bool result, const char* expr,
                          std::source_location sourceLoc = std::source_location::current())
{
    if (!result)
    {
        RAD_LOG_GUI(err, "{} failed: {} (at {}:{} in {})", expr, SDL_GetError(),
                    sourceLoc.file_name(), sourceLoc.line(), sourceLoc.function_name());
        throw std::runtime_error(std::format("{} failed: {}", expr, SDL_GetError()));
    }
    return result;
}

// Check an SDL function call that returns bool, log SDL_GetError() on failure.
#define SDL_CHECK(expr) ::rad::SdlCheck((expr), #expr)
// Check an SDL function call that returns bool, log SDL_GetError() and throw on failure.
#define SDL_CHECK_THROW(expr) ::rad::SdlCheckThrow((expr), #expr)

} // namespace rad
