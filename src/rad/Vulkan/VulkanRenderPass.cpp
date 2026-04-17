#include <rad/Vulkan/VulkanRenderPass.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanRenderPass::VulkanRenderPass(Ref<VulkanDevice> device,
                                   const vk::RenderPassCreateInfo& createInfo) :
    m_device(device)
{
    m_handle = m_device->m_handle.createRenderPass(createInfo);
}

VulkanRenderPass::~VulkanRenderPass()
{
}

} // namespace rad
