#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/RHI/GpuCommandBuffer.h>

#include <rad/Gui/RHI/GpuDevice.h>
#include <rad/Gui/RHI/GpuFence.h>
#include <rad/Gui/RHI/GpuTexture.h>

#include <cassert>

namespace rad
{

GpuCommandBuffer::GpuCommandBuffer(Ref<GpuDevice> device, SDL_GPUCommandBuffer* handle) :
    m_device(std::move(device)),
    m_handle(handle)
{
}

GpuCommandBuffer::GpuCommandBuffer(GpuCommandBuffer&& other) noexcept :
    m_device(std::move(other.m_device)),
    m_handle(other.m_handle)
{
    other.m_handle = nullptr;
}

GpuCommandBuffer& GpuCommandBuffer::operator=(GpuCommandBuffer&& other) noexcept
{
    if (this != &other)
    {
        assert(
            m_handle == nullptr &&
            "GpuCommandBuffer move-assign requires an empty destination; Submit or Cancel first");
        m_device = std::move(other.m_device);
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

GpuCommandBuffer::~GpuCommandBuffer()
{
    if (m_handle)
    {
        RAD_LOG_GUI(err,
                    "GpuCommandBuffer destroyed without Submit/Cancel; pending GPU work will not "
                    "be executed.");
    }
}

bool GpuCommandBuffer::Submit()
{
    if (!m_handle)
    {
        return false;
    }
    // SDL_SubmitGPUCommandBuffer: "It is invalid to use the command buffer after this is called"
    // (wiki), including on failure - always drop the handle after the call.
    const bool ok = RAD_SDL_CHECK_GUI(SDL_SubmitGPUCommandBuffer(m_handle));
    m_handle = nullptr;
    m_device = nullptr;
    return ok;
}

Ref<GpuFence> GpuCommandBuffer::SubmitAndAcquireFence()
{
    if (!m_handle)
    {
        return nullptr;
    }
    // Same invalid-after-call rule as Submit (SDL wiki).
    SDL_GPUFence* const fence = SDL_SubmitGPUCommandBufferAndAcquireFence(m_handle);
    m_handle = nullptr;
    if (!fence)
    {
        RAD_LOG_GUI(err, "SDL_SubmitGPUCommandBufferAndAcquireFence failed: {}", SDL_GetError());
        m_device = nullptr;
        return nullptr;
    }
    return GpuFence::Create(std::move(m_device), fence);
}

bool GpuCommandBuffer::Cancel()
{
    if (!m_handle)
    {
        return false;
    }
    const bool ok = RAD_SDL_CHECK_GUI(SDL_CancelGPUCommandBuffer(m_handle));
    m_handle = nullptr;
    m_device = nullptr;
    return ok;
}

bool GpuCommandBuffer::AcquireSwapchainTexture(SDL_Window* window,
                                               SDL_GPUTexture** swapchainTexture, Uint32* width,
                                               Uint32* height)
{
    if (!m_handle)
    {
        return false;
    }
    assert(window != nullptr);
    assert(swapchainTexture != nullptr);
    return RAD_SDL_CHECK_GUI(
        SDL_AcquireGPUSwapchainTexture(m_handle, window, swapchainTexture, width, height));
}

bool GpuCommandBuffer::WaitAndAcquireSwapchainTexture(SDL_Window* window,
                                                      SDL_GPUTexture** swapchainTexture,
                                                      Uint32* width, Uint32* height)
{
    if (!m_handle)
    {
        return false;
    }
    assert(window != nullptr);
    assert(swapchainTexture != nullptr);
    return RAD_SDL_CHECK_GUI(
        SDL_WaitAndAcquireGPUSwapchainTexture(m_handle, window, swapchainTexture, width, height));
}

void GpuCommandBuffer::PushVertexUniformData(Uint32 slotIndex, const void* data, Uint32 length)
{
    assert(m_handle != nullptr);
    SDL_PushGPUVertexUniformData(m_handle, slotIndex, data, length);
}

void GpuCommandBuffer::PushFragmentUniformData(Uint32 slotIndex, const void* data, Uint32 length)
{
    assert(m_handle != nullptr);
    SDL_PushGPUFragmentUniformData(m_handle, slotIndex, data, length);
}

void GpuCommandBuffer::PushComputeUniformData(Uint32 slotIndex, const void* data, Uint32 length)
{
    assert(m_handle != nullptr);
    SDL_PushGPUComputeUniformData(m_handle, slotIndex, data, length);
}

GpuRenderPass GpuCommandBuffer::BeginRenderPass(const SDL_GPUColorTargetInfo* colorTargets,
                                                Uint32 numColorTargets,
                                                const SDL_GPUDepthStencilTargetInfo* depthStencil)
{
    if (!m_handle)
    {
        return GpuRenderPass();
    }
    return GpuRenderPass(
        SDL_BeginGPURenderPass(m_handle, colorTargets, numColorTargets, depthStencil));
}

GpuComputePass GpuCommandBuffer::BeginComputePass(
    const SDL_GPUStorageTextureReadWriteBinding* storageTextures, Uint32 numStorageTextures,
    const SDL_GPUStorageBufferReadWriteBinding* storageBuffers, Uint32 numStorageBuffers)
{
    if (!m_handle)
    {
        return GpuComputePass();
    }
    return GpuComputePass(SDL_BeginGPUComputePass(m_handle, storageTextures, numStorageTextures,
                                                  storageBuffers, numStorageBuffers));
}

GpuCopyPass GpuCommandBuffer::BeginCopyPass()
{
    if (!m_handle)
    {
        return GpuCopyPass();
    }
    return GpuCopyPass(SDL_BeginGPUCopyPass(m_handle));
}

void GpuCommandBuffer::GenerateMipmaps(GpuTexture& texture)
{
    assert(m_handle != nullptr);
    assert(texture.GetHandle() != nullptr);
    SDL_GenerateMipmapsForGPUTexture(m_handle, texture.GetHandle());
}

void GpuCommandBuffer::BlitTexture(const SDL_GPUBlitInfo& info)
{
    assert(m_handle != nullptr);
    SDL_BlitGPUTexture(m_handle, &info);
}

void GpuCommandBuffer::InsertDebugLabel(const char* text)
{
    assert(m_handle != nullptr);
    assert(text != nullptr);
    SDL_InsertGPUDebugLabel(m_handle, text);
}

void GpuCommandBuffer::PushDebugGroup(const char* name)
{
    assert(m_handle != nullptr);
    assert(name != nullptr);
    SDL_PushGPUDebugGroup(m_handle, name);
}

void GpuCommandBuffer::PopDebugGroup()
{
    assert(m_handle != nullptr);
    SDL_PopGPUDebugGroup(m_handle);
}

} // namespace rad
