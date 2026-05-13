#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// SDL_GPU texture (distinct from GuiTexture / SDL_Render texture).
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUTexture
class GpuTexture : public RefCounted<GpuTexture>
{
public:
    static Ref<GpuTexture> Create(Ref<GpuDevice> device,
                                  const SDL_GPUTextureCreateInfo& createInfo);

    ~GpuTexture();
    void Destroy();

    SDL_GPUTexture* GetHandle() const { return m_handle; }

    // Not thread-safe if the texture may be in use on another thread; prefer SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING.
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUTextureName
    void SetDebugName(const char* text);

private:
    GpuTexture(Ref<GpuDevice> device, SDL_GPUTexture* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUTexture* m_handle = nullptr;

}; // class GpuTexture

} // namespace rad
