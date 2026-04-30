#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/VulkanCommand.h>

#include <rad/Vulkan/VulkanDevice.h>
#include <rad/Vulkan/VulkanFence.h>
#include <rad/Vulkan/VulkanEvent.h>
#include <rad/Vulkan/VulkanFramebuffer.h>
#include <rad/Vulkan/VulkanRenderPass.h>
#include <rad/Vulkan/VulkanPipeline.h>
#include <rad/Vulkan/VulkanDescriptor.h>
#include <rad/Vulkan/VulkanBuffer.h>
#include <rad/Vulkan/VulkanImage.h>
#include <rad/Vulkan/VulkanQuery.h>

namespace rad
{

VulkanCommandPool::VulkanCommandPool(Ref<VulkanDevice> device, VulkanQueueFamily queueFamily,
                                     const vk::CommandPoolCreateInfo& createInfo) :
    m_device(std::move(device)),
    m_queueFamily(queueFamily)
{
    m_handle = m_device->GetHandle().createCommandPool(createInfo, nullptr, GetDispatcher());
}

VulkanCommandPool::~VulkanCommandPool()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyCommandPool(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanCommandPool::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

std::vector<Ref<VulkanCommandBuffer>> VulkanCommandPool::AllocateCommandBuffers(
    vk::CommandBufferLevel level, uint32_t count)
{
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.commandPool = GetHandle();
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = count;
    std::vector<vk::CommandBuffer> cmdBufferHandles =
        GetDevice()->GetHandle().allocateCommandBuffers(allocateInfo, GetDispatcher());
    std::vector<Ref<VulkanCommandBuffer>> cmdBuffers(count);
    for (size_t i = 0; i < count; ++i)
    {
        cmdBuffers[i] = RAD_NEW VulkanCommandBuffer(this, cmdBufferHandles[i]);
    }
    return cmdBuffers;
}

Ref<VulkanCommandBuffer> VulkanCommandPool::AllocateCommandBuffer(vk::CommandBufferLevel level)
{
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.commandPool = GetHandle();
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = 1;
    vk::Device deviceHandle = m_device->GetHandle();
    vk::CommandBuffer cmdBufferHandle = {};
    VK_CHECK(GetDispatcher().vkAllocateCommandBuffers(
        m_device->GetHandle(), reinterpret_cast<const VkCommandBufferAllocateInfo*>(&allocateInfo),
        reinterpret_cast<VkCommandBuffer*>(&cmdBufferHandle)));
    return RAD_NEW VulkanCommandBuffer(this, cmdBufferHandle);
}

VulkanCommandBuffer::VulkanCommandBuffer(Ref<VulkanCommandPool> cmdPool,
                                         vk::CommandBuffer cmdBufferHandle) :
    m_cmdPool(std::move(cmdPool))
{
    m_handle = vk::CommandBuffer(cmdBufferHandle);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    if (m_handle)
    {
        GetDevice()->GetHandle().freeCommandBuffers(m_cmdPool->GetHandle(), 1, &m_handle,
                                                    GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanCommandBuffer::GetDispatcher() const
{
    return GetDevice()->GetDispatcher();
}

void VulkanCommandBuffer::Begin(vk::CommandBufferUsageFlags flags,
                                vk::CommandBufferInheritanceInfo* pInheritanceInfo)
{
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = pInheritanceInfo;
    m_handle.begin(beginInfo, GetDispatcher());
}

void VulkanCommandBuffer::End()
{
    m_handle.end(GetDispatcher());
}

void VulkanCommandBuffer::Reset(vk::CommandBufferResetFlags flags)
{
    m_handle.reset(flags, GetDispatcher());
}

void VulkanCommandBuffer::CopyBuffer(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer,
                                     vk::ArrayProxy<vk::BufferCopy> regions)
{
    m_handle.copyBuffer(srcBuffer->GetHandle(), dstBuffer->GetHandle(), regions, GetDispatcher());
}

void VulkanCommandBuffer::CopyBuffer2(const vk::CopyBufferInfo2& copyInfo)
{
    m_handle.copyBuffer2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyBuffer2(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer,
                                      vk::ArrayProxy<vk::BufferCopy2> regions)
{
    vk::CopyBufferInfo2 copyInfo;
    copyInfo.srcBuffer = srcBuffer->GetHandle();
    copyInfo.dstBuffer = dstBuffer->GetHandle();
    copyInfo.setRegions(regions);
    m_handle.copyBuffer2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyImage(VulkanImage* srcImage, VulkanImage* dstImage,
                                    vk::ArrayProxy<vk::ImageCopy> regions)
{
    m_handle.copyImage(srcImage->GetHandle(), srcImage->GetCurrentLayout(), dstImage->GetHandle(),
                       dstImage->GetCurrentLayout(), regions, GetDispatcher());
}

void VulkanCommandBuffer::CopyImage2(const vk::CopyImageInfo2& copyInfo)
{
    m_handle.copyImage2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyImage2(VulkanImage* srcImage, VulkanImage* dstImage,
                                     vk::ArrayProxy<vk::ImageCopy2> regions)
{
    vk::CopyImageInfo2 copyInfo;
    copyInfo.srcImage = srcImage->GetHandle();
    copyInfo.srcImageLayout = srcImage->GetCurrentLayout();
    copyInfo.dstImage = dstImage->GetHandle();
    copyInfo.dstImageLayout = dstImage->GetCurrentLayout();
    copyInfo.setRegions(regions);
    m_handle.copyImage2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyBufferToImage(VulkanBuffer* srcBuffer, VulkanImage* dstImage,
                                            vk::ArrayProxy<vk::BufferImageCopy> regions)
{
    m_handle.copyBufferToImage(srcBuffer->GetHandle(), dstImage->GetHandle(),
                               dstImage->GetCurrentLayout(), regions, GetDispatcher());
}

void VulkanCommandBuffer::CopyBufferToImage2(const vk::CopyBufferToImageInfo2& copyInfo)
{
    m_handle.copyBufferToImage2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyBufferToImage2(VulkanBuffer* srcBuffer, VulkanImage* dstImage,
                                             vk::ArrayProxy<vk::BufferImageCopy2> regions)
{
    vk::CopyBufferToImageInfo2 copyInfo;
    copyInfo.srcBuffer = srcBuffer->GetHandle();
    copyInfo.dstImage = dstImage->GetHandle();
    copyInfo.dstImageLayout = dstImage->GetCurrentLayout();
    copyInfo.setRegions(regions);
    m_handle.copyBufferToImage2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyImageToBuffer(VulkanImage* srcImage, VulkanBuffer* dstBuffer,
                                            vk::ArrayProxy<vk::BufferImageCopy> regions)
{
    m_handle.copyImageToBuffer(srcImage->GetHandle(), srcImage->GetCurrentLayout(),
                               dstBuffer->GetHandle(), regions, GetDispatcher());
}

void VulkanCommandBuffer::CopyImageToBuffer2(const vk::CopyImageToBufferInfo2& copyInfo)
{
    m_handle.copyImageToBuffer2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::CopyImageToBuffer2(VulkanImage* srcImage, VulkanBuffer* dstBuffer,
                                             vk::ArrayProxy<vk::BufferImageCopy2> regions)
{
    vk::CopyImageToBufferInfo2 copyInfo;
    copyInfo.srcImage = srcImage->GetHandle();
    copyInfo.srcImageLayout = srcImage->GetCurrentLayout();
    copyInfo.dstBuffer = dstBuffer->GetHandle();
    copyInfo.setRegions(regions);
    m_handle.copyImageToBuffer2(copyInfo, GetDispatcher());
}

void VulkanCommandBuffer::UpdateBuffer(VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset,
                                       vk::DeviceSize size, const void* data)
{
    m_handle.updateBuffer(dstBuffer->GetHandle(), dstOffset, size, data, GetDispatcher());
}

void VulkanCommandBuffer::FillBuffer(VulkanBuffer* dstBuffer, vk::DeviceSize dstOffset,
                                     vk::DeviceSize size, uint32_t data)
{
    m_handle.fillBuffer(dstBuffer->GetHandle(), dstOffset, size, data, GetDispatcher());
}

void VulkanCommandBuffer::SetPipelineBarrier(
    vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
    vk::DependencyFlags dependencyFlags, vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
    vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
    vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers)
{
    m_handle.pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, memoryBarriers,
                             bufferMemoryBarriers, imageMemoryBarriers, GetDispatcher());
}

void VulkanCommandBuffer::SetPipelineBarrier2(const vk::DependencyInfoKHR& dependencyInfo)
{
    m_handle.pipelineBarrier2(dependencyInfo, GetDispatcher());
}

void VulkanCommandBuffer::SetPipelineBarrier2(
    vk::DependencyFlags flags, vk::ArrayProxy<vk::MemoryBarrier2> memoryBarriers,
    vk::ArrayProxy<vk::BufferMemoryBarrier2> bufferMemoryBarriers,
    vk::ArrayProxy<vk::ImageMemoryBarrier2> imageMemoryBarriers)
{
    vk::DependencyInfoKHR dependencyInfo;
    dependencyInfo.setDependencyFlags(flags);
    dependencyInfo.setMemoryBarriers(memoryBarriers);
    dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);
    dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);
    m_handle.pipelineBarrier2(dependencyInfo, GetDispatcher());
}

void VulkanCommandBuffer::SetPipelineBarrier_ComputeToComputeRAW()
{
    vk::MemoryBarrier2 memoryBarriers;
    memoryBarriers.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    memoryBarriers.srcAccessMask = vk::AccessFlagBits2::eShaderWrite;
    memoryBarriers.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    memoryBarriers.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
    vk::DependencyInfo dependencyInfo;
    dependencyInfo.setMemoryBarriers(memoryBarriers);
    m_handle.pipelineBarrier2(dependencyInfo, GetDispatcher());
}

void VulkanCommandBuffer::SetPipelineBarrier_ComputeToComputeWAR()
{
    // A pipeline barrier or event without a any access flags is an execution dependency.
    vk::MemoryBarrier2 memoryBarriers;
    memoryBarriers.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    memoryBarriers.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    vk::DependencyInfo dependencyInfo;
    dependencyInfo.setMemoryBarriers(memoryBarriers);
    m_handle.pipelineBarrier2(dependencyInfo, GetDispatcher());
}

void VulkanCommandBuffer::ExecuteCommands(vk::ArrayProxy<VulkanCommandBuffer*> cmdBuffers)
{
    SmallVector<vk::CommandBuffer, 8> cmdBufferHandles(cmdBuffers.size());
    for (size_t i = 0; i < cmdBuffers.size(); ++i)
    {
        cmdBufferHandles[i] = cmdBuffers.data()[i]->GetHandle();
    }
    m_handle.executeCommands(cmdBufferHandles, GetDispatcher());
}

void VulkanCommandBuffer::BindPipeline(VulkanPipeline* pipeline)
{
    m_handle.bindPipeline(pipeline->GetBindPoint(), pipeline->GetHandle(), GetDispatcher());
}

void VulkanCommandBuffer::BindDescriptorSets(vk::PipelineBindPoint bindPoint,
                                             VulkanPipelineLayout* layout, uint32_t firstSet,
                                             vk::ArrayProxy<VulkanDescriptorSet*> descriptorSets,
                                             vk::ArrayProxy<uint32_t> dynamicOffsets)
{
    SmallVector<vk::DescriptorSet, 8> descriptorSetHandles(descriptorSets.size());
    for (size_t i = 0; i < descriptorSets.size(); ++i)
    {
        descriptorSetHandles[i] = descriptorSets.data()[i]->GetHandle();
    }
    m_handle.bindDescriptorSets(bindPoint, layout->GetHandle(), firstSet, descriptorSetHandles,
                                dynamicOffsets, GetDispatcher());
}

void VulkanCommandBuffer::ClearColorImage(VulkanImage* image, const vk::ClearColorValue& color,
                                          vk::ArrayProxy<vk::ImageSubresourceRange> ranges)
{
    m_handle.clearColorImage(image->GetHandle(), image->GetCurrentLayout(), color, ranges,
                             GetDispatcher());
}

void VulkanCommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    m_handle.dispatch(groupCountX, groupCountY, groupCountZ, GetDispatcher());
}

void VulkanCommandBuffer::DispatchIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset)
{
    m_handle.dispatchIndirect(indirectBuffer->GetHandle(), offset, GetDispatcher());
}

void VulkanCommandBuffer::DispatchBase(uint32_t baseGroupX, uint32_t baseGroupY,
                                       uint32_t baseGroupZ, uint32_t groupCountX,
                                       uint32_t groupCountY, uint32_t groupCountZ)
{
    m_handle.dispatchBase(baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ,
                          GetDispatcher());
}

void VulkanCommandBuffer::SetEvent(VulkanEvent* event, vk::PipelineStageFlags stageMask)
{
    m_handle.setEvent(event->GetHandle(), stageMask, GetDispatcher());
}

void VulkanCommandBuffer::ResetEvent(VulkanEvent* event, vk::PipelineStageFlags stageMask)
{
    m_handle.resetEvent(event->GetHandle(), stageMask, GetDispatcher());
}

void VulkanCommandBuffer::WaitEvents(vk::ArrayProxy<VulkanEvent*> events,
                                     vk::PipelineStageFlags srcStageMask,
                                     vk::PipelineStageFlags dstStageMask,
                                     vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
                                     vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
                                     vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers)
{
    SmallVector<vk::Event, 8> eventHandles(events.size());
    for (size_t i = 0; i < events.size(); ++i)
    {
        eventHandles[i] = events.data()[i]->GetHandle();
    }
    m_handle.waitEvents(eventHandles, srcStageMask, dstStageMask, memoryBarriers,
                        bufferMemoryBarriers, imageMemoryBarriers, GetDispatcher());
}

void VulkanCommandBuffer::PushConstants(VulkanPipelineLayout* layout,
                                        vk::ShaderStageFlags stageFlags, uint32_t offset,
                                        uint32_t size, const void* values)
{
    m_handle.pushConstants(layout->GetHandle(), stageFlags, offset, size, values, GetDispatcher());
}

void VulkanCommandBuffer::SetViewport(uint32_t firstViewport,
                                      vk::ArrayProxy<vk::Viewport> viewports)
{
    m_handle.setViewport(firstViewport, viewports, GetDispatcher());
}

void VulkanCommandBuffer::SetScissor(uint32_t firstScissor, vk::ArrayProxy<vk::Rect2D> scissors)
{
    m_handle.setScissor(firstScissor, scissors, GetDispatcher());
}

void VulkanCommandBuffer::SetViewportWithCount(vk::ArrayProxy<vk::Viewport> viewports)
{
    m_handle.setViewportWithCount(viewports, GetDispatcher());
}

void VulkanCommandBuffer::SetScissorWithCount(vk::ArrayProxy<vk::Rect2D> scissors)
{
    m_handle.setScissorWithCount(scissors, GetDispatcher());
}

void VulkanCommandBuffer::SetLineWidth(float lineWidth)
{
    m_handle.setLineWidth(lineWidth, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp,
                                       float depthBiasSlopeFactor)
{
    m_handle.setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor,
                          GetDispatcher());
}

void VulkanCommandBuffer::SetBlendConstants(vk::ArrayProxy<float> blendConstants)
{
    assert(blendConstants.size() == 4);
    m_handle.setBlendConstants(blendConstants.data(), GetDispatcher());
}

void VulkanCommandBuffer::SetDepthBounds(float minDepthBounds, float maxDepthBounds)
{
    m_handle.setDepthBounds(minDepthBounds, maxDepthBounds, GetDispatcher());
}

void VulkanCommandBuffer::SetCullMode(vk::CullModeFlags cullMode)
{
    m_handle.setCullMode(cullMode, GetDispatcher());
}

void VulkanCommandBuffer::SetFrontFace(vk::FrontFace frontFace)
{
    m_handle.setFrontFace(frontFace, GetDispatcher());
}

void VulkanCommandBuffer::SetPrimitiveTopology(vk::PrimitiveTopology primitiveTopology)
{
    m_handle.setPrimitiveTopology(primitiveTopology, GetDispatcher());
}

void VulkanCommandBuffer::SetRasterizerDiscardEnable(bool rasterizerDiscardEnable)
{
    m_handle.setRasterizerDiscardEnable(rasterizerDiscardEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthBiasEnable(bool depthBiasEnable)
{
    m_handle.setDepthBiasEnable(depthBiasEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetPrimitiveRestartEnable(bool primitiveRestartEnable)
{
    m_handle.setPrimitiveRestartEnable(primitiveRestartEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthTestEnable(bool depthTestEnable)
{
    m_handle.setDepthTestEnable(depthTestEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthWriteEnable(bool depthWriteEnable)
{
    m_handle.setDepthWriteEnable(depthWriteEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthCompareOp(vk::CompareOp depthCompareOp)
{
    m_handle.setDepthCompareOp(depthCompareOp, GetDispatcher());
}

void VulkanCommandBuffer::SetDepthBoundsTestEnable(bool depthBoundsTestEnable)
{
    m_handle.setDepthBoundsTestEnable(depthBoundsTestEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetStencilTestEnable(bool stencilTestEnable)
{
    m_handle.setStencilTestEnable(stencilTestEnable, GetDispatcher());
}

void VulkanCommandBuffer::SetStencilCompareMask(vk::StencilFaceFlags faceMask, uint32_t compareMask)
{
    m_handle.setStencilCompareMask(faceMask, compareMask, GetDispatcher());
}

void VulkanCommandBuffer::SetStencilWriteMask(vk::StencilFaceFlags faceMask, uint32_t writeMask)
{
    m_handle.setStencilWriteMask(faceMask, writeMask, GetDispatcher());
}

void VulkanCommandBuffer::SetStencilReference(vk::StencilFaceFlags faceMask, uint32_t reference)
{
    m_handle.setStencilReference(faceMask, reference, GetDispatcher());
}

void VulkanCommandBuffer::SetStencilOp(vk::StencilFaceFlags faceMask, vk::StencilOp failOp,
                                       vk::StencilOp passOp, vk::StencilOp depthFailOp,
                                       vk::CompareOp compareOp)
{
    m_handle.setStencilOp(faceMask, failOp, passOp, depthFailOp, compareOp, GetDispatcher());
}

void VulkanCommandBuffer::BindIndexBuffer(VulkanBuffer* buffer, vk::DeviceSize offset,
                                          vk::IndexType indexType)
{
    m_handle.bindIndexBuffer(buffer->GetHandle(), offset, indexType, GetDispatcher());
}

void VulkanCommandBuffer::BindVertexBuffers(uint32_t firstBinding,
                                            vk::ArrayProxy<VulkanBuffer*> buffers,
                                            vk::ArrayProxy<vk::DeviceSize> offsets)
{
    SmallVector<vk::Buffer, 8> bufferHandles(buffers.size());
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        bufferHandles[i] = buffers.data()[i]->GetHandle();
    }
    m_handle.bindVertexBuffers(firstBinding, bufferHandles, offsets, GetDispatcher());
}

void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
                               uint32_t firstInstance)
{
    m_handle.draw(vertexCount, instanceCount, firstVertex, firstInstance, GetDispatcher());
}

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                      uint32_t firstIndex, int32_t vertexOffset,
                                      uint32_t firstInstance)
{
    m_handle.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance,
                         GetDispatcher());
}

void VulkanCommandBuffer::DrawIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                                       uint32_t drawCount, uint32_t stride)
{
    m_handle.drawIndirect(indirectBuffer->GetHandle(), offset, drawCount, stride, GetDispatcher());
}

void VulkanCommandBuffer::DrawIndexedIndirect(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                                              uint32_t drawCount, uint32_t stride)
{
    m_handle.drawIndexedIndirect(indirectBuffer->GetHandle(), offset, drawCount, stride,
                                 GetDispatcher());
}

void VulkanCommandBuffer::DrawIndirectCount(VulkanBuffer* indirectBuffer, vk::DeviceSize offset,
                                            VulkanBuffer* countBuffer,
                                            vk::DeviceSize countBufferOffset, uint32_t maxDrawCount,
                                            uint32_t stride)
{
    m_handle.drawIndirectCount(indirectBuffer->GetHandle(), offset, countBuffer->GetHandle(),
                               countBufferOffset, maxDrawCount, stride, GetDispatcher());
}

void VulkanCommandBuffer::DrawIndexedIndirectCount(VulkanBuffer* indirectBuffer,
                                                   vk::DeviceSize offset, VulkanBuffer* countBuffer,
                                                   vk::DeviceSize countBufferOffset,
                                                   uint32_t maxDrawCount, uint32_t stride)
{
    m_handle.drawIndexedIndirectCount(indirectBuffer->GetHandle(), offset, countBuffer->GetHandle(),
                                      countBufferOffset, maxDrawCount, stride, GetDispatcher());
}

void VulkanCommandBuffer::BlitImage(VulkanImage* srcImage, VulkanImage* dstImage,
                                    vk::ArrayProxy<vk::ImageBlit> regions, vk::Filter filter)
{
    m_handle.blitImage(srcImage->GetHandle(), srcImage->GetCurrentLayout(), dstImage->GetHandle(),
                       dstImage->GetCurrentLayout(), regions, filter, GetDispatcher());
}

void VulkanCommandBuffer::ClearDepthStencilImage(VulkanImage* image,
                                                 vk::ClearDepthStencilValue clearValue,
                                                 vk::ArrayProxy<vk::ImageSubresourceRange> ranges)
{
    m_handle.clearDepthStencilImage(image->GetHandle(), image->GetCurrentLayout(), clearValue,
                                    ranges, GetDispatcher());
}

void VulkanCommandBuffer::ClearAttachments(vk::ArrayProxy<vk::ClearAttachment> attachments,
                                           vk::ArrayProxy<vk::ClearRect> rects)
{
    m_handle.clearAttachments(attachments, rects, GetDispatcher());
}

void VulkanCommandBuffer::ResolveImage(VulkanImage* srcImage, VulkanImage* dstImage,
                                       vk::ArrayProxy<vk::ImageResolve> regions)
{
    m_handle.resolveImage(srcImage->GetHandle(), srcImage->GetCurrentLayout(),
                          dstImage->GetHandle(), dstImage->GetCurrentLayout(), regions,
                          GetDispatcher());
}

void VulkanCommandBuffer::BeginRenderPass(const vk::RenderPassBeginInfo& renderPassBeginInfo,
                                          vk::SubpassContents contents)
{
    m_handle.beginRenderPass(renderPassBeginInfo, contents, GetDispatcher());
}

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass* renderPass,
                                          VulkanFramebuffer* framebuffer,
                                          const vk::Rect2D& renderArea,
                                          vk::ArrayProxy<vk::ClearValue> clearValues,
                                          vk::SubpassContents contents)
{
    vk::RenderPassBeginInfo beginInfo;
    beginInfo.renderPass = renderPass->GetHandle();
    beginInfo.framebuffer = framebuffer->GetHandle();
    beginInfo.renderArea = renderArea;
    beginInfo.setClearValues(clearValues);
    m_handle.beginRenderPass(beginInfo, contents, GetDispatcher());
}

void VulkanCommandBuffer::NextSubpass(vk::SubpassContents contents)
{
    m_handle.nextSubpass(contents, GetDispatcher());
}

void VulkanCommandBuffer::EndRenderPass()
{
    m_handle.endRenderPass(GetDispatcher());
}

void VulkanCommandBuffer::SetDeviceMask(uint32_t deviceMask)
{
    m_handle.setDeviceMask(deviceMask, GetDispatcher());
}

void VulkanCommandBuffer::ResetQueryPool(VulkanQueryPool* queryPool, uint32_t firstQuery,
                                         uint32_t queryCount)
{
    m_handle.resetQueryPool(queryPool->GetHandle(), firstQuery, queryCount, GetDispatcher());
}

void VulkanCommandBuffer::BeginQuery(VulkanQueryPool* queryPool, uint32_t query,
                                     vk::QueryControlFlags flags)
{
    m_handle.beginQuery(queryPool->GetHandle(), query, flags, GetDispatcher());
}

void VulkanCommandBuffer::EndQuery(VulkanQueryPool* queryPool, uint32_t query)
{
    m_handle.endQuery(queryPool->GetHandle(), query, GetDispatcher());
}

void VulkanCommandBuffer::CopyQueryPoolResults(VulkanQueryPool* queryPool, uint32_t firstQuery,
                                               uint32_t queryCount, VulkanBuffer* dstBuffer,
                                               vk::DeviceSize dstOffset, vk::DeviceSize stride,
                                               vk::QueryResultFlags flags)
{
    m_handle.copyQueryPoolResults(queryPool->GetHandle(), firstQuery, queryCount,
                                  dstBuffer->GetHandle(), dstOffset, stride, flags,
                                  GetDispatcher());
}

void VulkanCommandBuffer::WriteTimestamp(vk::PipelineStageFlagBits stage,
                                         VulkanQueryPool* queryPool, uint32_t query)
{
    m_handle.writeTimestamp(stage, queryPool->GetHandle(), query, GetDispatcher());
}

void VulkanCommandBuffer::WriteTimestamp2(vk::PipelineStageFlagBits2 stage,
                                          VulkanQueryPool* queryPool, uint32_t query)
{
    m_handle.writeTimestamp2(stage, queryPool->GetHandle(), query, GetDispatcher());
}

} // namespace rad
