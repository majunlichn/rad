#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanCommandPool : public RefCounted<VulkanCommandPool>
{
public:
    VulkanCommandPool(Ref<VulkanDevice> device, VulkanQueueFamily queueFamily,
                      const vk::CommandPoolCreateInfo& createInfo);
    ~VulkanCommandPool();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::CommandPool& GetHandle() const { return m_handle; }

    std::vector<Ref<VulkanCommandBuffer>> AllocateCommandBuffers(vk::CommandBufferLevel level,
                                                                 uint32_t count);

    Ref<VulkanCommandBuffer> AllocateCommandBuffer(vk::CommandBufferLevel level);

private:
    Ref<VulkanDevice> m_device;
    VulkanQueueFamily m_queueFamily;
    vk::CommandPool m_handle = nullptr;

}; // class VulkanCommandPool

class VulkanCommandBuffer : public RefCounted<VulkanCommandBuffer>
{
public:
    VulkanCommandBuffer(Ref<VulkanCommandPool> cmdPool, vk::CommandBuffer cmdBufferHandle);
    ~VulkanCommandBuffer();

    VulkanDevice* GetDevice() const { return m_cmdPool->GetDevice(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    VulkanCommandPool* GetCommandPool() const { return m_cmdPool.get(); }
    const vk::CommandBuffer& GetHandle() const { return m_handle; }

    void Begin(vk::CommandBufferUsageFlags flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
               vk::CommandBufferInheritanceInfo* pInheritanceInfo = nullptr);
    void End();
    void Reset(vk::CommandBufferResetFlags flags);

    void CopyBuffer(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer,
                    vk::ArrayProxy<vk::BufferCopy> regions);
    void CopyBuffer2(const vk::CopyBufferInfo2& copyInfo);
    void CopyBuffer2(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer,
                     vk::ArrayProxy<vk::BufferCopy2> regions);
    void CopyImage(VulkanImage* srcImage, VulkanImage* dstImage,
                   vk::ArrayProxy<vk::ImageCopy> regions);
    void CopyImage2(const vk::CopyImageInfo2& copyInfo);
    void CopyImage2(VulkanImage* srcImage, VulkanImage* dstImage,
                    vk::ArrayProxy<vk::ImageCopy2> regions);
    void CopyBufferToImage(VulkanBuffer* srcBuffer, VulkanImage* dstImage,
                           vk::ArrayProxy<vk::BufferImageCopy> regions);
    void CopyBufferToImage2(const vk::CopyBufferToImageInfo2& copyInfo);
    void CopyBufferToImage2(VulkanBuffer* srcBuffer, VulkanImage* dstImage,
                            vk::ArrayProxy<vk::BufferImageCopy2> regions);
    void CopyImageToBuffer(VulkanImage* srcImage, VulkanBuffer* dstBuffer,
                           vk::ArrayProxy<vk::BufferImageCopy> regions);
    void CopyImageToBuffer2(const vk::CopyImageToBufferInfo2& copyInfo);
    void CopyImageToBuffer2(VulkanImage* srcImage, VulkanBuffer* dstBuffer,
                            vk::ArrayProxy<vk::BufferImageCopy2> regions);

    void UpdateBuffer(VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size,
                      const void* data);

    template <TriviallyCopyable T>
    void UpdateBuffer(VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset, vk::ArrayProxy<T> data)
    {
        UpdateBuffer(dstBuffer, dstOffset, data.size() * sizeof(T), data.data());
    }

    void FillBuffer(VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size,
                    uint32_t data);

    void SetPipelineBarrier(vk::PipelineStageFlags srcStageMask,
                            vk::PipelineStageFlags dstStageMask,
                            vk::DependencyFlags dependencyFlags,
                            vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
                            vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
                            vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers);
    void SetPipelineBarrier2(const vk::DependencyInfoKHR& dependencyInfo);
    void SetPipelineBarrier2(vk::DependencyFlags flags,
                             vk::ArrayProxy<vk::MemoryBarrier2> memoryBarriers,
                             vk::ArrayProxy<vk::BufferMemoryBarrier2> bufferMemoryBarriers,
                             vk::ArrayProxy<vk::ImageMemoryBarrier2> imageMemoryBarriers);

    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
    // Compute to compute Read-After-Write (storage buffer and storage image).
    void SetPipelineBarrier_ComputeToComputeRAW();
    // Compute to compute Write-After-Read (storage buffer and storage image).
    void SetPipelineBarrier_ComputeToComputeWAR();

    void ExecuteCommands(vk::ArrayProxy<VulkanCommandBuffer*> cmdBuffers);

    void BindPipeline(VulkanPipeline* pipeline);

    void BindDescriptorSets(vk::PipelineBindPoint bindPoint, VulkanPipelineLayout* layout,
                            uint32_t firstSet, vk::ArrayProxy<VulkanDescriptorSet*> descriptorSets,
                            vk::ArrayProxy<uint32_t> dynamicOffsets);

    void ClearColorImage(VulkanImage* image, const vk::ClearColorValue& color,
                         vk::ArrayProxy<vk::ImageSubresourceRange> ranges);

    void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    void DispatchIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset);
    void DispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
                      uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

    void SetEvent(VulkanEvent* event, vk::PipelineStageFlags stageMask);
    void ResetEvent(VulkanEvent* event, vk::PipelineStageFlags stageMask);
    void WaitEvents(vk::ArrayProxy<VulkanEvent*> events, vk::PipelineStageFlags srcStageMask,
                    vk::PipelineStageFlags dstStageMask,
                    vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
                    vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
                    vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers);

    void PushConstants(VulkanPipelineLayout* layout, vk::ShaderStageFlags stageFlags,
                       uint32_t offset, uint32_t size, const void* values);
    template <TriviallyCopyable T>
    void PushConstants(VulkanPipelineLayout* layout, vk::ShaderStageFlags stageFlags,
                       uint32_t offset, vk::ArrayProxy<T> values)
    {
        PushConstants(layout, stageFlags, offset, values.size() * sizeof(T), values.data());
    }

    void SetViewport(uint32_t firstViewport, vk::ArrayProxy<vk::Viewport> viewports);
    void SetScissor(uint32_t firstScissor, vk::ArrayProxy<vk::Rect2D> scissors);
    void SetViewportWithCount(vk::ArrayProxy<vk::Viewport> viewports);
    void SetScissorWithCount(vk::ArrayProxy<vk::Rect2D> scissors);
    void SetLineWidth(float lineWidth);
    void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp,
                      float depthBiasSlopeFactor);
    void SetBlendConstants(vk::ArrayProxy<float> blendConstants);
    void SetDepthBounds(float minDepthBounds, float maxDepthBounds);
    void SetCullMode(vk::CullModeFlags cullMode);
    void SetFrontFace(vk::FrontFace frontFace);
    void SetPrimitiveTopology(vk::PrimitiveTopology primitiveTopology);
    void SetRasterizerDiscardEnable(bool rasterizerDiscardEnable);
    void SetDepthBiasEnable(bool depthBiasEnable);
    void SetPrimitiveRestartEnable(bool primitiveRestartEnable);
    void SetDepthTestEnable(bool depthTestEnable);
    void SetDepthWriteEnable(bool depthWriteEnable);
    void SetDepthCompareOp(vk::CompareOp depthCompareOp);
    void SetDepthBoundsTestEnable(bool depthBoundsTestEnable);
    void SetStencilTestEnable(bool stencilTestEnable);
    void SetStencilCompareMask(vk::StencilFaceFlags faceMask, uint32_t compareMask);
    void SetStencilWriteMask(vk::StencilFaceFlags faceMask, uint32_t writeMask);
    void SetStencilReference(vk::StencilFaceFlags faceMask, uint32_t reference);
    void SetStencilOp(vk::StencilFaceFlags faceMask, vk::StencilOp failOp, vk::StencilOp passOp,
                      vk::StencilOp depthFailOp, vk::CompareOp compareOp);

    void BindIndexBuffer(VulkanBuffer* buffer, vk::DeviceSize offset, vk::IndexType indexType);
    void BindVertexBuffers(uint32_t firstBinding, vk::ArrayProxy<VulkanBuffer*> buffers,
                           vk::ArrayProxy<vk::DeviceSize> offsets);

    void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
              uint32_t firstInstance);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                     int32_t vertexOffset, uint32_t firstInstance);
    void DrawIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset, uint32_t drawCount,
                      uint32_t stride);
    void DrawIndexedIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                             uint32_t drawCount, uint32_t stride);
    void DrawIndirectCount(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                           VulkanBuffer* countBuffer, vk::DeviceSize countBufferOffset,
                           uint32_t maxDrawCount, uint32_t stride);
    void DrawIndexedIndirectCount(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                                  VulkanBuffer* countBuffer, vk::DeviceSize countBufferOffset,
                                  uint32_t maxDrawCount, uint32_t stride);

    void BlitImage(VulkanImage* srcImage, VulkanImage* dstImage,
                   vk::ArrayProxy<vk::ImageBlit> regions, vk::Filter filter);

    void ClearDepthStencilImage(VulkanImage* image, vk::ClearDepthStencilValue clearValue,
                                vk::ArrayProxy<vk::ImageSubresourceRange> ranges);
    void ClearAttachments(vk::ArrayProxy<vk::ClearAttachment> attachments,
                          vk::ArrayProxy<vk::ClearRect> rects);
    void ResolveImage(VulkanImage* srcImage, VulkanImage* dstImage,
                      vk::ArrayProxy<vk::ImageResolve> regions);

    void BeginRenderPass(const vk::RenderPassBeginInfo& renderPassBeginInfo,
                         vk::SubpassContents contents);
    void BeginRenderPass(VulkanRenderPass* renderPass, VulkanFramebuffer* framebuffer,
                         const vk::Rect2D& renderArea, vk::ArrayProxy<vk::ClearValue> clearValues,
                         vk::SubpassContents contents = vk::SubpassContents::eInline);
    void NextSubpass(vk::SubpassContents contents);
    void EndRenderPass();

    void SetDeviceMask(uint32_t deviceMask);

    void ResetQueryPool(VulkanQueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount);
    void BeginQuery(VulkanQueryPool* queryPool, uint32_t query, vk::QueryControlFlags flags);
    void EndQuery(VulkanQueryPool* queryPool, uint32_t query);
    void CopyQueryPoolResults(VulkanQueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount,
                              VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset,
                              vk::DeviceSize stride, vk::QueryResultFlags flags);
    void WriteTimestamp(vk::PipelineStageFlagBits stage, VulkanQueryPool* queryPool,
                        uint32_t query);
    void WriteTimestamp2(vk::PipelineStageFlagBits2 stage, VulkanQueryPool* queryPool,
                         uint32_t query);

private:
    Ref<VulkanCommandPool> m_cmdPool;
    vk::CommandBuffer m_handle = nullptr;

}; // class VulkanCommandBuffer

} // namespace rad
