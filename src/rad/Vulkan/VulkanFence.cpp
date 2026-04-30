#include <rad/Vulkan/VulkanFence.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanFence::VulkanFence(Ref<VulkanDevice> device, const vk::FenceCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createFence(createInfo, nullptr, GetDispatcher());
}

VulkanFence::~VulkanFence()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyFence(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanFence::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

vk::Result VulkanFence::Wait(uint64_t timeout)
{
    return m_device->GetHandle().waitForFences(m_handle, vk::True, timeout, GetDispatcher());
}

void VulkanFence::Reset()
{
    m_device->GetHandle().resetFences(m_handle, GetDispatcher());
}

} // namespace rad
