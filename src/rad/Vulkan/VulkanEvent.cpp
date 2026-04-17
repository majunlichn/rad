#include <rad/Vulkan/VulkanEvent.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanEvent::VulkanEvent(Ref<VulkanDevice> device, const vk::EventCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->m_handle.createEvent(createInfo);
}

VulkanEvent::~VulkanEvent()
{
}

} // namespace rad
