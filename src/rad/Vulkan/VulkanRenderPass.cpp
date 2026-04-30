#include <rad/Vulkan/VulkanRenderPass.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanRenderPass::VulkanRenderPass(Ref<VulkanDevice> device,
                                   const vk::RenderPassCreateInfo& createInfo) :
    m_device(device)
{
    m_handle = m_device->GetHandle().createRenderPass(createInfo, nullptr, GetDispatcher());
}

VulkanRenderPass::~VulkanRenderPass()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyRenderPass(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanRenderPass::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

} // namespace rad
