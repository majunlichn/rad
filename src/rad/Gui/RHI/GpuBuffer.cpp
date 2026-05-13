#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuBuffer.h>

#include <cassert>

namespace rad
{

Ref<GpuBuffer> GpuBuffer::Create(Ref<GpuDevice> device, const SDL_GPUBufferCreateInfo& createInfo)
{
    if (SDL_GPUBuffer* handle = SDL_CreateGPUBuffer(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuBuffer(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUBuffer failed: {}", SDL_GetError());
    return nullptr;
}

GpuBuffer::GpuBuffer(Ref<GpuDevice> device, SDL_GPUBuffer* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuBuffer::~GpuBuffer()
{
    Destroy();
}

void GpuBuffer::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUBuffer(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

void GpuBuffer::SetDebugName(const char* text)
{
    if (m_handle && m_device)
    {
        SDL_SetGPUBufferName(m_device->GetHandle(), m_handle, text);
    }
}

} // namespace rad
