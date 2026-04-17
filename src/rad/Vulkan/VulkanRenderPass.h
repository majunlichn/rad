#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanRenderPass : public RefCounted<VulkanRenderPass>
{
public:
    VulkanRenderPass(Ref<VulkanDevice> device, const vk::RenderPassCreateInfo& createInfo);
    ~VulkanRenderPass();

    vk::RenderPass GetHandle() const { return m_handle; }

    Ref<VulkanDevice> m_device;
    vk::raii::RenderPass m_handle = {nullptr};

}; // class VulkanRenderPass

} // namespace rad
