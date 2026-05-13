#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// https://wiki.libsdl.org/SDL3/SDL_CreateGPUSampler
class GpuSampler : public RefCounted<GpuSampler>
{
public:
    static Ref<GpuSampler> Create(Ref<GpuDevice> device,
                                  const SDL_GPUSamplerCreateInfo& createInfo);

    ~GpuSampler();
    void Destroy();

    SDL_GPUSampler* GetHandle() const { return m_handle; }

private:
    GpuSampler(Ref<GpuDevice> device, SDL_GPUSampler* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUSampler* m_handle = nullptr;

}; // class GpuSampler

} // namespace rad
