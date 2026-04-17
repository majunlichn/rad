#include <rad/System/MemoryDebug.h>

#include <rad/Vulkan/VulkanCommand.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanCommandPool::VulkanCommandPool(Ref<VulkanDevice> device, VulkanQueueFamily queueFamily,
                                     vk::CommandPoolCreateFlags flags) :
    m_device(std::move(device)),
    m_queueFamily(queueFamily)
{
    vk::CommandPoolCreateInfo createInfo(flags, m_device->GetQueueFamilyIndex(queueFamily));
    m_handle = m_device->m_handle.createCommandPool(createInfo);
}

VulkanCommandPool::~VulkanCommandPool()
{
}

std::vector<Ref<VulkanCommandBuffer>> VulkanCommandPool::AllocateCommandBuffers(
    vk::CommandBufferLevel level, uint32_t count)
{
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.commandPool = GetHandle();
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = count;
    std::vector<vk::CommandBuffer> cmdBufferHandles(count);
    VK_CHECK_RETURN(m_device->GetDispatcher()->vkAllocateCommandBuffers(
        m_device->GetHandle(), reinterpret_cast<const VkCommandBufferAllocateInfo*>(&allocateInfo),
        reinterpret_cast<VkCommandBuffer*>(cmdBufferHandles.data())));
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
    vk::CommandPool cmdPoolHandle = GetHandle();
    vk::CommandBuffer cmdBufferHandle = {};
    VK_CHECK_RETURN(m_device->GetDispatcher()->vkAllocateCommandBuffers(
        m_device->GetHandle(), reinterpret_cast<const VkCommandBufferAllocateInfo*>(&allocateInfo),
        reinterpret_cast<VkCommandBuffer*>(&cmdBufferHandle)));
    return RAD_NEW VulkanCommandBuffer(this, cmdBufferHandle);
}

VulkanCommandBuffer::VulkanCommandBuffer(Ref<VulkanCommandPool> cmdPool,
                                         vk::CommandBuffer cmdBufferHandle) :
    m_cmdPool(std::move(cmdPool))
{
    m_handle = vk::raii::CommandBuffer(m_cmdPool->m_device->m_handle, cmdBufferHandle,
                                       m_cmdPool->GetHandle());
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::Begin(vk::CommandBufferUsageFlags flags,
                                vk::CommandBufferInheritanceInfo* pInheritanceInfo)
{
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = pInheritanceInfo;
    m_handle.begin(beginInfo);
}

void VulkanCommandBuffer::End()
{
    m_handle.end();
}

} // namespace rad
