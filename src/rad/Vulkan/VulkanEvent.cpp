#include <rad/Vulkan/VulkanEvent.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanEvent::VulkanEvent(Ref<VulkanDevice> device, const vk::EventCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createEvent(createInfo, nullptr, GetDispatcher());
}

VulkanEvent::~VulkanEvent()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyEvent(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanEvent::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

vk::Result VulkanEvent::GetStatus() const
{
    return m_device->GetHandle().getEventStatus(m_handle, GetDispatcher());
}

void VulkanEvent::Set()
{
    return m_device->GetHandle().setEvent(m_handle, GetDispatcher());
}

void VulkanEvent::Reset()
{
    return m_device->GetHandle().resetEvent(m_handle, GetDispatcher());
}

} // namespace rad
