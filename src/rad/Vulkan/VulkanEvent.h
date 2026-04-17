#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanEvent : public RefCounted<VulkanEvent>
{
public:
    VulkanEvent(Ref<VulkanDevice> device, const vk::EventCreateInfo& createInfo);
    ~VulkanEvent();

    vk::Event GetHandle() const { return m_handle; }

    Ref<VulkanDevice> m_device;
    vk::raii::Event m_handle = {nullptr};

}; // class VulkanEvent

} // namespace rad
