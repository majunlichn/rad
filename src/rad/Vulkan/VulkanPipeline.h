#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanShaderModule : public RefCounted<VulkanShaderModule>
{
public:
    VulkanShaderModule(Ref<VulkanDevice> device, const vk::ShaderModuleCreateInfo& createInfo);
    ~VulkanShaderModule();

    vk::ShaderModule GetHandle() const { return static_cast<vk::ShaderModule>(m_handle); }

    Ref<VulkanDevice> m_device;
    vk::raii::ShaderModule m_handle = {nullptr};

}; // class VulkanShaderModule

} // namespace rad
