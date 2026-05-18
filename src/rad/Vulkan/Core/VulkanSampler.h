#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanSampler : public RefCounted<VulkanSampler>
{
public:
    VulkanSampler(Ref<VulkanDevice> device, const vk::SamplerCreateInfo& createInfo);
    ~VulkanSampler();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Sampler& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::Sampler m_handle{nullptr};

}; // class VulkanSampler

} // namespace rad
