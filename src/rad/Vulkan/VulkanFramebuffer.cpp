#include <rad/Vulkan/VulkanFramebuffer.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanFramebuffer::VulkanFramebuffer(Ref<VulkanDevice> device,
                                     const vk::FramebufferCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createFramebuffer(createInfo, nullptr, GetDispatcher());
    if (m_handle)
    {
        m_width = createInfo.width;
        m_height = createInfo.height;
        m_layers = createInfo.layers;
    }
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyFramebuffer(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanFramebuffer::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

} // namespace rad
