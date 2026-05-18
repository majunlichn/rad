#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/Integer.h>
#include <rad/Common/Float.h>
#include <rad/Common/Memory.h>
#include <rad/Common/Result.h>
#include <rad/Common/RefCounted.h>
#include <rad/Container/SmallVector.h>
#include <rad/Common/Span.h>
#include <rad/IO/Logging.h>

#include <SDL3/SDL.h>

#include <cassert>
#include <source_location>
#include <string>

namespace rad
{

class GuiError
{
public:
    GuiError(std::string message);
    ~GuiError() = default;

    const std::string& GetMessage() const { return m_message; }

private:
    std::string m_message;
}; // class GuiError

template <typename T>
using GuiResult = Result<T, GuiError>;

spdlog::logger* GetGuiLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_GUI(LogLevel, ...)                                                                 \
    SPDLOG_LOGGER_CALL(::rad::GetGuiLogger(), spdlog::level::LogLevel, __VA_ARGS__)

// Returns a stable string literal for common SDL event types (for logging/debugging).
// If unknown, returns "SDL_EVENT_UNKNOWN".
const char* SDL_GetEventName(Uint32 type);

// Convenience wrapper that returns a description string (for logging/debugging).
std::string SDL_GetEventDescription(const SDL_Event& event);

// Copy of SDL_GetError() for this thread, or "Unknown error" if unset. Call right after a failed SDL API.
inline std::string GetLastSdlErrorString()
{
    const char* sdlErrRaw = SDL_GetError();
    const std::string sdlErr =
        (sdlErrRaw && (sdlErrRaw[0] != '\0')) ? std::string(sdlErrRaw) : "Unknown error";
    return sdlErr;
}

[[nodiscard]] inline bool CheckGuiSdlCall(
    bool ok, const char* expr, std::source_location sourceLoc = std::source_location::current())
{
    if (ok) [[likely]]
    {
        return true;
    }
    const std::string sdlErr = GetLastSdlErrorString();
    GetGuiLogger()->error("{} failed: {} (at {}:{} in {})", expr, sdlErr, sourceLoc.file_name(),
                          sourceLoc.line(), sourceLoc.function_name());
    return false;
}

#define RAD_GUI_CHECK_SDL(expr) ::rad::CheckGuiSdlCall((expr), #expr)

// Wraps an SDL call that returns bool. On success, returns true.
// On failure, copies this thread's SDL error string, logs, and returns GuiError.
[[nodiscard]] inline GuiResult<bool> WrapGuiSdlCall(
    bool ok, const char* expr, std::source_location sourceLoc = std::source_location::current())
{
    if (ok) [[likely]]
    {
        return true;
    }
    const std::string sdlErr = GetLastSdlErrorString();
    GetGuiLogger()->error("{} failed: {} (at {}:{} in {})", expr, sdlErr, sourceLoc.file_name(),
                          sourceLoc.line(), sourceLoc.function_name());
    return GuiError(sdlErr);
}

#define RAD_GUI_WRAP_SDL(expr) ::rad::WrapGuiSdlCall((expr), #expr)

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

} // namespace rad
