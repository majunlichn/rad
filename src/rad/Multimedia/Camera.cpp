#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/Camera.h>

namespace rad
{

CameraSystem::CameraSystem()
{
}

CameraSystem::~CameraSystem()
{
}

CameraSystem& CameraSystem::GetInstance()
{
    static CameraSystem cameraSystem;
    return cameraSystem;
}

bool CameraSystem::Init()
{
    m_initialized = SDL_InitSubSystem(SDL_INIT_CAMERA);
    if (!m_initialized)
    {
        RAD_LOG_MULTIMEDIA(err, "Failed to init CameraSystem: {}", SDL_GetError());
    }
    return m_initialized;
}

void CameraSystem::Shutdown()
{
    if (m_initialized)
    {
        SDL_QuitSubSystem(SDL_INIT_CAMERA);
        m_initialized = false;
    }
}

std::vector<const char*> CameraSystem::GetCameraDrivers()
{
    std::vector<const char*> drivers;
    int count = SDL_GetNumCameraDrivers();
    for (int i = 0; i < count; ++i)
    {
        const char* driver = SDL_GetCameraDriver(i);
        drivers.push_back(driver);
    }
    return drivers;
}

const char* CameraSystem::GetCurrentCameraDriver()
{
    return SDL_GetCurrentCameraDriver();
}

std::vector<Ref<Camera>> CameraSystem::GetCameras()
{
    std::vector<Ref<Camera>> cameras;
    int count = 0;
    SDL_ClearError();
    SDL_CameraID* ids = SDL_GetCameras(&count);
    if (ids)
    {
        if (count > 0)
        {
            cameras.resize(count);
            for (int i = 0; i < count; ++i)
            {
                cameras[i] = RAD_NEW Camera(ids[i]);
            }
        }
        SDL_free(ids);
    }
    else if (!ids)
    {
        const char* err = SDL_GetError();
        if (err && *err)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetCameras failed: {}", err);
        }
    }
    return cameras;
}

Camera::Camera(SDL_CameraID id) :
    m_id(id)
{
    const char* name = SDL_GetCameraName(id);
    if (name)
    {
        m_name = name;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetCameraName failed: {}", SDL_GetError());
    }
    SDL_ClearError();
    int specCount = 0;
    SDL_CameraSpec** specs = SDL_GetCameraSupportedFormats(id, &specCount);
    if (!specs)
    {
        // SDL allows returning NULL with a count of 0 for an empty list (not an error).
        const char* err = SDL_GetError();
        if (err && *err)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetCameraSupportedFormats failed: {}", err);
        }
        specCount = 0;
    }
    else
    {
        m_supportedFormats.reserve(specCount > 0 ? static_cast<size_t>(specCount) : 0u);
        for (int i = 0; i < specCount; ++i)
        {
            if (specs[i])
            {
                m_supportedFormats.push_back(*specs[i]); // copy value type
            }
        }
        SDL_free(specs);
    }
    m_position = SDL_GetCameraPosition(id);
}

Camera::~Camera()
{
    Close();
}

bool Camera::Open(const SDL_CameraSpec* spec)
{
    assert(m_handle == nullptr);

    m_handle = SDL_OpenCamera(m_id, spec);
    if (m_handle)
    {
#if defined(_DEBUG)
        SDL_CameraID id = SDL_GetCameraID(m_handle);
        assert(id == m_id);
#endif
        RAD_LOG_MULTIMEDIA(info, "Camera {} opened successfully.", m_name);
        m_propertiesId = SDL_GetCameraProperties(m_handle);
        if (m_propertiesId == 0)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetCameraProperties failed: {}", SDL_GetError());
        }
        return true;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_OpenCamera failed: {}", SDL_GetError());
        return false;
    }
}

void Camera::Close()
{
    if (m_handle)
    {
        SDL_CloseCamera(m_handle);
        m_handle = nullptr;
        m_propertiesId = 0;
    }
}

SDL_CameraPermissionState Camera::GetPermissionState() const
{
    assert(m_handle);
    return SDL_GetCameraPermissionState(m_handle);
}

bool Camera::GetFormat(SDL_CameraSpec* spec)
{
    const bool result = SDL_GetCameraFormat(m_handle, spec);
    if (result)
    {
        return true;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetCameraFormat failed: {}", SDL_GetError());
        return false;
    }
}

SDL_Surface* Camera::AcquireFrame(Uint64* timestamp)
{
    assert(m_handle);
    SDL_Surface* frame = SDL_AcquireCameraFrame(m_handle, timestamp);
    if (!frame)
    {
        const char* err = SDL_GetError();
        if (err && *err)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_AcquireCameraFrame failed: {}", err);
        }
    }
    return frame;
}

void Camera::ReleaseFrame(SDL_Surface* surface)
{
    assert(m_handle && surface);
    SDL_ReleaseCameraFrame(m_handle, surface);
}

} // namespace rad
