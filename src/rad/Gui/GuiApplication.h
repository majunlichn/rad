#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/GuiEventHandler.h>

#include <rad/Common/String.h>
#include <rad/System/Application.h>

#include <atomic>
#include <chrono>
#include <optional>
#include <string>

namespace rad
{

// Snapshot of one connected display; populated by UpdateDisplayInfos().
// https://wiki.libsdl.org/SDL3/CategoryVideo
struct DisplayInfo
{
    SDL_DisplayID id = 0;
    std::string name;
    SDL_Rect bounds = {};
    // This is the same area as bounds, but with portions reserved by the system removed.
    SDL_Rect usableBounds = {};
    SDL_DisplayOrientation naturalOrientation = SDL_ORIENTATION_UNKNOWN;
    SDL_DisplayOrientation currentOrientation = SDL_ORIENTATION_UNKNOWN;
    // UI/content scale for this display (DPI / platform scaling).
    float scale = 0.0f;

    // https://wiki.libsdl.org/SDL3/SDL_GetDisplayProperties
    SDL_PropertiesID propID = 0;
    bool hdrEnabled = false;
    Sint64 kmsdrmOrientation = 0;

    std::vector<SDL_DisplayMode> modes;

    std::optional<SDL_DisplayMode> desktopMode;
    std::optional<SDL_DisplayMode> currentMode;

}; // struct DisplayInfo

// Battery / AC state from SDL_GetPowerInfo().
// https://wiki.libsdl.org/SDL3/SDL_GetPowerInfo
struct PowerInfo
{
    SDL_PowerState state = SDL_POWERSTATE_ERROR;
    int seconds = -1;
    int percent = -1;
};

// GuiApplication instance registered via Init(); accessed through GetInstance() while active.
class GuiApplication : public Application
{
public:
    enum class Status : int
    {
        Unknown,
        Initialized,
        Running,
        Exited,
    };

    GuiApplication();
    // Calls Destroy(), which invokes SDL_Quit when the application had been initialized.
    // https://wiki.libsdl.org/SDL3/SDL_Quit
    ~GuiApplication();

    // Process-wide accessor (Qt-style). Null before Init() and after Destroy().
    static GuiApplication* GetInstance();

    // Initializes the Application base class, then SDL (audio, video, and events). Registers this
    // object for GetInstance(). On success, logs runtime version, platform, revision, base path, and
    // current audio/video drivers, then calls UpdateDisplayInfos().
    // https://wiki.libsdl.org/SDL3/CategoryInit
    bool Init(int argc, char** argv);
    // Tears down SDL and unregisters this object from GetInstance(). Safe to call multiple times.
    // https://wiki.libsdl.org/SDL3/SDL_Quit
    void Destroy();
    // True after a successful Init().
    bool IsInitialized() const { return m_initialized; }
    // Runs until Exit() or no handlers: waits briefly for the next event (SDL_WaitEventTimeout),
    // then drains the queue with SDL_PollEvent, dispatches via OnEvent, and calls OnIdle().
    // https://wiki.libsdl.org/SDL3/CategoryEvents
    int Run();

    // Cached display list from the last UpdateDisplayInfos().
    const std::vector<DisplayInfo>& GetDisplayInfos() { return m_displays; }

    // https://wiki.libsdl.org/SDL3/SDL_WasInit
    SDL_InitFlags GetInitFlags() const;
    // https://wiki.libsdl.org/SDL3/SDL_WasInit
    bool IsSubsystemInitialized(SDL_InitFlags flags);

    // App metadata. SDL recommends calling SetAppMetadata as early as possible, ideally before SDL_Init().
    // https://wiki.libsdl.org/SDL3/SDL_SetAppMetadata
    bool SetAppMetadata(cstring_view appName, cstring_view appVersion, cstring_view appIdentifier);
    // SDL requires null-terminated strings for metadata properties.
    // https://wiki.libsdl.org/SDL3/SDL_SetAppMetadataProperty
    bool SetMetadataProperty(cstring_view name, cstring_view value);
    // https://wiki.libsdl.org/SDL3/SDL_GetAppMetadataProperty
    const char* GetMetadataProperty(cstring_view name);

    // https://wiki.libsdl.org/SDL3/SDL_InitSubSystem
    bool InitSubSystem(SDL_InitFlags flags);
    // https://wiki.libsdl.org/SDL3/SDL_QuitSubSystem
    void QuitSubSystem(SDL_InitFlags flags);

    // https://wiki.libsdl.org/SDL3/SDL_IsMainThread
    bool IsMainThread();
    // https://wiki.libsdl.org/SDL3/SDL_RunOnMainThread
    bool RunOnMainThread(SDL_MainThreadCallback callback, void* userData, bool waitComplete);

    // Not thread-safe. Expected to be called on the main thread.
    // Registered handlers will be included on the next dispatch.
    void RegisterEventHandler(GuiEventHandler* handler);
    void UnregisterEventHandler(GuiEventHandler* handler);
    // Returns true on success, or false if the event was filtered or the queue push failed.
    // https://wiki.libsdl.org/SDL3/SDL_PushEvent
    bool PushEvent(SDL_Event& event);
    // Dispatches to handlers and handles built-in lifecycle/display events.
    // https://wiki.libsdl.org/SDL3/CategoryEvents
    void OnEvent(const SDL_Event& event);
    void OnIdle();

    // https://wiki.libsdl.org/SDL3/SDL_PumpEvents
    void PumpEvents();
    // https://wiki.libsdl.org/SDL3/SDL_PollEvent
    bool PollEvent(SDL_Event* outEvent);
    // Delegates to PollEvent(SDL_Event*).
    bool PollEvent(SDL_Event& outEvent) { return PollEvent(&outEvent); }
    // https://wiki.libsdl.org/SDL3/SDL_WaitEvent
    bool WaitEvent(SDL_Event* outEvent);
    // Delegates to WaitEvent(SDL_Event*).
    bool WaitEvent(SDL_Event& outEvent) { return WaitEvent(&outEvent); }
    // https://wiki.libsdl.org/SDL3/SDL_WaitEventTimeout
    bool WaitEventTimeout(SDL_Event* outEvent, Sint32 timeoutMs);
    // Delegates to WaitEventTimeout(SDL_Event*, timeoutMs).
    bool WaitEventTimeout(SDL_Event& outEvent, Sint32 timeoutMs)
    {
        return WaitEventTimeout(&outEvent, timeoutMs);
    }
    // https://wiki.libsdl.org/SDL3/SDL_HasEvent
    bool HasEvent(Uint32 type);
    // https://wiki.libsdl.org/SDL3/SDL_FlushEvent
    void FlushEvent(Uint32 type);
    // https://wiki.libsdl.org/SDL3/SDL_FlushEvents
    void FlushEvents(Uint32 minType, Uint32 maxType);

    // Application-wide quit: queues SDL_EVENT_QUIT via PushEvent so Run() stops.
    // To tear down one window, call that window's Destroy(); the default OnCloseRequested() calls
    // Destroy() on the window.
    // https://wiki.libsdl.org/SDL3/SDL_PushEvent
    bool RequestQuit();

    void SetStatus(Status status) { m_status.store(status); }
    Status GetStatus() const { return m_status.load(); }
    void SetErrorCode(int errCode) { m_errCode.store(errCode); }
    int GetErrorCode() const { return m_errCode.load(); }

    // Signals Run() to stop.
    void Exit(int errCode);

    // SDL3 timer / high-resolution clock.
    // https://wiki.libsdl.org/SDL3/SDL_GetTicks
    [[nodiscard]] static Uint64 GetTicksInMilliseconds();
    // https://wiki.libsdl.org/SDL3/SDL_GetTicksNS
    [[nodiscard]] static Uint64 GetTicksInNanoseconds();
    // https://wiki.libsdl.org/SDL3/SDL_GetPerformanceCounter
    [[nodiscard]] static Uint64 GetPerformanceCounter();
    // https://wiki.libsdl.org/SDL3/SDL_GetPerformanceFrequency
    [[nodiscard]] static Uint64 GetPerformanceFrequency();

    /// Waits at least `duration` (may sleep longer depending on the OS). Uses `SDL_DelayNS`.
    // https://wiki.libsdl.org/SDL3/SDL_DelayNS
    template <typename Rep, typename Period>
    static void Delay(std::chrono::duration<Rep, Period> duration)
    {
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        const auto count = ns.count();
        if (count <= 0)
        {
            return;
        }
        SDL_DelayNS(static_cast<Uint64>(count));
    }

    /// May busy-wait (`SDL_DelayPrecise`).
    // https://wiki.libsdl.org/SDL3/SDL_DelayPrecise
    static void DelayPrecise(std::chrono::nanoseconds duration);

    // https://wiki.libsdl.org/SDL3/SDL_ScreenSaverEnabled
    bool IsScreenSaverEnabled();
    // https://wiki.libsdl.org/SDL3/SDL_EnableScreenSaver
    bool EnableScreenSaver();
    // https://wiki.libsdl.org/SDL3/SDL_DisableScreenSaver
    bool DisableScreenSaver();

    // Put UTF-8 text into the clipboard.
    // https://wiki.libsdl.org/SDL3/SDL_SetClipboardText
    bool SetClipboardText(const char* text);
    // https://wiki.libsdl.org/SDL3/SDL_GetClipboardText
    std::string GetClipboardText();
    // https://wiki.libsdl.org/SDL3/SDL_HasClipboardText
    bool HasClipboardText();

    // Put UTF-8 text into the primary selection.
    // https://wiki.libsdl.org/SDL3/SDL_SetPrimarySelectionText
    bool SetPrimarySelectionText(const char* text);
    // https://wiki.libsdl.org/SDL3/SDL_GetPrimarySelectionText
    std::string GetPrimarySelectionText();
    // https://wiki.libsdl.org/SDL3/SDL_HasPrimarySelectionText
    bool HasPrimarySelectionText();

    // Tell the operating system that the application is offering clipboard data
    // for each of the provided mime types.
    // https://wiki.libsdl.org/SDL3/SDL_SetClipboardData
    bool SetClipboardData(SDL_ClipboardDataCallback callback, SDL_ClipboardCleanupCallback cleanup,
                          void* userData, const char** mimeTypes, size_t mimeTypeCount);
    // https://wiki.libsdl.org/SDL3/SDL_ClearClipboardData
    bool ClearClipboardData();
    // https://wiki.libsdl.org/SDL3/SDL_GetClipboardData
    const void* GetClipboardData(const char* mimeType, size_t* size);
    // https://wiki.libsdl.org/SDL3/SDL_HasClipboardData
    bool HasClipboardData(const char* mimeType);

    // Filesystem / platform.
    // https://wiki.libsdl.org/SDL3/SDL_GetBasePath
    std::string GetBasePath();
    // Preference directory from SDL_GetPrefPath; copies the path and frees the SDL-allocated buffer with SDL_free.
    // https://wiki.libsdl.org/SDL3/CategoryFilesystem
    std::string GetPrefPath(const char* org, const char* app);

    // https://wiki.libsdl.org/SDL3/SDL_GetPowerInfo
    PowerInfo GetPowerInfo();

    // https://wiki.libsdl.org/SDL3/SDL_ShowSimpleMessageBox
    bool ShowSimpleMessageBox(Uint32 flags, cstring_view title, cstring_view message,
                              SDL_Window* parent = nullptr);

private:
    // Refreshes m_displays: enumerates displays, then for each display queries name, bounds, usable bounds,
    // orientations, content scale, display properties (HDR, KMSDRM orientation), fullscreen modes,
    // and desktop/current modes.
    // https://wiki.libsdl.org/SDL3/CategoryVideo
    void UpdateDisplayInfos();

    std::vector<GuiEventHandler*> m_eventHandlers;

    bool m_initialized = false;
    std::atomic<Status> m_status = Status::Unknown;
    std::atomic_int m_errCode = 0;

    std::vector<DisplayInfo> m_displays;

}; // class GuiApplication

} // namespace rad
