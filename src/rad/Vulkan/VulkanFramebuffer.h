#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanFramebuffer : public RefCounted<VulkanFramebuffer>
{
public:
    VulkanFramebuffer(Ref<VulkanDevice> device, const vk::FramebufferCreateInfo& createInfo);
    ~VulkanFramebuffer();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Framebuffer& GetHandle() const { return m_handle; }

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    uint32_t GetLayers() const { return m_layers; }

private:
    Ref<VulkanDevice> m_device;
    vk::Framebuffer m_handle = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_layers = 0;

}; // class VulkanFramebuffer

} // namespace rad
