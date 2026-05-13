#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// https://wiki.libsdl.org/SDL3/SDL_CreateGPUComputePipeline
class GpuComputePipeline : public RefCounted<GpuComputePipeline>
{
public:
    static Ref<GpuComputePipeline> Create(Ref<GpuDevice> device,
                                          const SDL_GPUComputePipelineCreateInfo& createInfo);

    ~GpuComputePipeline();
    void Destroy();

    SDL_GPUComputePipeline* GetHandle() const { return m_handle; }

private:
    GpuComputePipeline(Ref<GpuDevice> device, SDL_GPUComputePipeline* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUComputePipeline* m_handle = nullptr;

}; // class GpuComputePipeline

} // namespace rad
