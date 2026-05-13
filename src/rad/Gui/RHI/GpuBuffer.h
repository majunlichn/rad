#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// https://wiki.libsdl.org/SDL3/SDL_CreateGPUBuffer
class GpuBuffer : public RefCounted<GpuBuffer>
{
public:
    static Ref<GpuBuffer> Create(Ref<GpuDevice> device, const SDL_GPUBufferCreateInfo& createInfo);

    ~GpuBuffer();
    void Destroy();

    SDL_GPUBuffer* GetHandle() const { return m_handle; }

    // Not thread-safe if the buffer may be in use on another thread; prefer SDL_PROP_GPU_BUFFER_CREATE_NAME_STRING.
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUBufferName
    void SetDebugName(const char* text);

private:
    GpuBuffer(Ref<GpuDevice> device, SDL_GPUBuffer* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUBuffer* m_handle = nullptr;

}; // class GpuBuffer

} // namespace rad
