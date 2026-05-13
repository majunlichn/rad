#pragma once

#include <rad/Container/Span.h>

#include <SDL3/SDL_gpu.h>

#include <utility>

namespace rad
{

// Active render pass; call End() or let destructor close the pass.
// https://wiki.libsdl.org/SDL3/SDL_BeginGPURenderPass
class GpuRenderPass
{
public:
    GpuRenderPass() = default;
    GpuRenderPass(const GpuRenderPass&) = delete;
    GpuRenderPass& operator=(const GpuRenderPass&) = delete;
    GpuRenderPass(GpuRenderPass&& other) noexcept;
    GpuRenderPass& operator=(GpuRenderPass&& other) noexcept;
    ~GpuRenderPass();

    bool IsValid() const { return m_handle != nullptr; }
    explicit operator bool() const { return IsValid(); }

    SDL_GPURenderPass* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_EndGPURenderPass
    void End();

    // https://wiki.libsdl.org/SDL3/SDL_BindGPUGraphicsPipeline
    void BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUViewport
    void SetViewport(const SDL_GPUViewport& viewport);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUScissor
    void SetScissor(const SDL_Rect& scissor);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUBlendConstants
    void SetBlendConstants(const SDL_FColor& color);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPUStencilReference
    void SetStencilReference(Uint8 reference);

    // https://wiki.libsdl.org/SDL3/SDL_BindGPUVertexBuffers
    void BindVertexBuffers(Uint32 firstSlot, const SDL_GPUBufferBinding* bindings,
                           Uint32 numBindings);
    void BindVertexBuffers(Uint32 firstSlot, Span<const SDL_GPUBufferBinding> bindings)
    {
        BindVertexBuffers(firstSlot, bindings.data(), static_cast<Uint32>(bindings.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUIndexBuffer
    void BindIndexBuffer(const SDL_GPUBufferBinding& binding, SDL_GPUIndexElementSize indexSize);
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUVertexSamplers
    void BindVertexSamplers(Uint32 firstSlot, const SDL_GPUTextureSamplerBinding* bindings,
                            Uint32 numBindings);
    void BindVertexSamplers(Uint32 firstSlot, Span<const SDL_GPUTextureSamplerBinding> bindings)
    {
        BindVertexSamplers(firstSlot, bindings.data(), static_cast<Uint32>(bindings.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUFragmentSamplers
    void BindFragmentSamplers(Uint32 firstSlot, const SDL_GPUTextureSamplerBinding* bindings,
                              Uint32 numBindings);
    void BindFragmentSamplers(Uint32 firstSlot, Span<const SDL_GPUTextureSamplerBinding> bindings)
    {
        BindFragmentSamplers(firstSlot, bindings.data(), static_cast<Uint32>(bindings.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUVertexStorageTextures
    void BindVertexStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures,
                                   Uint32 numBindings);
    void BindVertexStorageTextures(Uint32 firstSlot, Span<SDL_GPUTexture* const> textures)
    {
        BindVertexStorageTextures(firstSlot, textures.data(), static_cast<Uint32>(textures.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUFragmentStorageTextures
    void BindFragmentStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures,
                                     Uint32 numBindings);
    void BindFragmentStorageTextures(Uint32 firstSlot, Span<SDL_GPUTexture* const> textures)
    {
        BindFragmentStorageTextures(firstSlot, textures.data(),
                                    static_cast<Uint32>(textures.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUVertexStorageBuffers
    void BindVertexStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers,
                                  Uint32 numBindings);
    void BindVertexStorageBuffers(Uint32 firstSlot, Span<SDL_GPUBuffer* const> buffers)
    {
        BindVertexStorageBuffers(firstSlot, buffers.data(), static_cast<Uint32>(buffers.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUFragmentStorageBuffers
    void BindFragmentStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers,
                                    Uint32 numBindings);
    void BindFragmentStorageBuffers(Uint32 firstSlot, Span<SDL_GPUBuffer* const> buffers)
    {
        BindFragmentStorageBuffers(firstSlot, buffers.data(), static_cast<Uint32>(buffers.size()));
    }

    // https://wiki.libsdl.org/SDL3/SDL_DrawGPUPrimitives
    void DrawPrimitives(Uint32 numVertices, Uint32 numInstances, Uint32 firstVertex,
                        Uint32 firstInstance);
    // https://wiki.libsdl.org/SDL3/SDL_DrawGPUIndexedPrimitives
    void DrawIndexedPrimitives(Uint32 numIndices, Uint32 numInstances, Uint32 firstIndex,
                               Sint32 vertexOffset, Uint32 firstInstance);
    // https://wiki.libsdl.org/SDL3/SDL_DrawGPUPrimitivesIndirect
    void DrawPrimitivesIndirect(SDL_GPUBuffer* buffer, Uint32 offset, Uint32 drawCount);
    // https://wiki.libsdl.org/SDL3/SDL_DrawGPUIndexedPrimitivesIndirect
    void DrawIndexedPrimitivesIndirect(SDL_GPUBuffer* buffer, Uint32 offset, Uint32 drawCount);

private:
    friend class GpuCommandBuffer;

    GpuRenderPass(SDL_GPURenderPass* pass);

    SDL_GPURenderPass* m_handle = nullptr;

}; // class GpuRenderPass

// https://wiki.libsdl.org/SDL3/SDL_BeginGPUComputePass
class GpuComputePass
{
public:
    GpuComputePass() = default;
    GpuComputePass(const GpuComputePass&) = delete;
    GpuComputePass& operator=(const GpuComputePass&) = delete;
    GpuComputePass(GpuComputePass&& other) noexcept;
    GpuComputePass& operator=(GpuComputePass&& other) noexcept;
    ~GpuComputePass();

    bool IsValid() const { return m_handle != nullptr; }
    explicit operator bool() const { return IsValid(); }

    SDL_GPUComputePass* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_EndGPUComputePass
    void End();

    // https://wiki.libsdl.org/SDL3/SDL_BindGPUComputePipeline
    void BindComputePipeline(SDL_GPUComputePipeline* pipeline);
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUComputeSamplers
    void BindSamplers(Uint32 firstSlot, const SDL_GPUTextureSamplerBinding* bindings,
                      Uint32 numBindings);
    void BindSamplers(Uint32 firstSlot, Span<const SDL_GPUTextureSamplerBinding> bindings)
    {
        BindSamplers(firstSlot, bindings.data(), static_cast<Uint32>(bindings.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUComputeStorageTextures
    void BindStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures, Uint32 numBindings);
    void BindStorageTextures(Uint32 firstSlot, Span<SDL_GPUTexture* const> textures)
    {
        BindStorageTextures(firstSlot, textures.data(), static_cast<Uint32>(textures.size()));
    }
    // https://wiki.libsdl.org/SDL3/SDL_BindGPUComputeStorageBuffers
    void BindStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers, Uint32 numBindings);
    void BindStorageBuffers(Uint32 firstSlot, Span<SDL_GPUBuffer* const> buffers)
    {
        BindStorageBuffers(firstSlot, buffers.data(), static_cast<Uint32>(buffers.size()));
    }

    // https://wiki.libsdl.org/SDL3/SDL_DispatchGPUCompute
    void Dispatch(Uint32 groupCountX, Uint32 groupCountY, Uint32 groupCountZ);
    // https://wiki.libsdl.org/SDL3/SDL_DispatchGPUComputeIndirect
    void DispatchIndirect(SDL_GPUBuffer* buffer, Uint32 offset);

private:
    friend class GpuCommandBuffer;

    GpuComputePass(SDL_GPUComputePass* pass);

    SDL_GPUComputePass* m_handle = nullptr;

}; // class GpuComputePass

// https://wiki.libsdl.org/SDL3/SDL_BeginGPUCopyPass
class GpuCopyPass
{
public:
    GpuCopyPass() = default;
    GpuCopyPass(const GpuCopyPass&) = delete;
    GpuCopyPass& operator=(const GpuCopyPass&) = delete;
    GpuCopyPass(GpuCopyPass&& other) noexcept;
    GpuCopyPass& operator=(GpuCopyPass&& other) noexcept;
    ~GpuCopyPass();

    bool IsValid() const { return m_handle != nullptr; }
    explicit operator bool() const { return IsValid(); }

    SDL_GPUCopyPass* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_EndGPUCopyPass
    void End();

    // https://wiki.libsdl.org/SDL3/SDL_UploadToGPUTexture
    void UploadToTexture(const SDL_GPUTextureTransferInfo& source, const SDL_GPUTextureRegion& dest,
                         bool cycle);
    // https://wiki.libsdl.org/SDL3/SDL_UploadToGPUBuffer
    void UploadToBuffer(const SDL_GPUTransferBufferLocation& source,
                        const SDL_GPUBufferRegion& dest, bool cycle);
    // https://wiki.libsdl.org/SDL3/SDL_CopyGPUTextureToTexture
    void CopyTextureToTexture(const SDL_GPUTextureLocation& source,
                              const SDL_GPUTextureLocation& dest, Uint32 width, Uint32 height,
                              Uint32 depth, bool cycle);
    // https://wiki.libsdl.org/SDL3/SDL_CopyGPUBufferToBuffer
    void CopyBufferToBuffer(const SDL_GPUBufferLocation& source, const SDL_GPUBufferLocation& dest,
                            Uint32 size, bool cycle);
    // https://wiki.libsdl.org/SDL3/SDL_DownloadFromGPUTexture
    void DownloadFromTexture(const SDL_GPUTextureRegion& source,
                             const SDL_GPUTextureTransferInfo& dest);
    // https://wiki.libsdl.org/SDL3/SDL_DownloadFromGPUBuffer
    void DownloadFromBuffer(const SDL_GPUBufferRegion& source,
                            const SDL_GPUTransferBufferLocation& dest);

private:
    friend class GpuCommandBuffer;

    GpuCopyPass(SDL_GPUCopyPass* pass);

    SDL_GPUCopyPass* m_handle = nullptr;

}; // class GpuCopyPass

} // namespace rad
