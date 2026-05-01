#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/GuiEventHandler.h>

#include <rad/Common/String.h>
#include <rad/System/Application.h>

#include <atomic>
#include <mutex>
#include <string>

namespace rad
{

struct DisplayInfo
{
    SDL_DisplayID id;
    std::string name;
    SDL_Rect bounds;
    // This is the same area as bounds, but with portions reserved by the system removed.
    SDL_Rect usableBounds;
    SDL_DisplayOrientation naturalOrientation;
    SDL_DisplayOrientation currentOrientation;
    // The content scale is the expected scale for content based on the DPI settings of the display.
    float scale;

    // https://wiki.libsdl.org/SDL3/SDL_GetDisplayProperties
    SDL_PropertiesID propID;
    bool hdrEnabled;
    float sdrWhitePoint;
    float hdrHeadroom;
    Sint64 kmsdrmOrientation;

    std::vector<SDL_DisplayMode> modes;

    bool hasDesktopMode = false;
    SDL_DisplayMode desktopMode = {};

    bool hasCurrentMode = false;
    SDL_DisplayMode currentMode = {};

}; // struct DisplayInfo

struct PowerInfo
{
    SDL_PowerState state = SDL_POWERSTATE_ERROR;
    int seconds = -1;
    int percent = -1;
};

// Make sure only one instance of GuiApplication exists.
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

    bool Init(int argc, char** argv);
    void Destroy();

    const std::vector<DisplayInfo>& GetDisplayInfos() { return m_displays; }

    SDL_InitFlags GetInitFlags() const;
    bool IsSubsystemInitialized(SDL_InitFlags flags);

    bool SetMetadataProperty(std::string_view name, std::string_view value);
    const char* GetMetadataProperty(std::string_view name);

    // SDL init/subsystem helpers.
    bool InitSubSystem(SDL_InitFlags flags);
    void QuitSubSystem(SDL_InitFlags flags);

    bool IsMainThread();
    bool RunOnMainThread(SDL_MainThreadCallback callback, void* userData, bool waitComplete);

    void RegisterEventHandler(GuiEventHandler* handler);
    void UnregisterEventHandler(GuiEventHandler* handler);
    // Return true on success; false if the event is filtered or on failure (event queue being full).
    bool PushEvent(SDL_Event& event);
    void OnEvent(const SDL_Event& event);
    void OnIdle();

    // Event loop utilities.
    void PumpEvents();
    bool PollEvent(SDL_Event* outEvent);
    bool WaitEventTimeout(SDL_Event* outEvent, Sint32 timeoutMs);

    void SetStatus(Status status) { m_status = status; }
    Status GetStatus() { return m_status; }
    void SetErrorCode(int errCode) { m_errCode = errCode; }
    int GetErrorCode() const { return m_errCode; }

    void Exit(int errCode);

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

    std::mutex m_eventMutex;
    std::vector<GuiEventHandler*> m_eventHandlers;

    bool m_initialized = false;
    std::atomic<Status> m_status = Status::Unknown;
    std::atomic_int m_errCode = 0;

    std::vector<DisplayInfo> m_displays;

}; // class GuiApplication

} // namespace rad
