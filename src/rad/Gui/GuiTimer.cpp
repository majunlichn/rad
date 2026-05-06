#include <rad/Gui/GuiTimer.h>

#include <cassert>

namespace rad
{

GuiTimer::~GuiTimer()
{
    Stop();
}

bool GuiTimer::Start(Uint32 interval, SDL_TimerCallback callback, void* userData)
{
    assert(m_id == 0);
    m_id = SDL_AddTimer(interval, callback, userData);
    if (m_id == 0)
    {
        RAD_LOG_GUI(err, "SDL_AddTimer failed: {}", SDL_GetError());
        return false;
    }
    return true;
}

bool GuiTimer::Stop()
{
    if (m_id != 0)
    {
        if (RAD_SDL_CHECK_GUI(SDL_RemoveTimer(m_id)))
        {
            m_id = 0;
            return true;
        }
    }
    return false;
}

GuiTimerPrecise::~GuiTimerPrecise()
{
    Stop();
}

bool GuiTimerPrecise::Start(Uint64 interval, SDL_NSTimerCallback callback, void* userData)
{
    assert(m_id == 0);
    m_id = SDL_AddTimerNS(interval, callback, userData);
    if (m_id == 0)
    {
        RAD_LOG_GUI(err, "SDL_AddTimerNS failed: {}", SDL_GetError());
        return false;
    }
    return true;
}

bool GuiTimerPrecise::Stop()
{
    if (m_id != 0)
    {
        if (RAD_SDL_CHECK_GUI(SDL_RemoveTimer(m_id)))
        {
            m_id = 0;
            return true;
        }
    }
    return false;
}

} // namespace rad
