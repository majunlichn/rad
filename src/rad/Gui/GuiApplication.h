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

struct PowerInfo
{
    SDL_PowerState state = SDL_POWERSTATE_ERROR;
    int seconds = -1;
    int percent = -1;
};

// Singleton: only one instance of GuiApplication exists.
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
    ~GuiApplication();

    static GuiApplication* GetInstance();

    bool Init(int argc, char** argv);
    void Destroy();
    bool IsInitialized() const { return m_initialized; }
    // Explicit shutdown for tests/tools; safe to call multiple times.
    void Shutdown() { Destroy(); }
    // Run the SDL event loop until Exit() is called or all handlers are gone.
    int Run();

    const std::vector<DisplayInfo>& GetDisplayInfos() { return m_displays; }

    SDL_InitFlags GetInitFlags() const;
    bool IsSubsystemInitialized(SDL_InitFlags flags);

    // App metadata.
    // Note: SDL recommends calling this as early as possible, ideally before SDL_Init().
    bool SetAppMetadata(cstring_view appName, cstring_view appVersion, cstring_view appIdentifier);
    // SDL requires null-terminated strings for metadata properties.
    bool SetMetadataProperty(cstring_view name, cstring_view value);
    const char* GetMetadataProperty(cstring_view name);

    // SDL init/subsystem helpers.
    bool InitSubSystem(SDL_InitFlags flags);
    void QuitSubSystem(SDL_InitFlags flags);

    bool IsMainThread();
    bool RunOnMainThread(SDL_MainThreadCallback callback, void* userData, bool waitComplete);

    // Not thread-safe. Expected to be called on the main thread.
    // Registered handlers will be included on the next dispatch.
    void RegisterEventHandler(GuiEventHandler* handler);
    void UnregisterEventHandler(GuiEventHandler* handler);
    // Return true on success; false if filtered or if pushing fails (for example, queue full).
    bool PushEvent(SDL_Event& event);
    void OnEvent(const SDL_Event& event);
    void OnIdle();

    // Event loop utilities.
    void PumpEvents();
    bool PollEvent(SDL_Event* outEvent);
    bool PollEvent(SDL_Event& outEvent) { return PollEvent(&outEvent); }
    bool WaitEvent(SDL_Event* outEvent);
    bool WaitEvent(SDL_Event& outEvent) { return WaitEvent(&outEvent); }
    bool WaitEventTimeout(SDL_Event* outEvent, Sint32 timeoutMs);
    bool WaitEventTimeout(SDL_Event& outEvent, Sint32 timeoutMs)
    {
        return WaitEventTimeout(&outEvent, timeoutMs);
    }
    bool HasEvent(Uint32 type);
    void FlushEvent(Uint32 type);
    void FlushEvents(Uint32 minType, Uint32 maxType);

    // Application-wide quit: pushes SDL_EVENT_QUIT and stops Run(). To tear down one window,
    // call that window's Destroy(); the default OnCloseRequested() does the same via Destroy().
    bool RequestQuit();

    void SetStatus(Status status) { m_status.store(status); }
    Status GetStatus() const { return m_status.load(); }
    void SetErrorCode(int errCode) { m_errCode.store(errCode); }
    int GetErrorCode() const { return m_errCode.load(); }

    void Exit(int errCode);

    // SDL3 timer / high-resolution clock (see https://wiki.libsdl.org/SDL3/CategoryTimer).
    static [[nodiscard]] Uint64 GetTicksInMilliseconds();
    static [[nodiscard]] Uint64 GetTicksInNanoseconds();
    static [[nodiscard]] Uint64 GetPerformanceCounter();
    static [[nodiscard]] Uint64 GetPerformanceFrequency();

    /// Waits at least `duration` (may sleep longer depending on the OS). Uses `SDL_DelayNS`.
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
    static void DelayPrecise(std::chrono::nanoseconds duration);

    bool IsScreenSaverEnabled();
    bool EnableScreenSaver();
    bool DisableScreenSaver();

    // Put UTF-8 text into the clipboard.
    bool SetClipboardText(const char* text);
    std::string GetClipboardText();
    bool HasClipboardText();

    // Put UTF-8 text into the primary selection.
    bool SetPrimarySelectionText(const char* text);
    std::string GetPrimarySelectionText();
    bool HasPrimarySelectionText();

    // Tell the operating system that the application is offering clipboard data
    // for each of the provided mime types.
    bool SetClipboardData(SDL_ClipboardDataCallback callback, SDL_ClipboardCleanupCallback cleanup,
                          void* userData, const char** mimeTypes, size_t mimeTypeCount);
    bool ClearClipboardData();
    const void* GetClipboardData(const char* mimeType, size_t* size);
    bool HasClipboardData(const char* mimeType);

    // Filesystem / platform.
    std::string GetBasePath();
    std::string GetPrefPath(const char* org, const char* app);

    PowerInfo GetPowerInfo();

    bool ShowSimpleMessageBox(Uint32 flags, cstring_view title, cstring_view message,
                              SDL_Window* parent = nullptr);

private:
    void UpdateDisplayInfos();

    std::vector<GuiEventHandler*> m_eventHandlers;

    bool m_initialized = false;
    std::atomic<Status> m_status = Status::Unknown;
    std::atomic_int m_errCode = 0;

    std::vector<DisplayInfo> m_displays;

}; // class GuiApplication

} // namespace rad
