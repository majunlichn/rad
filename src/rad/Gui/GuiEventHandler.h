#pragma once

#include <rad/Gui/GuiCommon.h>

#include <SDL3/SDL_events.h>

namespace rad
{

class GuiEventHandler : public RefCounted<GuiEventHandler>
{
public:
    GuiEventHandler();
    virtual ~GuiEventHandler();

    // Return true if the event was handled; it will not be forwarded to other handlers.
    virtual bool OnEvent(const SDL_Event& event) = 0;
    // Called once per main-loop iteration (update, render, etc.).
    virtual void OnIdle() {}

}; // class GuiEventHandler

} // namespace rad
