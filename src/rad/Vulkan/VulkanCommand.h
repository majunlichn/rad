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

}; // class VulkanCommandBuffer

} // namespace rad
