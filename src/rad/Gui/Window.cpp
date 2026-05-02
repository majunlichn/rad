#include <rad/Gui/Window.h>

#include <rad/Gui/Surface.h>

#include <cstring>

namespace rad
{

Window::Window()
{
}

Window::~Window()
{
    Destroy();
}

bool Window::Create(cstring_view title, int w, int h, SDL_WindowFlags flags)
{
    m_handle = SDL_CreateWindow(title.c_str(), w, h, flags);
    if (m_handle)
    {
        m_id = SDL_GetWindowID(m_handle);
        if (m_id != 0)
        {
            GuiApplication::GetInstance()->RegisterEventHandler(this);
            return true;
        }
        else
        {
            RAD_LOG_GUI(err, "SDL_GetWindowID failed: {}", SDL_GetError());
            SDL_DestroyWindow(m_handle);
            m_handle = nullptr;
            m_id = 0;
            return false;
        }
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_CreateWindow failed: {}", SDL_GetError());
        return false;
    }
}

void Window::Destroy()
{
    if (m_handle)
    {
        GuiApplication::GetInstance()->UnregisterEventHandler(this);
        SDL_DestroyWindow(m_handle);
        m_handle = nullptr;
        m_id = 0;
    }
}

float Window::GetPixelDensity()
{
    float density = SDL_GetWindowPixelDensity(m_handle);
    if (density == 0)
    {
        RAD_LOG_GUI(err, "SDL_GetWindowPixelDensity failed: {}", SDL_GetError());
    }
    return density;
}

float Window::GetDisplayScale()
{
    float scale = SDL_GetWindowDisplayScale(m_handle);
    if (scale == 0)
    {
        RAD_LOG_GUI(err, "SDL_GetWindowDisplayScale failed: {}", SDL_GetError());
    }
    return scale;
}

bool Window::SetFullscreenMode(const SDL_DisplayMode* mode)
{
    return SDL_CHECK(SDL_SetWindowFullscreenMode(m_handle, mode));
}

bool Window::SetFullscreenMode(const SDL_DisplayMode& mode)
{
    return SetFullscreenMode(&mode);
}

bool Window::SetBorderlessFullscreenDesktopMode()
{
    return SetFullscreenMode(nullptr);
}

const SDL_DisplayMode* Window::GetFullscreenMode()
{
    return SDL_GetWindowFullscreenMode(m_handle);
}

std::vector<uint8_t> Window::GetICCProfile()
{
    std::vector<uint8_t> out;

    size_t size = 0;
    void* data = SDL_GetWindowICCProfile(m_handle, &size);
    if (!data)
    {
        RAD_LOG_GUI(err, "SDL_GetWindowICCProfile failed: {}", SDL_GetError());
        return out;
    }

    out.resize(size);
    if (size > 0)
    {
        std::memcpy(out.data(), data, size);
    }
    SDL_free(data);
    return out;
}

Uint32 Window::GetPixelFormat()
{
    Uint32 format = SDL_GetWindowPixelFormat(m_handle);
    if (format == SDL_PIXELFORMAT_UNKNOWN)
    {
        RAD_LOG_GUI(err, "SDL_GetWindowPixelFormat failed: {}", SDL_GetError());
    }
    return format;
}

SDL_WindowFlags Window::GetFlags()
{
    return SDL_GetWindowFlags(m_handle);
}

bool Window::SetTitle(cstring_view title)
{
    return SDL_CHECK(SDL_SetWindowTitle(m_handle, title.c_str()));
}

const char* Window::GetTitle()
{
    return SDL_GetWindowTitle(m_handle);
}

bool Window::SetIcon(SDL_Surface* icon)
{
    return SDL_CHECK(SDL_SetWindowIcon(m_handle, icon));
}

bool Window::SetIcon(Surface* icon)
{
    return SetIcon(icon->GetHandle());
}

bool Window::SetPosition(int x, int y)
{
    return SDL_CHECK(SDL_SetWindowPosition(m_handle, x, y));
}

bool Window::GetPosition(int* x, int* y)
{
    return SDL_CHECK(SDL_GetWindowPosition(m_handle, x, y));
}

bool Window::SetSize(int w, int h)
{
    return SDL_CHECK(SDL_SetWindowSize(m_handle, w, h));
}

bool Window::Resize(int w, int h)
{
    return SetSize(w, h);
}

bool Window::GetSize(int* w, int* h)
{
    return SDL_CHECK(SDL_GetWindowSize(m_handle, w, h));
}

bool Window::GetSafeArea(SDL_Rect* rect)
{
    return SDL_CHECK(SDL_GetWindowSafeArea(m_handle, rect));
}

bool Window::SetAspectRatio(float minAspect, float maxAspect)
{
    return SDL_CHECK(SDL_SetWindowAspectRatio(m_handle, minAspect, maxAspect));
}

bool Window::GetAspectRatio(float* minAspect, float* maxAspect)
{
    return SDL_CHECK(SDL_GetWindowAspectRatio(m_handle, minAspect, maxAspect));
}

bool Window::GetBordersSize(int* top, int* left, int* bottom, int* right)
{
    return SDL_CHECK(SDL_GetWindowBordersSize(m_handle, top, left, bottom, right));
}

bool Window::GetSizeInPixels(int* w, int* h)
{
    return SDL_CHECK(SDL_GetWindowSizeInPixels(m_handle, w, h));
}

bool Window::SetMinimumSize(int w, int h)
{
    return SDL_CHECK(SDL_SetWindowMinimumSize(m_handle, w, h));
}

bool Window::GetMinimumSize(int* w, int* h)
{
    return SDL_CHECK(SDL_GetWindowMinimumSize(m_handle, w, h));
}

bool Window::SetMaximumSize(int w, int h)
{
    return SDL_CHECK(SDL_SetWindowMaximumSize(m_handle, w, h));
}

bool Window::GetMaximumSize(int* w, int* h)
{
    return SDL_CHECK(SDL_GetWindowMaximumSize(m_handle, w, h));
}

bool Window::SetBordered(bool bordered)
{
    return SDL_CHECK(SDL_SetWindowBordered(m_handle, bordered));
}

bool Window::SetResizable(bool resizable)
{
    return SDL_CHECK(SDL_SetWindowResizable(m_handle, resizable));
}

bool Window::SetAlwaysOnTop(bool onTop)
{
    return SDL_CHECK(SDL_SetWindowAlwaysOnTop(m_handle, onTop));
}

bool Window::Show()
{
    return SDL_CHECK(SDL_ShowWindow(m_handle));
}

bool Window::Hide()
{
    return SDL_CHECK(SDL_HideWindow(m_handle));
}

bool Window::Raise()
{
    return SDL_CHECK(SDL_RaiseWindow(m_handle));
}

bool Window::Maximize()
{
    return SDL_CHECK(SDL_MaximizeWindow(m_handle));
}

bool Window::Minimize()
{
    return SDL_CHECK(SDL_MinimizeWindow(m_handle));
}

bool Window::Restore()
{
    return SDL_CHECK(SDL_RestoreWindow(m_handle));
}

bool Window::SetFullscreen(bool fullscreen)
{
    return SDL_CHECK(SDL_SetWindowFullscreen(m_handle, fullscreen));
}

bool Window::Sync()
{
    return SDL_CHECK(SDL_SyncWindow(m_handle));
}

bool Window::HasSurface()
{
    return SDL_WindowHasSurface(m_handle);
}

SDL_Surface* Window::GetSurface()
{
    SDL_Surface* surface = SDL_GetWindowSurface(m_handle);
    if (surface == nullptr)
    {
        RAD_LOG_GUI(err, "SDL_GetWindowSurface failed: {}", SDL_GetError());
    }
    return surface;
}

bool Window::SetSurfaceVSync(int vsync)
{
    return SDL_CHECK(SDL_SetWindowSurfaceVSync(m_handle, vsync));
}

bool Window::GetSurfaceVSync(int* vsync)
{
    return SDL_CHECK(SDL_GetWindowSurfaceVSync(m_handle, vsync));
}

bool Window::UpdateSurface()
{
    return SDL_CHECK(SDL_UpdateWindowSurface(m_handle));
}

bool Window::UpdateSurfaceRects(Span<SDL_Rect> rects)
{
    return SDL_CHECK(
        SDL_UpdateWindowSurfaceRects(m_handle, rects.data(), static_cast<int>(rects.size())));
}

bool Window::DestroySurface()
{
    return SDL_CHECK(SDL_DestroyWindowSurface(m_handle));
}

bool Window::SetKeyboardGrab(bool grabbed)
{
    return SDL_CHECK(SDL_SetWindowKeyboardGrab(m_handle, grabbed));
}

bool Window::SetMouseGrab(bool grabbed)
{
    return SDL_CHECK(SDL_SetWindowMouseGrab(m_handle, grabbed));
}

bool Window::GetKeyboardGrab()
{
    return SDL_GetWindowKeyboardGrab(m_handle);
}

bool Window::GetMouseGrab()
{
    return SDL_GetWindowMouseGrab(m_handle);
}

bool Window::SetMouseRect(const SDL_Rect* rect)
{
    return SDL_CHECK(SDL_SetWindowMouseRect(m_handle, rect));
}

bool Window::SetMouseRect(const SDL_Rect& rect)
{
    return SetMouseRect(&rect);
}

const SDL_Rect* Window::GetMouseRect()
{
    return SDL_GetWindowMouseRect(m_handle);
}

bool Window::SetOpacity(float opacity)
{
    return SDL_CHECK(SDL_SetWindowOpacity(m_handle, opacity));
}

bool Window::GetOpacity(float* opacity)
{
    float result = SDL_GetWindowOpacity(m_handle);
    if (result >= 0)
    {
        *opacity = result;
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetWindowOpacity failed: {}", SDL_GetError());
        return false;
    }
}

bool Window::SetFocusable(bool focusable)
{
    return SDL_CHECK(SDL_SetWindowFocusable(m_handle, focusable));
}

bool Window::ShowSystemMenu(int x, int y)
{
    return SDL_CHECK(SDL_ShowWindowSystemMenu(m_handle, x, y));
}

bool Window::SetShape(SDL_Surface* shape)
{
    return SDL_CHECK(SDL_SetWindowShape(m_handle, shape));
}

bool Window::SetShape(Surface* shape)
{
    return SetShape(shape->GetHandle());
}

bool Window::Flash(SDL_FlashOperation operation)
{
    return SDL_CHECK(SDL_FlashWindow(m_handle, operation));
}

bool Window::OnEvent(const SDL_Event& event)
{
    for (GuiEventHandler* handler : m_eventHandlers)
    {
        if (handler->OnEvent(event))
        {
            return true;
        }
    }

    if ((event.type >= SDL_EVENT_WINDOW_FIRST) && (event.type <= SDL_EVENT_WINDOW_LAST))
    {
        if (event.window.windowID == m_id)
        {
            OnWindowEvent(event.window);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (event.key.windowID == m_id)
        {
            OnKeyDown(event.key);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_KEY_UP)
    {
        if (event.key.windowID == m_id)
        {
            OnKeyUp(event.key);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_TEXT_EDITING)
    {
        if (event.edit.windowID == m_id)
        {
            OnTextEditing(event.edit);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_TEXT_INPUT)
    {
        if (event.text.windowID == m_id)
        {
            OnTextInput(event.text);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
        if (event.motion.windowID == m_id)
        {
            OnMouseMove(event.motion);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (event.button.windowID == m_id)
        {
            OnMouseButtonDown(event.button);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (event.button.windowID == m_id)
        {
            OnMouseButtonUp(event.button);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        if (event.wheel.windowID == m_id)
        {
            OnMouseWheel(event.wheel);
            return true;
        }
    }
    else if (event.type == SDL_EVENT_USER)
    {
        if ((event.user.windowID == 0) || (event.user.windowID == m_id))
        {
            OnUserEvent(event.user);
            return (event.user.windowID == m_id);
        }
    }

    return false;
}

void Window::OnIdle()
{
}

void Window::OnWindowEvent(const SDL_WindowEvent& event)
{
    switch (event.type)
    {
    case SDL_EVENT_WINDOW_SHOWN:
        OnShown();
        break;
    case SDL_EVENT_WINDOW_HIDDEN:
        OnHidden();
        break;
    case SDL_EVENT_WINDOW_EXPOSED:
        OnExposed();
        break;
    case SDL_EVENT_WINDOW_MOVED:
        OnMoved(event.data1, event.data2);
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        OnResized(event.data1, event.data2);
        break;
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        OnPixelSizeChanged(event.data1, event.data2);
        break;
    case SDL_EVENT_WINDOW_MINIMIZED:
        OnMinimized();
        break;
    case SDL_EVENT_WINDOW_MAXIMIZED:
        OnMaximized();
        break;
    case SDL_EVENT_WINDOW_RESTORED:
        OnRestored();
        break;
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
        OnMouseEnter();
        break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        OnMouseLeave();
        break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        OnFocusGained();
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        OnFocusLost();
        break;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        OnCloseRequested();
        break;
    case SDL_EVENT_WINDOW_HIT_TEST:
        OnHitTest();
        break;
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
        OnIccProfileChanged();
        break;
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        OnDisplayChanged();
        break;
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        OnDisplayScaleChanged();
        break;
    case SDL_EVENT_WINDOW_OCCLUDED:
        OnOccluded();
        break;
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        OnEnterFullscreen();
        break;
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        OnLeaveFullscreen();
        break;
    case SDL_EVENT_WINDOW_DESTROYED:
        OnDestroyed();
        break;
    }
}

} // namespace rad
