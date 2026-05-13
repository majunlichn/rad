#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuComputePipeline.h>

#include <cassert>

namespace rad
{

Ref<GpuComputePipeline> GpuComputePipeline::Create(
    Ref<GpuDevice> device, const SDL_GPUComputePipelineCreateInfo& createInfo)
{
    if (SDL_GPUComputePipeline* handle =
            SDL_CreateGPUComputePipeline(device->GetHandle(), &createInfo))
    {
        return RAD_NEW GpuComputePipeline(std::move(device), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateGPUComputePipeline failed: {}", SDL_GetError());
    return nullptr;
}

GpuComputePipeline::GpuComputePipeline(Ref<GpuDevice> device, SDL_GPUComputePipeline* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
}

GpuComputePipeline::~GpuComputePipeline()
{
    Destroy();
}

void GpuComputePipeline::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    assert(m_device != nullptr);
    SDL_ReleaseGPUComputePipeline(m_device->GetHandle(), m_handle);
    m_handle = nullptr;
}

} // namespace rad
