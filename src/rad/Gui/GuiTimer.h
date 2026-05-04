#pragma once

#include <rad/Gui/GuiCommon.h>

#include <chrono>

namespace rad
{

/// Call a callback function at a future time, in milliseconds.
class GuiTimer
{
public:
    GuiTimer() = default;
    ~GuiTimer();

    GuiTimer(const GuiTimer&) = delete;
    GuiTimer& operator=(const GuiTimer&) = delete;

    /// Return the next interval in ms, or `0` to cancel (SDL semantics).
    [[nodiscard]] bool Start(Uint32 interval, SDL_TimerCallback callback, void* userData);
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
    [[nodiscard]] SDL_TimerID GetId() const { return m_id; }
    bool Stop();

private:
    SDL_TimerID m_id = 0;

}; // class GuiTimer

/// Call a callback function at a future time, in nanoseconds.
class GuiTimerPrecise
{
public:
    GuiTimerPrecise() = default;
    ~GuiTimerPrecise();

    GuiTimerPrecise(const GuiTimerPrecise&) = delete;
    GuiTimerPrecise& operator=(const GuiTimerPrecise&) = delete;

    [[nodiscard]] bool Start(Uint64 interval, SDL_NSTimerCallback callback, void* userData);
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
    [[nodiscard]] SDL_TimerID GetId() const { return m_id; }
    bool Stop();

private:
    SDL_TimerID m_id = 0;

}; // class GuiTimerPrecise

} // namespace rad
