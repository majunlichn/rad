#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuTransferBuffer.h>

#include <cassert>

namespace rad
{

Ref<GpuTransferBuffer> GpuTransferBuffer::Create(Ref<GpuDevice> device,
                                                 const SDL_GPUTransferBufferCreateInfo& createInfo)
{
    if (SDL_GPUTransferBuffer* handle =
            SDL_CreateGPUTransferBuffer(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuTransferBuffer(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUTransferBuffer failed: {}", SDL_GetError());
    return nullptr;
}

GpuTransferBuffer::GpuTransferBuffer(Ref<GpuDevice> device, SDL_GPUTransferBuffer* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuTransferBuffer::~GpuTransferBuffer()
{
    Destroy();
}

void GpuTransferBuffer::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUTransferBuffer(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

void* GpuTransferBuffer::Map(bool cycle)
{
    assert(m_device != nullptr);
    assert(m_handle != nullptr);
    return SDL_MapGPUTransferBuffer(m_device->GetHandle(), m_handle, cycle);
}

void GpuTransferBuffer::Unmap()
{
    assert(m_device != nullptr);
    assert(m_handle != nullptr);
    SDL_UnmapGPUTransferBuffer(m_device->GetHandle(), m_handle);
}

} // namespace rad
