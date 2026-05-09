#pragma once

#include <rad/Gui/GuiApplication.h>
#include <rad/Gui/GuiEventHandler.h>

#include <rad/Common/String.h>

#include <SDL3/SDL.h>

#include <cstdint>
#include <vector>

namespace rad
{

class Application;
class Surface;

// Thin wrapper around SDL_Window; SDL3 video/window APIs.
// https://wiki.libsdl.org/SDL3/CategoryVideo
class Window : public GuiEventHandler
{
public:
    Window();
    // Calls Destroy(), which invokes SDL_DestroyWindow when a window exists.
    // https://wiki.libsdl.org/SDL3/SDL_DestroyWindow
    ~Window();

    // SDL_CreateWindow and SDL_GetWindowID; SDL_DestroyWindow on failure after partial setup.
    // https://wiki.libsdl.org/SDL3/CategoryVideo
    virtual bool Create(cstring_view title, int w, int h, SDL_WindowFlags flags = 0);
    // https://wiki.libsdl.org/SDL3/SDL_DestroyWindow
    virtual void Destroy();

    // Returns m_handle.
    SDL_Window* GetHandle() { return m_handle; }
    // Id assigned at Create from SDL_GetWindowID.
    SDL_WindowID GetID() const { return m_id; }

    // https://wiki.libsdl.org/SDL3/SDL_GetWindowPixelDensity
    float GetPixelDensity();
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowDisplayScale
    float GetDisplayScale();
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowFullscreenMode
    bool SetFullscreenMode(const SDL_DisplayMode* mode);
    // Delegates to SetFullscreenMode(const SDL_DisplayMode*).
    bool SetFullscreenMode(const SDL_DisplayMode& mode);
    // Same as SetFullscreenMode(nullptr) for borderless desktop fullscreen.
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowFullscreenMode
    bool SetBorderlessFullscreenDesktopMode();
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowFullscreenMode
    const SDL_DisplayMode* GetFullscreenMode();
    // Returns ICC profile bytes for the window's current display. Empty on failure.
    // SDL_GetWindowICCProfile; buffer freed with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowICCProfile
    std::vector<uint8_t> GetICCProfile();
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowPixelFormat
    Uint32 GetPixelFormat();

    // https://wiki.libsdl.org/SDL3/SDL_GetWindowFlags
    SDL_WindowFlags GetFlags();
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowTitle
    bool SetTitle(cstring_view title);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowTitle
    const char* GetTitle();
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowIcon
    bool SetIcon(SDL_Surface* icon);
    // Delegates to SetIcon(SDL_Surface*).
    bool SetIcon(Surface* icon);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowPosition
    bool SetPosition(int x, int y);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowPosition
    bool GetPosition(int* x, int* y);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowSize
    bool SetSize(int w, int h);
    // Delegates to SetSize.
    bool Resize(int w, int h);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowSize
    bool GetSize(int* w, int* h);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowSafeArea
    bool GetSafeArea(SDL_Rect* rect);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowAspectRatio
    bool SetAspectRatio(float minAspect, float maxAspect);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowAspectRatio
    bool GetAspectRatio(float* minAspect, float* maxAspect);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowBordersSize
    bool GetBordersSize(int* top, int* left, int* bottom, int* right);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowSizeInPixels
    bool GetSizeInPixels(int* w, int* h);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowMinimumSize
    bool SetMinimumSize(int w, int h);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowMinimumSize
    bool GetMinimumSize(int* w, int* h);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowMaximumSize
    bool SetMaximumSize(int w, int h);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowMaximumSize
    bool GetMaximumSize(int* w, int* h);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowBordered
    bool SetBordered(bool bordered);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowResizable
    bool SetResizable(bool resizable);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowAlwaysOnTop
    bool SetAlwaysOnTop(bool onTop);

    // https://wiki.libsdl.org/SDL3/SDL_ShowWindow
    bool Show();
    // https://wiki.libsdl.org/SDL3/SDL_HideWindow
    bool Hide();
    // https://wiki.libsdl.org/SDL3/SDL_RaiseWindow
    bool Raise();
    // https://wiki.libsdl.org/SDL3/SDL_MaximizeWindow
    bool Maximize();
    // https://wiki.libsdl.org/SDL3/SDL_MinimizeWindow
    bool Minimize();
    // https://wiki.libsdl.org/SDL3/SDL_RestoreWindow
    bool Restore();
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowFullscreen
    bool SetFullscreen(bool fullscreen);
    // https://wiki.libsdl.org/SDL3/SDL_SyncWindow
    bool Sync();

    // https://wiki.libsdl.org/SDL3/SDL_WindowHasSurface
    bool HasSurface();
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowSurface
    SDL_Surface* GetSurface();
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowSurfaceVSync
    bool SetSurfaceVSync(int vsync);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowSurfaceVSync
    bool GetSurfaceVSync(int* vsync);
    // https://wiki.libsdl.org/SDL3/SDL_UpdateWindowSurface
    bool UpdateSurface();
    // https://wiki.libsdl.org/SDL3/SDL_UpdateWindowSurfaceRects
    bool UpdateSurfaceRects(Span<SDL_Rect> rects);
    // https://wiki.libsdl.org/SDL3/SDL_DestroyWindowSurface
    bool DestroySurface();

    // https://wiki.libsdl.org/SDL3/SDL_SetWindowKeyboardGrab
    bool SetKeyboardGrab(bool grabbed);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowMouseGrab
    bool SetMouseGrab(bool grabbed);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowKeyboardGrab
    bool GetKeyboardGrab();
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowMouseGrab
    bool GetMouseGrab();

    // https://wiki.libsdl.org/SDL3/SDL_SetWindowMouseRect
    bool SetMouseRect(const SDL_Rect* rect);
    // Delegates to SetMouseRect(const SDL_Rect*).
    bool SetMouseRect(const SDL_Rect& rect);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowMouseRect
    const SDL_Rect* GetMouseRect();

    // https://wiki.libsdl.org/SDL3/SDL_SetWindowOpacity
    bool SetOpacity(float opacity);
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowOpacity
    bool GetOpacity(float* opacity);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowFocusable
    bool SetFocusable(bool focusable);

    // https://wiki.libsdl.org/SDL3/SDL_ShowWindowSystemMenu
    bool ShowSystemMenu(int x, int y);
    // https://wiki.libsdl.org/SDL3/SDL_SetWindowShape
    bool SetShape(SDL_Surface* shape);
    // Delegates to SetShape(SDL_Surface*).
    bool SetShape(Surface* shape);
    // https://wiki.libsdl.org/SDL3/SDL_FlashWindow
    bool Flash(SDL_FlashOperation operation);

protected:
    SDL_Window* m_handle = nullptr;
    SDL_WindowID m_id = 0;
    // Custom event handlers.
    std::vector<GuiEventHandler*> m_eventHandlers;

    // Dispatches child handlers, then SDL window/keyboard/text/mouse/user events targeted at this
    // window (by windowID).
    // https://wiki.libsdl.org/SDL3/CategoryEvents
    virtual bool OnEvent(const SDL_Event& event) override;
    virtual void OnIdle() override;

    // Maps SDL_EVENT_WINDOW_* (dispatched from OnEvent) to the virtual hooks below.
    // https://wiki.libsdl.org/SDL3/CategoryEvents
    virtual void OnWindowEvent(const SDL_WindowEvent& event);
    // Default overrides for SDL window events (see OnWindowEvent).
    virtual void OnShown() {}
    virtual void OnHidden() {}
    virtual void OnExposed() {}
    virtual void OnMoved(int x, int y) {}
    virtual void OnResized(int width, int height) {}
    virtual void OnPixelSizeChanged(int width, int height) {}
    virtual void OnMinimized() {}
    virtual void OnMaximized() {}
    virtual void OnRestored() {}
    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}
    // Window has gained keyboard focus.
    virtual void OnFocusGained() {}
    // Window has lost keyboard focus.
    virtual void OnFocusLost() {}
    virtual void OnCloseRequested() { Destroy(); }
    virtual void OnHitTest() {}
    virtual void OnIccProfileChanged() {}
    virtual void OnDisplayChanged() {}
    virtual void OnDisplayScaleChanged() {}
    virtual void OnOccluded() {}
    virtual void OnEnterFullscreen() {}
    virtual void OnLeaveFullscreen() {}
    virtual void OnDestroyed() {}

    // Invoked from OnEvent for SDL keyboard events targeting this window.
    // https://wiki.libsdl.org/SDL3/CategoryKeyboard
    virtual void OnKeyDown(const SDL_KeyboardEvent& keyDown) {}
    virtual void OnKeyUp(const SDL_KeyboardEvent& keyUp) {}
    virtual void OnTextEditing(const SDL_TextEditingEvent& textEditing) {}
    virtual void OnTextInput(const SDL_TextInputEvent& textInput) {}

    // Invoked from OnEvent for SDL mouse events targeting this window.
    // https://wiki.libsdl.org/SDL3/CategoryMouse
    virtual void OnMouseMove(const SDL_MouseMotionEvent& mouseMotion) {}
    virtual void OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton) {}
    virtual void OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton) {}
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel) {}

    // Invoked from OnEvent for SDL_EVENT_USER when windowID is 0 (broadcast) or matches this window.
    // https://wiki.libsdl.org/SDL3/CategoryEvents
    virtual void OnUserEvent(const SDL_UserEvent& user) {}

}; // class Window

} // namespace rad
