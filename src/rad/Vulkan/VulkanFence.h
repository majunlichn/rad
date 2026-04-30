#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanFence : public RefCounted<VulkanFence>
{
public:
    VulkanFence(Ref<VulkanDevice> device, const vk::FenceCreateInfo& createInfo);
    ~VulkanFence();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Fence& GetHandle() const { return m_handle; }

    // @param timeout: in nanoseconds, will be adjusted to the closest value allowed by implementation.
    vk::Result Wait(uint64_t timeout = UINT64_MAX);
    void Reset();

private:
    Ref<VulkanDevice> m_device;
    vk::Fence m_handle = nullptr;

}; // class VulkanFence

} // namespace rad
