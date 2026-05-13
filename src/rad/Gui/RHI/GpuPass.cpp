#include <rad/Gui/RHI/GpuPass.h>

#include <cassert>

namespace rad
{

// --- GpuRenderPass ---

GpuRenderPass::GpuRenderPass(SDL_GPURenderPass* pass) :
    m_handle(pass)
{
}

GpuRenderPass::GpuRenderPass(GpuRenderPass&& other) noexcept :
    m_handle(other.m_handle)
{
    other.m_handle = nullptr;
}

GpuRenderPass& GpuRenderPass::operator=(GpuRenderPass&& other) noexcept
{
    if (this != &other)
    {
        End();
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

GpuRenderPass::~GpuRenderPass()
{
    End();
}

void GpuRenderPass::End()
{
    if (m_handle)
    {
        SDL_EndGPURenderPass(m_handle);
        m_handle = nullptr;
    }
}

void GpuRenderPass::BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline)
{
    assert(m_handle != nullptr);
    SDL_BindGPUGraphicsPipeline(m_handle, pipeline);
}

void GpuRenderPass::SetViewport(const SDL_GPUViewport& viewport)
{
    assert(m_handle != nullptr);
    SDL_SetGPUViewport(m_handle, &viewport);
}

void GpuRenderPass::SetScissor(const SDL_Rect& scissor)
{
    assert(m_handle != nullptr);
    SDL_SetGPUScissor(m_handle, &scissor);
}

void GpuRenderPass::SetBlendConstants(const SDL_FColor& color)
{
    assert(m_handle != nullptr);
    SDL_SetGPUBlendConstants(m_handle, color);
}

void GpuRenderPass::SetStencilReference(Uint8 reference)
{
    assert(m_handle != nullptr);
    SDL_SetGPUStencilReference(m_handle, reference);
}

void GpuRenderPass::BindVertexBuffers(Uint32 firstSlot, const SDL_GPUBufferBinding* bindings,
                                      Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUVertexBuffers(m_handle, firstSlot, bindings, numBindings);
}

void GpuRenderPass::BindIndexBuffer(const SDL_GPUBufferBinding& binding,
                                    SDL_GPUIndexElementSize indexSize)
{
    assert(m_handle != nullptr);
    SDL_BindGPUIndexBuffer(m_handle, &binding, indexSize);
}

void GpuRenderPass::BindVertexSamplers(Uint32 firstSlot,
                                       const SDL_GPUTextureSamplerBinding* bindings,
                                       Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUVertexSamplers(m_handle, firstSlot, bindings, numBindings);
}

void GpuRenderPass::BindFragmentSamplers(Uint32 firstSlot,
                                         const SDL_GPUTextureSamplerBinding* bindings,
                                         Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUFragmentSamplers(m_handle, firstSlot, bindings, numBindings);
}

void GpuRenderPass::BindVertexStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures,
                                              Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUVertexStorageTextures(m_handle, firstSlot, textures, numBindings);
}

void GpuRenderPass::BindFragmentStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures,
                                                Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUFragmentStorageTextures(m_handle, firstSlot, textures, numBindings);
}

void GpuRenderPass::BindVertexStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers,
                                             Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUVertexStorageBuffers(m_handle, firstSlot, buffers, numBindings);
}

void GpuRenderPass::BindFragmentStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers,
                                               Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUFragmentStorageBuffers(m_handle, firstSlot, buffers, numBindings);
}

void GpuRenderPass::DrawPrimitives(Uint32 numVertices, Uint32 numInstances, Uint32 firstVertex,
                                   Uint32 firstInstance)
{
    assert(m_handle != nullptr);
    SDL_DrawGPUPrimitives(m_handle, numVertices, numInstances, firstVertex, firstInstance);
}

void GpuRenderPass::DrawIndexedPrimitives(Uint32 numIndices, Uint32 numInstances, Uint32 firstIndex,
                                          Sint32 vertexOffset, Uint32 firstInstance)
{
    assert(m_handle != nullptr);
    SDL_DrawGPUIndexedPrimitives(m_handle, numIndices, numInstances, firstIndex, vertexOffset,
                                 firstInstance);
}

void GpuRenderPass::DrawPrimitivesIndirect(SDL_GPUBuffer* buffer, Uint32 offset, Uint32 drawCount)
{
    assert(m_handle != nullptr);
    SDL_DrawGPUPrimitivesIndirect(m_handle, buffer, offset, drawCount);
}

void GpuRenderPass::DrawIndexedPrimitivesIndirect(SDL_GPUBuffer* buffer, Uint32 offset,
                                                  Uint32 drawCount)
{
    assert(m_handle != nullptr);
    SDL_DrawGPUIndexedPrimitivesIndirect(m_handle, buffer, offset, drawCount);
}

// --- GpuComputePass ---

GpuComputePass::GpuComputePass(SDL_GPUComputePass* pass) :
    m_handle(pass)
{
}

GpuComputePass::GpuComputePass(GpuComputePass&& other) noexcept :
    m_handle(other.m_handle)
{
    other.m_handle = nullptr;
}

GpuComputePass& GpuComputePass::operator=(GpuComputePass&& other) noexcept
{
    if (this != &other)
    {
        End();
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

GpuComputePass::~GpuComputePass()
{
    End();
}

void GpuComputePass::End()
{
    if (m_handle)
    {
        SDL_EndGPUComputePass(m_handle);
        m_handle = nullptr;
    }
}

void GpuComputePass::BindComputePipeline(SDL_GPUComputePipeline* pipeline)
{
    assert(m_handle != nullptr);
    SDL_BindGPUComputePipeline(m_handle, pipeline);
}

void GpuComputePass::BindSamplers(Uint32 firstSlot, const SDL_GPUTextureSamplerBinding* bindings,
                                  Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUComputeSamplers(m_handle, firstSlot, bindings, numBindings);
}

void GpuComputePass::BindStorageTextures(Uint32 firstSlot, SDL_GPUTexture* const* textures,
                                         Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUComputeStorageTextures(m_handle, firstSlot, textures, numBindings);
}

void GpuComputePass::BindStorageBuffers(Uint32 firstSlot, SDL_GPUBuffer* const* buffers,
                                        Uint32 numBindings)
{
    assert(m_handle != nullptr);
    SDL_BindGPUComputeStorageBuffers(m_handle, firstSlot, buffers, numBindings);
}

void GpuComputePass::Dispatch(Uint32 groupCountX, Uint32 groupCountY, Uint32 groupCountZ)
{
    assert(m_handle != nullptr);
    SDL_DispatchGPUCompute(m_handle, groupCountX, groupCountY, groupCountZ);
}

void GpuComputePass::DispatchIndirect(SDL_GPUBuffer* buffer, Uint32 offset)
{
    assert(m_handle != nullptr);
    SDL_DispatchGPUComputeIndirect(m_handle, buffer, offset);
}

// --- GpuCopyPass ---

GpuCopyPass::GpuCopyPass(SDL_GPUCopyPass* pass) :
    m_handle(pass)
{
}

GpuCopyPass::GpuCopyPass(GpuCopyPass&& other) noexcept :
    m_handle(other.m_handle)
{
    other.m_handle = nullptr;
}

GpuCopyPass& GpuCopyPass::operator=(GpuCopyPass&& other) noexcept
{
    if (this != &other)
    {
        End();
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
}

GpuCopyPass::~GpuCopyPass()
{
    End();
}

void GpuCopyPass::End()
{
    if (m_handle)
    {
        SDL_EndGPUCopyPass(m_handle);
        m_handle = nullptr;
    }
}

void GpuCopyPass::UploadToTexture(const SDL_GPUTextureTransferInfo& source,
                                  const SDL_GPUTextureRegion& dest, bool cycle)
{
    assert(m_handle != nullptr);
    SDL_UploadToGPUTexture(m_handle, &source, &dest, cycle);
}

void GpuCopyPass::UploadToBuffer(const SDL_GPUTransferBufferLocation& source,
                                 const SDL_GPUBufferRegion& dest, bool cycle)
{
    assert(m_handle != nullptr);
    SDL_UploadToGPUBuffer(m_handle, &source, &dest, cycle);
}

void GpuCopyPass::CopyTextureToTexture(const SDL_GPUTextureLocation& source,
                                       const SDL_GPUTextureLocation& dest, Uint32 width,
                                       Uint32 height, Uint32 depth, bool cycle)
{
    assert(m_handle != nullptr);
    SDL_CopyGPUTextureToTexture(m_handle, &source, &dest, width, height, depth, cycle);
}

void GpuCopyPass::CopyBufferToBuffer(const SDL_GPUBufferLocation& source,
                                     const SDL_GPUBufferLocation& dest, Uint32 size, bool cycle)
{
    assert(m_handle != nullptr);
    SDL_CopyGPUBufferToBuffer(m_handle, &source, &dest, size, cycle);
}

void GpuCopyPass::DownloadFromTexture(const SDL_GPUTextureRegion& source,
                                      const SDL_GPUTextureTransferInfo& dest)
{
    assert(m_handle != nullptr);
    SDL_DownloadFromGPUTexture(m_handle, &source, &dest);
}

void GpuCopyPass::DownloadFromBuffer(const SDL_GPUBufferRegion& source,
                                     const SDL_GPUTransferBufferLocation& dest)
{
    assert(m_handle != nullptr);
    SDL_DownloadFromGPUBuffer(m_handle, &source, &dest);
}

} // namespace rad
