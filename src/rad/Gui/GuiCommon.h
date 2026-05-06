#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/Integer.h>
#include <rad/Common/Float.h>
#include <rad/Common/Memory.h>
#include <rad/Common/RefCounted.h>
#include <rad/Container/SmallVector.h>
#include <rad/Container/Span.h>
#include <rad/IO/Logging.h>

#include <SDL3/SDL.h>

#include <cassert>
#include <source_location>
#include <string>

namespace rad
{

spdlog::logger* GetGuiLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_GUI(LogLevel, ...)                                                                 \
    SPDLOG_LOGGER_CALL(::rad::GetGuiLogger(), spdlog::level::LogLevel, __VA_ARGS__)

// Returns a stable string literal for common SDL event types (for logging/debugging).
// If unknown, returns "SDL_EVENT_UNKNOWN".
const char* SDL_GetEventName(Uint32 type);

// Convenience wrapper that returns a description string (for logging/debugging).
std::string SDL_GetEventDescription(const SDL_Event& event);

inline bool SDL_CheckResult(bool result, const char* expr, spdlog::logger* logger,
                            std::source_location sourceLoc = std::source_location::current())
{
    assert(logger != nullptr);
    if (!result)
    {
        logger->error("{} failed: {} (at {}:{} in {})", expr, SDL_GetError(), sourceLoc.file_name(),
                      sourceLoc.line(), sourceLoc.function_name());
    }
    return result;
}

inline std::string SDL_InitFlagsToString(SDL_InitFlags flags)
{
    std::string subsystemNames;
    if (HasBits(flags, SDL_INIT_AUDIO))
    {
        subsystemNames += "Audio|";
    }
    if (HasBits(flags, SDL_INIT_VIDEO))
    {
        subsystemNames += "Video|";
    }
    if (HasBits(flags, SDL_INIT_JOYSTICK))
    {
        subsystemNames += "Joystick|";
    }
    if (HasBits(flags, SDL_INIT_HAPTIC))
    {
        subsystemNames += "Haptic|";
    }
    if (HasBits(flags, SDL_INIT_GAMEPAD))
    {
        subsystemNames += "Gamepad|";
    }
    if (HasBits(flags, SDL_INIT_EVENTS))
    {
        subsystemNames += "Events|";
    }
    if (HasBits(flags, SDL_INIT_SENSOR))
    {
        subsystemNames += "Sensor|";
    }
    if (HasBits(flags, SDL_INIT_CAMERA))
    {
        subsystemNames += "Camera|";
    }
    if (!subsystemNames.empty() && subsystemNames.back() == '|')
    {
        subsystemNames.pop_back();
    }
    return subsystemNames;
}

// Check an SDL function call that returns bool, log SDL_GetError() on failure.
#define RAD_SDL_CHECK(expr, logger) ::rad::SDL_CheckResult((expr), #expr, logger)
// GUI convenience wrapper for SDL bool-return APIs.
#define RAD_SDL_CHECK_GUI(expr) RAD_SDL_CHECK((expr), ::rad::GetGuiLogger())

} // namespace rad
