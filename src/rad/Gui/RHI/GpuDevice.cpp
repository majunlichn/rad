#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuDevice.h>

#include <rad/Gui/Window.h>

#include <cassert>

namespace rad
{

bool GpuDevice::SupportsShaderFormats(SDL_GPUShaderFormat formatFlags, const char* driverName)
{
    return SDL_GPUSupportsShaderFormats(formatFlags, driverName);
}

bool GpuDevice::SupportsProperties(SDL_PropertiesID props)
{
    return SDL_GPUSupportsProperties(props);
}

Ref<GpuDevice> GpuDevice::Create(SDL_GPUShaderFormat formatFlags, bool debugMode,
                                 const char* driverName)
{
    if (SDL_GPUDevice* handle = SDL_CreateGPUDevice(formatFlags, debugMode, driverName))
    {
        return RAD_NEW GpuDevice(handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUDevice failed: {}", SDL_GetError());
    return nullptr;
}

Ref<GpuDevice> GpuDevice::CreateWithProperties(SDL_PropertiesID props)
{
    if (SDL_GPUDevice* handle = SDL_CreateGPUDeviceWithProperties(props))
    {
        return RAD_NEW GpuDevice(handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUDeviceWithProperties failed: {}", SDL_GetError());
    return nullptr;
}

int GpuDevice::GetNumDrivers()
{
    return SDL_GetNumGPUDrivers();
}

const char* GpuDevice::GetDriver(int index)
{
    return SDL_GetGPUDriver(index);
}

GpuDevice::GpuDevice(SDL_GPUDevice* handle) :
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuDevice::~GpuDevice()
{
    Destroy();
}

void GpuDevice::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    SDL_DestroyGPUDevice(m_handle);
    m_handle = nullptr;
}

const char* GpuDevice::GetDeviceDriver() const
{
    return SDL_GetGPUDeviceDriver(m_handle);
}

SDL_GPUShaderFormat GpuDevice::GetShaderFormats() const
{
    return SDL_GetGPUShaderFormats(m_handle);
}

SDL_PropertiesID GpuDevice::GetProperties() const
{
    return SDL_GetGPUDeviceProperties(m_handle);
}

bool GpuDevice::TextureSupportsFormat(SDL_GPUTextureFormat format, SDL_GPUTextureUsageFlags usage,
                                      SDL_GPUTextureType type) const
{
    return SDL_GPUTextureSupportsFormat(m_handle, format, type, usage);
}

bool GpuDevice::FormatSupportsSampleCount(SDL_GPUTextureFormat format,
                                          SDL_GPUSampleCount sampleCount) const
{
    return SDL_GPUTextureSupportsSampleCount(m_handle, format, sampleCount);
}

Uint32 GpuDevice::TextureFormatTexelBlockSize(SDL_GPUTextureFormat format)
{
    return SDL_GPUTextureFormatTexelBlockSize(format);
}

Uint32 GpuDevice::CalculateTextureFormatSize(SDL_GPUTextureFormat format, Uint32 width,
                                             Uint32 height, Uint32 depthOrLayerCount)
{
    return SDL_CalculateGPUTextureFormatSize(format, width, height, depthOrLayerCount);
}

#if SDL_VERSION_ATLEAST(3, 4, 0)

SDL_PixelFormat GpuDevice::GetPixelFormatFromTextureFormat(SDL_GPUTextureFormat format)
{
    return SDL_GetPixelFormatFromGPUTextureFormat(format);
}

SDL_GPUTextureFormat GpuDevice::GetTextureFormatFromPixelFormat(SDL_PixelFormat format)
{
    return SDL_GetGPUTextureFormatFromPixelFormat(format);
}

#endif

#ifdef SDL_PLATFORM_GDK

void GpuDevice::GDKSuspendGPU()
{
    if (m_handle)
    {
        SDL_GDKSuspendGPU(m_handle);
    }
}

void GpuDevice::GDKResumeGPU()
{
    if (m_handle)
    {
        SDL_GDKResumeGPU(m_handle);
    }
}

#endif

bool GpuDevice::ClaimWindow(SDL_Window* window)
{
    return RAD_SDL_CHECK_GUI(SDL_ClaimWindowForGPUDevice(m_handle, window));
}

bool GpuDevice::ClaimWindow(Window* window)
{
    return ClaimWindow(window->GetHandle());
}

void GpuDevice::ReleaseWindow(SDL_Window* window)
{
    assert(m_handle != nullptr);
    assert(window != nullptr);
    SDL_ReleaseWindowFromGPUDevice(m_handle, window);
}

void GpuDevice::ReleaseWindow(Window* window)
{
    ReleaseWindow(window->GetHandle());
}

bool GpuDevice::SetSwapchainParameters(SDL_Window* window, SDL_GPUSwapchainComposition composition,
                                       SDL_GPUPresentMode presentMode)
{
    return RAD_SDL_CHECK_GUI(
        SDL_SetGPUSwapchainParameters(m_handle, window, composition, presentMode));
}

bool GpuDevice::SetAllowedFramesInFlight(Uint32 count)
{
    return RAD_SDL_CHECK_GUI(SDL_SetGPUAllowedFramesInFlight(m_handle, count));
}

SDL_GPUTextureFormat GpuDevice::GetSwapchainTextureFormat(SDL_Window* window) const
{
    return SDL_GetGPUSwapchainTextureFormat(m_handle, window);
}

bool GpuDevice::WindowSupportsPresentMode(SDL_Window* window, SDL_GPUPresentMode mode) const
{
    return SDL_WindowSupportsGPUPresentMode(m_handle, window, mode);
}

bool GpuDevice::WindowSupportsSwapchainComposition(SDL_Window* window,
                                                   SDL_GPUSwapchainComposition composition) const
{
    return SDL_WindowSupportsGPUSwapchainComposition(m_handle, window, composition);
}

bool GpuDevice::WaitForSwapchain(SDL_Window* window)
{
    return RAD_SDL_CHECK_GUI(SDL_WaitForGPUSwapchain(m_handle, window));
}

bool GpuDevice::WaitIdle()
{
    return RAD_SDL_CHECK_GUI(SDL_WaitForGPUIdle(m_handle));
}

bool GpuDevice::WaitForFences(bool waitAll, SDL_GPUFence* const* fences, Uint32 numFences)
{
    return RAD_SDL_CHECK_GUI(SDL_WaitForGPUFences(m_handle, waitAll, fences, numFences));
}

GpuCommandBuffer GpuDevice::AcquireCommandBuffer()
{
    if (SDL_GPUCommandBuffer* buffer = SDL_AcquireGPUCommandBuffer(m_handle))
    {
        return GpuCommandBuffer(Ref<GpuDevice>(this), buffer);
    }
    RAD_LOG_GUI(err, "SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError());
    return GpuCommandBuffer();
}

} // namespace rad
