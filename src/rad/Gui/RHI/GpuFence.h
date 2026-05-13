#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// GPU timeline fence from SDL_SubmitGPUCommandBufferAndAcquireFence.
// https://wiki.libsdl.org/SDL3/SDL_SubmitGPUCommandBufferAndAcquireFence
class GpuFence : public RefCounted<GpuFence>
{
public:
    static Ref<GpuFence> Create(Ref<GpuDevice> device, SDL_GPUFence* fence);

    ~GpuFence();
    void Destroy();

    SDL_GPUFence* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_QueryGPUFence
    bool Query() const;

private:
    GpuFence(Ref<GpuDevice> device, SDL_GPUFence* fence);

    Ref<GpuDevice> m_device;
    SDL_GPUFence* m_handle = nullptr;

}; // class GpuFence

} // namespace rad
