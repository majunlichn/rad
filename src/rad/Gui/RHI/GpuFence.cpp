#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuFence.h>

#include <cassert>

namespace rad
{

Ref<GpuFence> GpuFence::Create(Ref<GpuDevice> device, SDL_GPUFence* fence)
{
    if (!fence)
    {
        return nullptr;
    }
    return RAD_NEW GpuFence(std::move(device), fence);
}

GpuFence::GpuFence(Ref<GpuDevice> device, SDL_GPUFence* fence) :
    m_device(std::move(device)),
    m_handle(fence)
{
    assert(m_handle != nullptr);
}

GpuFence::~GpuFence()
{
    Destroy();
}

void GpuFence::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUFence(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

bool GpuFence::Query() const
{
    if (!m_handle || !m_device)
    {
        return false;
    }
    return SDL_QueryGPUFence(m_device->GetHandle(), m_handle);
}

} // namespace rad
