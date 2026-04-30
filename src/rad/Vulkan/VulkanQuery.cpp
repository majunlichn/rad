#include <rad/Vulkan/VulkanQuery.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanQueryPool::VulkanQueryPool(Ref<VulkanDevice> device,
                                 const vk::QueryPoolCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createQueryPool(createInfo, nullptr, GetDispatcher());
    if (m_handle)
    {
        m_flags = createInfo.flags;
        m_queryType = createInfo.queryType;
        m_queryCount = createInfo.queryCount;
    }
}

VulkanQueryPool::~VulkanQueryPool()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyQueryPool(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanQueryPool::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

} // namespace rad
