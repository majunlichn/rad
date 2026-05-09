#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <SDL3/SDL_camera.h>

#include <vector>

namespace rad
{

class Camera;

// SDL_INIT_CAMERA subsystem and camera discovery.
// https://wiki.libsdl.org/SDL3/CategoryCamera
class CameraSystem
{
public:
    // Meyers' singleton; does not call SDL by itself.
    static CameraSystem& GetInstance();

    // https://wiki.libsdl.org/SDL3/SDL_InitSubSystem
    bool Init();
    // https://wiki.libsdl.org/SDL3/SDL_QuitSubSystem
    void Shutdown();

    // Tracks successful SDL_InitSubSystem(SDL_INIT_CAMERA).
    bool IsInitialized() const { return m_initialized; }

    // SDL_GetNumCameraDrivers / SDL_GetCameraDriver.
    // https://wiki.libsdl.org/SDL3/SDL_GetNumCameraDrivers
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraDriver
    std::vector<const char*> GetCameraDrivers();
    // https://wiki.libsdl.org/SDL3/SDL_GetCurrentCameraDriver
    const char* GetCurrentCameraDriver();

    // https://wiki.libsdl.org/SDL3/SDL_GetCameras
    std::vector<Ref<Camera>> GetCameras();

private:
    CameraSystem();
    ~CameraSystem();
    bool m_initialized = false;

}; // class CameraSystem

// Per-device wrapper around SDL_Camera / SDL_CameraID.
// https://wiki.libsdl.org/SDL3/CategoryCamera
class Camera : public RefCounted<Camera>
{
public:
    // SDL_GetCameraName, SDL_GetCameraSupportedFormats, SDL_GetCameraPosition.
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraName
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraSupportedFormats
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraPosition
    Camera(SDL_CameraID id);
    // Calls Close(); SDL_CloseCamera when the device was open.
    // https://wiki.libsdl.org/SDL3/SDL_CloseCamera
    ~Camera();

    // https://wiki.libsdl.org/SDL3/SDL_CameraID
    SDL_CameraID GetId() const { return m_id; }
    // https://wiki.libsdl.org/SDL3/SDL_Camera
    SDL_Camera* GetHandle() const { return m_handle; }
    // Cached from SDL_GetCameraProperties after a successful Open().
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraProperties
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    // True for a non-zero SDL_CameraID (as returned by SDL_GetCameras).
    // https://wiki.libsdl.org/SDL3/SDL_CameraID
    bool IsValid() const { return m_id != 0; }
    // Delegates to IsValid().
    explicit operator bool() const { return IsValid(); }
    // True when SDL_OpenCamera produced a non-null SDL_Camera.
    // https://wiki.libsdl.org/SDL3/SDL_OpenCamera
    bool IsOpen() const { return m_handle != nullptr; }

    // Cached in constructor from SDL_GetCameraName.
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraName
    const std::string& GetName() const { return m_name; }
    // Cached in constructor from SDL_GetCameraSupportedFormats (copied SDL_CameraSpec values).
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraSupportedFormats
    Span<const SDL_CameraSpec> GetSupportedFormats() const
    {
        return Span<const SDL_CameraSpec>(m_supportedFormats.data(), m_supportedFormats.size());
    }
    // Cached in constructor from SDL_GetCameraPosition.
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraPosition
    SDL_CameraPosition GetPosition() const { return m_position; }

    // SDL_OpenCamera; on success caches SDL_GetCameraProperties (and SDL_GetCameraID in debug).
    // https://wiki.libsdl.org/SDL3/SDL_OpenCamera
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraProperties
    bool Open(const SDL_CameraSpec* spec = nullptr);
    // https://wiki.libsdl.org/SDL3/SDL_CloseCamera
    void Close();

    // https://wiki.libsdl.org/SDL3/SDL_GetCameraPermissionState
    SDL_CameraPermissionState GetPermissionState() const;
    // https://wiki.libsdl.org/SDL3/SDL_GetCameraFormat
    bool GetFormat(SDL_CameraSpec* spec);

    // https://wiki.libsdl.org/SDL3/SDL_AcquireCameraFrame
    // Do not call SDL_DestroySurface() on the returned surface!
    SDL_Surface* AcquireFrame(Uint64* timestamp);
    // https://wiki.libsdl.org/SDL3/SDL_ReleaseCameraFrame
    void ReleaseFrame(SDL_Surface* surface);

private:
    SDL_CameraID m_id = 0;
    std::string m_name;
    std::vector<SDL_CameraSpec> m_supportedFormats;
    SDL_CameraPosition m_position;
    // The opaque structure used to identify an opened SDL camera.
    SDL_Camera* m_handle = nullptr;
    SDL_PropertiesID m_propertiesId = 0;

}; // class Camera

} // namespace rad
