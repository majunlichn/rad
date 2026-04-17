#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanCommandPool : public RefCounted<VulkanCommandPool>
{
public:
    VulkanCommandPool(Ref<VulkanDevice> device, VulkanQueueFamily queueFamily,
                      vk::CommandPoolCreateFlags flags);
    ~VulkanCommandPool();

    vk::CommandPool GetHandle() const { return static_cast<vk::CommandPool>(m_handle); }

    std::vector<Ref<VulkanCommandBuffer>> AllocateCommandBuffers(vk::CommandBufferLevel level,
                                                                 uint32_t count);

    Ref<VulkanCommandBuffer> AllocateCommandBuffer(vk::CommandBufferLevel level);

    Ref<VulkanDevice> m_device;
    VulkanQueueFamily m_queueFamily;
    vk::raii::CommandPool m_handle = {nullptr};

}; // class VulkanCommandPool

class VulkanCommandBuffer : public RefCounted<VulkanCommandBuffer>
{
public:
    Ref<VulkanCommandPool> m_cmdPool;
    vk::raii::CommandBuffer m_handle = {nullptr};

    VulkanCommandBuffer(Ref<VulkanCommandPool> cmdPool, vk::CommandBuffer cmdBufferHandle);
    ~VulkanCommandBuffer();

    vk::CommandBuffer GetHandle() const { return static_cast<vk::CommandBuffer>(m_handle); }

    void Begin(vk::CommandBufferUsageFlags flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
               vk::CommandBufferInheritanceInfo* pInheritanceInfo = nullptr);
    void End();
    void Reset(vk::CommandBufferResetFlags flags) { m_handle.reset(flags); }

    void SetEvent(vk::Event event, vk::PipelineStageFlags stageMask)
    {
        m_handle.setEvent(event, stageMask);
    }

    void ResetEvent(vk::Event event, vk::PipelineStageFlags stageMask)
    {
        m_handle.resetEvent(event, stageMask);
    }

    void WaitEvents(vk::ArrayProxy<vk::Event> events, vk::PipelineStageFlags srcStageMask,
                    vk::PipelineStageFlags dstStageMask,
                    vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
                    vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
                    vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers)
    {
        m_handle.waitEvents(events, srcStageMask, dstStageMask, memoryBarriers,
                            bufferMemoryBarriers, imageMemoryBarriers);
    }

    void SetPipelineBarrier(vk::PipelineStageFlags srcStageMask,
                            vk::PipelineStageFlags dstStageMask,
                            vk::DependencyFlags dependencyFlags,
                            vk::ArrayProxy<vk::MemoryBarrier> memoryBarriers,
                            vk::ArrayProxy<vk::BufferMemoryBarrier> bufferMemoryBarriers,
                            vk::ArrayProxy<vk::ImageMemoryBarrier> imageMemoryBarriers)
    {
        m_handle.pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, memoryBarriers,
                                 bufferMemoryBarriers, imageMemoryBarriers);
    }

    void SetPipelineBarrier2(const vk::DependencyInfoKHR& dependencyInfo)
    {
        m_handle.pipelineBarrier2(dependencyInfo);
    }

    void SetPipelineBarrier2(vk::DependencyFlags flags,
                             vk::ArrayProxy<vk::MemoryBarrier2> memoryBarriers,
                             vk::ArrayProxy<vk::BufferMemoryBarrier2> bufferMemoryBarriers,
                             vk::ArrayProxy<vk::ImageMemoryBarrier2> imageMemoryBarriers)
    {
        vk::DependencyInfoKHR dependencyInfo;
        dependencyInfo.setDependencyFlags(flags);
        dependencyInfo.setMemoryBarriers(memoryBarriers);
        dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);
        dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);
        m_handle.pipelineBarrier2(dependencyInfo);
    }

}; // class VulkanCommandBuffer

} // namespace rad
