#include <rad/Vulkan/VulkanSampler.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanSampler::VulkanSampler(Ref<VulkanDevice> device, const vk::SamplerCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createSampler(createInfo, nullptr, GetDispatcher());
}

VulkanSampler::~VulkanSampler()
{
    if (m_handle)
    {
        m_device->GetHandle().destroySampler(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanSampler::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

} // namespace rad
