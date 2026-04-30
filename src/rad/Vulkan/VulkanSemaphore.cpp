#include <rad/Vulkan/VulkanSemaphore.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanSemaphore::VulkanSemaphore(Ref<VulkanDevice> device,
                                 const vk::SemaphoreCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createSemaphore(createInfo, nullptr, GetDispatcher());
}

VulkanSemaphore::~VulkanSemaphore()
{
    if (m_handle)
    {
        m_device->GetHandle().destroySemaphore(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanSemaphore::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

} // namespace rad
