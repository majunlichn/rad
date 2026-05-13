#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuCommandBuffer.h>

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_version.h>

namespace rad
{

class Window;

// Cross-backend GPU context (Vulkan, D3D12, Metal). Threading rules follow SDL_GPU.
// https://wiki.libsdl.org/SDL3/CategoryGPU
class GpuDevice : public RefCounted<GpuDevice>
{
public:
    // https://wiki.libsdl.org/SDL3/SDL_GPUSupportsShaderFormats
    static bool SupportsShaderFormats(SDL_GPUShaderFormat formatFlags, const char* driverName);
    // https://wiki.libsdl.org/SDL3/SDL_GPUSupportsProperties
    static bool SupportsProperties(SDL_PropertiesID props);

    // https://wiki.libsdl.org/SDL3/SDL_CreateGPUDevice
    static Ref<GpuDevice> Create(SDL_GPUShaderFormat formatFlags, bool debugMode,
                                 const char* driverName);
    // https://wiki.libsdl.org/SDL3/SDL_CreateGPUDeviceWithProperties
    static Ref<GpuDevice> CreateWithProperties(SDL_PropertiesID props);

    // https://wiki.libsdl.org/SDL3/SDL_GetNumGPUDrivers
    static int GetNumDrivers();
    // https://wiki.libsdl.org/SDL3/SDL_GetGPUDriver
    static const char* GetDriver(int index);

    ~GpuDevice();
    // https://wiki.libsdl.org/SDL3/SDL_DestroyGPUDevice
    void Destroy();

    SDL_GPUDevice* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_GetGPUDeviceDriver
    const char* GetDeviceDriver() const;
    // https://wiki.libsdl.org/SDL3/SDL_GetGPUShaderFormats
    SDL_GPUShaderFormat GetShaderFormats() const;
    // https://wiki.libsdl.org/SDL3/SDL_GetGPUDeviceProperties
    SDL_PropertiesID GetProperties() const;

    // https://wiki.libsdl.org/SDL3/SDL_GPUTextureSupportsFormat
    bool TextureSupportsFormat(SDL_GPUTextureFormat format, SDL_GPUTextureUsageFlags usage,
                               SDL_GPUTextureType type = SDL_GPU_TEXTURETYPE_2D) const;
    // https://wiki.libsdl.org/SDL3/SDL_GPUTextureSupportsSampleCount
    bool FormatSupportsSampleCount(SDL_GPUTextureFormat format,
                                   SDL_GPUSampleCount sampleCount) const;

    // https://wiki.libsdl.org/SDL3/SDL_GPUTextureFormatTexelBlockSize
    static Uint32 TextureFormatTexelBlockSize(SDL_GPUTextureFormat format);
    // https://wiki.libsdl.org/SDL3/SDL_CalculateGPUTextureFormatSize
    static Uint32 CalculateTextureFormatSize(SDL_GPUTextureFormat format, Uint32 width,
                                             Uint32 height, Uint32 depthOrLayerCount);

#if SDL_VERSION_ATLEAST(3, 4, 0)
    // https://wiki.libsdl.org/SDL3/SDL_GetPixelFormatFromGPUTextureFormat
    static SDL_PixelFormat GetPixelFormatFromTextureFormat(SDL_GPUTextureFormat format);
    // https://wiki.libsdl.org/SDL3/SDL_GetGPUTextureFormatFromPixelFormat
    static SDL_GPUTextureFormat GetTextureFormatFromPixelFormat(SDL_PixelFormat format);
#endif

#ifdef SDL_PLATFORM_GDK
    // https://wiki.libsdl.org/SDL3/SDL_GDKSuspendGPU
    void GDKSuspendGPU();
    // https://wiki.libsdl.org/SDL3/SDL_GDKResumeGPU
    void GDKResumeGPU();
#endif

    // https://wiki.libsdl.org/SDL3/SDL_ClaimWindowForGPUDevice
    bool ClaimWindow(SDL_Window* window);
    bool ClaimWindow(Window* window);
    // https://wiki.libsdl.org/SDL3/SDL_ReleaseWindowFromGPUDevice
    void ReleaseWindow(SDL_Window* window);
    void ReleaseWindow(Window* window);

    // https://wiki.libsdl.org/SDL3/SDL_SetGPUSwapchainParameters
    bool SetSwapchainParameters(SDL_Window* window, SDL_GPUSwapchainComposition composition,
                                SDL_GPUPresentMode presentMode);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUAllowedFramesInFlight
    bool SetAllowedFramesInFlight(Uint32 count);
    // https://wiki.libsdl.org/SDL3/SDL_GetGPUSwapchainTextureFormat
    SDL_GPUTextureFormat GetSwapchainTextureFormat(SDL_Window* window) const;

    // https://wiki.libsdl.org/SDL3/SDL_WindowSupportsGPUPresentMode
    bool WindowSupportsPresentMode(SDL_Window* window, SDL_GPUPresentMode mode) const;
    // https://wiki.libsdl.org/SDL3/SDL_WindowSupportsGPUSwapchainComposition
    bool WindowSupportsSwapchainComposition(SDL_Window* window,
                                            SDL_GPUSwapchainComposition composition) const;

    // https://wiki.libsdl.org/SDL3/SDL_WaitForGPUSwapchain
    bool WaitForSwapchain(SDL_Window* window);
    // https://wiki.libsdl.org/SDL3/SDL_WaitForGPUIdle
    bool WaitIdle();
    // https://wiki.libsdl.org/SDL3/SDL_WaitForGPUFences
    bool WaitForFences(bool waitAll, SDL_GPUFence* const* fences, Uint32 numFences);
    bool WaitForFences(bool waitAll, Span<SDL_GPUFence* const> fences)
    {
        return WaitForFences(waitAll, fences.data(), static_cast<Uint32>(fences.size()));
    }

    // https://wiki.libsdl.org/SDL3/SDL_AcquireGPUCommandBuffer
    GpuCommandBuffer AcquireCommandBuffer();

private:
    explicit GpuDevice(SDL_GPUDevice* handle);

    SDL_GPUDevice* m_handle = nullptr;

}; // class GpuDevice

} // namespace rad
