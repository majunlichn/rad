#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanRenderPass : public RefCounted<VulkanRenderPass>
{
public:
    VulkanRenderPass(Ref<VulkanDevice> device, const vk::RenderPassCreateInfo& createInfo);
    ~VulkanRenderPass();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::RenderPass& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::RenderPass m_handle = nullptr;

}; // class VulkanRenderPass

} // namespace rad
