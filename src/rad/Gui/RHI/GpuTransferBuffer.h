#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// Staging memory for uploads/downloads (see SDL_MapGPUTransferBuffer).
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUTransferBuffer
class GpuTransferBuffer : public RefCounted<GpuTransferBuffer>
{
public:
    static Ref<GpuTransferBuffer> Create(Ref<GpuDevice> device,
                                         const SDL_GPUTransferBufferCreateInfo& createInfo);

    ~GpuTransferBuffer();
    void Destroy();

    SDL_GPUTransferBuffer* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_MapGPUTransferBuffer
    void* Map(bool cycle);
    // https://wiki.libsdl.org/SDL3/SDL_UnmapGPUTransferBuffer
    void Unmap();

private:
    GpuTransferBuffer(Ref<GpuDevice> device, SDL_GPUTransferBuffer* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUTransferBuffer* m_handle = nullptr;

}; // class GpuTransferBuffer

} // namespace rad
