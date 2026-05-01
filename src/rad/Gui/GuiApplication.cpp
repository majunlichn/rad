#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/GuiApplication.h>

namespace rad
{

GuiApplication::GuiApplication()
{
}

GuiApplication::~GuiApplication()
{
    Destroy();
}

GuiApplication* GuiApplication::GetInstance()
{
    static Ref<GuiApplication> g_app = RAD_NEW GuiApplication();
    return g_app.get();
}

bool GuiApplication::Init(int argc, char** argv)
{
    if (m_initialized)
    {
        return true;
    }

    Application::Init(argc, argv);
    RAD_LOG_GUI(info, "SDL compiled version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
                SDL_MICRO_VERSION);
    // Initialize basic subsystems; initialize others later via SDL_InitSubSystem if needed.
    m_initialized = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (!m_initialized)
    {
        RAD_LOG_GUI(err, "SDL_Init failed: {}", SDL_GetError());
        m_status = Status::Unknown;
        return false;
    }
    int version = SDL_GetVersion();
    RAD_LOG_GUI(info, "SDL initialized on {}: runtime version {}.{}.{} ({})", SDL_GetPlatform(),
                SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version),
                SDL_VERSIONNUM_MICRO(version), SDL_GetRevision());
    if (const char* basePath = SDL_GetBasePath())
    {
        RAD_LOG_GUI(info, "Base path: {}", basePath);
    }
    if (const char* pVideoDriver = SDL_GetCurrentVideoDriver())
    {
        RAD_LOG_GUI(info, "Current Video Driver: {}", pVideoDriver);
    }
    if (const char* pAudioDriver = SDL_GetCurrentAudioDriver())
    {
        RAD_LOG_GUI(info, "Current Audio Driver: {}", pAudioDriver);
    }
    UpdateDisplayInfos();
    m_status = Status::Initialized;
    return true;
}

void GuiApplication::Destroy()
{
    if (!m_initialized)
    {
        m_status = Status::Unknown;
        return;
    }

    SDL_Quit();
    m_initialized = false;
    m_status = Status::Unknown;
}

int GuiApplication::Run()
{
    if (!m_initialized)
    {
        RAD_LOG_GUI(err, "GuiApplication::Run called before Init().");
        return -1;
    }

    // If the caller didn't set a status, treat this as starting execution.
    if (m_status == Status::Initialized)
    {
        m_status = Status::Running;
    }

    SDL_Event event;
    while (m_status != Status::Exited)
    {
        // Process all pending events.
        while (SDL_PollEvent(&event))
        {
            OnEvent(event);
            if (m_status == Status::Exited)
            {
                break;
            }
        }

        // One idle tick per loop iteration (update/render, etc.).
        if (m_status != Status::Exited)
        {
            OnIdle();
        }

        // Avoid a tight spin-loop when idle with no events.
        if (m_status != Status::Exited)
        {
            SDL_Delay(1);
        }
    }

    return m_errCode.load();
}

SDL_InitFlags GuiApplication::GetInitFlags() const
{
    // Returns a mask of all initialized subsystems if flags is 0.
    return SDL_WasInit(0);
}

bool GuiApplication::IsSubsystemInitialized(SDL_InitFlags flags)
{
    SDL_InitFlags mask = SDL_WasInit(flags);
    return HasBits(flags, mask);
}

bool GuiApplication::SetMetadataProperty(cstring_view name, cstring_view value)
{
    if (SDL_SetAppMetadataProperty(name.c_str(), value.c_str()))
    {
        return true;
    }
    RAD_LOG_GUI(err, "SDL_SetAppMetadataProperty({}, {}) failed: {}", name, value, SDL_GetError());
    return false;
}

const char* GuiApplication::GetMetadataProperty(cstring_view name)
{
    return SDL_GetAppMetadataProperty(name.c_str());
}

bool GuiApplication::InitSubSystem(SDL_InitFlags flags)
{
    bool result = SDL_InitSubSystem(flags);
    if (!result)
    {
        RAD_LOG_GUI(err, "SDL_InitSubSystem({}) failed: {}", (uint32_t)flags, SDL_GetError());
    }
    return result;
}

void GuiApplication::QuitSubSystem(SDL_InitFlags flags)
{
    SDL_QuitSubSystem(flags);
}

bool GuiApplication::IsMainThread()
{
    return SDL_IsMainThread();
}

bool GuiApplication::RunOnMainThread(SDL_MainThreadCallback callback, void* userData,
                                     bool waitComplete)
{
    return SDL_RunOnMainThread(callback, userData, waitComplete);
}

void GuiApplication::RegisterEventHandler(GuiEventHandler* handler)
{
    m_eventHandlers.push_back(handler);
}

void GuiApplication::UnregisterEventHandler(GuiEventHandler* handler)
{
    std::erase(m_eventHandlers, handler);
}

bool GuiApplication::PushEvent(SDL_Event& event)
{
    bool result = SDL_PushEvent(&event);
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_PushEvent failed: {}", SDL_GetError());
        return false;
    }
}

void GuiApplication::PumpEvents()
{
    SDL_PumpEvents();
}

bool GuiApplication::PollEvent(SDL_Event* outEvent)
{
    return SDL_PollEvent(outEvent);
}

bool GuiApplication::WaitEventTimeout(SDL_Event* outEvent, Sint32 timeoutMs)
{
    return SDL_WaitEventTimeout(outEvent, timeoutMs);
}

bool GuiApplication::RequestQuit()
{
    SDL_Event event = {};
    event.type = SDL_EVENT_QUIT;
    return PushEvent(event);
}

void GuiApplication::OnEvent(const SDL_Event& event)
{
    m_status = Status::Running;
    const std::vector<GuiEventHandler*> handlers = m_eventHandlers;
    for (GuiEventHandler* handler : handlers)
    {
        if (handler->OnEvent(event))
        {
            return;
        }
    }

    switch (event.type)
    {
    case SDL_EVENT_QUIT: // User-requested quit.
        m_status = Status::Exited;
        break;
    case SDL_EVENT_TERMINATING:
        RAD_LOG_GUI(info, "SDL_EVENT_TERMINATING: "
                          "The application is being terminated by the OS.");
        m_status = Status::Exited;
        break;
    case SDL_EVENT_LOW_MEMORY:
        RAD_LOG_GUI(info, "SDL_EVENT_LOW_MEMORY: "
                          "The application is low on memory, free memory if possible.");
        break;
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
        RAD_LOG_GUI(info, "SDL_EVENT_WILL_ENTER_BACKGROUND: "
                          "The application is about to enter the background.");
        break;
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        RAD_LOG_GUI(info,
                    "SDL_EVENT_DID_ENTER_BACKGROUND: "
                    "The application did enter the background and may not get CPU for some time.");
        break;
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
        RAD_LOG_GUI(info, "SDL_EVENT_WILL_ENTER_FOREGROUND: "
                          "The application is about to enter the foreground.");
        break;
    case SDL_EVENT_DID_ENTER_FOREGROUND:
        RAD_LOG_GUI(info, "SDL_EVENT_DID_ENTER_FOREGROUND: "
                          "The application is now interactive.");
        break;
    case SDL_EVENT_LOCALE_CHANGED:
        RAD_LOG_GUI(info, "SDL_EVENT_LOCALE_CHANGED: "
                          "The user's locale preferences have changed.");
        break;
    case SDL_EVENT_SYSTEM_THEME_CHANGED:
        RAD_LOG_GUI(info, "SDL_EVENT_SYSTEM_THEME_CHANGED: "
                          "The system theme changed.");
        break;
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
    case SDL_EVENT_DISPLAY_REMOVED:
    case SDL_EVENT_DISPLAY_MOVED:
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
        RAD_LOG_GUI(info, "Display state changed.");
        UpdateDisplayInfos();
        break;
    }
}

void GuiApplication::OnIdle()
{
    const std::vector<GuiEventHandler*> handlers = m_eventHandlers;
    for (GuiEventHandler* handler : handlers)
    {
        handler->OnIdle();
    }

    if (handlers.empty())
    {
        m_status = Status::Exited;
    }
}

void GuiApplication::Exit(int errCode)
{
    m_errCode = errCode;
    m_status = Status::Exited;
}

bool GuiApplication::IsScreenSaverEnabled()
{
    return SDL_ScreenSaverEnabled();
}

bool GuiApplication::EnableScreenSaver()
{
    bool result = SDL_EnableScreenSaver();
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_EnableScreenSaver failed: {}", SDL_GetError());
        return false;
    }
}

bool GuiApplication::DisableScreenSaver()
{
    bool result = SDL_DisableScreenSaver();
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_DisableScreenSaver failed: {}", SDL_GetError());
        return false;
    }
}

bool GuiApplication::SetClipboardText(const char* text)
{
    bool result = SDL_SetClipboardText(text);
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_SetClipboardText failed: {}", SDL_GetError());
        return false;
    }
}

std::string GuiApplication::GetClipboardText()
{
    std::string buffer;
    const char* text = SDL_GetClipboardText();
    if (text)
    {
        buffer = text;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetClipboardText failed: {}", SDL_GetError());
    }
    return buffer;
}

bool GuiApplication::HasClipboardText()
{
    return SDL_HasClipboardText();
}

bool GuiApplication::SetPrimarySelectionText(const char* text)
{
    bool result = SDL_SetPrimarySelectionText(text);
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_SetPrimarySelectionText failed: {}", SDL_GetError());
        return false;
    }
}

std::string GuiApplication::GetPrimarySelectionText()
{
    std::string buffer;
    const char* text = SDL_GetPrimarySelectionText();
    if (text)
    {
        buffer = text;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetPrimarySelectionText failed: {}", SDL_GetError());
    }
    return buffer;
}

bool GuiApplication::HasPrimarySelectionText()
{
    return SDL_HasPrimarySelectionText();
}

bool GuiApplication::SetClipboardData(SDL_ClipboardDataCallback callback,
                                      SDL_ClipboardCleanupCallback cleanup, void* userData,
                                      const char** mimeTypes, size_t mimeTypeCount)
{
    bool result = SDL_SetClipboardData(callback, cleanup, userData, mimeTypes, mimeTypeCount);
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_SetClipboardData failed: {}", SDL_GetError());
        return false;
    }
}

bool GuiApplication::ClearClipboardData()
{
    return SDL_ClearClipboardData();
}

const void* GuiApplication::GetClipboardData(const char* mimeType, size_t* size)
{
    if (const void* data = SDL_GetClipboardData(mimeType, size))
    {
        return data;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetClipboardData failed: {}", SDL_GetError());
        return nullptr;
    }
}

bool GuiApplication::HasClipboardData(const char* mimeType)
{
    return SDL_HasClipboardData(mimeType);
}

std::string GuiApplication::GetBasePath()
{
    std::string buffer;
    if (const char* p = SDL_GetBasePath())
    {
        buffer = p;
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetBasePath failed: {}", SDL_GetError());
    }
    return buffer;
}

std::string GuiApplication::GetPrefPath(const char* org, const char* app)
{
    std::string buffer;
    if (char* p = SDL_GetPrefPath(org, app))
    {
        buffer = p;
        SDL_free(p);
    }
    else
    {
        RAD_LOG_GUI(err, "SDL_GetPrefPath({}, {}) failed: {}", org ? org : "(null)",
                    app ? app : "(null)", SDL_GetError());
    }
    return buffer;
}

PowerInfo GuiApplication::GetPowerInfo()
{
    PowerInfo info;
    info.state = SDL_GetPowerInfo(&info.seconds, &info.percent);
    if (info.state == SDL_POWERSTATE_ERROR)
    {
        RAD_LOG_GUI(err, "SDL_GetPowerInfo failed: {}", SDL_GetError());
    }
    return info;
}

bool GuiApplication::ShowSimpleMessageBox(Uint32 flags, cstring_view title, cstring_view message,
                                          SDL_Window* parent)
{
    bool result = SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), parent);
    if (!result)
    {
        RAD_LOG_GUI(err, "SDL_ShowSimpleMessageBox failed: {}", SDL_GetError());
    }
    return result;
}

void GuiApplication::UpdateDisplayInfos()
{
    int displayCount = 0;
    const SDL_DisplayID* ids = SDL_GetDisplays(&displayCount);
    if (ids == nullptr)
    {
        RAD_LOG_GUI(err, "SDL_GetDisplays failed: {}", SDL_GetError());
        m_displays.clear();
        return;
    }
    m_displays.clear();
    m_displays.resize(displayCount);
    for (int i = 0; i < displayCount; ++i)
    {
        DisplayInfo info = {};
        SDL_DisplayID id = ids[i];
        info.id = id;
        if (const char* pName = SDL_GetDisplayName(id))
        {
            info.name = pName;
        }
        else
        {
            RAD_LOG_GUI(err, "SDL_GetDisplayName failed: {}", SDL_GetError());
            info.name = "Unknown";
        }

        if (SDL_GetDisplayBounds(id, &info.bounds) != true)
        {
            RAD_LOG_GUI(err, "SDL_GetDisplayBounds failed: {}", SDL_GetError());
        }

        if (SDL_GetDisplayUsableBounds(id, &info.usableBounds) != true)
        {
            RAD_LOG_GUI(err, "SDL_GetDisplayUsableBounds failed: {}", SDL_GetError());
        }

        info.naturalOrientation = SDL_GetNaturalDisplayOrientation(id);
        info.currentOrientation = SDL_GetCurrentDisplayOrientation(id);

        info.scale = SDL_GetDisplayContentScale(id);
        if (info.scale == 0.0f)
        {
            RAD_LOG_GUI(err, "SDL_GetDisplayContentScale failed: {}", SDL_GetError());
        }

        info.propID = SDL_GetDisplayProperties(id);
        if (info.propID != 0)
        {
            info.hdrEnabled =
                SDL_GetBooleanProperty(info.propID, SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, false);
            info.kmsdrmOrientation = SDL_GetNumberProperty(
                info.propID, SDL_PROP_DISPLAY_KMSDRM_PANEL_ORIENTATION_NUMBER, 0);
        }
        else
        {
            RAD_LOG_GUI(err, "SDL_GetDisplayProperties failed: {}", SDL_GetError());
        }

        int count = 0;
        const SDL_DisplayMode* const* modes = SDL_GetFullscreenDisplayModes(id, &count);
        if (modes && (count > 0))
        {
            info.modes.reserve(count);
            for (int j = 0; j < count; ++j)
            {
                if (modes[j])
                {
                    info.modes.push_back(*modes[j]);
                }
            }
        }
        if (modes)
        {
            SDL_free((void*)modes);
        }

        if (const SDL_DisplayMode* desktopMode = SDL_GetDesktopDisplayMode(id))
        {
            info.hasDesktopMode = true;
            info.desktopMode = *desktopMode;
        }
        else
        {
            info.hasDesktopMode = false;
            RAD_LOG_GUI(err, "SDL_GetDesktopDisplayMode failed: {}", SDL_GetError());
        }

        if (const SDL_DisplayMode* currentMode = SDL_GetCurrentDisplayMode(id))
        {
            info.hasCurrentMode = true;
            info.currentMode = *currentMode;
        }
        else
        {
            info.hasCurrentMode = false;
            RAD_LOG_GUI(err, "SDL_GetCurrentDisplayMode failed: {}", SDL_GetError());
        }

        if (info.hasCurrentMode)
        {
            RAD_LOG_GUI(info, "Display#{}: {} ({}x{}@{}Hz, {})", i, info.name, info.currentMode.w,
                        info.currentMode.h, info.currentMode.refresh_rate,
                        SDL_GetPixelFormatName(info.currentMode.format));
        }
        else
        {
            RAD_LOG_GUI(info, "Display#{}: {} (current mode unavailable)", i, info.name);
        }

        // Persist results (was previously discarded).
        m_displays[i] = info;
    }

    SDL_free((void*)ids);
}

} // namespace rad
