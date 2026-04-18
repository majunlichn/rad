#include <rad/Vulkan/VulkanPipeline.h>

#include <rad/Vulkan/VulkanDevice.h>

namespace rad
{

VulkanShaderModule::VulkanShaderModule(Ref<VulkanDevice> device,
                                       const vk::ShaderModuleCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->m_handle.createShaderModule(createInfo);
}

VulkanShaderModule::~VulkanShaderModule()
{
}

} // namespace rad
