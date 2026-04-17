#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanSemaphore : public rad::RefCounted<VulkanSemaphore>
{
public:
    VulkanSemaphore(rad::Ref<VulkanDevice> device, const vk::SemaphoreCreateInfo& createInfo);
    ~VulkanSemaphore();

    vk::Semaphore GetHandle() const { return m_handle; }

    rad::Ref<VulkanDevice> m_device;
    vk::raii::Semaphore m_handle = {nullptr};

}; // class VulkanSemaphore

} // namespace rad
