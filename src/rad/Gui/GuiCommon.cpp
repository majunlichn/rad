#include <rad/Gui/GuiCommon.h>

namespace rad
{

spdlog::logger* GetGuiLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("GUI");
    return logger.get();
}

namespace SDL
{

Uint64 GetTicksInMilliseconds()
{
    return SDL_GetTicks();
}

Uint64 GetTicksInNanoseconds()
{
    return SDL_GetTicksNS();
}

Uint64 GetPerformanceCounter()
{
    return SDL_GetPerformanceCounter();
}

Uint64 GetPerformanceCounterFrequency()
{
    return SDL_GetPerformanceFrequency();
}

void DelayPrecise(std::chrono::nanoseconds duration)
{
    const auto count = duration.count();
    if (count <= 0)
    {
        return;
    }
    SDL_DelayPrecise(static_cast<Uint64>(count));
}

} // namespace SDL

} // namespace rad
