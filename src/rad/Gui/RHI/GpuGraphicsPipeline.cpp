#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuGraphicsPipeline.h>

#include <cassert>

namespace rad
{

Ref<GpuGraphicsPipeline> GpuGraphicsPipeline::Create(
    Ref<GpuDevice> device, const SDL_GPUGraphicsPipelineCreateInfo& createInfo)
{
    if (SDL_GPUGraphicsPipeline* handle =
            SDL_CreateGPUGraphicsPipeline(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuGraphicsPipeline(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUGraphicsPipeline failed: {}", SDL_GetError());
    return nullptr;
}

GpuGraphicsPipeline::GpuGraphicsPipeline(Ref<GpuDevice> device, SDL_GPUGraphicsPipeline* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuGraphicsPipeline::~GpuGraphicsPipeline()
{
    Destroy();
}

void GpuGraphicsPipeline::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUGraphicsPipeline(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

} // namespace rad
