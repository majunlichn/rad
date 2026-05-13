#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuSampler.h>

#include <cassert>

namespace rad
{

Ref<GpuSampler> GpuSampler::Create(Ref<GpuDevice> device,
                                   const SDL_GPUSamplerCreateInfo& createInfo)
{
    if (SDL_GPUSampler* handle = SDL_CreateGPUSampler(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuSampler(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUSampler failed: {}", SDL_GetError());
    return nullptr;
}

GpuSampler::GpuSampler(Ref<GpuDevice> device, SDL_GPUSampler* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuSampler::~GpuSampler()
{
    Destroy();
}

void GpuSampler::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUSampler(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

} // namespace rad
