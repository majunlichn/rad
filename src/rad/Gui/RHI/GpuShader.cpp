#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuShader.h>

#include <cassert>

namespace rad
{

Ref<GpuShader> GpuShader::Create(Ref<GpuDevice> device, const SDL_GPUShaderCreateInfo& createInfo)
{
    if (SDL_GPUShader* handle = SDL_CreateGPUShader(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuShader(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUShader failed: {}", SDL_GetError());
    return nullptr;
}

GpuShader::GpuShader(Ref<GpuDevice> device, SDL_GPUShader* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuShader::~GpuShader()
{
    Destroy();
}

void GpuShader::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUShader(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

} // namespace rad
