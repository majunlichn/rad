#include <rad/Vulkan/VulkanSemaphore.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanSemaphore::VulkanSemaphore(rad::Ref<VulkanDevice> device,
                                 const vk::SemaphoreCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->m_handle.createSemaphore(createInfo);
}

VulkanSemaphore::~VulkanSemaphore()
{
}

} // namespace rad
