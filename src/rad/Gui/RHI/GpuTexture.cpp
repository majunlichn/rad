#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuTexture.h>

#include <cassert>

namespace rad
{

Ref<GpuTexture> GpuTexture::Create(Ref<GpuDevice> device,
                                   const SDL_GPUTextureCreateInfo& createInfo)
{
    if (SDL_GPUTexture* handle = SDL_CreateGPUTexture(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuTexture(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUTexture failed: {}", SDL_GetError());
    return nullptr;
}

GpuTexture::GpuTexture(Ref<GpuDevice> device, SDL_GPUTexture* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuTexture::~GpuTexture()
{
    Destroy();
}

void GpuTexture::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUTexture(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

void GpuTexture::SetDebugName(const char* text)
{
    if (m_handle && m_device)
    {
        SDL_SetGPUTextureName(m_device->GetHandle(), m_handle, text);
    }
}

} // namespace rad
