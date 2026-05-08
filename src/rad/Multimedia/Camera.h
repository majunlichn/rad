#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <SDL3/SDL_camera.h>

#include <vector>

namespace rad
{

class Camera;

class CameraSystem
{
public:
    static CameraSystem& GetInstance();

    bool Init();
    void Shutdown();

    bool IsInitialized() const { return m_initialized; }

    std::vector<const char*> GetCameraDrivers();
    const char* GetCurrentCameraDriver();

    std::vector<Ref<Camera>> GetCameras();

private:
    CameraSystem();
    ~CameraSystem();
    bool m_initialized = false;

}; // class CameraSystem

// Simple wrapper for SDL3 camera APIs.
class Camera : public RefCounted<Camera>
{
public:
    Camera(SDL_CameraID id);
    ~Camera();

    SDL_CameraID GetId() const { return m_id; }
    SDL_Camera* GetHandle() const { return m_handle; }
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    bool IsValid() const { return m_id != 0; }
    explicit operator bool() const { return IsValid(); }
    bool IsOpen() const { return m_handle != nullptr; }

    const std::string& GetName() const { return m_name; }
    Span<const SDL_CameraSpec> GetSupportedFormats() const
    {
        return Span<const SDL_CameraSpec>(m_supportedFormats.data(), m_supportedFormats.size());
    }
    SDL_CameraPosition GetPosition() const { return m_position; }

    bool Open(const SDL_CameraSpec* spec = nullptr);
    void Close();

    SDL_CameraPermissionState GetPermissionState() const;
    bool GetFormat(SDL_CameraSpec* spec);

    // Do not call SDL_DestroySurface() on the returned surface!
    SDL_Surface* AcquireFrame(Uint64* timestamp);
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
