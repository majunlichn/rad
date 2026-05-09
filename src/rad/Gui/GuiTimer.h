#pragma once

#include <rad/Gui/GuiCommon.h>

#include <chrono>

namespace rad
{

// Millisecond-resolution repeating timer; the callback returns the next interval, or 0 to cancel.
// https://wiki.libsdl.org/SDL3/CategoryTimer
class GuiTimer
{
public:
    GuiTimer() = default;
    // Calls Stop(), which invokes SDL_RemoveTimer when a timer is active.
    // https://wiki.libsdl.org/SDL3/SDL_RemoveTimer
    ~GuiTimer();

    GuiTimer(const GuiTimer&) = delete;
    GuiTimer& operator=(const GuiTimer&) = delete;

    // SDL_AddTimer; the callback returns the next interval in ms, or 0 to cancel.
    // https://wiki.libsdl.org/SDL3/SDL_AddTimer
    [[nodiscard]] bool Start(Uint32 interval, SDL_TimerCallback callback, void* userData);
    // Converts duration to milliseconds, then Start(Uint32, ...).
    template <typename Rep, typename Period>
    [[nodiscard]] bool Start(std::chrono::duration<Rep, Period> interval,
                             SDL_TimerCallback callback, void* userData)
    {
        const auto intervalMillisec =
            std::chrono::duration_cast<std::chrono::milliseconds>(interval).count();
        if (intervalMillisec <= 0)
        {
            return false;
        }
        return Start(static_cast<Uint32>(intervalMillisec), callback, userData);
    }
    // Timer ID from the last successful Start(), or 0.
    [[nodiscard]] SDL_TimerID GetId() const { return m_id; }
    // https://wiki.libsdl.org/SDL3/SDL_RemoveTimer
    bool Stop();

private:
    SDL_TimerID m_id = 0;

}; // class GuiTimer

// Nanosecond-resolution repeating timer; the callback returns the next interval, or 0 to cancel.
// https://wiki.libsdl.org/SDL3/CategoryTimer
class GuiTimerPrecise
{
public:
    GuiTimerPrecise() = default;
    // Calls Stop(), which invokes SDL_RemoveTimer when a timer is active.
    // https://wiki.libsdl.org/SDL3/SDL_RemoveTimer
    ~GuiTimerPrecise();

    GuiTimerPrecise(const GuiTimerPrecise&) = delete;
    GuiTimerPrecise& operator=(const GuiTimerPrecise&) = delete;

    // https://wiki.libsdl.org/SDL3/SDL_AddTimerNS
    [[nodiscard]] bool Start(Uint64 interval, SDL_NSTimerCallback callback, void* userData);
    // Converts duration to nanoseconds, then Start(Uint64, ...).
    template <typename Rep, typename Period>
    [[nodiscard]] bool Start(std::chrono::duration<Rep, Period> interval,
                             SDL_NSTimerCallback callback, void* userData)
    {
        const Uint64 intervalNanosec =
            std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count();
        if (intervalNanosec <= 0)
        {
            return false;
        }
        return Start(static_cast<Uint64>(intervalNanosec), callback, userData);
    }
    // Timer ID from the last successful Start(), or 0.
    [[nodiscard]] SDL_TimerID GetId() const { return m_id; }
    // https://wiki.libsdl.org/SDL3/SDL_RemoveTimer
    bool Stop();

private:
    SDL_TimerID m_id = 0;

}; // class GuiTimerPrecise

} // namespace rad
