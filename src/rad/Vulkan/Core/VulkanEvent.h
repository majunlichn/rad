#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanEvent : public RefCounted<VulkanEvent>
{
public:
    VulkanEvent(Ref<VulkanDevice> device, const vk::EventCreateInfo& createInfo);
    ~VulkanEvent();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Event& GetHandle() const { return m_handle; }

    vk::Result GetStatus() const;

    void Set();
    void Reset();

private:
    Ref<VulkanDevice> m_device;
    vk::Event m_handle = nullptr;

}; // class VulkanEvent

} // namespace rad
