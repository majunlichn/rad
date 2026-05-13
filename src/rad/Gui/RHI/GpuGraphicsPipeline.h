#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// https://wiki.libsdl.org/SDL3/SDL_CreateGPUGraphicsPipeline
class GpuGraphicsPipeline : public RefCounted<GpuGraphicsPipeline>
{
public:
    static Ref<GpuGraphicsPipeline> Create(Ref<GpuDevice> device,
                                           const SDL_GPUGraphicsPipelineCreateInfo& createInfo);

    ~GpuGraphicsPipeline();
    void Destroy();

    SDL_GPUGraphicsPipeline* GetHandle() const { return m_handle; }

private:
    GpuGraphicsPipeline(Ref<GpuDevice> device, SDL_GPUGraphicsPipeline* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUGraphicsPipeline* m_handle = nullptr;

}; // class GpuGraphicsPipeline

} // namespace rad
