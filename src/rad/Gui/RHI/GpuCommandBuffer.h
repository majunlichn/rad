#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuPass.h>

#include <SDL3/SDL_gpu.h>

#include <limits>
#include <type_traits>
#include <utility>

#include <cassert>

namespace rad
{

class GpuDevice;
class GpuFence;
class GpuTexture;

// Frame-scoped command recording handle. Valid only until Submit, SubmitAndAcquireFence, or Cancel.
// Must be acquired, used, and submitted on the same thread (see SDL docs).
// Move construction transfers ownership from the source (which becomes empty). Move-assignment
// requires IsValid() == false on the destination, or the program will assert.
// https://wiki.libsdl.org/SDL3/SDL_AcquireGPUCommandBuffer
class GpuCommandBuffer
{
public:
    GpuCommandBuffer() = default;
    GpuCommandBuffer(const GpuCommandBuffer&) = delete;
    GpuCommandBuffer& operator=(const GpuCommandBuffer&) = delete;
    GpuCommandBuffer(GpuCommandBuffer&& other) noexcept;
    GpuCommandBuffer& operator=(GpuCommandBuffer&& other) noexcept;
    ~GpuCommandBuffer();

    bool IsValid() const { return m_handle != nullptr; }
    explicit operator bool() const { return IsValid(); }

    SDL_GPUCommandBuffer* GetHandle() const { return m_handle; }
    GpuDevice* GetDevice() const { return m_device.get(); }

    // Per SDL wiki, the command buffer is invalid after this returns (success or failure).
    // https://wiki.libsdl.org/SDL3/SDL_SubmitGPUCommandBuffer
    bool Submit();
    // https://wiki.libsdl.org/SDL3/SDL_SubmitGPUCommandBufferAndAcquireFence
    Ref<GpuFence> SubmitAndAcquireFence();
    // https://wiki.libsdl.org/SDL3/SDL_CancelGPUCommandBuffer
    bool Cancel();

    // https://wiki.libsdl.org/SDL3/SDL_AcquireGPUSwapchainTexture
    bool AcquireSwapchainTexture(SDL_Window* window, SDL_GPUTexture** swapchainTexture,
                                 Uint32* width = nullptr, Uint32* height = nullptr);
    // https://wiki.libsdl.org/SDL3/SDL_WaitAndAcquireGPUSwapchainTexture
    bool WaitAndAcquireSwapchainTexture(SDL_Window* window, SDL_GPUTexture** swapchainTexture,
                                        Uint32* width = nullptr, Uint32* height = nullptr);

    // https://wiki.libsdl.org/SDL3/SDL_PushGPUVertexUniformData
    void PushVertexUniformData(Uint32 slotIndex, const void* data, Uint32 length);
    // Span elements must be standard-layout and trivially copyable; size is values.size_bytes().
    // Matching the shader's uniform layout (e.g. std140 packing) remains the caller's responsibility.
    template <typename T>
        requires(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>)
    void PushVertexUniformData(Uint32 slotIndex, Span<const T> values)
    {
        const size_t n = values.size_bytes();
        assert(n <= static_cast<size_t>(std::numeric_limits<Uint32>::max()));
        PushVertexUniformData(slotIndex, static_cast<const void*>(values.data()),
                              static_cast<Uint32>(n));
    }
    // https://wiki.libsdl.org/SDL3/SDL_PushGPUFragmentUniformData
    void PushFragmentUniformData(Uint32 slotIndex, const void* data, Uint32 length);
    template <typename T>
        requires(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>)
    void PushFragmentUniformData(Uint32 slotIndex, Span<const T> values)
    {
        const size_t n = values.size_bytes();
        assert(n <= static_cast<size_t>(std::numeric_limits<Uint32>::max()));
        PushFragmentUniformData(slotIndex, static_cast<const void*>(values.data()),
                                static_cast<Uint32>(n));
    }
    // https://wiki.libsdl.org/SDL3/SDL_PushGPUComputeUniformData
    void PushComputeUniformData(Uint32 slotIndex, const void* data, Uint32 length);
    template <typename T>
        requires(std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>)
    void PushComputeUniformData(Uint32 slotIndex, Span<const T> values)
    {
        const size_t n = values.size_bytes();
        assert(n <= static_cast<size_t>(std::numeric_limits<Uint32>::max()));
        PushComputeUniformData(slotIndex, static_cast<const void*>(values.data()),
                               static_cast<Uint32>(n));
    }

    // https://wiki.libsdl.org/SDL3/SDL_BeginGPURenderPass
    GpuRenderPass BeginRenderPass(const SDL_GPUColorTargetInfo* colorTargets,
                                  Uint32 numColorTargets,
                                  const SDL_GPUDepthStencilTargetInfo* depthStencilTarget);
    GpuRenderPass BeginRenderPass(Span<const SDL_GPUColorTargetInfo> colorTargets,
                                  const SDL_GPUDepthStencilTargetInfo* depthStencilTarget)
    {
        return BeginRenderPass(colorTargets.data(), static_cast<Uint32>(colorTargets.size()),
                               depthStencilTarget);
    }
    // https://wiki.libsdl.org/SDL3/SDL_BeginGPUComputePass
    GpuComputePass BeginComputePass(const SDL_GPUStorageTextureReadWriteBinding* storageTextures,
                                    Uint32 numStorageTextures,
                                    const SDL_GPUStorageBufferReadWriteBinding* storageBuffers,
                                    Uint32 numStorageBuffers);
    GpuComputePass BeginComputePass(
        Span<const SDL_GPUStorageTextureReadWriteBinding> storageTextures,
        Span<const SDL_GPUStorageBufferReadWriteBinding> storageBuffers)
    {
        return BeginComputePass(storageTextures.data(), static_cast<Uint32>(storageTextures.size()),
                                storageBuffers.data(), static_cast<Uint32>(storageBuffers.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BeginGPUCopyPass
    GpuCopyPass BeginCopyPass();

    // Outside of any pass.
    // https://wiki.libsdl.org/SDL3/SDL_GenerateMipmapsForGPUTexture
    void GenerateMipmaps(GpuTexture& texture);
    // https://wiki.libsdl.org/SDL3/SDL_BlitGPUTexture
    void BlitTexture(const SDL_GPUBlitInfo& info);

    // https://wiki.libsdl.org/SDL3/SDL_InsertGPUDebugLabel
    void InsertDebugLabel(const char* text);
    // https://wiki.libsdl.org/SDL3/SDL_PushGPUDebugGroup
    void PushDebugGroup(const char* name);
    // https://wiki.libsdl.org/SDL3/SDL_PopGPUDebugGroup
    void PopDebugGroup();

private:
    friend class GpuDevice;

    GpuCommandBuffer(Ref<GpuDevice> device, SDL_GPUCommandBuffer* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUCommandBuffer* m_handle = nullptr;

}; // class GpuCommandBuffer

} // namespace rad
