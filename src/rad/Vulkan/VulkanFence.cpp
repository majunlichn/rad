#include <rad/Vulkan/VulkanFence.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanFence::VulkanFence(Ref<VulkanDevice> device, const vk::FenceCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->m_handle.createFence(createInfo);
}

VulkanFence::~VulkanFence()
{
}

vk::Result VulkanFence::Wait(uint64_t timeout)
{
    return m_device->m_handle.waitForFences({m_handle}, vk::True, timeout);
}

void VulkanFence::Reset()
{
    m_device->m_handle.resetFences({m_handle});
}

} // namespace rad
